Gestion des erreurs
===================

Code générique ``EOS_Error``
----------------------------

Toutes les méthodes de calcul renvoient un code générique ``EOS_Error``
(``EOS_Error.hxx``) :

.. list-table::
   :header-rows: 1
   :widths: 22 8 70

   * - Valeur
     - Entier
     - Signification
   * - ``EOS_Error::good``
     - 0
     - Tout s'est bien passé.
   * - ``EOS_Error::ok``
     - 1
     - Résultat peut-être physiquement faux — p. ex. hors du domaine de
       validité de la méthode.
   * - ``EOS_Error::bad``
     - 2
     - Résultat probablement faux — p. ex. itérations de Newton non
       convergées.
   * - ``EOS_Error::error``
     - 3
     - Erreur informatique — p. ex. fonction non implémentée pour cette
       méthode.

.. warning::

   La convention numérique a changé à partir des versions 0.9.x
   (auparavant : ``error=-1, bad=0, ok=1, good=2``). Ne jamais comparer à un
   entier littéral : utiliser systématiquement les valeurs nommées
   ``EOS_Error::good``, etc.

Règles d'agrégation :

* pour un **champ**, le code générique retourné est le **pire** code parmi
  les points calculés ;
* pour un calcul en **plusieurs étapes** (différentiation numérique,
  composition de fonctions…), le code retourné est le pire code rencontré ;
* pour des **itérations de Newton**, c'est l'erreur de la dernière itération
  qui est retournée.

Code interne ``EOS_Internal_Error``
-----------------------------------

Chaque méthode thermodynamique définit ses propres codes détaillés,
encapsulés dans :cpp:class:`NEPTUNE::EOS_Internal_Error` :

* ``get_code()`` — code complet (entier, avec les drapeaux génériques) ;
* ``get_partial_code()`` — code spécifique sans les drapeaux ;
* ``get_library_code()`` — code de la bibliothèque sous-jacente
  (principalement REFPROP) ;
* ``generic_error()`` — conversion vers le code générique ``EOS_Error``
  (les 2 bits de poids fort du code encodent la gravité) ;
* constantes prédéfinies : ``OK``, ``NOT_IMPLEMENTED``,
  ``EOS_BAD_COMPUTE``, ``DATA_NUMBER``, ``INPUT_PROPERTY``.

Une description textuelle est fournie par
``EOS_Fluid::describe_error(err, description)`` (accessible via
``eos.fluid()``).

Champ d'erreurs
---------------

Les appels par champ remplissent un :cpp:class:`NEPTUNE::EOS_Error_Field`
(un ``EOS_Internal_Error`` par point) ; voir :doc:`fields`.

Gestionnaires d'erreurs
-----------------------

La façade :cpp:class:`NEPTUNE::EOS` porte une **pile de gestionnaires**
(``EOS_Error_Handler_Stack``). Le gestionnaire standard
:cpp:class:`NEPTUNE::EOS_Std_Error_Handler` offre trois actions
configurables, chacune déclenchée à partir d'un niveau de gravité donné :

.. code-block:: c++

   #include "EOS/API/EOS_Std_Error_Handler.hxx"

   EOS_Std_Error_Handler handler;
   // provoquer exit() dès le niveau « bad » :
   handler.set_exit_on_error(EOS_Error::bad);
   // lever une exception EOS_Std_Exception dès « ok » :
   handler.set_throw_on_error(EOS_Error::ok);
   // tracer un diagnostic dès « ok », sur un flux au choix :
   handler.set_dump_on_error(EOS_Error::ok);
   handler.set_dump_stream(cerr);

   // désactiver une action :
   handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);

   // installation sur un objet EOS :
   eos.save_error_handler()    ;   // empiler l'état courant
   eos.set_error_handler(handler); // installer
   // ... calculs ...
   eos.restore_error_handler() ;   // restaurer

L'exception levée est de type ``EOS_Std_Exception`` et transporte le
``EOS_Internal_Error`` fautif ainsi qu'un message.

.. note::

   Par défaut (sans configuration explicite), les calculs ne s'interrompent
   pas : il appartient au code appelant de tester les codes de retour et le
   champ d'erreurs.

Initialisation status of an EOS object
--------------------------------------

The constructors of :cpp:class:`NEPTUNE::EOS` never fail: when the
thermodynamic method cannot be initialised (``.eosmm`` manifest or ``.med``
table not found, unreadable file...), the object is built all the same and
only a message is written on ``cerr``. The ``EOS_Error`` code returned by the
initialisation is kept in the object and reported by two accessors:

* ``init_status()`` — the ``EOS_Error`` code of the last initialisation
  (constructor or ``init()``): ``good`` or ``ok`` when the model is usable,
  ``bad`` or ``error`` when the initialisation failed;
* ``is_initialized()`` — ``true`` when ``init_status()`` is ``good`` or
  ``ok``.

.. code-block:: c++

   EOS *liquid = new EOS("EOS_Ipp", "./EOS_Refprop10_WaterLiquid.eosmm");
   if (!liquid->is_initialized())
     { cerr << "cannot load the table (EOS_Error " << liquid->init_status()
            << ")" << endl;
       delete liquid;
       return EOS_Error::error;
     }

An object built from the class name alone (``EOS("EOS_Ipp")``) attempts no
initialisation and reports ``good``; a later call to ``init(args)`` replaces
the status with its own result.

.. note::

   Two cases still escape this mechanism and stop the program with
   ``exit(-1)``: a method/equation pair missing from ``index.eos``, and a
   missing data file in the methods that check it themselves (perfect gas,
   stiffened gas, REFPROP, THETIS, CoolProp).
