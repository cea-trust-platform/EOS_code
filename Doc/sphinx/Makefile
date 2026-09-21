# Makefile de la documentation Sphinx du code EOS.
#
#   make html    : génère source/doxygen/xml puis la documentation HTML dans _build/html
#   make clean   : supprime les répertoires générés

SPHINXBUILD   ?= python3 -m sphinx
SOURCEDIR     = source
BUILDDIR      = _build

.PHONY: html doxygen clean help

html: doxygen
	$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)/html"
	@echo
	@echo "Documentation générée : $(BUILDDIR)/html/index.html"

# Le Doxyfile utilise des chemins relatifs à son propre répertoire : doxygen
# doit donc être lancé depuis $(SOURCEDIR), pas depuis Doc/sphinx.
doxygen:
	cd "$(SOURCEDIR)" && doxygen Doxyfile

clean:
	rm -rf "$(BUILDDIR)" "$(SOURCEDIR)/doxygen"

help:
	@echo "Cibles : html (défaut), doxygen, clean"
