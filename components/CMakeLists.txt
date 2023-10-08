project (Components)

# Version file
set (VERSION_IN_FILE "${OpenMW_SOURCE_DIR}/files/version.in")
set (VERSION_FILE_PATH_BASE "${OpenMW_BINARY_DIR}")
set (VERSION_FILE_PATH_RELATIVE resources/version)
if (GIT_CHECKOUT)
    get_generator_is_multi_config(multi_config)
    add_custom_target (git-version
       COMMAND ${CMAKE_COMMAND}
            -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
            -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
            -DVERSION_IN_FILE=${VERSION_IN_FILE}
            -DVERSION_FILE_PATH_BASE=${VERSION_FILE_PATH_BASE}
            -DVERSION_FILE_PATH_RELATIVE=${VERSION_FILE_PATH_RELATIVE}
            -DOPENMW_VERSION_MAJOR=${OPENMW_VERSION_MAJOR}
            -DOPENMW_VERSION_MINOR=${OPENMW_VERSION_MINOR}
            -DOPENMW_VERSION_RELEASE=${OPENMW_VERSION_RELEASE}
            -DOPENMW_VERSION=${OPENMW_VERSION}
            -DMACROSFILE=${CMAKE_SOURCE_DIR}/cmake/OpenMWMacros.cmake
            "-DCMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}"
            -Dgenerator_is_multi_config_var=${multi_config}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/GitVersion.cmake
            VERBATIM)
else (GIT_CHECKOUT)
    configure_resource_file(${VERSION_IN_FILE} ${VERSION_FILE_PATH_BASE} ${VERSION_FILE_PATH_RELATIVE})
endif (GIT_CHECKOUT)

# source files

add_component_dir (settings
    settings parser
    )

# Start of tes3mp change
#
# Don't include certain components in server-only builds
IF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change
add_component_dir (bsa
    bsa_file compressedbsafile memorystream
    )

add_component_dir (vfs
    manager archive bsaarchive filesystemarchive registerarchives
    )

add_component_dir (resource
    scenemanager keyframemanager imagemanager bulletshapemanager bulletshape niffilemanager objectcache multiobjectcache resourcesystem
    resourcemanager stats animation
    )

add_component_dir (shader
    shadermanager shadervisitor removedalphafunc
    )

add_component_dir (sceneutil
    clone attach visitor util statesetupdater controller skeleton riggeometry morphgeometry lightcontroller
    lightmanager lightutil positionattitudetransform workqueue unrefqueue pathgridutil waterutil writescene serialize optimizer
    actorutil detourdebugdraw navmesh agentpath shadow mwshadowtechnique recastmesh shadowsbin osgacontroller
    )

add_component_dir (nif
    controlled effect niftypes record controller extra node record_ptr data niffile property nifkey base nifstream
    )

add_component_dir (nifosg
    nifloader controller particle matrixtransform
    )

add_component_dir (nifbullet
    bulletnifloader
    )
# Start of tes3mp change
#
# Don't include certain components in server-only builds
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change

add_component_dir (to_utf8
    to_utf8
    )

add_component_dir (esm
    attr defs esmcommon esmreader esmwriter loadacti loadalch loadappa loadarmo loadbody loadbook loadbsgn loadcell
    loadclas loadclot loadcont loadcrea loaddial loaddoor loadench loadfact loadglob loadgmst
    loadinfo loadingr loadland loadlevlist loadligh loadlock loadprob loadrepa loadltex loadmgef loadmisc
    loadnpc loadpgrd loadrace loadregn loadscpt loadskil loadsndg loadsoun loadspel loadsscr loadstat
    loadweap records aipackage effectlist spelllist variant variantimp loadtes3 cellref filter
    savedgame journalentry queststate locals globalscript player objectstate cellid cellstate globalmap inventorystate containerstate npcstate creaturestate dialoguestate statstate
    npcstats creaturestats weatherstate quickkeys fogstate spellstate activespells creaturelevliststate doorstate projectilestate debugprofile
    aisequence magiceffects util custommarkerstate stolenitems transport animationstate controlsstate mappings
    )

# Start of tes3mp change
#
# Don't include certain components in server-only builds
IF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change
add_component_dir (esmterrain
    storage
    )
# Start of tes3mp change
#
# Don't include certain components in server-only builds
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change

add_component_dir (misc
    constants utf8stream stringops resourcehelpers rng messageformatparser weakcache thread
    )

add_component_dir (debug
    debugging debuglog gldebug
    )

IF(NOT WIN32 AND NOT APPLE)
    add_definitions(-DGLOBAL_DATA_PATH="${GLOBAL_DATA_PATH}")
    add_definitions(-DGLOBAL_CONFIG_PATH="${GLOBAL_CONFIG_PATH}")
ENDIF()

add_component_dir (files
    linuxpath androidpath windowspath macospath fixedpath multidircollection collections configurationmanager escape
    lowlevelfile constrainedfilestream memorystream
    )

add_component_dir (compiler
    context controlparser errorhandler exception exprparser extensions fileparser generator
    lineparser literals locals output parser scanner scriptparser skipparser streamerrorhandler
    stringparser tokenloc nullerrorhandler opcodes extensions0 declarationparser
    quickfileparser discardparser junkparser
    )

add_component_dir (interpreter
    context controlopcodes genericopcodes installopcodes interpreter localopcodes mathopcodes
    miscopcodes opcodes runtime types defines
    )

add_component_dir (translation
    translation
    )

# Start of tes3mp addition
#
# Don't include certain components in server-only builds
IF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change
add_component_dir (terrain
    storage world buffercache defs terraingrid material terraindrawable texturemanager chunkmanager compositemaprenderer quadtreeworld quadtreenode viewdata cellborder
    )

add_component_dir (loadinglistener
    loadinglistener
    )

add_component_dir (myguiplatform
    myguirendermanager myguidatamanager myguiplatform myguitexture myguiloglistener additivelayer scalinglayer
    )

add_component_dir (widgets
    box fontwrapper imagebutton tags list numericeditbox sharedstatebutton windowcaption widgets
    )

add_component_dir (fontloader
    fontloader
    )

add_component_dir (sdlutil
    gl4es_init sdlgraphicswindow imagetosurface sdlinputwrapper sdlvideowrapper events sdlcursormanager
    )
# Start of tes3mp change
#
# Don't include certain components in server-only builds
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change

add_component_dir (version
    version
    )

add_component_dir (openmw-mp
        TimedLog Utils ErrorMessages NetworkMessages Version
        )

add_component_dir (openmw-mp/Base
        BaseActor BaseObject BasePacketProcessor BasePlayer BaseStructs BaseSystem BaseWorldstate
        )

add_component_dir (openmw-mp/Controllers
        SystemPacketController PlayerPacketController ActorPacketController ObjectPacketController WorldstatePacketController
        )
         
add_component_dir(openmw-mp/Master
        MasterData PacketMasterQuery PacketMasterUpdate PacketMasterAnnounce BaseMasterPacket ProxyMasterPacket
        )

add_component_dir (openmw-mp/Packets
        BasePacket PacketPreInit
        )

add_component_dir (openmw-mp/Packets/Actor
        ActorPacket

        PacketActorList PacketActorAuthority PacketActorTest PacketActorAI PacketActorAnimFlags PacketActorAnimPlay
        PacketActorAttack PacketActorCast PacketActorCellChange PacketActorDeath PacketActorEquipment PacketActorPosition
        PacketActorSpeech PacketActorSpellsActive PacketActorStatsDynamic
        )

add_component_dir (openmw-mp/Packets/System
        SystemPacket

        PacketSystemHandshake
        )

add_component_dir (openmw-mp/Packets/Player
        PlayerPacket

        PacketChatMessage PacketGUIBoxes PacketGameSettings

        PacketPlayerBaseInfo PacketPlayerCharGen PacketPlayerAlly PacketPlayerAnimFlags PacketPlayerAnimPlay
        PacketPlayerAttack PacketPlayerAttribute PacketPlayerBehavior PacketPlayerBook PacketPlayerBounty
        PacketPlayerCast PacketPlayerCellChange PacketPlayerCellState PacketPlayerClass PacketPlayerCooldowns
        PacketPlayerDeath PacketPlayerEquipment PacketPlayerFaction PacketPlayerInput PacketPlayerInventory
        PacketPlayerItemUse PacketPlayerJail PacketPlayerJournal PacketPlayerLevel PacketPlayerMiscellaneous
        PacketPlayerMomentum PacketPlayerPosition PacketPlayerQuickKeys PacketPlayerReputation PacketPlayerRest
        PacketPlayerResurrect PacketPlayerShapeshift PacketPlayerSkill PacketPlayerSpeech PacketPlayerSpellbook
        PacketPlayerSpellsActive PacketPlayerStatsDynamic PacketPlayerTopic
        )

add_component_dir (openmw-mp/Packets/Object
        ObjectPacket

        PacketDoorDestination PacketDoorState

        PacketConsoleCommand PacketContainer PacketObjectActivate PacketObjectAnimPlay PacketObjectAttach
        PacketObjectDelete PacketObjectDialogueChoice PacketObjectHit PacketObjectLock PacketObjectMove PacketObjectPlace
        PacketObjectRestock PacketObjectRotate PacketObjectScale PacketObjectSound PacketObjectSpawn PacketObjectState
        PacketObjectTrap PacketMusicPlay PacketVideoPlay PacketClientScriptLocal PacketScriptMemberShort
        PacketObjectMiscellaneous
        )

add_component_dir (openmw-mp/Packets/Worldstate
        WorldstatePacket

        PacketCellCreate PacketCellReset PacketClientScriptGlobal PacketClientScriptSettings PacketRecordDynamic
        PacketWorldCollisionOverride PacketWorldDestinationOverride PacketWorldKillCount PacketWorldMap
        PacketWorldRegionAuthority PacketWorldTime PacketWorldWeather
        )

add_component_dir (fallback
    fallback validate
    )

# Start of tes3mp change (major)
#
# Don't require the crashcatcher when building on platforms other than Windows or when building only the server,
# as it causes compilation problems
IF (BUILD_OPENMW OR BUILD_OPENCS)
    if(WIN32)
        add_component_dir (crashcatcher
            windows_crashcatcher
            windows_crashmonitor
            windows_crashshm
        )
    endif()
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change (major)

# Start of tes3mp change (major)
#
# Don't require the DetourNavigator when building the server
IF (BUILD_OPENMW OR BUILD_OPENCS)
    add_component_dir(detournavigator
            debug
            makenavmesh
            findsmoothpath
            recastmeshbuilder
            recastmeshmanager
            cachedrecastmeshmanager
            navmeshmanager
            navigatorimpl
            asyncnavmeshupdater
            recastmesh
            tilecachedrecastmeshmanager
            recastmeshobject
            navmeshtilescache
            settings
            navigator
            findrandompointaroundcircle
            raycast
            navmeshtileview
            oscillatingrecastmeshobject
            offmeshconnectionsmanager
            )
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change (major)

set (ESM_UI ${CMAKE_SOURCE_DIR}/files/ui/contentselector.ui
    )

# Start of tes3mp change (major)
#
# Don't require Qt when building the server
IF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change
if (USE_QT)
    add_component_qt_dir (contentselector
        model/modelitem model/esmfile
        model/naturalsort model/contentmodel
        model/loadordererror
        view/combobox view/contentselector
        )
    add_component_qt_dir (config
        gamesettings
        launchersettings
        settingsbase
        )

    add_component_qt_dir (process
        processinvoker
        )

    add_component_dir (misc
        helpviewer
        )

    QT5_WRAP_UI(ESM_UI_HDR ${ESM_UI})
    QT5_WRAP_CPP(MOC_SRCS ${COMPONENT_MOC_FILES})
endif()
# Start of tes3mp change (major)
#
# Don't require Qt when building the server
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" AND NOT APPLE)
        add_definitions(-fPIC)
    endif()
endif ()

include_directories(${BULLET_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

add_library(components STATIC ${COMPONENT_FILES} ${MOC_SRCS} ${ESM_UI_HDR})

# Start of tes3mp change (major)
#
# Don't require graphics related libs when building the server
IF (BUILD_OPENMW OR BUILD_OPENCS)
target_link_libraries(components
    # CMake's built-in OSG finder does not use pkgconfig, so we have to
    # manually ensure the order is correct for inter-library dependencies.
    # This only makes a difference with `-DOPENMW_USE_SYSTEM_OSG=ON -DOSG_STATIC=ON`.
    # https://gitlab.kitware.com/cmake/cmake/-/issues/21701
    ${OSGPARTICLE_LIBRARIES}
    ${OSGVIEWER_LIBRARIES}
    ${OSGSHADOW_LIBRARIES}
    ${OSGANIMATION_LIBRARIES}
    ${OSGGA_LIBRARIES}
    ${OSGTEXT_LIBRARIES}
    ${OSGDB_LIBRARIES}
    ${OSGUTIL_LIBRARIES}
    ${OSG_LIBRARIES}
    ${OPENTHREADS_LIBRARIES}

    ${SDL2_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${MyGUI_LIBRARIES}
    LZ4::LZ4
    )
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
target_link_libraries(components
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_IOSTREAMS_LIBRARY}
    )
# End of tes3mp change (major)

# Start of tes3mp change (major)
#
# Don't require RecastNavigation nor Bullet when building the server
IF (BUILD_OPENMW OR BUILD_OPENCS)
    target_link_libraries(components
            RecastNavigation::DebugUtils
            RecastNavigation::Detour
            RecastNavigation::Recast

            ${BULLET_LIBRARIES}
            )
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
target_link_libraries(components Base64)
# End of tes3mp change (major)

if (WIN32)
    target_link_libraries(components
    ${Boost_LOCALE_LIBRARY}
    ${Boost_ZLIB_LIBRARY})
endif()

# Start of tes3mp change (major)
#
# Don't require Qt when building the server
IF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change
if (USE_QT)
    target_link_libraries(components Qt5::Widgets Qt5::Core)
endif()
# Start of tes3mp change (major)
#
# Don't require Qt when building the server
ENDIF (BUILD_OPENMW OR BUILD_OPENCS)
# End of tes3mp change

if (GIT_CHECKOUT)
    add_dependencies (components git-version)
endif (GIT_CHECKOUT)

if (WIN32)
    target_link_libraries(components shlwapi)
endif()

# Fix for not visible pthreads functions for linker with glibc 2.15
if (UNIX AND NOT APPLE)
target_link_libraries(components ${CMAKE_THREAD_LIBS_INIT})
endif()

if (BUILD_WITH_CODE_COVERAGE)
    add_definitions(--coverage)
    target_link_libraries(components gcov)
endif()


# Make the variable accessible for other subdirectories
set(COMPONENT_FILES ${COMPONENT_FILES} PARENT_SCOPE)

target_compile_definitions(components PUBLIC BT_USE_DOUBLE_PRECISION)
