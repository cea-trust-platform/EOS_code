Architecture générale
=====================

Vue d'ensemble
--------------

Le dépôt est organisé autour du répertoire ``Modules/``, chaque module ayant
la structure type ``API/`` (interface publique), ``Src/`` (implémentation),
``Tests/`` (tests C++, F77, Python) et éventuellement ``PyAPI/`` (liaison
SWIG) et ``Data/`` (données) :

.. code-block:: text

   EOS_code/
   ├── configure               # façade de configuration (appelle CMake)
   ├── CMakeLists.txt
   ├── user_env.txt            # variables d'environnement utilisateur
   ├── Doc/                    # documentation (LaTeX historique + Sphinx)
   ├── Tools/                  # outillage
   └── Modules/
       ├── Language/           # types de base NEPTUNE (AString, ArrOfDouble…)
       ├── Common/             # utilitaires partagés (fxdr, func…)
       ├── Functions/          # évaluateur de fonctions analytiques
       ├── EOS/                # cœur : API EOS + méthodes thermodynamiques
       ├── EOS_IGen/           # générateur de tables d'interpolation (.ipp)
       ├── EOS_IHM/            # IHM graphique Qt/Python + wrapper SWIG
       └── system/             # intégration système

Dépendances entre modules
-------------------------

.. graphviz::

   digraph modules {
       rankdir=BT;
       node [shape=box, style="rounded,filled", fillcolor="#eef3fa",
             fontname="Helvetica"];
       "Language"  [label="Language\n(types NEPTUNE)"];
       "Common"    [label="Common\n(utilitaires)"];
       "Functions" [label="Functions\n(fonctions analytiques)"];
       "EOS"       [label="EOS\n(API + méthodes thermo)"];
       "EOS_IGen"  [label="EOS_IGen\n(générateur d'interpolateur)"];
       "EOS_IHM"   [label="EOS_IHM\n(IHM graphique)"];
       "Functions" -> "Language";
       "EOS"       -> "Language";
       "EOS"       -> "Common";
       "EOS"       -> "Functions";
       "EOS_IGen"  -> "EOS";
       "EOS_IHM"   -> "EOS";
       "EOS_IHM"   -> "EOS_IGen";
   }

Le cœur : module EOS
--------------------

Le module ``Modules/EOS`` est découpé en deux couches :

``EOS/API`` — l'interface client
    * :cpp:class:`NEPTUNE::EOS` : la façade unique. Elle est construite avec
      un couple (méthode, équation fluide), délègue tous les calculs à un
      objet ``EOS_Fluid`` interne et porte la pile de gestionnaires d'erreurs.
    * :cpp:class:`NEPTUNE::EOS_Field` / :cpp:class:`NEPTUNE::EOS_Fields` :
      champs de valeurs (vues sur des tableaux ``double*`` ou
      ``ArrOfDouble``) étiquetés par une propriété thermodynamique.
    * :cpp:class:`NEPTUNE::EOS_Error_Field` : champ de codes d'erreurs
      internes associé à un calcul par champ.
    * ``EOS_Error``, :cpp:class:`NEPTUNE::EOS_Internal_Error`,
      ``EOS_Error_Handler`` / ``EOS_Std_Error_Handler`` : gestion d'erreurs
      (voir :doc:`usage/errors`).
    * ``EOS_properties`` et les en-têtes ``therm_properties.hxx``,
      ``satur_properties.hxx``, ``splim_properties.hxx``,
      ``camix_properties.hxx``, ``c2iap_properties.hxx`` : nomenclature et
      numérotation des propriétés (voir :doc:`usage/properties`).

``EOS/Src`` — les implémentations
    La classe abstraite :cpp:class:`NEPTUNE::EOS_Fluid` (déclarée dans
    ``API/``) définit le contrat : plusieurs centaines de méthodes virtuelles
    ``compute_*`` avec une implémentation par défaut renvoyant
    ``NOT_IMPLEMENTED``. Chaque sous-répertoire de ``Src/`` fournit une
    dérivation :

    * ``EOS_Cathare``, ``EOS_Cathare2`` — corrélations CATHARE/CATHARE2 ;
    * ``EOS_Thetis`` — tables THETIS ;
    * ``EOS_Refprop9``, ``EOS_Refprop10`` — couplage NIST REFPROP ;
    * ``EOS_CoolProp`` — couplage CoolProp ;
    * ``EOS_PerfectGas``, ``EOS_StiffenedGas`` — lois analytiques ;
    * ``EOS_Flica4``, ``EOS_Hitec``, ``EOS_Nak`` — méthodes spécifiques ;
    * ``EOS_Ipp`` — lecture des tables d'interpolation générées par
      EOS_IGen ;
    * ``EOS_Mixing.hxx`` — mélange fluide principal + incondensables.

    Voir :doc:`models` pour le détail de chaque méthode.

Mécanisme d'enregistrement dynamique
------------------------------------

Le module ``Language`` fournit une petite couche « objet » (``UObject``,
``Object_ID``, ``RegisteredClass``, ``Types_Info``) qui permet
l'**instanciation par nom** : chaque classe dérivée de ``EOS_Fluid`` est
enregistrée avec un identifiant de type ; le constructeur
``EOS(const char* method, const char* reference)`` retrouve et instancie la
bonne classe à l'exécution. C'est ce qui permet d'ajouter une méthode sans
modifier les codes clients.

Cheminement d'un appel ``compute``
----------------------------------

#. Le client appelle par exemple
   ``eos.compute_rho_ph(p, h, rho)`` ou la forme générique
   ``eos.compute(champs_entree, champs_sortie, champ_erreurs)``.
#. La façade :cpp:class:`NEPTUNE::EOS` transmet à l'objet ``EOS_Fluid``
   concret (CATHARE2, REFPROP…).
#. L'implémentation calcule point par point et renseigne, pour un appel par
   champ, un :cpp:class:`NEPTUNE::EOS_Error_Field` (un code interne par
   point).
#. Le code de retour global ``EOS_Error`` est le **pire** code rencontré ;
   la pile de gestionnaires d'erreurs peut déclencher trace, exception ou
   arrêt selon sa configuration.

Interfaces de liaison
---------------------

* **Fortran 77** : ``Language/API/F77Language.hxx`` et les tests
  ``Modules/*/Tests/F77`` montrent l'appel des mêmes services depuis le
  Fortran.
* **Python** : ``Modules/EOS/PyAPI`` (``EOS_py.i``) expose l'API via SWIG ;
  le module s'importe avec ``import eos_py`` (voir
  ``Modules/EOS/Tests/Python/test_eos_py.py``).
* **IHM** : ``Modules/EOS_IHM`` fournit une application PyQt de tracé des
  propriétés et un wrapper C++/SWIG dédié (``eosihm.i``), avec export MED
  optionnel.
