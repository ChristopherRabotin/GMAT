################
#
# README file about FindMatlab.cmake module
# By: Ravi Mathur
# Date: 2/27/2015
#
################

As of CMake 3.0.2, the built-in FindMatlab.cmake module is completely unusable. The CMake development repo has a fully rewritten version of FindMatlab.cmake, which is included here. This comes from commit [0c2d530] of the CMake Git repo.

Once the new version of FindMatlab.cmake is included in CMake, the file included here can be removed from the GMAT repo. Note that this can only be done once all GMAT users are able to use the appropriate CMake version. For example, Ubuntu Linux often ships with an old version of CMake, so this must be taken into account.
