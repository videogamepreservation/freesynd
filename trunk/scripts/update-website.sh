#!/bin/bash

# This script simply updates the current website code from the svn repository
# $Id$

rm -rf /home/groups/f/fr/freesynd/htdocs/* && svn export https://svn.sourceforge.net/svnroot/freesynd/trunk/www /home/groups/f/fr/freesynd/htdocs
