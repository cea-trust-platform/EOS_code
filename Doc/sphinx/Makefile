# Makefile de la documentation Sphinx du code EOS.
#
#   make html    : génère doxygen/xml puis la documentation HTML dans _build/html
#   make clean   : supprime les répertoires générés

SPHINXBUILD   ?= python3 -m sphinx
SOURCEDIR     = .
BUILDDIR      = _build

.PHONY: html doxygen clean help

html: doxygen
	$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)/html"
	@echo
	@echo "Documentation générée : $(BUILDDIR)/html/index.html"

doxygen:
	doxygen Doxyfile

clean:
	rm -rf "$(BUILDDIR)" doxygen

help:
	@echo "Cibles : html (défaut), doxygen, clean"
