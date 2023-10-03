# Comes form project edunetgames
# - Try to find RakNet
# Once done this will define
#
#  RakNet_FOUND - system has RakNet
#  RakNet_INCLUDES - the RakNet include directory
#  RakNet_LIBRARY - Link these to use RakNet

FIND_LIBRARY (RakNet_LIBRARY_RELEASE NAMES RakNetLibStatic
    PATHS
    ENV LD_LIBRARY_PATH
    ENV LIBRARY_PATH
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /opt/local/lib
    $ENV{RAKNET_ROOT}/lib
    $ENV{RAKNET_ROOT}/build/lib/Release
    ${RAKNET_ROOT}/build/lib/Release
    )
	
FIND_LIBRARY (RakNet_LIBRARY_DEBUG NAMES RakNetLibStaticd
    PATHS
    ENV LD_LIBRARY_PATH
    ENV LIBRARY_PATH
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /opt/local/lib
    $ENV{RAKNET_ROOT}/lib
    $ENV{RAKNET_ROOT}/build/lib/Debug
    ${RAKNET_ROOT}/build/lib/Debug
    ${CMAKE_SOURCE_DIR}/extern/raknet/lib/
    ${CMAKE_SOURCE_DIR}/extern/raknet/build/lib
    ${CMAKE_SOURCE_DIR}/extern/raknet/Debug/lib/
    ${CMAKE_SOURCE_DIR}/extern/raknet/build/Debug/lib/
    )


set(RakNet_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/extern/raknet/lib/libRakNetLibStaticd.a)
set(RakNet_LIBRARY_RELEASE ${CMAKE_SOURCE_DIR}/extern/raknet/lib/libRakNetLibStatic.a)

	

FIND_PATH (RakNet_INCLUDES raknet/RakPeer.h ${CMAKE_SOURCE_DIR}/extern/raknet/include)
 
MESSAGE(STATUS ${RakNet_INCLUDES})
MESSAGE(STATUS ${RakNet_LIBRARY_RELEASE})
MESSAGE(STATUS ${RakNet_LIBRARY_DEBUG})

IF(RakNet_INCLUDES)
    SET(RakNet_FOUND TRUE)
ENDIF(RakNet_INCLUDES)

IF(NOT RakNet_FOUND)
  IF(RakNet_FIND_REQUIRED)
    MESSAGE(STATUS "Could not find RakNet, building from local copy")

    find_package(Git QUIET)

    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
      # Update submodules as needed
      option(GIT_SUBMODULE "Check submodules during build" ON)
      if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extern/raknet
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
      endif()
    endif()

    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/raknet/CMakeLists.txt")
      message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    endif()
  ENDIF(RakNet_FIND_REQUIRED)
ENDIF(NOT RakNet_FOUND)

add_subdirectory(extern/raknet)

SET(RakNet_INCLUDES ${RakNet_INCLUDES}/raknet)
  
IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
   SET(RakNet_LIBRARY optimized ${RakNet_LIBRARY_RELEASE} debug ${RakNet_LIBRARY_DEBUG})
   IF(WIN32)
    SET(RakNet_LIBRARY optimized ${RakNet_LIBRARY_RELEASE} debug ${RakNet_LIBRARY_DEBUG} ws2_32.lib)
   ENDIF(WIN32)
ELSE()
  # if there are no configuration types and CMAKE_BUILD_TYPE has no value
  # then just use the release libraries
  SET(RakNet_LIBRARY ${RakNet_LIBRARY_RELEASE} )
  IF(WIN32)
    SET(RakNet_LIBRARY ${RakNet_LIBRARY_RELEASE} ws2_32.lib)
  ENDIF(WIN32)
ENDIF()