Premiers pas
============

Créer un objet EOS
------------------

Un objet :cpp:class:`NEPTUNE::EOS` se construit à partir du nom de la
**méthode thermodynamique** et du nom de l'**équation fluide** :

.. code-block:: c++

   #include "EOS/API/EOS.hxx"
   using namespace NEPTUNE;

   EOS liquide("EOS_Cathare2", "WaterLiquid");   // eau liquide, CATHARE2
   EOS vapeur ("EOS_Cathare2", "WaterVapor");    // vapeur d'eau, CATHARE2
   EOS air    ("EOS_PerfectGas", "Air");         // gaz parfait « Air »

Des arguments d'initialisation supplémentaires peuvent être passés via un
objet ``Strings`` (tableau de ``AString``), notamment pour les méthodes
paramétrables (gaz parfait, gaz raidi, interpolateur) :

.. code-block:: c++

   Strings args(2);
   args[0] = AString("Cp");     // exemple : nom d'un paramètre
   args[1] = AString("1005.");  // et sa valeur
   EOS gaz("EOS_PerfectGas", "Air", args);

   // Variante avec état de référence (h0, s0, T0, p0) :
   EOS gaz2("EOS_PerfectGas", "Air", args, h0, s0, t0, p0);

Les noms de méthode et d'équation effectivement disponibles dépendent des
bibliothèques activées à la compilation ; voir :doc:`../models` pour la liste
complète.

Calcul au point
---------------

Chaque propriété possède une méthode dédiée, nommée
``compute_<propriété>_<plan>`` (voir :doc:`properties` pour la nomenclature) :

.. code-block:: c++

   double p = 155.e5 ;        // pression [Pa]
   double h = 1.2e6  ;        // enthalpie massique [J/kg]
   double T, rho, cp ;

   EOS_Error cr ;
   cr = liquide.compute_T_ph  (p, h, T)   ;  // température [K]
   cr = liquide.compute_rho_ph(p, h, rho) ;  // masse volumique [kg/m3]
   cr = liquide.compute_cp_ph (p, h, cp)  ;  // capacité isobare [J/kg/K]

   // Plan (p, T) :
   cr = liquide.compute_h_pT  (p, 583.15, h) ;

   // Saturation :
   double tsat, hlsat ;
   cr = liquide.compute_T_sat_p    (p, tsat)  ;
   cr = liquide.compute_h_l_sat_p  (p, hlsat) ;

   // Dérivées :
   double dTdp ;
   cr = liquide.compute_d_T_d_p_h_ph(p, h, dTdp) ;

Il existe aussi une forme générique par nom de propriété :

.. code-block:: c++

   double x ;
   cr = liquide.compute("T",   p, h, x) ;   // équivalent de compute_T_ph
   cr = liquide.compute_Ph  ("rho", p, h, x) ;
   cr = liquide.compute_PT  ("h",   p, T, x) ;
   cr = liquide.compute_Psat("hlsat", p, x)  ;
   cr = liquide.compute_Tsat("psat",  T, x)  ;

La valeur de retour ``EOS_Error`` doit toujours être testée
(``good``/``ok``/``bad``/``error``, voir :doc:`errors`).

Calcul par champ
----------------

Pour calculer une propriété sur ``n`` points, on enveloppe les tableaux dans
des :cpp:class:`NEPTUNE::EOS_Field` et on fournit un champ d'erreurs
:cpp:class:`NEPTUNE::EOS_Error_Field` :

.. code-block:: c++

   #include "EOS/API/EOS_Field.hxx"
   #include "EOS/API/EOS_Fields.hxx"
   #include "EOS/API/EOS_Error_Field.hxx"

   int n = 1000 ;
   ArrOfDouble xp(n), xh(n), xT(n) ;
   ArrOfInt    ierr(n) ;

   // ... remplir xp et xh ...

   EOS_Field P("Pressure",    "p", xp) ;
   EOS_Field H("Enthalpy",    "h", xh) ;
   EOS_Field T("Temperature", "T", xT) ;
   EOS_Error_Field err(ierr) ;

   EOS_Error cr = liquide.compute(P, H, T, err) ;

Pour plusieurs propriétés de sortie (et la mutualisation des calculs
intermédiaires), on utilise :cpp:class:`NEPTUNE::EOS_Fields` :

.. code-block:: c++

   ArrOfDouble xrho(n), xmu(n) ;
   EOS_Fields sorties(3) ;
   sorties[0] = EOS_Field("Temperature",     "T",   xT)   ;
   sorties[1] = EOS_Field("Density",         "rho", xrho) ;
   sorties[2] = EOS_Field("DynamicViscosity","mu",  xmu)  ;

   EOS_Fields entrees(2) ;
   entrees[0] = P ;
   entrees[1] = H ;

   EOS_Error cr = liquide.compute(entrees, sorties, err) ;

Voir :doc:`fields` pour les détails (propriétés d'entrée reconnues,
sémantique du champ d'erreurs).

Python
------

L'API Python est générée par SWIG (module ``eos_py``, répertoire
``Modules/EOS/PyAPI``) et suit fidèlement l'API C++ :

.. code-block:: python

   import eos_py

   fluide = eos_py.EOS("EOS_Cathare2", "WaterLiquid")

   # calcul au point
   cr, T = fluide.compute_T_ph(155.e5, 1.2e6)

   # informations
   print(fluide.fluid_name(), fluide.table_name(), fluide.version_name())

Un exemple complet (calculs par champs, affichage tabulé) se trouve dans
``Modules/EOS/Tests/Python/test_eos_py.py``.

Fortran 77
----------

Une interface F77 est fournie par le module ``Language``
(``F77Language.hxx``) ; des exemples d'appel se trouvent dans
``Modules/EOS/Tests/F77`` et ``Modules/Language/Tests/F77``.

Informations sur le fluide
--------------------------

.. code-block:: c++

   const AString &meth  = liquide.table_name()    ;  // méthode (« Cathare2 »)
   const AString &ref   = liquide.equation_name() ;  // équation (« WaterLiquid »)
   const AString &fld   = liquide.fluid_name()    ;  // fluide (« Water »)
   const AString &phase = liquide.phase_name()    ;  // phase  (« Liquid »)
   const AString &vers  = liquide.version_name()  ;  // version de la méthode

   double pc, hc, Tc ;
   liquide.get_p_crit(pc) ;   // pression critique
   liquide.get_h_crit(hc) ;   // enthalpie critique
   liquide.get_T_crit(Tc) ;   // température critique

   double pmin, pmax, hmin, hmax, Tmin, Tmax ;
   liquide.get_p_min(pmin) ;  liquide.get_p_max(pmax) ;  // domaine de validité
   liquide.get_h_min(hmin) ;  liquide.get_h_max(hmax) ;
   liquide.get_T_min(Tmin) ;  liquide.get_T_max(Tmax) ;
