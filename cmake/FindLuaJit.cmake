# Once found, defines:
#  LuaJit_FOUND
<<<<<<< HEAD
#  LuaJit_INCLUDE_DIRS
=======
#  LuaJit_INCLUDE_DIR
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
#  LuaJit_LIBRARIES

include(LibFindMacros)

libfind_pkg_detect(LuaJit luajit
<<<<<<< HEAD
        FIND_PATH luajit.h
        PATH_SUFFIXES luajit
        FIND_LIBRARY NAMES luajit-5.1 luajit
        )

libfind_process(LuaJit)
=======
        FIND_PATH luajit.h PATH_SUFFIXES luajit luajit-2.1
        FIND_LIBRARY luajit-5.1 luajit
        )

libfind_process(LuaJit)

>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
