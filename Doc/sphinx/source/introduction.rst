Introduction
============

Objectifs
---------

Le composant EOS a été conçu pour permettre à toutes les applications NEPTUNE
(et à d'autres codes) de :

* **partager** les propriétés de base des fluides au travers d'une API unique ;
* **ajouter de nouveaux fluides** ou de nouvelles méthodes de calcul sans
  modifier les codes clients ;
* **traiter les mélanges** d'un fluide principal (vapeur) et de gaz
  incondensables (module ``EOS_Mixing``).

Le principe central est la séparation entre :

* l'**interface générique** — la classe :cpp:class:`NEPTUNE::EOS` et ses
  méthodes ``compute*`` — utilisée par les codes clients ;
* les **implémentations** (« méthodes thermodynamiques ») — classes dérivées de
  :cpp:class:`NEPTUNE::EOS_Fluid` — qui encapsulent chacune une bibliothèque ou
  une famille de corrélations (CATHARE, CATHARE2, THETIS, REFPROP, CoolProp,
  gaz parfait, gaz raidi, interpolateur…).

Une instance ``EOS`` est construite à partir de deux chaînes de caractères :
le nom de la *méthode* (« Thermodynamic Model ») et le nom de l'*équation
fluide* (« Fluid Equation »), par exemple :

.. code-block:: c++

   EOS liquide("EOS_Cathare2", "IAPWSLiquid");
   EOS vapeur ("EOS_Cathare2", "IAPWSVapor");
   EOS air    ("EOS_PerfectGas", "Air");

Fonctionnalités principales
---------------------------

* Calcul de propriétés **monophasiques** dans les plans thermodynamiques
  :math:`(p,h)`, :math:`(p,T)` et :math:`(p,s)` : température, masse
  volumique, énergie interne, entropie, viscosité, conductivité, capacités
  thermiques, tension superficielle, vitesse du son, énergies de Gibbs et de
  Helmholtz, nombre de Prandtl, coefficient de dilatation, :math:`\gamma`.
* Calcul des propriétés **à saturation** en fonction de :math:`p` ou de
  :math:`T`, avec dérivées premières et secondes.
* Calcul des **dérivées partielles premières** de toutes les propriétés planes
  par rapport aux variables du plan.
* **Enthalpies limites** (limites spinodales) ``h_l_lim`` / ``h_v_lim``.
* **Mélanges** fluide principal + incondensables avec fractions massiques
  :math:`C_0 \dots C_4` et dérivées associées.
* Trois granularités d'appel : **point**, **champ** (``EOS_Field``) et
  **ensemble de champs** (``EOS_Fields``), cette dernière permettant de
  mutualiser les calculs intermédiaires.
* **Gestion d'erreurs** hiérarchisée par point et par champ, avec gestionnaires
  configurables (arrêt, exception, trace).
* Interfaces **C++**, **Fortran 77** et **Python** (SWIG), plus une **IHM**
  graphique (Qt) de visualisation et de tracé.

Historique et contexte
----------------------

EOS est développé par le CEA (licence BSD à trois clauses, voir les en-têtes
de fichiers). Il est issu de la plate-forme NEPTUNE (CEA/EDF/Framatome/IRSN)
— d'où l'espace de noms ``NEPTUNE`` — et est utilisé notamment par les codes
CATHARE, FLICA4 et TrioCFD.

Organisation de cette documentation
-----------------------------------

* :doc:`installation` — compilation et installation via ``configure``/CMake.
* :doc:`architecture` — découpage en modules et flux de données.
* :doc:`usage/quickstart` — premiers pas C++ et Python.
* :doc:`usage/properties` — nomenclature complète des propriétés calculables.
* :doc:`usage/fields` — appels par point, champ et champs.
* :doc:`usage/errors` — codes d'erreurs et gestionnaires.
* :doc:`models` — description de chaque méthode thermodynamique.
* :doc:`api/index` — référence API C++ extraite des sources (Doxygen/Breathe).
