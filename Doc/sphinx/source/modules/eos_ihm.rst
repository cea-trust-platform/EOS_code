Module EOS_IHM (interface graphique)
====================================

Rôle
----

``Modules/EOS_IHM`` fournit une **application graphique** (PyQt) de
consultation des méthodes EOS : choix d'une méthode et d'une équation,
calcul et tracé de propriétés le long de profils, comparaison entre
méthodes, export des résultats. Guide utilisateur : ``Doc/GUIGuide``
(``desfonc.tex``, copies d'écran ``CaptureEOS``).

Composition
-----------

``cpp/`` — couche d'accès C++
    ``eosihmSrc1.{hxx,cxx}`` encapsule les appels EOS pour l'IHM ;
    ``eosihm.i`` est l'interface SWIG qui expose cette couche en Python ;
    le sous-répertoire ``med/`` fournit l'export de champs au format MED
    (activé si ``USER_MED_PATH`` est renseigné à la configuration).

``qt/`` — application PyQt
    * ``eosMain4.py`` / ``eosGUI4a.py.in`` — fenêtre principale ;
    * ``eosComponent.py`` — découverte des méthodes/équations disponibles ;
    * ``eosRunFunction.py`` — exécution des calculs ;
    * ``eosPrint4.py.in`` — tracés et impressions ;
    * ``eosFileManager.py``, ``eosUtil.py`` — utilitaires ;
    * ``eosAva.py.in``, ``eosHelp4.py.in``, ``eosHelp.txt`` — aide en ligne.

    Les fichiers ``*.py.in`` sont instanciés par CMake à l'installation
    (chemins d'installation substitués).

Lancement
---------

Après installation, l'IHM se lance par le script installé dans ``bin/``
(nom selon la plate-forme, voir ``install/bin``). Elle nécessite Python,
PyQt et l'API Python d'EOS.
