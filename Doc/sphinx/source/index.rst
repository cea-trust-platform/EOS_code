Documentation du code EOS
=========================

**EOS** (*Equation Of State*) est une bibliothèque C++ développée par le CEA
fournissant aux applications de thermohydraulique (plate-forme NEPTUNE,
CATHARE, FLICA4, etc.) un accès unifié aux **propriétés thermodynamiques et de
transport des fluides** : lois d'état, propriétés à saturation, dérivées
premières et secondes, mélanges vapeur/gaz incondensables.

La bibliothèque repose sur une architecture à *méthodes* interchangeables : un
même appel ``compute`` peut être servi indifféremment par les corrélations
CATHARE ou CATHARE2, les tables THETIS, NIST REFPROP, CoolProp, un gaz parfait
analytique, un gaz raidi (*stiffened gas*), ou encore un interpolateur généré
(EOS_IGen/EOS_Ipp).

.. toctree::
   :maxdepth: 2
   :caption: Présentation

   introduction
   installation
   architecture

.. toctree::
   :maxdepth: 2
   :caption: Guide d'utilisation

   usage/quickstart
   usage/properties
   usage/fields
   usage/errors
   usage/python

.. toctree::
   :maxdepth: 2
   :caption: Modules et modèles

   modules/eos
   models
   modules/eos_mixing
   modules/language
   modules/functions
   modules/eos_igen
   modules/eos_ihm
   modules/common

.. toctree::
   :maxdepth: 2
   :caption: Référence API C++

   api/index

Index et tables
---------------

* :ref:`genindex`
* :ref:`search`
