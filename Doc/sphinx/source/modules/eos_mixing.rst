.. _eos-mixing:

EOS_Mixing (mélange vapeur et gaz incondensables)
=================================================

Le problème physique
--------------------

Dans une enceinte de confinement, un pressuriseur en cours de remplissage ou
un générateur de vapeur qui a aspiré de l'air, la phase gazeuse n'est pas de
la vapeur pure : elle contient de l'azote, de l'air, de l'hydrogène ou un
autre gaz qui ne condense pas aux conditions rencontrées. Ces gaz dits
*incondensables* changent profondément le comportement de la phase. La vapeur
ne voit plus la pression totale mais seulement sa pression partielle, si bien
que sa température de saturation baisse ; la condensation en paroi est freinée
par la couche de gaz qu'elle accumule ; la masse volumique, la capacité
thermique et les propriétés de transport deviennent celles d'un mélange.

Un code de thermohydraulique qui transporte les fractions massiques des
incondensables a donc besoin, à chaque maille et à chaque pas de temps, de
répondre à la question suivante : connaissant la pression totale :math:`p`,
l'enthalpie massique du mélange :math:`h` et les fractions massiques
:math:`c_0, c_1, \dots`, quelles sont la température, la masse volumique, la
pression partielle de vapeur et les propriétés de transport du gaz, ainsi que
leurs dérivées par rapport à :math:`p`, :math:`h` et :math:`c_i` ? C'est le
service que rend la classe :cpp:class:`NEPTUNE_EOS::EOS_Mixing`.

Par convention, le composant d'indice 0 est le **fluide principal**, celui qui
peut condenser (la vapeur d'eau dans la quasi-totalité des usages), et les
composants 1 à :math:`N` sont les incondensables. Les fractions
:math:`c_i` sont des fractions **massiques** ; elles doivent être fournies
toutes, y compris :math:`c_0`, et c'est à l'appelant de garantir que leur
somme vaut 1.

Vue d'ensemble de la classe
---------------------------

``EOS_Mixing`` est une méthode thermodynamique comme une autre : elle dérive
de :cpp:class:`NEPTUNE::EOS_Fluid` et s'instancie par son nom,
``EOS("EOS_Mixing")``. Sa particularité est de ne porter aucune loi d'état
propre. Elle agrège des objets :cpp:class:`NEPTUNE::EOS` déjà construits, un
par composant, et les interroge pour bâtir les propriétés du mélange. La
figure :ref:`ci-dessous <fig-mixing-classes>` résume cette composition.

.. _fig-mixing-classes:

.. graphviz::
   :caption: Diagramme de classes d'EOS_Mixing. La classe agrège des façades
             EOS (elle n'en est pas propriétaire) ; les incondensables sont
             des gaz parfaits, soit ``EOS_PerfectGas``, soit sa
             spécialisation ``EOS_CathareIncondensableGas``.

   digraph mixing_classes {
       rankdir=BT;
       node [shape=record, fontname="Helvetica", fontsize=10,
             style=filled, fillcolor="#eef3fa"];
       edge [fontname="Helvetica", fontsize=9];

       Fluid  [label="{EOS_Fluid|+ compute(EOS_Fields, EOS_Fields, EOS_Error_Field)\l+ get_mm(double&)\l+ get_prxr()\l}"];
       Mixing [label="{EOS_Mixing|- nb_fluids : int\l- the_fluids : EOS**\l- alpha : ArrOfDouble\l- compute_mode : MixingType\l|+ set_components(EOS**, int)\l+ set_component(int, const EOS&)\l+ operator[](int) : EOS&\l+ compute(EOS_Fields, EOS_Fields, EOS_Error_Field)\l+ compute_pv_hv_ph(...)\l- compute_cathare_mix(...)\l- compute_cathare2_mix(...)\l- compute_perfect_gas(...)\l}"];
       EOS    [label="{EOS|+ fluid() : EOS_Fluid&\l+ table_name()\l+ set_components(EOS**, int)\l}"];
       PG     [label="{EOS_PerfectGas|+ get_prxr()\l+ get_prxm()\l+ get_prxdv()\l+ get_prxl0() ... get_prxm2()\l}"];
       CIG    [label="{EOS_CathareIncondensableGas|+ get_prxcp0() ... get_prxcp6()\l}"];
       C2V    [label="{EOS_Cathare2Vapor|+ set_mixing_properties(...)\l+ calc2_mixing(int, EOS_Fields, EOS_Fields, EOS_Error_Field)\l}"];

       Mixing -> Fluid [arrowhead=empty];
       PG     -> Fluid [arrowhead=empty];
       C2V    -> Fluid [arrowhead=empty, style=dashed, label="via EOS_Cathare2"];
       CIG    -> PG    [arrowhead=empty];
       Mixing -> EOS   [arrowhead=odiamond, dir=back, arrowtail=odiamond,
                        label="the_fluids [1..6]"];
       EOS    -> Fluid [arrowhead=vee, label="délègue à"];
   }

Deux points de ce diagramme méritent d'être soulignés. D'abord
l'agrégation est faible : ``set_components`` recopie les *pointeurs* reçus, et
le destructeur d'``EOS_Mixing`` ne libère que le tableau de pointeurs. Les
objets ``EOS`` des composants doivent donc vivre au moins aussi longtemps que
le mélange, et ils restent utilisables seuls. Ensuite, le mélange ne connaît
les incondensables qu'à travers quelques constantes exposées par
``EOS_Fluid`` (accesseurs « provisoires » ``get_prxr``, ``get_prxm``,
``get_prxdv``…) : constante spécifique :math:`r_j = R/M_j`, masse molaire,
volume de diffusion, coefficients des lois :math:`\lambda_j(T)` et
:math:`\mu_j(T)`. Pour un ``EOS_PerfectGas`` ces constantes viennent des
fichiers ``Modules/EOS/Data/EOS_PerfectGas/*.data`` (clés ``xr``, ``xcp``,
``xm``, ``xl0``…``xl2``, ``xm0``…``xm2``, ``xdv``).

Le choix du modèle de mélange
-----------------------------

Trois algorithmes cohabitent dans la classe, hérités de l'histoire de la
bibliothèque. L'utilisateur ne choisit pas explicitement : la méthode privée
``set_compute_mode``, appelée par ``set_components``, examine la nature des
composants et fixe une fois pour toutes le mode de calcul. Les règles, dans
l'ordre où elles sont testées, sont les suivantes.

.. list-table:: Sélection du modèle de mélange selon les composants
   :header-rows: 1
   :widths: 40 22 38

   * - Composants
     - Mode retenu
     - Algorithme
   * - Fluide principal ``EOS_Cathare`` / ``WaterVapor``
     - ``Cathare``
     - Routines du greffon CATHARE (``calca_all_mixing``)
   * - Tous les incondensables sont des ``EOS_PerfectGas`` (quel que soit le
       fluide principal, hors cas précédent)
     - ``WithPerfectGas``
     - Algorithme générique écrit en C++ dans ``EOS_Mixing.cxx``
   * - Fluide principal ``EOS_Cathare2`` en phase vapeur, incondensables
       ``EOS_CathareIncondensableGas``
     - ``Cathare2``
     - Routines du greffon CATHARE2 (``calc2_mixing``)
   * - Toute autre combinaison
     - ``Unsupported``
     - Aucun : ``compute`` renvoie ``EOS_Error::error`` et remplit le champ
       d'erreurs avec ``NOT_IMPLEMENTED``

Le test porte sur ``table_name()`` : un ``EOS_CathareIncondensableGas`` a beau
hériter d'``EOS_PerfectGas``, son nom de table est
``CathareIncondensableGas`` et il n'oriente donc pas vers le mode
``WithPerfectGas``. Inversement, associer une vapeur CATHARE2 à des
``EOS_PerfectGas`` sélectionne l'algorithme générique et non celui du greffon.

Les trois modes reposent sur le même socle physique, décrit dans la section
suivante ; ils diffèrent par le lieu où le calcul est fait, par les lois de
transport disponibles et par quelques détails numériques. Le mode
``WithPerfectGas`` est le seul dont le code source complet figure dans le
dépôt, et c'est lui qui sert de fil conducteur.

Le socle commun : mélange idéal de Gibbs-Dalton
-----------------------------------------------

Hypothèses
~~~~~~~~~~

Le commentaire placé en tête de ``compute_pv_hv_ph`` les énonce sans
détour : *« mixture follows Dalton law, noncondensible gas are perfect »*.
Plus précisément :

* tous les constituants occupent le même volume à la même température
  :math:`T` (équilibre thermique et mécanique local, pas de glissement entre
  espèces) ;
* la pression totale est la somme des pressions partielles, chaque
  constituant se comportant comme s'il était seul dans le volume (loi de
  Dalton [Dalton1802]_) ;
* chaque incondensable est un gaz parfait de constante :math:`r_j` ; seule la
  vapeur est un gaz réel, décrit par la méthode du composant 0 évaluée à *sa*
  pression partielle :math:`p_v` et à *son* enthalpie :math:`h_v` ;
* le mélange est idéal : pas d'enthalpie de mélange, les grandeurs extensives
  s'additionnent (modèle de Gibbs-Dalton, voir par exemple [Cengel]_,
  chapitre « Gas mixtures »).

Ces hypothèses sont bonnes tant que les incondensables sont loin de leur point
critique et que la pression reste modérée, ce qui est le cas de l'air, de
l'azote ou de l'hydrogène dans les transitoires réacteur. Elles se dégradent à
haute pression, où les interactions entre molécules de nature différente ne
sont plus négligeables, et elles ignorent la solubilité des gaz dans le
liquide.

Équations
~~~~~~~~~

Notons :math:`\rho` la masse volumique du mélange et :math:`\rho_v` celle de
la vapeur prise à :math:`(p_v, h_v)`. Puisque la vapeur occupe tout le volume,
:math:`\rho_v = c_0\,\rho`, et la pression partielle de l'incondensable
:math:`j` s'écrit :math:`p_j = c_j\,\rho\,r_j\,T`. La loi de Dalton donne
alors

.. math::
   :label: mix-dalton

   p \;=\; p_v + \sum_{j\ge 1} p_j
     \;=\; p_v + \frac{\rho_v(p_v,h_v)\,T(p_v,h_v)}{c_0}\sum_{j\ge 1} c_j\, r_j ,

et l'additivité des enthalpies

.. math::
   :label: mix-enthalpie

   h \;=\; c_0\, h_v + \sum_{j\ge 1} c_j\, h_j(T).

Les deux inconnues sont :math:`p_v` et :math:`h_v` ; la température s'en
déduit par la loi d'état de la vapeur, :math:`T = T(p_v, h_v)`. Le code résout
ce système sous la forme

.. math::

   F(p_v,h_v) &= \sum_{j\ge 1} c_j\, h_j(T) + c_0\,h_v - h = 0, \\
   G(p_v,h_v) &= \rho_v\, T \sum_{j\ge 1} c_j\, r_j + c_0\,(p_v - p) = 0,

par une méthode de Newton à deux variables. Le jacobien est analytique : il
n'utilise que les dérivées :math:`\partial T/\partial p`,
:math:`\partial T/\partial h`, :math:`\partial\rho/\partial p`,
:math:`\partial\rho/\partial h` de la vapeur, que la méthode du composant 0
doit donc savoir fournir dans le plan :math:`(p,h)`.

L'enthalpie des incondensables n'est pas appelée directement à chaque
itération. Suivant l'usage de CATHARE, elle est développée autour de la
température de saturation de l'eau à 7 bar, :math:`T_7` (calculée une fois par
``compute_T_sat_p(7\cdot10^5)`` sur le fluide principal) :

.. math::

   h_j(T) \;\approx\; h_j(T_7) + c_{p,j}(T)\,\bigl(T - T_7\bigr).

C'est le rôle de la méthode privée ``compute_mixture_thermal_terms``, qui
accumule :math:`\sum c_j c_{p,j}`, :math:`\sum c_j\,\mathrm{d}c_{p,j}/\mathrm{d}T`,
:math:`\sum c_j r_j` et le terme constant
:math:`\sum c_j\,[h_j(T_7) - c_{p,j}\,T_7]`.

Une fois :math:`(p_v, h_v, T)` connus, les grandeurs du mélange suivent.
La constante spécifique du mélange (propriété ``prgr``) et la masse volumique
sont

.. math::

   r_g = c_0\, r_v + \sum_{j\ge1} c_j\, r_j ,
   \qquad
   \rho = \frac{(p - p_v) + r_v\,\rho_v\,T}{r_g\,T},

où :math:`r_v = R/M_0` avec :math:`R = 8{,}31447` J/mol/K. La seconde relation
est la loi de Dalton réécrite : le numérateur vaut :math:`\rho\, r_g\, T`
lorsque la vapeur est elle-même parfaite, et l'écriture retenue conserve le
caractère réel de la vapeur à travers :math:`\rho_v`. La capacité thermique
est la moyenne massique

.. math::

   c_p = c_0\, c_{p,v}(p_v,h_v) + \sum_{j\ge1} c_j\, c_{p,j}(T).

La tension superficielle reste celle du fluide principal, évaluée à la
pression partielle de vapeur, et les grandeurs de saturation sont calculées
deux fois : à la pression totale (``T_sat``, ``h_l_sat``…) et à la pression
partielle (``T_sat_0``, ``h_l_sat_0``…). C'est la seconde qui gouverne la
condensation.

Résolution numérique
~~~~~~~~~~~~~~~~~~~~

La figure :ref:`suivante <fig-mixing-activite>` déroule le calcul d'un point
dans le plan :math:`(p,h)`.

.. _fig-mixing-activite:

.. graphviz::
   :caption: Diagramme d'activité du calcul d'un point (p, h, c) en mode
             ``WithPerfectGas`` (``compute_perfect_gas`` et
             ``compute_pv_hv_ph``).

   digraph mixing_activity {
       rankdir=TB;
       node [shape=box, style="rounded,filled", fillcolor="#eef3fa",
             fontname="Helvetica", fontsize=10];
       edge [fontname="Helvetica", fontsize=9];

       start [shape=circle, label="", width=0.2, style=filled, fillcolor=black];
       check [label="Contrôle des champs d'entrée\n(p ou T, h, c_0 ... c_N ; tailles)"];
       init  [label="Initialisation\npv = c_0 p ; hv estimé par une loi linéaire en T"];
       vap   [label="Vapeur : T, rho et leurs dérivées en (pv, hv)\n(*this)[0].fluid().compute(...)"];
       terms [label="Incondensables : compute_mixture_thermal_terms\n(cp_j, dcp_j/dT, r_j, h_j(T7))"];
       newton[label="Résidus F, G et jacobien\nIncréments dpv, dhv"];
       test  [shape=diamond, fillcolor="#fff6d6",
              label="|dpv| <= tol_p et |dhv| <= 1 J/kg\nou 50 itérations ?"];
       props [label="Propriétés de la vapeur seule en (pv, hv)\net saturation en p et en pv"];
       mix   [label="Propriétés du mélange et dérivées\n(rho, cp, lambda, mu, dncv, d/dp, d/dh, d/dc_j)"];
       out   [label="Recopie dans les champs de sortie demandés"];
       end   [shape=doublecircle, label="", width=0.15, style=filled, fillcolor=black];

       start -> check -> init -> vap -> terms -> newton -> test;
       test -> vap   [label="non"];
       test -> props [label="oui"];
       props -> mix -> out -> end;
   }

L'initialisation prend :math:`p_v = c_0\,p` et estime :math:`h_v` en supposant
pour la vapeur une capacité thermique :math:`2000 + 10^{-4} p_v` J/kg/K à
partir de la saturation. À chaque itération, :math:`p_v` est maintenu
supérieur à :math:`\max(10^{-10}, 10^{-2} c_0)` Pa. Le critère d'arrêt porte
sur les incréments : :math:`|\delta h_v| \le 1` J/kg et
:math:`|\delta p_v| \le \mathrm{tol}_p`, où la tolérance en pression vaut
:math:`10^3 \min_i c_i` Pa, bornée entre 5 et 1000 Pa. La tolérance se
resserre donc automatiquement quand un constituant est présent à l'état de
traces, ce qui évite de noyer sa pression partielle dans l'erreur de
convergence. Le nombre d'itérations est limité à 50.

Lorsque les entrées sont :math:`(p, T)` au lieu de :math:`(p, h)`, la classe
se ramène au cas précédent par une seconde boucle de Newton, externe, sur
l'enthalpie du mélange :
:math:`h \leftarrow h + (T_\text{cible} - T)/(\partial T/\partial h)_p`,
jusqu'à :math:`|T_\text{cible} - T| \le 10^{-2}` K, avec le même plafond de 50
itérations et une borne supérieure :math:`h \le h_{max}` du fluide principal.
Un calcul en :math:`(p,T)` coûte donc plusieurs calculs complets en
:math:`(p,h)`. La propriété ``h`` peut être demandée en sortie dans ce plan ;
c'est ce que fait le notebook de démonstration pour passer d'un plan à
l'autre.

Enfin, si l'on ne fournit que la pression (ou la température) et les
fractions, l'appel est transmis tel quel au fluide principal : on obtient ses
propriétés de saturation à la pression **totale**.

Dérivées par rapport aux fractions massiques
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Les codes implicites ont besoin des dérivées du mélange par rapport à
:math:`p`, :math:`h` et :math:`c_j`. Elles sont obtenues analytiquement par le
théorème des fonctions implicites appliqué au système :math:`(F,G)` : le
jacobien inversé donne d'abord :math:`\partial p_v/\partial p`,
:math:`\partial h_v/\partial p`, :math:`\partial p_v/\partial h`,
:math:`\partial h_v/\partial h` (propriétés ``d_p_0_d_p_h``,
``d_h_0_d_p_h``…), puis les dérivées par rapport à :math:`c_j`
(``d_p_0_d_c_j_ph``, ``d_h_0_d_c_j_ph``), et toutes les autres s'en déduisent
par dérivation en chaîne. Dans ces dérivées, :math:`c_0` est éliminé par la
contrainte :math:`\sum c_i = 1` : augmenter :math:`c_j` revient à retirer la
même masse de vapeur.

Les modèles un par un
---------------------

Mode ``WithPerfectGas`` : algorithme générique
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

C'est la transcription en C++, au-dessus de l'API EOS, de l'algorithme de
CATHARE2 (le code le désigne comme « algorithme complet c2 »). Son intérêt est
de ne rien supposer du fluide principal : toute méthode capable de fournir
dans le plan :math:`(p,h)` la température, la masse volumique, :math:`c_p`,
:math:`\lambda`, :math:`\mu`, :math:`\sigma` et leurs dérivées premières,
ainsi que la saturation, peut jouer le rôle de la vapeur — CATHARE2, mais
aussi REFPROP ou un interpolateur :ref:`EOS_Ipp <eos-igen>`. Lorsque le fluide
principal est un ``EOS_Cathare2``, ``set_components`` aligne l'état de
référence des gaz parfaits sur le sien (``set_reference_state`` avec
:math:`h_{ref}`, :math:`T_{ref}`, :math:`p_{ref}` de la vapeur) pour que les
enthalpies s'additionnent sur une origine commune ; avec un autre fluide
principal, cet alignement est à la charge de l'utilisateur.

Thermodynamique
   Celle de la section précédente, sans variante.

Conductivité thermique et viscosité
   Le code calcule

   .. math::

      \lambda = \frac{c_0 r_v\,\lambda_v + \sum_j c_j r_j\,\lambda_j(T)}
                     {c_0 r_v + \sum_j c_j r_j},
      \qquad
      \mu = \frac{c_0 r_v\,\mu_v + \sum_j c_j r_j\,\mu_j(T)}
                 {c_0 r_v + \sum_j c_j r_j}.

   Comme :math:`c_i r_i = R\,c_i/M_i` est proportionnel au nombre de moles du
   constituant :math:`i`, ces expressions ne sont rien d'autre que la moyenne
   pondérée par les **fractions molaires**,
   :math:`\lambda = \sum_i x_i \lambda_i` et :math:`\mu = \sum_i x_i \mu_i`.
   C'est la loi de mélange la plus simple qui soit ; elle ne tient pas compte
   des interactions entre espèces de masses molaires très différentes et
   surestime typiquement la viscosité d'un mélange vapeur-hydrogène. Les lois
   de Wilke et de Mason-Saxena, qui corrigent ce défaut, ne sont **pas**
   implémentées dans ce mode (voir le mode ``Cathare2``). Les lois des corps
   purs sont, pour les gaz parfaits, des polynômes de degré 2 en température,
   :math:`\lambda_j = l_0 + l_1 T + l_2 T^2` et
   :math:`\mu_j = m_0 + m_1 T + m_2 T^2`.

Coefficient de diffusion
   La propriété ``dncv`` est le coefficient de diffusion des incondensables
   dans la vapeur, utilisé par les modèles de condensation en présence
   d'incondensables. Chaque coefficient binaire est donné par la corrélation
   de Fuller, Schettler et Giddings [Fuller1966]_, écrite en unités SI :

   .. math::

      D_{j} \;=\; \frac{0{,}0143\; T^{1{,}75}}
                       {p\,\sqrt{M_{j0}}\,
                        \bigl(v_j^{1/3} + v_0^{1/3}\bigr)^{2}},
      \qquad
      M_{j0} = \frac{2}{1/M_j + 1/M_0},

   avec :math:`p` en Pa, :math:`M` en g/mol, :math:`D` en m²/s et
   :math:`v` le volume de diffusion atomique de Fuller (``xdv`` dans les
   fichiers de données ; 18,5 pour l'azote). Le coefficient 0,0143 est
   le 0,00143 de la forme usuelle (:math:`p` en bar, :math:`D` en cm²/s,
   voir [Poling2001]_, éq. 11-4.4) converti en SI. Les coefficients binaires
   sont ensuite combinés par une loi de type Blanc [Blanc1908]_,

   .. math::

      D = \frac{\sum_{j\ge1} c_j}{\sum_{j\ge1} c_j / D_j},

   à ceci près que la pondération utilise les fractions massiques là où la loi
   de Blanc utilise les fractions molaires.

Limites connues
   Le volume de diffusion du fluide principal est codé en dur à 13,1, valeur
   de la vapeur d'eau ; de même l'initialisation de :math:`h_v` et celle du
   Newton en :math:`(p,T)` utilisent une capacité thermique de l'ordre de
   2000 J/kg/K propre à l'eau. Avec un autre fluide principal, ``dncv`` est
   faux et la convergence peut demander plus d'itérations. Les valeurs
   planchers que CATHARE impose à :math:`\lambda` et :math:`\mu` ne sont pas
   reprises. Enfin, le code de retour de ``compute_pv_hv_ph`` n'est pas
   exploité par ``compute_perfect_gas`` : une non-convergence du Newton en
   :math:`(p_v,h_v)` n'est pas signalée dans le champ d'erreurs, et
   l'appel renvoie ``EOS_Error::good``. Il est prudent de contrôler la
   vraisemblance des résultats aux très faibles teneurs en vapeur.

Mode ``Cathare2`` : délégation au greffon CATHARE2
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Quand le fluide principal est un ``EOS_Cathare2`` en phase vapeur et que les
incondensables sont des ``EOS_CathareIncondensableGas``
(``NitrogenIncondensable``, ``HydrogenIncondensable``,
``OxygenIncondensable``, ``AirIncondensable``, ``ArgonIncondensable``,
``HeliumIncondensable`` ou ``UserIncondensable``), ``EOS_Mixing`` ne calcule
rien lui-même. Il collecte les seize constantes de chaque gaz, les transmet à
la vapeur par ``EOS_Cathare2Vapor::set_mixing_properties``, appelle
``calc2_mixing`` puis remet la vapeur dans son état « sans incondensable ».
Tout le calcul est fait par les routines Fortran du greffon (famille
``FHVAPA``, ``FHGASA``, ``FHGAXA``, ``FHDIGA``), qui ne sont pas distribuées
avec EOS ; ce mode n'existe que si la bibliothèque a été configurée avec
``--with-cathare2``.

La physique est celle du socle commun, avec trois différences par rapport au
mode générique. La capacité thermique des incondensables est un polynôme en
température à sept coefficients (``prxcp0`` … ``prxcp6``) au lieu d'une
constante. Le nombre d'incondensables est limité à quatre. Surtout, les lois
de mélange des propriétés de transport sont au choix de l'utilisateur, par
les arguments d'initialisation ``LATYPML`` (conductivité) et ``MUTYPML``
(viscosité) de la **vapeur** :

.. list-table:: Lois de transport du mode ``Cathare2``
   :header-rows: 1
   :widths: 12 28 60

   * - Valeur
     - Loi
     - Expression
   * - 0 (défaut)
     - Standard
     - Moyenne molaire :math:`\sum_i x_i\,\lambda_i`, :math:`\sum_i x_i\,\mu_i`,
       avec les planchers :math:`\lambda \ge 5\cdot10^{-3}` W/m/K et
       :math:`\mu \ge 10^{-6}` Pa.s.
   * - 1 (``MUTYPML``)
     - Wilke [Wilke1950]_
     - :math:`\mu = \sum_i \dfrac{x_i\,\mu_i}{\sum_k x_k\,\Phi_{ik}}`
   * - 2 (``LATYPML``)
     - Mason et Saxena [MasonSaxena1958]_
     - :math:`\lambda = \sum_i \dfrac{x_i\,\lambda_i}{\sum_k x_k\,\Phi_{ik}}`

Les deux arguments acceptent les valeurs 0 à 2, mais seules les combinaisons
du tableau ont un sens : ``MUTYPML`` = 2 ou ``LATYPML`` = 1 ne correspondent à
aucune branche des routines et laissent la propriété non calculée. Dans les
deux lois, le coefficient d'interaction est celui de Wilke,

.. math::

   \Phi_{ik} = \frac{\Bigl[1 + (\mu_i/\mu_k)^{1/2}\,(M_k/M_i)^{1/4}\Bigr]^2}
                    {\sqrt{8\,(1 + M_i/M_k)}} .

La forme retenue pour la conductivité est l'équation de Wassiljewa
[Wassiljewa1904]_ avec les coefficients proposés par Mason et Saxena ; le
facteur multiplicatif 1,065 de la publication d'origine n'apparaît pas dans
la version 25_3 des sources CATHARE2 que nous avons consultée, ce qui revient
à l'approximation souvent attribuée à Lindsay et Bromley ou à Wilke (voir la
discussion de [Poling2001]_, section 10-6).

.. code-block:: c++

   // Vapeur CATHARE2 avec loi de Wilke pour mu et Mason-Saxena pour lambda
   Strings args(4);
   args[0] = AString("MUTYPML"); args[1] = AString("1");
   args[2] = AString("LATYPML"); args[3] = AString("2");
   EOS vapeur("EOS_Cathare2", "WaterVapor", args);

Le coefficient de diffusion suit la même corrélation de Fuller que dans le
mode générique (mêmes constantes 0,0143 et 1,75).

.. todo::

   La note de conception CATHARE2 décrivant le traitement des incondensables
   (routines ``FHVAPA``/``FHGASA``/``FHDIGA``) n'est pas référencée dans le
   dépôt. La référence publique la plus proche est [Bestion1990]_, qui
   présente les lois de fermeture du code mais pas le détail des lois de
   mélange. À compléter par la référence interne exacte.

Mode ``Cathare`` : délégation au greffon CATHARE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

C'est le mode historique, conservé pour la vapeur d'eau de la méthode
``EOS_Cathare`` (``EOS("EOS_Cathare", "WaterVapor")``). ``EOS_Mixing``
rassemble pour chaque incondensable huit constantes — :math:`r_j`, un
:math:`c_{p,j}` **constant**, et les trois coefficients de chacune des lois
:math:`\lambda_j(T)` et :math:`\mu_j(T)` — et appelle
``CATHARE_Water::calca_all_mixing``, qui enchaîne les routines du greffon
(``FPSATT``, ``FHVAPA``, ``FHGASA``, ``FHGAXA``…). Les incondensables sont en
pratique des ``EOS_PerfectGas``. Le modèle est le même mélange idéal de
Dalton ; il est limité à quatre incondensables, à l'eau, et l'appel doit
comporter entre quatre et sept champs d'entrée, sans quoi le champ d'erreurs
est mis à ``NOT_IMPLEMENTED``. Les propriétés disponibles sont plus
restreintes que dans les deux autres modes : :math:`T`, :math:`\rho`,
:math:`c_p`, :math:`\lambda`, :math:`\mu`, :math:`\sigma`, leurs dérivées en
:math:`p`, :math:`h`, :math:`c_1` … :math:`c_4`, et la saturation. Ce mode
suppose la bibliothèque configurée avec le greffon CATHARE
(``WITH_PLUGIN_CATHARE``) ; c'est celui qu'exerce le « Test 6 » de
``Modules/EOS/Tests/C++/main.cxx``.

Composer un mélange
-------------------

En C++
~~~~~~

On construit d'abord chaque composant comme un objet ``EOS`` ordinaire, puis
l'objet mélange, et l'on relie le tout par ``set_components``. L'ordre du
tableau fixe la numérotation : l'indice 0 est le fluide principal, l'indice
:math:`j` correspond à la fraction :math:`c_j`.

.. code-block:: c++

   #include "EOS/API/EOS.hxx"
   #include "EOS/API/EOS_Field.hxx"
   #include "EOS/API/EOS_Fields.hxx"
   #include "EOS/API/EOS_Error_Field.hxx"
   using namespace NEPTUNE;

   EOS vapeur("EOS_Cathare2", "WaterVapor");
   EOS azote ("EOS_PerfectGas", "Nitrogen");
   EOS argon ("EOS_PerfectGas", "Argon");

   EOS melange("EOS_Mixing");
   EOS* composants[3] = { &vapeur, &azote, &argon };
   melange.set_components(composants, 3);      // mode WithPerfectGas

   const int n = 2;
   ArrOfDouble xp(n), xh(n), xc0(n), xc1(n), xc2(n), xT(n), xrho(n), xpv(n);
   ArrOfInt    ierr(n);
   xp[0] = 1.e5;  xh[0] = 2.70e6;  xc0[0] = 0.6;  xc1[0] = 0.3;  xc2[0] = 0.1;
   xp[1] = 5.e5;  xh[1] = 2.75e6;  xc0[1] = 0.9;  xc1[1] = 0.1;  xc2[1] = 0.0;

   EOS_Fields entrees(5);
   entrees[0] = EOS_Field("p",  "p",   NEPTUNE::p,   xp);
   entrees[1] = EOS_Field("h",  "h",   NEPTUNE::h,   xh);
   entrees[2] = EOS_Field("c0", "c_0", NEPTUNE::c_0, xc0);
   entrees[3] = EOS_Field("c1", "c_1", NEPTUNE::c_1, xc1);
   entrees[4] = EOS_Field("c2", "c_2", NEPTUNE::c_2, xc2);

   EOS_Fields sorties(3);
   sorties[0] = EOS_Field("T",   "T",   NEPTUNE::T,   xT);
   sorties[1] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
   sorties[2] = EOS_Field("pv",  "p_0", NEPTUNE::p_0, xpv);

   EOS_Error_Field err(ierr);
   EOS_Error cr = melange.compute(entrees, sorties, err);

Deux contraintes sont vérifiées à l'entrée de ``compute`` en mode générique :
le nombre de champs :math:`c_i` fournis doit être exactement égal au nombre de
composants, et les champs thermodynamiques doivent être :math:`p` seul,
:math:`(p,h)` ou :math:`(p,T)`. Dans le cas contraire, le champ d'erreurs vaut
``INPUT_PROPERTY`` et le code de retour ``EOS_Error::error`` (voir
:doc:`../usage/errors`).

La figure :ref:`ci-dessous <fig-mixing-sequence>` montre ce qui se passe
derrière ces quelques lignes.

.. _fig-mixing-sequence:

.. graphviz::
   :caption: Séquence de composition puis de calcul d'un mélange (mode
             ``WithPerfectGas``). Chaque étape est préfixée par l'acteur qui
             l'exécute : code client (vert), objet ``EOS_Mixing`` (bleu),
             composants (jaune).

   digraph mixing_sequence {
       rankdir=TB;
       node [shape=box, fontname="Helvetica", fontsize=10, style=filled];
       edge [fontname="Helvetica", fontsize=9];

       c1 [fillcolor="#e8f5e2", label="1. [client] EOS vapeur(...), azote(...) ; EOS melange(\"EOS_Mixing\")"];
       c2 [fillcolor="#e8f5e2", label="2. [client] melange.set_components(tab, n)"];
       m1 [fillcolor="#eef3fa", label="3. [EOS_Mixing] set_compute_mode()\ntable_name() de chaque composant"];
       m2 [fillcolor="#eef3fa", label="4. [EOS_Mixing] set_mixing_reference_state()"];
       v1 [fillcolor="#fff6d6", label="5. [incondensables] set_reference_state(href, 0, Tref, pref)"];
       c3 [fillcolor="#e8f5e2", label="6. [client] melange.compute(entrees, sorties, err)"];
       m3 [fillcolor="#eef3fa", label="7. [EOS_Mixing] compute_perfect_gas()\ncontrôle des champs d'entrée"];
       m4 [fillcolor="#eef3fa", label="8. [EOS_Mixing] compute_pv_hv_ph() : Newton sur (pv, hv)"];
       v2 [fillcolor="#fff6d6", label="9. [composants] vapeur : compute(pv, hv)\nincondensables : compute_cp_pT, compute_h_pT,\ncompute_lambda_pT, compute_mu_pT"];
       m5 [fillcolor="#eef3fa", label="10. [EOS_Mixing] lois de mélange, dérivées,\nrecopie dans les champs de sortie"];

       c1 -> c2 -> m1 -> m2 -> v1 -> c3 -> m3 -> m4 -> v2 -> m5;
       v2 -> m4 [style=dashed, label="jusqu'à convergence"];
   }

Les étapes 3 à 5 n'ont lieu qu'une fois. L'étape 5 n'est exécutée que si le
fluide principal est un ``EOS_Cathare2``. Les étapes 8 et 9 sont répétées
point par point jusqu'à convergence ; c'est là que part l'essentiel du temps
de calcul, chaque itération coûtant un appel à la loi d'état de la vapeur.

On peut aussi remplacer un composant après coup par
``set_component(i, fluide)`` ; le mode de calcul est alors réévalué s'il était
``Unsupported``, ou invalidé si l'on introduit un gaz non parfait dans un
mélange ``WithPerfectGas``.

.. warning::

   La façade :cpp:class:`NEPTUNE::EOS` déclare des surcharges scalaires
   ``compute_T_ph(p, h, T, c_0, c_1, …)``, ``compute_rho_pT(…, c_0, …)``,
   etc. ``EOS_Mixing`` ne les surcharge pas : elles aboutissent à
   l'implémentation par défaut d'``EOS_Fluid``, qui écrit un message sur
   ``cerr`` et renvoie ``NOT_IMPLEMENTED``. Pour un mélange, seule
   l'interface par champs ``compute(EOS_Fields, EOS_Fields, EOS_Error_Field)``
   (ou sa variante à un seul champ de sortie) est opérationnelle ; pour un
   point unique, on utilise des champs de taille 1. De même, les méthodes
   ``set_alpha`` / ``get_alpha`` stockent un tableau de fractions volumiques
   qui n'intervient dans aucun calcul.

En Python
~~~~~~~~~

Le module ``eos_py`` fournit une classe dédiée, ``EOS_Mixing_py``, qui
construit les composants, les assemble et prend en charge la conversion des
listes Python en ``EOS_Fields`` (voir :ref:`api-python`). Le constructeur
reçoit deux listes de même longueur, les méthodes et les équations, toujours
avec le fluide principal en tête :

.. code-block:: python

   import eos_py

   mix = eos_py.EOS_Mixing_py(
       ["EOS_Cathare2", "EOS_Cathare2"],
       ["WaterVapor", "AirIncondensable"],
   )
   print(mix.describe())

.. code-block:: text

   ** eos mixing
      * object     : EOS_Mixing_py
      * fluid      : Unknown
      * table      : EOS_Mixing
      * version    : Unknown
      * ncomp      : 2
      * components :
         - [0] fluid=Water, table=Cathare2, version=v25_3_mod10.4
         - [1] fluid=Air, table=CathareIncondensableGas, version=V1

Ici les deux composants viennent de CATHARE2 : le mélange fonctionne en mode
``Cathare2``. La méthode ``compute`` prend les noms des champs d'entrée, leurs
valeurs (une liste par champ) et les noms des sorties ; elle renvoie une liste
de lignes, une par point, chaque ligne contenant les sorties dans l'ordre
demandé.

.. code-block:: python

   resultats = mix.compute(
       ["p", "T", "c_0", "c_1"],
       [[1.0e5, 2.0e5], [393.15, 393.15], [0.5, 0.5], [0.5, 0.5]],
       ["h", "rho", "cp"],
   )
   for ligne in resultats:
       print(["%.6e" % v for v in ligne])

.. code-block:: text

   ['2.723490e+06', '6.751019e-01', '1.358755e+03']
   ['2.720657e+06', '1.356328e+00', '1.305022e+03']

À 120 °C, avec autant d'air que de vapeur en masse, doubler la pression
double pratiquement la masse volumique, comme attendu d'un mélange proche du
gaz parfait. Contrairement à l'API C++, ``EOS_Mixing_py.compute`` lève une
``RuntimeError`` dès que le code de retour n'est pas ``good`` ; d'autres
exemples, dont l'effet de la composition sur :math:`T(p,h)`, figurent dans
:ref:`python-mixing`.

Propriétés disponibles
----------------------

Les noms suivent la nomenclature générale (:doc:`../usage/properties`),
complétée par les propriétés propres aux mélanges, définies dans
``camix_properties.hxx`` et ``c2iap_properties.hxx``. L'indice 0 désigne la
vapeur seule, les indices 1 à 5 les incondensables. Le tableau donne ce que
sait calculer le mode ``WithPerfectGas`` dans le plan :math:`(p,h)` ; le mode
``Cathare2`` offre un ensemble équivalent, le mode ``Cathare`` un
sous-ensemble (voir plus haut).

.. list-table:: Propriétés calculées par ``EOS_Mixing`` (mode générique)
   :header-rows: 1
   :widths: 34 66

   * - Noms
     - Signification
   * - ``T``, ``rho``, ``cp``, ``lambda``, ``mu``, ``sigma``
     - Propriétés du mélange ; ``h`` est en outre disponible en sortie dans le
       plan :math:`(p,T)`
   * - ``d_X_d_p_h``, ``d_X_d_h_p``
     - Dérivées des précédentes par rapport à :math:`p` et :math:`h`
   * - ``d_X_d_c_j_ph`` (j = 1 … 5)
     - Dérivées par rapport à la fraction massique de l'incondensable
       :math:`j`
   * - ``p_0``, ``h_0``, ``rho_0``, ``cp_0``, ``lambda_0``, ``mu_0``
     - Pression partielle, enthalpie et propriétés de la vapeur seule
   * - ``d_p_0_d_p_h``, ``d_h_0_d_h_p``, ``d_rho_0_d_p_0_h``, …
     - Dérivées de l'état de la vapeur par rapport à l'état du mélange, et
       dérivées des propriétés de la vapeur par rapport à
       :math:`(p_v, h_v)`
   * - ``p_1`` … ``p_5``, ``h_1`` … ``h_5``
     - Pression partielle et enthalpie de chaque incondensable
   * - ``prgr``, ``xnc``, ``mnc``, ``rnc``
     - Constante spécifique du mélange :math:`r_g`, fraction massique totale
       d'incondensables, masse molaire et constante spécifique moyennes des
       incondensables
   * - ``dncv``, ``d_dncv_d_p_h``, ``d_dncv_d_h_p``, ``d_dncv_d_c_j_ph``
     - Coefficient de diffusion incondensables/vapeur et ses dérivées
   * - ``T_sat``, ``h_l_sat``, ``h_v_sat``, ``rho_l_sat``, … et
       ``d_T_sat_d_p``, …
     - Saturation du fluide principal à la pression **totale**
   * - ``T_sat_0``, ``h_l_sat_0``, … et ``d_T_sat_0_d_p_0_h``, …
     - Saturation à la pression **partielle** de vapeur

Une propriété demandée qui ne figure pas dans cette liste est ignorée par le
mode générique : le champ de sortie garde son contenu initial. Il est donc
recommandé d'initialiser les tableaux de sortie à une valeur reconnaissable
pendant la mise au point.

La description complète des membres de la classe est donnée dans la
:doc:`référence API <../api/index>` (section « Mélanges »).

Références
----------

.. [Dalton1802] J. Dalton, « Essay IV. On the expansion of elastic fluids by
   heat », dans *Experimental essays on the constitution of mixed gases…*,
   Memoirs of the Literary and Philosophical Society of Manchester, vol. 5,
   part 2, 1802.

.. [Cengel] Y. A. Çengel, M. A. Boles, *Thermodynamics: An Engineering
   Approach*, McGraw-Hill, chapitre « Gas mixtures ».

.. [Poling2001] B. E. Poling, J. M. Prausnitz, J. P. O'Connell, *The
   Properties of Gases and Liquids*, 5\ :sup:`e` édition, McGraw-Hill, 2001
   (chapitres 9 à 11 : viscosité, conductivité thermique et diffusion des
   mélanges de gaz).

.. [Wilke1950] C. R. Wilke, « A viscosity equation for gas mixtures »,
   *The Journal of Chemical Physics*, vol. 18, n° 4, p. 517-519, 1950.

.. [MasonSaxena1958] E. A. Mason, S. C. Saxena, « Approximate formula for the
   thermal conductivity of gas mixtures », *Physics of Fluids*, vol. 1, n° 5,
   p. 361-369, 1958.

.. [Wassiljewa1904] A. Wassiljewa, « Wärmeleitung in Gasgemischen »,
   *Physikalische Zeitschrift*, vol. 5, p. 737-742, 1904.

.. [Fuller1966] E. N. Fuller, P. D. Schettler, J. C. Giddings, « A new method
   for prediction of binary gas-phase diffusion coefficients », *Industrial &
   Engineering Chemistry*, vol. 58, n° 5, p. 18-27, 1966.

.. [Blanc1908] A. Blanc, « Recherches sur les mobilités des ions dans les
   gaz », *Journal de Physique Théorique et Appliquée*, vol. 7, p. 825-839,
   1908.

.. [Bestion1990] D. Bestion, « The physical closure laws in the CATHARE
   code », *Nuclear Engineering and Design*, vol. 124, n° 3, p. 229-245, 1990.

.. todo::

   Références à confirmer sur pièce : la pagination exacte de [Dalton1802]_
   (l'essai IV est généralement cité p. 595-602 du volume 5) et celle de
   [Blanc1908]_. L'édition de [Cengel]_ n'est volontairement pas précisée, la
   numérotation du chapitre variant d'une édition à l'autre.
