Module EOS (cœur)
=================

Rôle
----

``Modules/EOS`` est le cœur de la bibliothèque : il définit l'API cliente
(répertoire ``API/``), la classe de base des méthodes thermodynamiques et
les implémentations (répertoire ``Src/``), les données associées
(``Data/``) et les tests (``Tests/`` en C++, Fortran 77 et Python).

Classes principales
-------------------

:cpp:class:`NEPTUNE::EOS` (``API/EOS.hxx``)
    Façade unique de la bibliothèque. Construite par nom de méthode et
    d'équation fluide, elle délègue tous les calculs à l'objet
    ``EOS_Fluid`` concret (accessible par ``fluid()``) et porte la pile de
    gestionnaires d'erreurs. Elle expose :

    * les constructeurs et ``init`` (arguments optionnels, état de
      référence) ;
    * les métadonnées : ``eos_version``, ``table_name()``,
      ``equation_name()``, ``fluid_name()``, ``phase_name()``,
      ``version_name()`` ;
    * les bornes et constantes : ``get_p_crit()``, ``get_T_min()``, … ;
    * les calculs par point ``compute_<x>_<plan>`` et génériques
      ``compute(...)``, ``compute_Ph/PT/Psat/Tsat`` ;
    * les calculs par champ(s) ``compute(EOS_Field...)``,
      ``compute(EOS_Fields...)`` ;
    * le support des mélanges (``set_components``, surcharges avec
      ``c_0 … c_4``, accesseurs ``get_prx*``) ;
    * la gestion d'erreurs (``set_error_handler``, ``save_error_handler``,
      ``restore_error_handler``) ;
    * l'interpolateur (``init_model``, ``compute_Ipp_error``,
      ``compute_Ipp_sat_error``).

:cpp:class:`NEPTUNE::EOS_Fluid` (``API/EOS_Fluid.hxx``)
    Classe de base abstraite des méthodes thermodynamiques (plus de 600
    méthodes virtuelles). Chaque ``compute_*`` a une implémentation par
    défaut retournant ``NOT_IMPLEMENTED`` : une méthode concrète ne
    surcharge que ce qu'elle sait calculer. C'est le point d'extension pour
    ajouter une nouvelle méthode (voir le guide de l'implémenteur LaTeX,
    ``Doc/ImplementersGuide``).

:cpp:class:`NEPTUNE::EOS_Field`, :cpp:class:`NEPTUNE::EOS_Fields`
    Champs de données étiquetés par une propriété ; voir
    :doc:`../usage/fields`.

:cpp:class:`NEPTUNE::EOS_Error_Field`, :cpp:class:`NEPTUNE::EOS_Internal_Error`, ``EOS_Std_Error_Handler``
    Gestion d'erreurs ; voir :doc:`../usage/errors`.

``EOS_properties`` (``API/EOS_properties.hxx``)
    Nomenclature et numérotation des propriétés ; voir
    :doc:`../usage/properties`.

Organisation des sources
------------------------

.. code-block:: text

   Modules/EOS/
   ├── API/                    # interface publique (voir ci-dessus)
   ├── Src/                    # implémentations
   │   ├── EOS_Cathare/        #   corrélations CATHARE
   │   ├── EOS_Cathare2/       #   corrélations CATHARE2 (C++ + F77)
   │   ├── EOS_CoolProp/       #   couplage CoolProp
   │   ├── EOS_Flica4/         #   corrélations FLICA4
   │   ├── EOS_Hitec/          #   greffon sel fondu HITEC
   │   ├── EOS_Ipp/            #   interpolateur (lecture des tables .med)
   │   ├── EOS_Mixing.{hxx,cxx}#   mélanges
   │   ├── EOS_Nak/            #   greffon NaK
   │   ├── EOS_PerfectGas/     #   gaz parfait
   │   ├── EOS_Refprop9/       #   couplage REFPROP 9
   │   ├── EOS_Refprop10/      #   couplage REFPROP 10
   │   ├── EOS_StiffenedGas/   #   gaz raidi
   │   └── EOS_Thetis/         #   tables THETIS
   ├── Data/                   # données (PerfectGas, StiffenedGas, Ipp)
   ├── PyAPI/                  # liaison Python SWIG (module eos_py)
   └── Tests/                  # tests C++ / F77 / Python

Tests
-----

* ``Tests/C++/main.cxx`` — batterie générique : parcourt les méthodes et
  vérifie les calculs par point/champ(s) pour toutes les propriétés ;
* ``Tests/C++/main_OpenMP.cxx`` — usage multi-threads ;
* ``Tests/C++/main_ipp.cxx`` — interpolateur ;
* ``Tests/C++/main_Refprop9*.cxx`` — non-régression REFPROP ;
* ``Tests/Python/test_eos_py.py`` — API Python ;
* ``Tests/F77`` — interface Fortran.

Ces tests constituent la meilleure source d'exemples d'utilisation à jour.
