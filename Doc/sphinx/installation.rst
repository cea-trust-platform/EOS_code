Installation
============

Prérequis
---------

* CMake (≥ 3.x ; ≥ 3.20 pour ``ctest --test-dir``)
* Un compilateur C++ et un compilateur Fortran (interfaces F77 et
  bibliothèques CATHARE/THETIS)
* Python + SWIG pour les interfaces Python (optionnel)
* Qt/PyQt pour l'IHM graphique (optionnel)
* Bibliothèques externes optionnelles selon les méthodes activées :
  NIST REFPROP 9/10, CoolProp, MED (export de champs depuis l'IHM)

Installation rapide
-------------------

.. code-block:: bash

   git clone ssh://gitolite@ssh-codev-tuleap.cea.fr:2044/eos/EOS_code.git
   cd EOS_code
   ./configure --execdir=build/ --prefix=install/
   make -C build/
   make -C build/ test
   make -C build/ install

Configuration détaillée
-----------------------

Le script ``configure`` est une façade au-dessus de CMake. Une bonne pratique
est de conserver un fichier ``user_env.txt`` **hors** du dépôt git et de le
passer en option :

.. code-block:: bash

   ./configure --user-env-file=/chemin/vers/user_env.txt \
               --prefix=/chemin/installation \
               --execdir=/chemin/build

Points de comportement importants :

* si ``--user-env-file`` n'est pas fourni, le fichier ``user_env.txt`` de la
  racine des sources est lu ;
* si une option est donnée plusieurs fois, **la dernière l'emporte** ; les
  options de la ligne de commande priment sur celles de ``user_env.txt`` ;
* à défaut de ``--execdir``, on peut lancer ``configure`` depuis le répertoire
  de build (qui doit être différent de la racine des sources) :

.. code-block:: bash

   mkdir my_build && cd my_build
   ${EOS_code_dir}/configure [--user-env-file=/chemin/user_env.txt ...]

Appel direct de CMake
---------------------

.. code-block:: bash

   cmake ${EOS_code_dir} -B ${build_dir} \
         [-DCMAKE_INSTALL_PREFIX:PATH=/chemin/install] \
         [-DUSER_MED_PATH:PATH=/chemin/med] [...]

Compilation, tests, installation
--------------------------------

.. code-block:: bash

   cmake --build ${build_dir}           # compilation
   ctest --test-dir ${build_dir} -j N   # tests en parallèle (CMake >= 3.20)
   cmake --build ${build_dir} -t install

Arborescence d'installation
---------------------------

L'installation produit l'arborescence classique :

========================  =====================================================
Répertoire                Contenu
========================  =====================================================
``bin/``                  Exécutables et utilitaires
``lib/``                  Bibliothèques (``libeos``, etc.)
``include/``              En-têtes publics (``EOS/API``, ``Language/API``, …)
``data/``                 Données des méthodes (fichiers ``.ipp``, tables…)
``doc/``                  Documentation installée
``share/``                Fichiers annexes
========================  =====================================================

Génération de la présente documentation
---------------------------------------

.. code-block:: bash

   cd Doc/sphinx
   pip install --user sphinx breathe sphinx_rtd_theme   # une seule fois
   make html
   # -> _build/html/index.html

``make html`` lance d'abord Doxygen (extraction XML des en-têtes C++), puis
Sphinx avec l'extension Breathe pour produire la référence API.
