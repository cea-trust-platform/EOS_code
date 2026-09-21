Modules Common et system
========================

Common
------

``Modules/Common`` regroupe les utilitaires bas niveau partagés :

``func.{hxx,cxx}``
    Fonctions utilitaires communes (incluses notamment par
    ``EOS_Field.hxx``).

``fxdr/``
    Bibliothèque **FXDR** : lecture/écriture de fichiers binaires portables
    au format XDR depuis le Fortran et le C (``cxdr*.c``, ``ixdr*.F``,
    ``initxdr.F``). Utilisée pour les fichiers de données binaires
    indépendants de la plate-forme (tables des méthodes).

system
------

``Modules/system`` contient l'intégration système de la plate-forme
(scripts et définitions dépendant de l'environnement de compilation).
