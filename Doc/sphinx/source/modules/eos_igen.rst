.. _eos-igen:

Module EOS_IGen (générateur d'interpolateur)
============================================

Rôle
----

``Modules/EOS_IGen`` génère des **tables d'interpolation** à partir de
n'importe quelle méthode thermodynamique EOS. Les tables produites
(fichiers ``.ipp``, avec sortie MED optionnelle pour visualisation) sont
ensuite évaluées à moindre coût par la méthode ``EOS_Ipp`` du module EOS.
Cas d'usage typique : remplacer une méthode précise mais coûteuse
(REFPROP, CoolProp) dans une boucle de calcul intensive.

La documentation détaillée de l'algorithme (maillage 2D, continuité des
propriétés, indicateurs de qualité) se trouve dans
``Doc/Interpolator/EOS_Interpolator.tex`` et ses figures.

Principe
--------

#. choix de la méthode et de l'équation de référence
   (``set_method`` / ``set_reference``) ;
#. définition du domaine :math:`[p_{min}, p_{max}] \times [T_{min},
   T_{max}]` (``set_extremum``) et du budget mémoire
   (``set_memory_max``) ;
#. construction d'un maillage initial dans le plan :math:`(p,h)` — ou 1D
   en :math:`p` pour les propriétés de saturation — via ``make_mesh_ph`` /
   ``make_mesh_p`` ;
#. choix des propriétés à tabuler (``set_list_propi``,
   ``set_list_propi_sat``, ``set_list_propi_lim``) ;
#. définition de critères de **qualité** par propriété (``set_quality`` :
   type d'indicateur, seuil absolu ou relatif) puis
   ``compute_qualities`` ;
#. **raffinement** global (``make_global_refine``) ou local adaptatif
   (``make_local_refine``) jusqu'à satisfaire les critères ;
#. écriture des fichiers (``write_med``, ``write_index``).

Classes
-------

``EOS_IGen`` (``API/EOS_IGen.hxx``)
    Pilote de la génération (étapes ci-dessus).

``EOS_IGen_QI`` (``API/EOS_IGen_QI.hxx``)
    Indicateurs de qualité de l'interpolation (écarts entre valeur
    interpolée et valeur de référence, contrôle par propriété).

Utilisation avec EOS_Ipp
------------------------

Une fois les tables générées et installées dans le répertoire de données
(``Modules/EOS/Data/EOS_Ipp`` ou répertoire pointé par l'installation), la
méthode s'utilise comme n'importe quelle autre :

.. code-block:: c++

   EOS eau_rapide("EOS_Ipp", "<NomTableGeneree>");

L'erreur d'interpolation peut être estimée a posteriori via
``EOS::compute_Ipp_error`` / ``EOS::compute_Ipp_sat_error``.

Tests
-----

``Modules/EOS_IGen/Tests/C++`` contient des exemples complets de
génération ; ``Modules/EOS/Tests/C++/main_ipp.cxx`` montre l'évaluation.
