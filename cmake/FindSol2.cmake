# Once found, defines:
#  Sol2_FOUND
#  Sol2_INCLUDE_DIRS

include(LibFindMacros)

libfind_pkg_detect(Sol2 sol2
    FIND_PATH sol.hpp
    PATHS ${CMAKE_SOURCE_DIR}/extern/sol/single/sol
)
libfind_process(Sol2)
