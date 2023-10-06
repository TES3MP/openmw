# Comes form project edunetgames
# - Try to find RakNet
# Once done this will define
#
#  RakNet_FOUND - system has RakNet
#  RakNet_INCLUDES - the RakNet include directory
#  RakNet_LIBRARY - Link these to use RakNet

IF(WIN32)
  set(RakNet_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/extern/raknet/lib/libRakNetLibStaticd.lib)
  set(RakNet_LIBRARY_RELEASE ${CMAKE_SOURCE_DIR}/extern/raknet/lib/libRakNetLibStatic.lib)
ELSE()
  set(RakNet_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/extern/raknet/lib/libRakNetLibStaticd.a)
  set(RakNet_LIBRARY_RELEASE ${CMAKE_SOURCE_DIR}/extern/raknet/lib/libRakNetLibStatic.a)
ENDIF(WIN32)

FIND_PATH (RakNet_INCLUDES raknet/RakPeer.h ${CMAKE_SOURCE_DIR}/extern/raknet/include)

MESSAGE(STATUS ${RakNet_INCLUDES})
MESSAGE(STATUS ${RakNet_LIBRARY_RELEASE})
MESSAGE(STATUS ${RakNet_LIBRARY_DEBUG})

IF(RakNet_FIND_REQUIRED)
	MESSAGE(STATUS "Could not find RakNet, building from local copy")

	find_package(Git QUIET)

	if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
	  option(GIT_SUBMODULE "Check submodules during build" ON)
	  if(GIT_SUBMODULE)
		message(STATUS "Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/extern/raknet
		RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	   endif()
        endif()

	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/raknet/CMakeLists.txt")
	       message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
	endif()
ENDIF(RakNet_FIND_REQUIRED)

add_subdirectory(extern/raknet)

SET(RakNet_INCLUDES ${CMAKE_SOURCE_DIR}/extern/raknet/include/raknet)

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