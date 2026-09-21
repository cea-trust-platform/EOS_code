Calculs par point, par champ, par champs
========================================

EOS offre trois granularités d'appel, du plus simple au plus performant.

Niveau point
------------

Une valeur d'entrée, une valeur de sortie :

.. code-block:: c++

   double T ;
   EOS_Error cr = fluide.compute_T_ph(p, h, T) ;

C'est la forme la plus lisible, adaptée aux calculs ponctuels ; pour des
maillages complets, préférer les formes par champ.

Niveau champ : ``EOS_Field``
----------------------------

:cpp:class:`NEPTUNE::EOS_Field` est une **vue étiquetée** sur un tableau de
``double`` : elle n'alloue pas la mémoire, elle référence un
``ArrOfDouble`` ou un couple ``(taille, double*)`` fourni par l'appelant, et
porte trois métadonnées :

* le *titre* de la propriété (libellé libre, p. ex. ``"Pressure"``) ;
* le *nom* de la propriété (identifiant reconnu, p. ex. ``"p"``) ;
* le *numéro* de propriété (optionnel, évite la résolution de chaîne).

.. code-block:: c++

   ArrOfDouble xp(n) ;                       // géré par Language
   EOS_Field P("Pressure", "p", xp) ;

   double *ptr = new double[n] ;             // ou mémoire du code client
   EOS_Field H("Enthalpy", "h", n, ptr) ;

Méthodes utiles : ``operator[]``, ``size()``, ``set_data()`` /
``get_data()``, ``reset_data_ptr()`` (rebrancher la vue sur une autre zone
mémoire sans reconstruire l'objet), ``set_property_title()`` /
``set_property_name()``.

Formes d'appel par champ :

.. code-block:: c++

   // saturation : 1 champ d'entrée (p ou T)
   eos.compute(P, Tsat, err) ;

   // plan (p,h) : 2 champs d'entrée
   eos.compute(P, H, T, err) ;          // 1 sortie
   eos.compute(P, H, sorties, err) ;    // n sorties (EOS_Fields)

La propriété de chaque champ d'entrée est identifiée par son **nom** : la
première forme à deux entrées attend par convention ``p`` puis ``h``, mais la
forme générique par ``EOS_Fields`` (ci-dessous) accepte tout couple
d'entrées reconnu par la méthode (p. ex. :math:`(p,T)`, :math:`(p,s)`).

Niveau champs : ``EOS_Fields``
------------------------------

:cpp:class:`NEPTUNE::EOS_Fields` est un tableau d'``EOS_Field``. Il permet :

* de passer **plusieurs sorties** en un seul appel — la méthode
  thermodynamique peut alors mutualiser les calculs intermédiaires
  (précalculs partagés entre propriétés), ce qui est nettement plus
  performant que des appels séparés ;
* de passer **plusieurs entrées** (p. ex. ``p`` et ``h``, plus les
  concentrations ``c_0``… pour un mélange).

.. code-block:: c++

   EOS_Fields entrees(2) ;
   entrees[0] = EOS_Field("Pressure", "p", xp) ;
   entrees[1] = EOS_Field("Enthalpy", "h", xh) ;

   EOS_Fields sorties(4) ;
   sorties[0] = EOS_Field("Temperature", "T",       xT)   ;
   sorties[1] = EOS_Field("Density",     "rho",     xrho) ;
   sorties[2] = EOS_Field("dTdp",        "d_T_d_p_h", xdT) ;
   sorties[3] = EOS_Field("Viscosity",   "mu",      xmu)  ;

   EOS_Error cr = eos.compute(entrees, sorties, err) ;

Champ d'erreurs : ``EOS_Error_Field``
-------------------------------------

Tout appel par champ renseigne un :cpp:class:`NEPTUNE::EOS_Error_Field`,
vue sur un ``ArrOfInt`` de même taille que les champs de données : chaque
point reçoit le code interne (``EOS_Internal_Error``) du calcul en ce point.

.. code-block:: c++

   ArrOfInt ierr(n) ;
   EOS_Error_Field err(ierr) ;

   EOS_Error cr = eos.compute(P, H, T, err) ;
   if (cr != EOS_Error::good)
      for (int i = 0 ; i < n ; i++)
         if (err[i].generic_error() != EOS_Error::good)
         {  AString description ;
            eos.fluid().describe_error(err[i], description) ;
            // traiter le point i ...
         }

Le code de retour global est le **pire** code des points calculés
(voir :doc:`errors`). Méthodes utiles : ``find_worst_error()``,
``set_worst_error()``.

Recommandations de performance
------------------------------

#. Regrouper les sorties d'un même plan dans un ``EOS_Fields`` unique.
#. Fournir le **numéro de propriété** aux constructeurs d'``EOS_Field``
   (constantes de ``therm_properties.hxx`` et al.) pour éviter les
   résolutions de chaînes répétées.
#. Réutiliser les objets ``EOS_Field`` avec ``reset_data_ptr()`` d'un pas de
   temps à l'autre plutôt que de les reconstruire.
#. Pour les méthodes coûteuses (REFPROP, CoolProp), envisager la génération
   d'un interpolateur avec :doc:`EOS_IGen <../modules/eos_igen>` puis
   l'utilisation de la méthode ``EOS_Ipp``.
