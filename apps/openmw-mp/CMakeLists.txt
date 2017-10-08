project(tes3mp-server)

option(BUILD_WITH_PAWN "Enable Pawn language" OFF)
option(ENABLE_BREAKPAD "Enable Google Breakpad for Crash reporting" OFF)

if(ENABLE_BREAKPAD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_BREAKPAD")
    if (UNIX)
        set(Breakpad_Headers "${CMAKE_SOURCE_DIR}/extern/breakpad/src/client/linux")
        set(Breakpad_Library "${CMAKE_SOURCE_DIR}/extern/breakpad/src/client/linux/libbreakpad_client.a")
    elseif(WIN32)
        set(Breakpad_Headers "${CMAKE_SOURCE_DIR}/extern/breakpad/src/client/windows")
        set(Breakpad_Library "-lbreakpad_client")
    endif (UNIX)
    include_directories(${CMAKE_SOURCE_DIR}/extern/breakpad/src ${Breakpad_Headers})
endif(ENABLE_BREAKPAD)

option(BUILD_WITH_LUA "Enable Terra/Lua language" ON)
option(FORCE_LUA "Use Lua instead Terra" OFF)


#set(Terra_ROOT ${CMAKE_SOURCE_DIR}/external/terra/)
#if(WIN32 OR FORCE_LUA)
    find_package(LuaJit REQUIRED)
#[[else()
    find_package(Terra REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_TERRA")]]
#endif()
set(LuaScript_Headers ${Terra_INCLUDES} ${LUA_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/extern/sol/sol.hpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_LUA")
include_directories(${Terra_INCLUDES} ${LUA_INCLUDE_DIR} ${LUAJIT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/extern/sol)


# local files
set(SERVER
    main.cpp
    Player.cpp Players.cpp
    Networking.cpp
    MasterClient.cpp
    Cell.cpp
    CellController.cpp
    Utils.cpp
    CharClass.cpp
    Inventory.cpp
    Settings.cpp
    Timer.cpp
    Books.cpp
    GUI.cpp
    Dialogue.cpp
    Factions.cpp
    Cells.cpp
    Quests.cpp
    Spells.cpp
    Actors.cpp
    NetActor.cpp
    CellState.cpp
    Object.cpp
    Script/CommandController.cpp Script/EventController.cpp Script/LuaState.cpp Script/luaUtils.cpp
)

set(SERVER_HEADER

)
source_group(tes3mp-server FILES ${SERVER} ${SERVER_HEADER})

set(PROCESSORS_ACTOR
        processors/actor/ProcessorActorAnimFlags.hpp processors/actor/ProcessorActorAnimPlay.hpp
        processors/actor/ProcessorActorAttack.hpp processors/actor/ProcessorActorCellChange.hpp
        processors/actor/ProcessorActorEquipment.hpp processors/actor/ProcessorActorList.hpp
        processors/actor/ProcessorActorPosition.hpp processors/actor/ProcessorActorSpeech.hpp
        processors/actor/ProcessorActorStatsDynamic.hpp processors/actor/ProcessorActorTest.hpp
        )

source_group(tes3mp-server\\processors\\actor FILES ${PROCESSORS_ACTOR})

set(PROCESSORS_PLAYER
        processors/player/ProcessorChatMsg.hpp processors/player/ProcessorGUIMessageBox.hpp
        processors/player/ProcessorGameWeather.hpp processors/player/ProcessorPlayerAnimFlags.hpp
        processors/player/ProcessorPlayerAnimPlay.hpp processors/player/ProcessorPlayerAttack.hpp
        processors/player/ProcessorPlayerAttribute.hpp processors/player/ProcessorPlayerBook.hpp
        processors/player/ProcessorPlayerBounty.hpp processors/player/ProcessorPlayerCellChange.hpp
        processors/player/ProcessorPlayerCellState.hpp processors/player/ProcessorPlayerCharClass.hpp
        processors/player/ProcessorPlayerCharGen.hpp processors/player/ProcessorPlayerDeath.hpp
        processors/player/ProcessorPlayerDisposition.hpp processors/player/ProcessorPlayerEquipment.hpp
        processors/player/ProcessorPlayerFaction.hpp processors/player/ProcessorPlayerInventory.hpp
        processors/player/ProcessorPlayerJournal.hpp processors/player/ProcessorPlayerKillCount.hpp
        processors/player/ProcessorPlayerLevel.hpp processors/player/ProcessorPlayerMap.hpp
        processors/player/ProcessorPlayerPosition.hpp processors/player/ProcessorPlayerRest.hpp
        processors/player/ProcessorPlayerResurrect.hpp processors/player/ProcessorPlayerShapeshift.hpp
        processors/player/ProcessorPlayerSkill.hpp processors/player/ProcessorPlayerSpeech.hpp
        processors/player/ProcessorPlayerSpellbook.hpp processors/player/ProcessorPlayerStatsDynamic.hpp
        processors/player/ProcessorPlayerTopic.hpp
        )

source_group(tes3mp-server\\processors\\player FILES ${PROCESSORS_PLAYER})

set(PROCESSORS_WORLD
        processors/world/ProcessorContainer.hpp processors/world/ProcessorDoorState.hpp
        processors/world/ProcessorMusicPlay.hpp processors/world/ProcessorObjectAnimPlay.hpp
        processors/world/ProcessorObjectDelete.hpp processors/world/ProcessorObjectLock.hpp
        processors/world/ProcessorObjectMove.hpp processors/world/ProcessorObjectPlace.hpp
        processors/world/ProcessorObjectRotate.hpp processors/world/ProcessorObjectScale.hpp
        processors/world/ProcessorObjectSpawn.hpp processors/world/ProcessorObjectState.hpp
        processors/world/ProcessorObjectTrap.hpp processors/world/ProcessorScriptLocalShort.hpp
        processors/world/ProcessorScriptLocalFloat.hpp processors/world/ProcessorScriptMemberShort.hpp
        processors/world/ProcessorScriptGlobalShort.hpp processors/world/ProcessorVideoPlay.hpp
        )

source_group(tes3mp-server\\processors\\world FILES ${PROCESSORS_WORLD})

set(PROCESSORS
        processors/ProcessorInitializer.cpp
        processors/PlayerProcessor.cpp
        processors/ActorProcessor.cpp
        processors/WorldProcessor.cpp
        )

source_group(tes3mp-server\\processors FILES ${PROCESSORS})

include_directories("./")

# Main executable

add_executable(tes3mp-server
        ${SERVER} ${SERVER_HEADER}
        ${PROCESSORS_ACTOR} ${PROCESSORS_PLAYER} ${PROCESSORS_WORLD} ${PROCESSORS}
        ${APPLE_BUNDLE_RESOURCES}
        )

# For Lua debugging
target_compile_definitions(tes3mp-server PRIVATE $<$<CONFIG:Debug>:SOL_SAFE_FUNCTIONS> $<$<CONFIG:RelWithDebInfo>:SOL_SAFE_FUNCTIONS>)
target_compile_definitions(tes3mp-server PRIVATE $<$<CONFIG:Debug>:SERVER_DEBUG> $<$<CONFIG:RelWithDebInfo>:SERVER_DEBUG>)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(tes3mp-server PRIVATE -std=gnu++14 -Wno-ignored-qualifiers -ftemplate-depth=2048)
endif()

target_link_libraries(tes3mp-server
    #${Boost_SYSTEM_LIBRARY}
    #${Boost_THREAD_LIBRARY}
    #${Boost_FILESYSTEM_LIBRARY}
    #${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${RakNet_LIBRARY}
    components
    ${Terra_LIBRARY}
    ${LUA_LIBRARIES}
    ${LUAJIT_LIBRARY}
    ${Breakpad_Library}
)

if (UNIX)
    target_link_libraries(tes3mp-server dl)
    # Fix for not visible pthreads functions for linker with glibc 2.15
    if(NOT APPLE)
        target_link_libraries(tes3mp-server ${CMAKE_THREAD_LIBS_INIT})
    endif(NOT APPLE)
endif(UNIX)

if (BUILD_WITH_CODE_COVERAGE)
  add_definitions (--coverage)
  target_link_libraries(tes3mp-server gcov)
endif()

if (MSVC)
    # Debug version needs increased number of sections beyond 2^16
    if (CMAKE_CL_64)
        set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
    endif (CMAKE_CL_64)
    add_definitions("-D_USE_MATH_DEFINES")
endif (MSVC)
