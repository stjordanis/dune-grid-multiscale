# This file is part of the dune-grid-multiscale project:
#   http://users.dune-project.org/projects/dune-grid-multiscale
# Copyright holders: Felix Albrecht
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

dnl -*- autoconf -*-
# Macros needed to find dune-grid-multiscale and dependent libraries.  They are called by
# the macros in ${top_src_dir}/dependencies.m4, which is generated by
# "dunecontrol autogen"

# Additional checks needed to build dune-grid-multiscale
# This macro should be invoked by every module which depends on dune-grid-multiscale, as
# well as by dune-grid-multiscale itself
AC_DEFUN([DUNE_GRID_MULTISCALE_CHECKS],
[
])
# Additional checks needed to find dune-grid-multiscale
# This macro should be invoked by every module which depends on dune-grid-multiscale, but
# not by dune-grid-multiscale itself
AC_DEFUN([DUNE_GRID_MULTISCALE_CHECK_MODULE],
[
  DUNE_CHECK_MODULES([dune-grid-multiscale],[grid/multiscale/exists.hh])
])
