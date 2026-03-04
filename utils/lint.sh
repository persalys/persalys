#!/bin/sh

set -e
# pip install flake8 

# TODO: reduce max line length
PATH=~/.local/bin/:$PATH flake8 python/ --ignore=W503 --max-line-length=130
PATH=~/.local/bin/:$PATH doc8 python/doc --ignore D000 --max-line-length=160
