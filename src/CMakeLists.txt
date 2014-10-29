# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the GMAT source directory.
# That is, we should have the directory structure:
#   ./base 
#   ./console
#   ./gui
#
# Original Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
# Modified for CMake standardization by: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
# GMAT base library
SUBDIRS(base)

# ====================================================================
# Console binary
SUBDIRS(console)

# ====================================================================
# GUI binary
if (GUI_BUILD)
  SUBDIRS(gui)
endif()
