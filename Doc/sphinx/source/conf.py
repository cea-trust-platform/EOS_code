# Configuration Sphinx pour la documentation du code EOS.
#
# Prérequis : doxygen, sphinx (>= 7.2), breathe, sphinx_rtd_theme
# Génération :  make html   (depuis Doc/sphinx)

import os
import subprocess

# -- Informations sur le projet ----------------------------------------------

project = "EOS"
author = "CEA"
copyright = "2023, CEA"

# Version lue si possible depuis l'en-tête API
release = "développement"

# -- Configuration générale ---------------------------------------------------

extensions = [
    "breathe",
    "sphinx.ext.mathjax",
    "sphinx.ext.graphviz",
    "sphinx.ext.todo",
]

language = "fr"
templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store", "doxygen"]

todo_include_todos = True

# -- Breathe (pont Doxygen -> Sphinx) -----------------------------------------

breathe_projects = {"EOS": "doxygen/xml"}
breathe_default_project = "EOS"
breathe_default_members = ("members",)
breathe_show_include = False

# Lancement automatique de doxygen si le XML est absent (utile pour
# Read the Docs ou un premier « make html » sans passer par le Makefile).
if not os.path.isdir(os.path.join(os.path.dirname(__file__), "doxygen", "xml")):
    subprocess.run(["doxygen", "Doxyfile"], cwd=os.path.dirname(__file__) or ".")

# -- Sortie HTML ---------------------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_theme_options = {
    "collapse_navigation": False,
    "navigation_depth": 4,
    "titles_only": False,
}
html_static_path = []
html_title = "EOS — Documentation du code"
html_show_sourcelink = True

# -- Options C++ ---------------------------------------------------------------

primary_domain = "cpp"
highlight_language = "c++"
