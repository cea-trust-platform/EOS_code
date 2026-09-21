Propriétés thermodynamiques
===========================

Nomenclature des méthodes ``compute``
-------------------------------------

Les noms de méthodes suivent des conventions systématiques :

* ``compute_<x>_ph(p, h, r)`` — propriété :math:`x` dans le plan
  :math:`(p,h)` ;
* ``compute_<x>_pT(p, T, r)`` — plan :math:`(p,T)` ;
* ``compute_<x>_ps(p, s, r)`` — plan :math:`(p,s)` ;
* ``compute_d_<x>_d_<v>_<w>_<plan>`` — dérivée partielle
  :math:`\left(\partial x/\partial v\right)_w` dans le plan considéré,
  p. ex. ``compute_d_rho_d_p_h_ph`` pour
  :math:`\left(\partial\rho/\partial p\right)_h` ;
* ``compute_<x>_sat_p(p, r)`` / ``compute_<x>_sat_T(T, r)`` — propriété à
  saturation en fonction de :math:`p` ou :math:`T` ;
* ``compute_d_<x>_sat_d_p_p`` / ``compute_d2_<x>_sat_d_p_d_p_p`` — dérivées
  première et seconde à saturation ;
* ``compute_<x>_lim_p`` — enthalpies limites (spinodales) ;
* les surcharges avec ``c_0 … c_4`` concernent les **mélanges**
  (fractions massiques d'incondensables, voir :doc:`../models`), avec les
  dérivées associées ``compute_d_<x>_d_c_<i>_ph_ph``.

Toutes ces méthodes renvoient un code ``EOS_Error`` et écrivent le résultat
dans leur dernier argument ``double &r`` (ou avant les ``c_i``).

Propriétés planes (monophasiques)
---------------------------------

Disponibles dans les plans :math:`(p,h)`, :math:`(p,T)` et :math:`(p,s)` :

==============  =============================================  ==================
Nom             Signification                                  Unité SI
==============  =============================================  ==================
``p``           Pression (variable d'entrée)                   Pa
``h``           Enthalpie massique                             J/kg
``T``           Température                                    K
``s``           Entropie massique                              J/(kg·K)
``rho``         Masse volumique                                kg/m³
``u``           Énergie interne massique                       J/kg
``mu``          Viscosité dynamique                            Pa·s
``lambda``      Conductivité thermique                         W/(m·K)
``cp``          Capacité thermique massique isobare            J/(kg·K)
``cv``          Capacité thermique massique isochore           J/(kg·K)
``sigma``       Tension superficielle                          N/m
``w``           Vitesse du son                                 m/s
``g``           Enthalpie libre massique (Gibbs)               J/kg
``f``           Énergie libre massique (Helmholtz)             J/kg
``pr``          Nombre de Prandtl                              —
``beta``        Coefficient de dilatation isobare              1/K
``gamma``       Rapport :math:`c_p/c_v`                        —
==============  =============================================  ==================

Chaque propriété plane :math:`x` dispose de ses deux dérivées partielles
premières dans le plan : par exemple dans :math:`(p,h)`,
``d_<x>_d_p_h`` (:math:`(\partial x/\partial p)_h`) et
``d_<x>_d_h_p`` (:math:`(\partial x/\partial h)_p`).

Propriétés à saturation
-----------------------

En fonction de :math:`p` (suffixe ``_sat_p``) ou de :math:`T`
(suffixe ``_sat_T``) :

=================  ================================================  ==========
Nom                Signification                                     Unité SI
=================  ================================================  ==========
``T_sat``          Température de saturation :math:`T_{sat}(p)`      K
``p_sat``          Pression de saturation :math:`p_{sat}(T)`         Pa
``rho_l_sat``      Masse volumique du liquide saturé                 kg/m³
``rho_v_sat``      Masse volumique de la vapeur saturée              kg/m³
``h_l_sat``        Enthalpie du liquide saturé                       J/kg
``h_v_sat``        Enthalpie de la vapeur saturée                    J/kg
``cp_l_sat``       :math:`c_p` du liquide saturé                     J/(kg·K)
``cp_v_sat``       :math:`c_p` de la vapeur saturée                  J/(kg·K)
=================  ================================================  ==========

Pour chacune : dérivée première (``compute_d_T_sat_d_p_p``, …) et dérivée
seconde (``compute_d2_T_sat_d_p_d_p_p``, …).

Enthalpies limites (spinodales)
-------------------------------

===============  ================================================  ==========
Nom              Signification                                     Unité SI
===============  ================================================  ==========
``h_l_lim``      Enthalpie limite de la phase liquide              J/kg
``h_v_lim``      Enthalpie limite de la phase vapeur               J/kg
===============  ================================================  ==========

Avec dérivées première (``compute_d_h_l_lim_d_p_p``) et seconde
(``compute_d2_h_l_lim_d_p_d_p_p``).

Bornes du domaine et constantes du fluide
-----------------------------------------

Les accesseurs ``get_*`` renseignent les caractéristiques de la méthode :

* point critique : ``get_p_crit``, ``get_T_crit``, ``get_h_crit``,
  ``get_rho_crit`` ;
* domaine de validité : ``get_p_min``/``get_p_max``,
  ``get_h_min``/``get_h_max``, ``get_T_min``/``get_T_max``,
  ``get_rho_min``/``get_rho_max`` ;
* état de référence : ``get_p_ref``, ``get_h_ref``, ``get_T_ref`` ;
* masse molaire : ``get_mm``.

Numérotation interne des propriétés
-----------------------------------

Chaque propriété est identifiée par un entier ``EOS_Property``
(``EOS_properties.hxx``). Les plages sont réparties par famille dans les
en-têtes dédiés :

* ``therm_properties.hxx`` — propriétés planes et leurs dérivées
  (énumération ``EOS_thermprop``) ;
* ``satur_properties.hxx`` — propriétés à saturation
  (``EOS_saturprop``) ;
* ``splim_properties.hxx`` — enthalpies limites (``EOS_splimprop``) ;
* ``camix_properties.hxx`` — propriétés de mélange CATHARE
  (``EOS_camixprop``) ;
* ``c2iap_properties.hxx`` — propriétés spécifiques CATHARE2/IAPWS
  (``EOS_c2iapprop``).

La fonction ``gen_property_number(const char*)`` convertit un nom en numéro
et ``get_property_name(EOS_Property)`` fait l'inverse. Les constructeurs de
:cpp:class:`NEPTUNE::EOS_Field` acceptent le numéro de propriété en plus du
nom, ce qui évite la résolution de chaîne à chaque appel :

.. code-block:: c++

   EOS_Field P("Pressure", "p", NEPTUNE::p, xp);   // avec numéro explicite

.. note::

   La casse et les synonymes reconnus pour les noms de propriétés sont gérés
   par ``EOS_properties.cxx``. En cas de doute, utiliser les chaînes courtes
   du tableau ci-dessus (``"T"``, ``"rho"``, ``"h_l_sat"``, …), telles
   qu'utilisées dans les tests ``Modules/EOS/Tests``.
