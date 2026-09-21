Module Functions
================

Rôle
----

``Modules/Functions`` fournit un **évaluateur de fonctions analytiques**
défini à l'exécution à partir d'une chaîne de caractères. Il est utilisé
notamment pour paramétrer des lois (données utilisateur) sans recompiler.

Classe ``AFunction``
--------------------

Déclarée dans ``Functions/API/Functions.hxx`` :

.. code-block:: c++

   #include "Functions/API/Functions.hxx"

   // fonction d'une variable
   AFunction f("x*x", "x");
   double y = f.compute(2.0);        // f(x) = x², y = 4

   // fonction de plusieurs variables
   Strings vars(2);
   vars[0] = AString("x");
   vars[1] = AString("y");
   AFunction g("x*x*y", vars);
   double z = g.compute(2.0, 3.0);   // g(2,3) = 12

Fonctions nommées et composition
--------------------------------

Une fonction peut être **enregistrée sous un nom** puis réutilisée dans
l'expression d'une autre fonction :

.. code-block:: c++

   AFunction f("f", "x*x", "x");          // enregistre f(x) = x²
   AFunction h("f(x*y)", vars);           // utilise f dans une expression

   AFunction k("k", "x*x*y", vars);       // k(x,y) enregistrée
   AFunction m("k(x*x, y*y)", vars);      // composition à plusieurs arguments

Les appels ``compute`` incomplets complètent les arguments manquants par
zéro (``f(x)``, ``f(x,0)``, ``f(x,0,0,…)``).

Organisation
------------

.. code-block:: text

   Modules/Functions/
   ├── API/        # AFunction, Function
   ├── Src/        # analyseur et évaluateur d'expressions
   ├── PyAPI/      # liaison Python
   └── Tests/C++/  # exemples et non-régression
