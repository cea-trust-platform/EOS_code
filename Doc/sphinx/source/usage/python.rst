.. _api-python:

API Python
==========

.. highlight:: python

Pourquoi une API Python, et ce qu'elle n'est pas
------------------------------------------------

La bibliothèque EOS est faite pour être appelée des millions de fois depuis un
code de calcul C++ ou Fortran. Ce n'est pas le bon outil pour regarder une
courbe de saturation, vérifier un ordre de grandeur, comparer deux méthodes
sur un domaine ou préparer une table d'interpolation : pour ces tâches, un
script Python ou un notebook de dix lignes remplace avantageusement un
programme C++ à compiler. Le module ``eos_py`` a été écrit pour cela.

Il ne s'agit pas d'une traduction ligne à ligne de l'API C++. Exposer par SWIG
les quelque six cents méthodes ``compute_*`` de :cpp:class:`NEPTUNE::EOS`,
les :cpp:class:`NEPTUNE::EOS_Field` et les tableaux ``ArrOfDouble`` aurait
donné une interface pénible à utiliser depuis Python. Le parti pris est
inverse : trois petites classes C++ écrites pour l'occasion
(``Modules/EOS/PyAPI/EOS_py.hxx``) cachent toute la mécanique des champs et ne
manipulent que des chaînes de caractères et des listes de flottants.

.. list-table:: Contenu du module ``eos_py``
   :header-rows: 1
   :widths: 28 72

   * - Nom
     - Rôle
   * - ``EOS_py``
     - Un fluide : propriétés dans un plan thermodynamique, saturation, bornes
       et point critique. Sert aussi à relire une table d'interpolation
       (méthode ``"EOS_Ipp"``).
   * - ``EOS_IGen_py``
     - Génération d'une table d'interpolation (:ref:`eos-igen`).
   * - ``EOS_Mixing_py``
     - Mélange vapeur et gaz incondensables (:ref:`eos-mixing`).
   * - ``generate_mesh_igen``, ``IGenMesh``
     - Raccourci en Python pur pour générer une table en un appel, et la
       fiche descriptive qu'il renvoie.

Deux conséquences pratiques découlent de ce choix. Les classes C++ de l'API
(``EOS``, ``EOS_Field``, ``EOS_Error``…) ne sont **pas** visibles depuis
Python ; et il n'y a jamais de code de retour à tester : une méthode renvoie
directement la valeur demandée, et tout problème se manifeste par une
exception.

.. note::

   Les fichiers ``Modules/Language/PyAPI/Language.i`` et
   ``Modules/Functions/PyAPI/Functions.i`` décrivent deux autres modules SWIG
   (``Language`` et ``Functions``) datant de 2015-2016. Aucun
   ``CMakeLists.txt`` ne les construit plus et rien n'est installé : ils ne
   font pas partie de l'API disponible. Leurs scripts ``test.py`` restent
   lisibles comme témoignage de l'interface d'origine (``AString``,
   ``Strings``, ``AFunction``…).

Installation et environnement
-----------------------------

Compilation
~~~~~~~~~~~

L'API Python est désactivée par défaut. On l'active à la configuration, en
même temps que l'interpolateur dont elle dépend (``EOS_py.cxx`` inclut
``EOS_Ipp`` et ``EOS_IGen`` sans condition) :

.. code-block:: bash

   ./configure --prefix=$PWD/install \
               --with-python-api --with-interpolator \
               ...                       # greffons : --with-cathare2=..., etc.
   cd build && make -j8 && make install

``--with-python-api`` positionne l'option CMake ``WITH_PYTHON_API=ON``, qui
exige SWIG et les en-têtes de développement de Python 3
(``find_package(SWIG REQUIRED)``,
``find_package(Python3 COMPONENTS Interpreter Development REQUIRED)``). Si
plusieurs Python cohabitent sur la machine, ``--with-python-exec``,
``--with-python-lib``, ``--with-python-include`` et ``--with-swig-exec``
permettent de désigner les bons. NumPy n'est pas nécessaire à la
compilation ni à l'exécution : l'interface ne contient aucun *typemap* NumPy.
Les exemples de cette page s'en servent néanmoins, ainsi que de matplotlib,
par commodité. Voir :doc:`../installation` pour le reste de la procédure.

L'étape ``make install`` est indispensable. Elle dépose, tous au même
endroit, ``<prefix>/lib/eos_py.py``, ``<prefix>/lib/_eos_py.so`` et les
bibliothèques partagées dont ils dépendent ; le chemin de recherche de
celles-ci est inscrit dans ``_eos_py.so``, si bien que ``LD_LIBRARY_PATH``
n'a pas à être modifié tant que l'installation reste à son préfixe.

Variables d'environnement
~~~~~~~~~~~~~~~~~~~~~~~~~

Une seule variable est nécessaire à l'import :

.. code-block:: bash

   export PYTHONPATH=<prefix>/lib:$PYTHONPATH
   python3 -c "import eos_py; print(eos_py.__file__)"

La seconde concerne les données. EOS cherche son répertoire de données dans
``NEPTUNE_EOS_DATA``, puis dans ``USER_EOS_DATA``, puis à défaut dans
``<prefix>/data``. Pour calculer les propriétés d'un fluide avec une méthode
ordinaire, le défaut convient. Pour l'interpolateur en revanche, il faut
maîtriser cet emplacement, car c'est là que les tables sont écrites
(``EOS_Ipp/<nom>.med``) puis retrouvées (fichier ``index.eos``) ; le
répertoire doit exister et être inscriptible.

.. code-block:: bash

   export USER_EOS_DATA=<prefix>/data      # ou une copie de travail

Dans un notebook, on obtient le même résultat sans dépendre du shell qui a
lancé Jupyter, à condition de le faire **avant** l'import :

.. code-block:: python

   import os, sys
   sys.path.insert(0, "/chemin/vers/install/lib")
   os.environ.setdefault("USER_EOS_DATA", "/chemin/vers/donnees")
   import eos_py

Comment un appel traverse les couches
-------------------------------------

Avant les exemples, il est utile de savoir ce qui se passe entre la ligne
Python et le calcul thermodynamique. La figure
:ref:`ci-dessous <fig-python-classes>` montre les classes en présence.

.. _fig-python-classes:

.. graphviz::
   :caption: Diagramme de classes de l'API Python. Les trois classes ``*_py``
             sont écrites en C++ dans ``EOS_py.hxx`` ; SWIG en génère le
             reflet Python dans ``eos_py.py``. Chacune possède (composition)
             les objets de la bibliothèque qu'elle pilote.

   digraph python_classes {
       rankdir=LR;
       node [shape=record, fontname="Helvetica", fontsize=10,
             style=filled, fillcolor="#eef3fa"];
       edge [fontname="Helvetica", fontsize=9];

       subgraph cluster_py { label="Module Python eos_py"; color="#bbbbbb";
           Mesh [fillcolor="#e8f5e2", label="{IGenMesh (dataclass)|file_name, method, reference\lp_min, p_max, T_min, T_max\lh_min, h_max, nx, ny, level_max\l|+ open_interpolator() : EOS_py\l}"];
           Gen  [fillcolor="#e8f5e2", label="{generate_mesh_igen(...)|fonction Python pure\l}"];
       }
       subgraph cluster_wrap { label="Classes enveloppes (EOS_py.hxx)"; color="#bbbbbb";
           Epy [label="{EOS_py|- eos_ : NEPTUNE::EOS*\l|+ compute(n1, n2, v1, v2, sorties)\l+ compute_sat(n, v, sorties)\l+ describe()\l+ get_p_crit() ... get_T_max()\l+ set_interpolation_method(mode)\l+ get_interpolation_method()\l+ has_bicubic_first_derivative_data(prop)\l+ has_bicubic_cross_derivative_data(prop)\l+ fluids_available() (static)\l}"];
           Ipy [label="{EOS_IGen_py|+ set_extremum(pmin, pmax, Tmin, Tmax)\l+ set_list_properties(props)\l+ make_mesh(np, nh, level_max=-1)\l+ set_quality(prop, type, is_abs, limite)\l+ make_global_refine()\l+ make_local_refine(cont=true)\l+ write_med(nom)\l+ describe()\l}"];
           Mpy [label="{EOS_Mixing_py|- mixing_ : NEPTUNE::EOS*\l- components_ : vector\<EOS*\>\l|+ compute(noms_entree, valeurs, noms_sortie)\l+ describe()\l}"];
       }
       subgraph cluster_cpp { label="Bibliothèque EOS"; color="#bbbbbb";
           EOS  [label="{NEPTUNE::EOS|+ compute(EOS_Field, EOS_Field, EOS_Fields, EOS_Error_Field)\l+ compute(EOS_Fields, EOS_Fields, EOS_Error_Field)\l+ set_components(EOS**, int)\l}"];
           IGen [label="{NEPTUNE_EOS_IGEN::EOS_IGen|+ make_mesh(...)\l+ set_quality(...)\l+ write_med(...)\l}"];
       }
       Gen -> Ipy  [style=dashed, label="utilise"];
       Gen -> Mesh [style=dashed, label="renvoie"];
       Mesh -> Epy [style=dashed, label="crée"];
       Epy -> EOS  [arrowtail=diamond, dir=both, arrowhead=vee, label="1"];
       Mpy -> EOS  [arrowtail=diamond, dir=both, arrowhead=vee, label="1 + N"];
       Ipy -> IGen [arrowtail=diamond, dir=both, arrowhead=vee, label="1"];
   }

Le déroulement d'un appel ``compute`` est détaillé par le diagramme de
séquence :ref:`suivant <fig-python-sequence>`.

.. _fig-python-sequence:

.. graphviz::
   :caption: Séquence d'un appel ``fluide.compute("p", "h", P, H, ["T", "rho"])``
             de Python jusqu'à la méthode thermodynamique, et retour.

   digraph python_sequence {
       rankdir=TB;
       node [shape=box, fontname="Helvetica", fontsize=10,
             style=filled, fillcolor="#eef3fa"];
       edge [fontname="Helvetica", fontsize=9];

       a [label="Script Python\nfluide.compute(\"p\", \"h\", P, H, [\"T\", \"rho\"])"];
       b [label="eos_py.py puis _eos_py.so (code généré par SWIG)\nlistes Python converties en std::vector (copie)"];
       c [label="EOS_py::compute (EOS_py.cxx)\ncontrôle des tailles ; construit 2 EOS_Field d'entrée,\nun EOS_Fields de sortie, un EOS_Error_Field"];
       d [label="NEPTUNE::EOS::compute(in1, in2, sorties, erreurs)"];
       e [label="EOS_Fluid concret (EOS_Cathare2, EOS_Refprop9, EOS_Ipp...)\ncalcul point par point"];
       f [shape=diamond, fillcolor="#fff6d6", label="pire code == good ?"];
       g [label="std::vector<std::vector<double>>\nrésultat[point][propriété]"];
       h [fillcolor="#fde3e3", label="throw std::runtime_error\n\"compute: bad computation, worst error = N\""];
       i [label="Python : tuple de tuples"];
       j [fillcolor="#fde3e3", label="Python : RuntimeError\n(bloc %exception de EOS_py.i)"];

       a -> b -> c -> d -> e;
       e -> f [label="EOS_Error"];
       f -> g [label="oui"];
       f -> h [label="non"];
       g -> i;
       h -> j;
   }

On retiendra de ce schéma trois propriétés de l'interface. Les données sont
recopiées à l'entrée et à la sortie, ce qui est sans importance pour quelques
milliers de points mais rend l'API impropre à un usage intensif. Le calcul
lui-même est un appel par champ, donc vectorisé côté C++ : il est bien plus
efficace de passer mille points en une fois que d'appeler mille fois
``compute``. Enfin, la gestion d'erreur est en tout ou rien, comme on le verra
plus bas.

Créer un fluide
---------------

Un objet ``EOS_py`` se construit comme un objet ``EOS`` en C++, avec le nom de
la méthode et celui de l'équation fluide (voir :doc:`../models` pour la
liste). ``describe()`` en donne la fiche d'identité.

.. code-block:: python

   import eos_py

   fluide = eos_py.EOS_py("EOS_Cathare2", "WaterLiquid")
   print(fluide.describe())

.. code-block:: text

   Model table      : Cathare2
   Version           : v25_3_mod10.4
   Fluid             : Water
   Equation of fluid : WaterLiquid
   p_min : 0
   p_max : 2.6e+07
   T_min : 269.15
   T_max : 673.15
   h_min : 100
   h_max : 2.4e+06

Le préfixe ``EOS_`` est facultatif : ``eos_py.EOS_py("Cathare2",
"WaterLiquid")`` et ``eos_py.EOS_py("Refprop9", "WaterVapor")`` sont acceptés.
Pour connaître les méthodes compilées dans la bibliothèque,
``eos_py.EOS_py.fluids_available()`` imprime la table des classes
enregistrées.

Les bornes du domaine et le point critique sont accessibles par des
accesseurs qui renvoient un flottant :

.. code-block:: python

   print(fluide.get_p_crit(), fluide.get_T_crit(), fluide.get_h_crit())
   print(fluide.get_p_min(), fluide.get_p_max(),
         fluide.get_T_min(), fluide.get_T_max(),
         fluide.get_h_min(), fluide.get_h_max())
   print(fluide.get_mm())

.. code-block:: text

   22120000.0 647.8440603543174 1991149.555839671
   0.0 26000000.0 269.15 673.15 100.0 2400000.0
   0.0180153

Calculer une propriété en un point
----------------------------------

Il n'existe pas de méthode dédiée au calcul ponctuel : on appelle ``compute``
avec des listes à un élément. La signature mérite qu'on s'y arrête, car
l'ordre des arguments n'est pas celui que l'on devinerait : les **deux noms**
des grandeurs d'entrée d'abord, puis les **deux listes** de valeurs, puis la
liste des propriétés voulues.

.. code-block:: python

   res = fluide.compute("p", "h", [1.0e5], [4.0e5], ["T", "rho"])
   print(res)
   T, rho = res[0]

.. code-block:: text

   ((368.65934182657986, 962.0585026831999),)

À 1 bar et 400 kJ/kg, l'eau liquide est à 368,66 K et pèse 962 kg/m³. Tout
est en unités SI : pression en Pa, enthalpie en J/kg, température en K. Les
noms de propriétés sont ceux de la nomenclature générale
(:doc:`properties`) ; la température d'entrée s'écrit indifféremment ``"T"``
ou ``"t"``.

Calculer sur des tableaux
-------------------------

Le résultat de ``compute`` est un tuple de tuples indexé
``res[point][propriété]`` : une ligne par point, les colonnes dans l'ordre de
la liste des sorties. ``numpy.array`` le convertit directement en tableau de
forme ``(n_points, n_propriétés)``.

.. code-block:: python

   import numpy as np

   p = [1.2e7, 1.5e7, 1.8e7]
   T = [350.0, 400.0, 450.0]
   res = np.array(fluide.compute("p", "T", p, T, ["h", "rho"]))
   print(res.shape)
   print(res[:, 0])      # h   [J/kg]
   print(res[:, 1])      # rho [kg/m3]

.. code-block:: text

   (3, 2)
   [331069.95193975 543805.62248093 760554.80971237]
   [979.3374629  945.10495941 900.50660409]

Cet exemple travaille dans le plan :math:`(p,T)` et demande l'enthalpie ; dans
le plan :math:`(p,h)` on demanderait la température. Les deux listes d'entrée
doivent avoir la même longueur. En entrée, toute séquence de flottants
convient ; un tableau NumPy à une dimension est accepté, mais l'usage
constant des notebooks est de convertir explicitement par ``list(a)`` ou
``a.tolist()``, ce qui évite toute surprise sur les types.

Pour balayer un domaine à deux dimensions, on aplatit une grille puis on
remet le résultat en forme :

.. code-block:: python

   p_axe = np.linspace(1.0e7, 2.0e7, 60)
   h_axe = np.linspace(2.0e5, 9.0e5, 60)
   pp, hh = np.meshgrid(p_axe, h_axe)

   res = np.array(fluide.compute("p", "h", list(pp.ravel()), list(hh.ravel()),
                                 ["T", "rho"]))
   T_carte   = res[:, 0].reshape(pp.shape)
   rho_carte = res[:, 1].reshape(pp.shape)

Les 3600 points sont calculés en un seul appel C++. On peut demander autant de
propriétés que l'on veut, dérivées comprises ; le script de test
``Modules/EOS/Tests/Python/test_eos_py.py`` en demande dix-neuf à la fois
(``"beta"``, ``"cp"``, ``"d_rho_d_p_h"``, ``"d_T_d_h_p"``, ``"mu"``…) et
affiche le tout sous forme de table.

Saturation
----------

Les propriétés de saturation ne dépendent que d'une variable ; elles passent
par ``compute_sat``, qui prend le nom de la grandeur d'entrée, la liste de ses
valeurs et les sorties voulues.

.. code-block:: python

   p_sat = [1.0e6, 3.0e6, 5.0e6, 8.0e6, 1.0e7]
   res = np.array(fluide.compute_sat(
       "p", p_sat, ["T_sat", "rho_lsat", "rho_vsat", "h_lsat", "h_vsat"]))
   for p, ligne in zip(p_sat, res):
       print("%.1e  %8.3f  %8.3f  %7.3f  %.4e  %.4e" % (p, *ligne))

.. code-block:: text

   1.0e+06   452.968   886.535    5.120  7.6369e+05  2.7795e+06
   3.0e+06   506.988   821.138   15.014  1.0094e+06  2.8029e+06
   5.0e+06   537.116   776.991   25.408  1.1550e+06  2.7931e+06
   8.0e+06   568.148   722.509   42.605  1.3168e+06  2.7574e+06
   1.0e+07   584.094   689.096   55.532  1.4076e+06  2.7249e+06

On lit par exemple qu'à 10 bar l'eau bout à 452,97 K (179,8 °C). Les noms
canoniques (``"T_sat"``, ``"h_lsat"``…) ont des synonymes courts hérités de
CATHARE, que l'on rencontre dans les scripts de test : ``"tsat"``,
``"hlsat"``, ``"rhovsat"``, ainsi que les dérivées le long de la courbe de
saturation ``"dtsatdp"``, ``"dhlsatdp"``, ``"d2tsatdpdp"``… La casse compte :
avec CATHARE2, ``"Tsat"`` n'est pas reconnu et l'appel rend 0 sans lever
d'erreur, alors que ``"tsat"`` et ``"T_sat"`` donnent 452,97 K. Mieux vaut
s'en tenir aux noms canoniques de :doc:`properties`.

Gestion des erreurs
-------------------

L'API C++ renvoie des codes (:doc:`errors`) ; l'API Python lève des
exceptions. Le fichier d'interface contient un bloc ``%exception`` global qui
transforme toute ``std::exception`` C++ en ``RuntimeError`` Python, et les
classes enveloppes lèvent elles-mêmes une ``std::runtime_error`` dans deux
situations : arguments incohérents, ou calcul dont le pire code d'erreur
n'est pas ``good``.

.. code-block:: python

   try:
       fluide.compute("p", "h", [1.0e9], [4.0e5], ["T"])    # p hors domaine
   except RuntimeError as e:
       print("Erreur EOS :", e)

.. code-block:: text

   Erreur EOS : compute: bad computation, worst error = 2

Le nombre est la valeur de ``EOS_Error`` : 1 pour ``ok``, 2 pour ``bad``,
3 pour ``error``. Deux particularités sont à connaître. La première est que
le seuil est strict : un résultat simplement ``ok`` (hors du domaine de
validité mais calculé) suffit à lever l'exception. La seconde est que le
traitement est global : un seul point fautif dans un lot de dix mille fait
échouer tout l'appel, et le champ d'erreurs par point n'est pas accessible
depuis Python. Quand on balaye un domaine dont on ne connaît pas bien les
limites, il vaut donc mieux le borner d'abord avec ``get_p_min()`` …
``get_h_max()``, ou découper le calcul en lots pour isoler les points
fautifs.

Les erreurs d'usage produisent des messages explicites :

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - Appel
     - Message de la ``RuntimeError``
   * - ``compute("p", "h", [1e5, 2e5], [4e5], ["T"])``
     - ``compute: tab_P and tab_H must have same size``
   * - ``compute("p", "h", [], [], ["T"])``
     - ``compute: empty input arrays``
   * - ``compute("p", "h", [1e5], [4e5], [])``
     - ``compute: no output requested``
   * - ``EOS_py("EOS_Cathare2", "Bidon")``
     - ``this type is not a registered type`` (précédé, sur la sortie
       standard, de la table des types connus)
   * - ``fluide.set_interpolation_method("bicubic")`` sur un fluide ordinaire
     - ``set_interpolation_method: this EOS object does not wrap an EOS_Ipp
       interpolator``

.. warning::

   Certaines erreurs ne sont pas rattrapables, parce que la bibliothèque C++
   y répond par ``exit()`` ou par une assertion, ce qui termine
   l'interpréteur Python (et le noyau Jupyter) sans passer par le mécanisme
   d'exception :

   * les accesseurs que la méthode n'implémente pas — ``get_rho_crit()``,
     ``get_rho_min()``, ``get_rho_max()`` et ``get_p()`` avec
     ``EOS_Cathare2`` ;
   * ``compute("p", "T", …)`` sur un interpolateur, qui n'accepte que le plan
     ``("p", "h")`` ;
   * la demande, à un interpolateur, d'une propriété absente de sa table ;
   * ``write_med`` quand le répertoire de données est invalide.

   Par ailleurs la bibliothèque écrit directement sur ``stdout`` et
   ``stderr`` (journal du raffinement, messages d'avertissement) ; dans un
   notebook, ces lignes s'entrelacent avec les ``print`` Python.

.. _python-interpolateur:

Utiliser l'interpolateur
------------------------

Le principe et les réglages de l'interpolateur sont décrits dans
:ref:`eos-igen` ; on ne montre ici que la manière de le piloter depuis
Python. Le travail se fait en deux temps : on génère une table à partir d'une
méthode de référence, ce qui écrit un fichier ``.med`` dans le répertoire de
données, puis on ouvre cette table comme n'importe quel fluide.

Générer une table en un appel
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``generate_mesh_igen`` enchaîne les étapes pour le cas le plus courant, un
maillage régulier sur un domaine donné en pression et en température :

.. code-block:: python

   mesh = eos_py.generate_mesh_igen(
       method="EOS_Cathare2", reference="WaterLiquid",
       x_min=1.0e7, x_max=2.0e7,        # pression [Pa]
       y_min=300.0, y_max=500.0,        # température [K]
       nx=20, ny=20,
       properties=["T", "d_T_d_p_h", "d_T_d_h_p", "d2_T_d_p_d_h"],
       file_name="notebook_demo_mesh_20x20",
   )
   print(mesh)

.. code-block:: text

   IGenMesh(file_name='notebook_demo_mesh_20x20', method='EOS_Cathare2',
   reference='WaterLiquid', p_min=10000000.0, p_max=20000000.0, T_min=300.0,
   T_max=500.0, h_min=121830.74206843067, h_max=982172.7709777451, nx=20,
   ny=20, level_max=-1, properties=['T', 'd_T_d_p_h', 'd_T_d_h_p',
   'd2_T_d_p_d_h'])

La fiche renvoyée rappelle les paramètres et donne les bornes en enthalpie du
maillage, qui ne sont connues qu'après la génération puisque le domaine est
spécifié en température. La liste ``properties`` fixe ce que contiendra la
table. Pour que l'interpolation bicubique soit possible sur une propriété, il
faut stocker avec elle ses deux dérivées premières, et de préférence sa
dérivée croisée : d'où les quatre noms ci-dessus pour la température. Si
``properties`` est omis, toutes les propriétés que la méthode sait calculer
sont tabulées. La fonction refuse (``NotImplementedError``) toute autre
paramétrisation que ``x_variable="P"``, ``y_variable="T"`` et des échelles
linéaires : le générateur ne sait produire que des maillages à pas constant.

Avec CATHARE2, la génération d'une table 100 × 100 prend quelques centièmes de
seconde. Avec REFPROP, chaque nœud coûte de l'ordre de 50 ms et un grand
maillage raffiné se compte en dizaines de minutes : c'est précisément ce coût
que la table permet ensuite d'éviter.

Générer une table pas à pas, avec raffinement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pour piloter le raffinement adaptatif, on utilise directement
``EOS_IGen_py``. L'exemple suivant part d'un maillage 5 × 5, autorise trois
niveaux de raffinement et demande que l'erreur relative sur la température,
contrôlée au centre des mailles, reste sous :math:`1{,}2\cdot10^{-4}`.

.. code-block:: python

   igen = eos_py.EOS_IGen_py("EOS_Refprop9", "WaterLiquid")
   igen.set_extremum(1.0e7, 2.0e7, 300.0, 500.0)     # pmin, pmax, Tmin, Tmax
   igen.make_mesh(5, 5, level_max=3)
   igen.set_list_properties(["T", "d_T_d_p_h", "d_T_d_h_p", "d2_T_d_p_d_h"])
   igen.set_quality("T", "centre", 0, 0.00012)       # 0 : seuil relatif
   igen.make_local_refine()                          # cont=True par défaut
   igen.write_med("demo_mesh_raffinement_local")
   print(igen.describe())

.. code-block:: text

   ** eos igen (mesh generator) **
      * method    : EOS_Refprop9
      * reference : WaterLiquid
      * domain    : p in [1e+07, 2e+07] Pa, T in [300, 500] K
      * mesh      : built

Pendant le raffinement, la bibliothèque journalise sa progression sur la
sortie standard ; sur cet exemple le maillage passe de 25 à 72 puis 137
nœuds, contre 25, 81 puis 289 avec ``make_global_refine()`` qui découpe
uniformément toutes les mailles. Sans ``level_max`` (valeur −1), aucun
raffinement n'est possible et le maillage reste régulier. L'argument ``cont``
de ``make_local_refine`` commande l'ajout des nœuds de continuité entre
mailles de niveaux différents ; il n'y a pas de raison de le désactiver en
usage normal. L'enchaînement imposé est ``set_extremum``, ``make_mesh``, puis
éventuellement ``set_quality`` et un raffinement, enfin ``write_med`` : les
classes le vérifient et lèvent par exemple ``EOS_IGen_py::make_mesh: call
set_extremum() first``.

Relire une table et choisir la méthode d'interpolation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Une table s'ouvre par son nom avec la méthode ``"EOS_Ipp"`` ; à partir d'une
fiche ``IGenMesh``, ``mesh.open_interpolator()`` fait la même chose.

.. code-block:: python

   ipp = eos_py.EOS_py("EOS_Ipp", "notebook_demo_mesh_20x20")
   # équivalent : ipp = mesh.open_interpolator()

   print("méthode par défaut :", ipp.get_interpolation_method())
   print("dérivées premières :", ipp.has_bicubic_first_derivative_data("T"))
   print("dérivée croisée    :", ipp.has_bicubic_cross_derivative_data("T"))
   print("domaine            :", ipp.get_p_min(), ipp.get_p_max(),
                                 ipp.get_h_min(), ipp.get_h_max())

.. code-block:: text

   méthode par défaut : bicubic
   dérivées premières : True
   dérivée croisée    : True
   domaine            : 10000000.0 20000000.0 121830.74206843067 982172.7709777451

Les bornes renvoyées sont celles de la table, non celles de la méthode
d'origine : ce sont elles qu'il faut respecter. On compare ensuite les deux
méthodes d'interpolation à la référence en un point intérieur :

.. code-block:: python

   P, H = [1.5e7], [5.0e5]
   ipp.set_interpolation_method("bilinear")
   print("bilinéaire :", ipp.compute("p", "h", P, H, ["T"])[0][0])
   ipp.set_interpolation_method("bicubic")
   print("bicubique  :", ipp.compute("p", "h", P, H, ["T"])[0][0])
   print("référence  :", fluide.compute("p", "h", P, H, ["T"])[0][0])

.. code-block:: text

   bilinéaire : 389.64048489388836
   bicubique  : 389.6446563028633
   référence  : 389.64465525233265

Sur ce maillage 20 × 20, l'interpolation bilinéaire se trompe de 4 mK et la
bicubique de 1 µK. Le notebook ``eos_py_demo_interpolateur`` généralise la
comparaison à 400 points et trois maillages construits sur REFPROP 9 :

.. list-table:: Erreur sur T (K) selon le maillage et la méthode
   :header-rows: 1
   :widths: 34 22 22 22

   * - Maillage
     - Méthode
     - Écart quadratique moyen
     - Écart maximal
   * - régulier 8 × 8
     - bilinéaire
     - 3,5e-02
     - 5,6e-02
   * -
     - bicubique
     - 5,4e-05
     - 9,1e-05
   * - raffinement global (5 × 5, 2 niveaux)
     - bilinéaire
     - 4,0e-03
     - 9,3e-03
   * -
     - bicubique
     - 4,1e-07
     - 1,0e-06
   * - raffinement local (5 × 5, 3 niveaux)
     - bilinéaire
     - 1,1e-02
     - 3,7e-02
   * -
     - bicubique
     - 5,3e-03
     - 3,7e-02

À maillage égal, le bicubique gagne deux à quatre ordres de grandeur. Le
dernier cas fait exception : le raffinement local y est piloté par un critère
de qualité évalué en bilinéaire, et l'erreur maximale est dominée par les
mailles voisines des nœuds pendants, sur lesquelles le bicubique n'apporte
rien.

Un interpolateur a trois restrictions par rapport à un fluide ordinaire : il
ne connaît que le plan ``("p", "h")``, il ne sait rendre que les propriétés
stockées dans sa table, et l'enthalpie n'en fait pas partie puisqu'elle est
une coordonnée du maillage. Pour :math:`h(p,T)`, on interroge la méthode
d'origine. Hors du domaine de la table, ``compute`` lève la ``RuntimeError``
habituelle (``worst error = 2``).

.. note::

   Les notebooks ont été enregistrés à une époque où la méthode par défaut
   était ``bilinear`` ; elle est aujourd'hui ``bicubic`` dès que la table
   contient les dérivées nécessaires, avec repli automatique sur le
   bilinéaire sinon. Les sorties reproduites ici sont celles du code actuel.

.. _python-mixing:

Utiliser EOS_Mixing
-------------------

``EOS_Mixing_py`` compose un mélange à partir de deux listes parallèles, les
méthodes et les équations fluides, le fluide principal en premier ; le modèle
physique est décrit dans :ref:`eos-mixing`.

.. code-block:: python

   mix = eos_py.EOS_Mixing_py(
       ["EOS_Cathare2", "EOS_Cathare2"],
       ["WaterVapor", "AirIncondensable"],
   )

L'interface de calcul diffère de celle d'``EOS_py``, parce que le nombre de
grandeurs d'entrée dépend du nombre de composants : on passe la liste des
noms d'entrée, une liste de valeurs **par grandeur**, et les sorties. Les
fractions massiques ``c_0`` (vapeur), ``c_1``… doivent toutes être fournies.

.. code-block:: python

   res = mix.compute(
       ["p", "T", "c_0", "c_1"],
       [[1.0e5, 2.0e5], [393.15, 393.15], [0.5, 0.5], [0.5, 0.5]],
       ["h", "rho", "cp"],
   )
   print(np.array(res))

.. code-block:: text

   [[2.72348980e+06 6.75101889e-01 1.35875461e+03]
    [2.72065660e+06 1.35632808e+00 1.30502215e+03]]

Le résultat est, comme toujours, indexé ``[point][sortie]``. En ne donnant que
la pression et la composition, on obtient la saturation du fluide principal :

.. code-block:: python

   res = mix.compute(["p", "c_0", "c_1"],
                     [[1.0e5, 2.0e5], [0.5, 0.5], [0.5, 0.5]],
                     ["T_sat"])
   print(res)

.. code-block:: text

   ((372.78,), (393.56216108694747,))

Le dernier exemple du notebook ``eos_py_demo_mixing`` montre l'effet de la
teneur en air sur la température du gaz à enthalpie de mélange fixée, dans le
plan :math:`(p,h)` :

.. code-block:: python

   import matplotlib.pyplot as plt

   P = np.linspace(1e5, 5e5, 50)
   for c0, c1 in [(0.1, 0.9), (0.5, 0.5), (0.9, 0.1)]:
       res = mix.compute(["p", "h", "c_0", "c_1"],
                         [P.tolist(), [2.723490e6] * len(P),
                          [c0] * len(P), [c1] * len(P)],
                         ["T"])
       plt.plot(P, np.array(res)[:, 0], label=f"c0={c0}, c1={c1}")
   plt.xlabel("Pression [Pa]"); plt.ylabel("Température [K]")
   plt.legend(); plt.grid(True); plt.show()

Les erreurs de construction (listes vides ou de longueurs différentes) et de
calcul (tableaux de tailles inégales, code de retour différent de ``good``)
lèvent une ``RuntimeError`` dont le message commence par ``EOS_Mixing_py``.
Le script ``Modules/EOS/Tests/Python/test_eos_mixing.py`` les passe en revue
et vérifie l'aller-retour :math:`(p,T) \rightarrow h \rightarrow (p,h)
\rightarrow T`.

Tracer des résultats
--------------------

Le module ne fournit aucune fonction de tracé ; tout se fait avec matplotlib
à partir des tableaux obtenus plus haut. Trois gabarits couvrent l'essentiel
des besoins.

Une propriété le long d'un profil, ici la masse volumique de la vapeur en
fonction de la pression à température fixée :

.. code-block:: python

   vapeur = eos_py.EOS_py("EOS_Refprop9", "WaterVapor")
   P = np.linspace(1.0e5, 3.0e5, 21)
   res = np.array(vapeur.compute("p", "T", list(P), [500.0] * len(P),
                                 ["rho", "mu", "cp"]))
   plt.plot(P, res[:, 0], marker="o")
   plt.xlabel("P [Pa]"); plt.ylabel("rho [kg/m³]")
   plt.title("Masse volumique à T = 500 K"); plt.grid(True); plt.show()

À 500 K, REFPROP donne 0,435 kg/m³ à 1 bar, 0,874 à 2 bar et 1,317 à 3 bar.

L'enveloppe de saturation, liquide et vapeur sur un même graphique :

.. code-block:: python

   p_sat = np.linspace(1.0e6, 1.9e7, 40)
   env = np.array(fluide.compute_sat("p", list(p_sat),
                                     ["rho_lsat", "rho_vsat", "T_sat"]))
   plt.plot(env[:, 0], p_sat / 1e6, label="liquide saturé")
   plt.plot(env[:, 1], p_sat / 1e6, label="vapeur saturée")
   plt.xlabel("rho [kg/m³]"); plt.ylabel("p [MPa]")
   plt.legend(); plt.grid(True); plt.show()

Une carte à deux dimensions, à partir des tableaux ``T_carte`` et
``rho_carte`` construits dans la section sur les tableaux :

.. code-block:: python

   fig, ax = plt.subplots()
   im = ax.pcolormesh(pp / 1e6, hh / 1e3, T_carte, shading="auto")
   fig.colorbar(im, label="T [K]")
   ax.set_xlabel("p [MPa]"); ax.set_ylabel("h [kJ/kg]")
   plt.show()

Pour conserver les résultats, le notebook ``eos_py_demo_plots`` les range
dans un ``pandas.DataFrame`` puis les exporte par ``to_csv`` ou ``to_excel``.
Une remarque sur ce notebook : il étiquette l'enthalpie en kJ/kg alors que
les valeurs passées à EOS et rendues par lui sont en J/kg.

Pour qui préfère une interface graphique à un script, l':ref:`IHM <eos-ihm>`
offre les mêmes tracés sans écrire de code.

Exemples exécutés et notebooks de référence
-------------------------------------------

Les exemples de cette page sur un fluide CATHARE2 (création, point, tableaux,
saturation, erreurs), sur l'interpolateur construit à partir de CATHARE2 et
sur ``EOS_Mixing_py`` ont été exécutés contre l'installation courante, et les
sorties recopiées telles quelles. Ceux qui font appel à REFPROP 9 (génération
avec raffinement, tableau comparatif, tracé de la vapeur) reprennent les
sorties enregistrées dans les notebooks ; les tracés n'ont pas été rejoués.

Les notebooks d'origine vont plus loin que cette page, notamment sur la
convergence de l'erreur avec la taille du maillage :

* ``documentation_api_python_interpolateur.ipynb`` — tour complet de l'API et
  de ses pièges ;
* ``eos_py_demo_interpolateur.ipynb`` — maillages régulier, raffiné global et
  raffiné local, comparaison bilinéaire / bicubique ;
* ``eos_py_demo_mixing.ipynb`` — mélange vapeur-air ;
* ``eos_py_demo_plots.ipynb`` — tracés et export pandas.
