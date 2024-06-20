#!/bin/bash

DOXYGEN_CONFIG="Doxyfile"
DOC_MAIN_PAGE="hierarchy.html"

# Generate documentation with Doxygen
doxygen $DOXYGEN_CONFIG

# Move main page to the main folder

ln -s docs/html/$DOC_MAIN_PAGE main_page.html
