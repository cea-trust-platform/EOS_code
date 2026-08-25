Module Language (types NEPTUNE)
===============================

Rôle
----

``Modules/Language`` fournit la couche « langage » commune à tous les
modules : types de base, petite infrastructure objet avec identification de
type à l'exécution, et interface Fortran 77. Tous ces types vivent dans les
espaces de noms ``NEPTUNE`` et ``LANGUAGE_KERNEL``.

Types de base
-------------

``AString`` (``AString.hxx``)
    Chaîne de caractères de la plate-forme (conversion ``aschar()``,
    comparaison, flux). Utilisée pour tous les noms (méthodes, équations,
    propriétés…).

``Strings`` (``Strings.hxx``)
    Tableau d'``AString`` ; sert notamment à passer les arguments
    d'initialisation aux constructeurs d'``EOS``.

``ArrOfDouble``, ``ArrOfInt`` (``ArrOfDouble.hxx``, ``ArrOfInt.hxx``)
    Tableaux 1D de ``double`` / ``int`` fondés sur le patron ``ArrOfT``
    (``ArrOfT.hxx``). Deux modes de possession : allocation propre
    (``ArrOfDouble x(n)``) ou **vue** sur une zone mémoire existante
    (constructeur ``(int size, double* ptr)``) — c'est ce mode qui permet à
    ``EOS_Field`` de travailler directement dans la mémoire du code client
    sans copie.

``Boolean``, ``DynamicArray``, ``UnboundedSetByValue``
    Types utilitaires complémentaires.

Infrastructure objet
--------------------

``UObject`` / ``Object_i`` / ``NumberedObject``
    Racine de la hiérarchie : impression (``print_On`` / ``read_On``),
    identité d'objet (``Object_ID``).

``Types_Info`` / ``Type_Info`` / ``RegisterType`` / ``RegisteredClass``
    Registre des types : chaque classe (dont les méthodes thermodynamiques
    dérivées d'``EOS_Fluid``) est enregistrée sous un nom et un
    identifiant, ce qui permet l'**instanciation par nom** au cœur du
    constructeur d'``EOS``. La méthode virtuelle ``get_Type_Info()``
    renvoie la description de type d'un objet.

``Objects``, ``Objects_ptr``
    Collections d'objets génériques.

Interface Fortran 77
--------------------

``F77Language.hxx`` définit les conventions de liaison (noms, passage de
chaînes et de tableaux) utilisées par les interfaces F77 d'EOS ; exemples
dans ``Modules/Language/Tests/F77``.

Sous-répertoires
----------------

.. code-block:: text

   Modules/Language/
   ├── API/                 # en-têtes publics (types ci-dessus)
   ├── Src/
   │   ├── Kernel/          # noyau (objets, registre de types)
   │   ├── TypesHandling/   # gestion des types
   │   └── ObjectsHandling/ # gestion des collections
   ├── PyAPI/               # liaison Python
   └── Tests/               # tests C++ et F77
