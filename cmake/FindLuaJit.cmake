include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(LUAJIT_PKGCONF luajit)

find_path(LUAJIT_INCLUDE_DIR
        NAMES luajit.h
        PATHS ${LUAJIT_PKGCONF_INCLUDE_DIRS}
        )

find_library(LUAJIT_LIBRARY
        NAMES luajit-5.1 luajit
        PATHS ${LUAJIT_PKGCONF_LIBRARY_DIRS}
        )

set(LUAJIT_PROCESS_INCLUDES LUAJIT_INCLUDE_DIR)
set(LUAJIT_PROCESS_LIBS LUAJIT_LIBRARY)

libfind_process(LuaJit)

set(LUA_LIBRARY LUAJIT_LIBRARY)
set(LUA_INCLUDE_DIR LUAJIT_INCLUDE_DIR)