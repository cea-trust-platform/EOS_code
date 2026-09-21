Méthodes thermodynamiques
=========================

Chaque « méthode » est une implémentation de la classe abstraite
:cpp:class:`NEPTUNE::EOS_Fluid`, sélectionnée par le premier argument du
constructeur d'``EOS``. Le second argument (« équation fluide ») choisit le
fluide et la phase. Exemple : ``EOS("EOS_Cathare2", "WaterVapor")``.

.. list-table:: Synthèse des méthodes disponibles
   :header-rows: 1
   :widths: 18 30 52

   * - Méthode
     - Origine
     - Caractéristiques
   * - ``EOS_Cathare``
     - Corrélations du code CATHARE (CEA)
     - Eau et R12, phases liquide et vapeur ; corrélations analytiques
       rapides, domaine réacteur.
   * - ``EOS_Cathare2``
     - Corrélations CATHARE2 (CEA)
     - Nombreux fluides (voir ci-dessous), incondensables, mélanges ;
       méthode de référence pour les applications réacteur.
   * - ``EOS_Thetis``
     - Tables THETIS (EDF)
     - Eau liquide/vapeur par tables précalculées.
   * - ``EOS_Refprop9`` / ``EOS_Refprop10``
     - NIST REFPROP 9 / 10
     - Bibliothèque de référence NIST (Fortran, licence requise) ; grande
       précision, coût CPU élevé.
   * - ``EOS_CoolProp``
     - CoolProp (open source)
     - Alternative libre à REFPROP.
   * - ``EOS_PerfectGas``
     - Loi analytique
     - Gaz parfait paramétrable (Air, Hydrogen, …), état de référence
       ajustable (``set_reference_state``).
   * - ``EOS_StiffenedGas``
     - Loi analytique (Le Métayer et al., 2004)
     - Gaz raidi : liquide et vapeur compressibles, adapté aux schémas
       hyperboliques diphasiques.
   * - ``EOS_Flica4``
     - Code FLICA4 (CEA)
     - Corrélations du code de cœur FLICA4.
   * - ``EOS_Hitec``
     - Greffon externe
     - Sel fondu HITEC ; sources récupérées et patchées à la configuration
       (``HITEC_pre_configure.sh``).
   * - ``EOS_Nak``
     - Greffon externe
     - Alliage sodium-potassium (NaK) ; greffon comme HITEC
       (``NAK_pre_configure.sh``).
   * - ``EOS_Ipp``
     - Interpolateur EOS
     - Lecture de tables d'interpolation (fichiers MED) générées par
       :doc:`EOS_IGen <modules/eos_igen>` ; évaluation très rapide d'une
       méthode coûteuse.
   * - ``EOS_Mixing``
     - Mélange
     - Mélange d'un fluide principal (vapeur) et de gaz incondensables ;
       voir ci-dessous.

.. note::

   La disponibilité effective de chaque méthode dépend des options de
   compilation (``user_env.txt`` / options CMake) : REFPROP, CoolProp,
   THETIS, HITEC et NaK nécessitent des bibliothèques ou sources externes.

EOS_Cathare
-----------
Voir doc. Cathare

EOS_Cathare2
------------
Voir doc. Cathare2

EOS_PerfectGas
--------------

Gaz parfait : les données par fluide sont dans
``Modules/EOS/Data/EOS_PerfectGas``. L'état de référence
:math:`(h_0, s_0, T_0, p_0)` peut être fourni au constructeur ou via
``set_reference_state(h, s, T, p)``.

.. code-block:: c++

   EOS air("EOS_PerfectGas", "Air");
   EOS h2 ("EOS_PerfectGas", "Hydrogen");

EOS_StiffenedGas
----------------

Loi « gaz raidi » :math:`p = (\gamma - 1)\rho(e - q) - \gamma p_\infty`
(référence : O. Le Métayer, J. Massoni, R. Saurel, *Élaboration des lois
d'état d'un liquide et de sa vapeur pour les modèles d'écoulements
diphasiques*, 2004 — PDF dans ``Doc/Thermo_Models/EOS_StiffenedGas``).
Paramètres par fluide dans ``Modules/EOS/Data/EOS_StiffenedGas``.

EOS_Refprop9 / EOS_Refprop10 / EOS_CoolProp
-------------------------------------------

Couplages vers les bibliothèques externes NIST REFPROP (versions 9 et 10,
non redistribuées avec EOS) et CoolProp. Le code d'erreur natif de la
bibliothèque est conservé dans ``EOS_Internal_Error::get_library_code()``.
Chemins des bibliothèques à renseigner dans ``user_env.txt`` à la
configuration.

EOS_Ipp (interpolateur)
-----------------------

``EOS_Ipp`` évalue des tables d'interpolation (fichiers MED rangés dans le
sous-répertoire ``EOS_Ipp`` du répertoire de données) générées au préalable
par le module EOS_IGen à partir de n'importe quelle autre méthode. Intérêt :
remplacer une méthode coûteuse (REFPROP…) par une évaluation rapide à
précision contrôlée, en interpolation bilinéaire ou bicubique.

.. code-block:: c++

   EOS eau("EOS_Ipp", "<nom de la table>");
   eau.init_model("EOS_Refprop10", "WaterLiquid");  // secours hors table, facultatif

Génération, méthodes d'interpolation, réglages et exemple complet :
:doc:`modules/eos_igen`.

EOS_Mixing (mélanges)
---------------------

``EOS_Mixing`` (``Modules/EOS/Src/EOS_Mixing.hxx``) traite un mélange
constitué d'un **fluide principal** (typiquement la vapeur) et de **gaz
incondensables**, sous les hypothèses de Dalton :

.. code-block:: c++

   EOS vapeur("EOS_Cathare2", "WaterVapor");
   EOS azote ("EOS_Cathare2", "NitrogenIncondensable");

   EOS melange("EOS_Mixing");
   EOS* composants[2] = { &vapeur, &azote };
   melange.set_components(composants, 2);

La composition est décrite par les champs de fractions massiques
``c_0 … c_5`` ajoutés aux champs d'entrée de ``compute`` ; seule l'interface
par champs est opérationnelle pour un mélange. Selon la nature des
composants, la classe choisit l'un de ses trois algorithmes
(``compute_cathare_mix``, ``compute_cathare2_mix``, ``compute_perfect_gas``).
Modèles, équations, propriétés disponibles et références bibliographiques :
:doc:`modules/eos_mixing`.

Documentation de référence
--------------------------

Les notes techniques historiques (PDF) sont regroupées sous
``Doc/Thermo_Models/``
