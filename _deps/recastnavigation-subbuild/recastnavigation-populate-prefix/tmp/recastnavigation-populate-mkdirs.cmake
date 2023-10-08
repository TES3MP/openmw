# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/hebi/Documents/dw/extern/fetched/recastnavigation"
  "/home/hebi/Documents/dw/_deps/recastnavigation-build"
  "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix"
  "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix/tmp"
  "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix/src/recastnavigation-populate-stamp"
  "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix/src"
  "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix/src/recastnavigation-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix/src/recastnavigation-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/hebi/Documents/dw/_deps/recastnavigation-subbuild/recastnavigation-populate-prefix/src/recastnavigation-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
