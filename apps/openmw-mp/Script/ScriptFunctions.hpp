#ifndef SCRIPTFUNCTIONS_HPP
#define SCRIPTFUNCTIONS_HPP

#include <Script/Functions/Actors.hpp>
#include <Script/Functions/Books.hpp>
#include <Script/Functions/Cells.hpp>
#include <Script/Functions/CharClass.hpp>
#include <Script/Functions/Chat.hpp>
#include <Script/Functions/Dialogue.hpp>
#include <Script/Functions/Factions.hpp>
#include <Script/Functions/GUI.hpp>
#include <Script/Functions/Items.hpp>
#include <Script/Functions/Mechanics.hpp>
#include <Script/Functions/Miscellaneous.hpp>
#include <Script/Functions/Objects.hpp>
#include <Script/Functions/Positions.hpp>
#include <Script/Functions/Quests.hpp>
#include <Script/Functions/RecordsDynamic.hpp>
#include <Script/Functions/Shapeshift.hpp>
#include <Script/Functions/Server.hpp>
#include <Script/Functions/Settings.hpp>
#include <Script/Functions/Spells.hpp>
#include <Script/Functions/Stats.hpp>
#include <Script/Functions/Worldstate.hpp>
#include <RakNetTypes.h>
#include <tuple>
#include <apps/openmw-mp/Player.hpp>
#include "ScriptFunction.hpp"
#include "Types.hpp"

#include <components/openmw-mp/MWMPLog.hpp>

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define GET_PLAYER(pid, pl, retvalue) \
     pl = Players::getPlayer(pid); \
     if (player == 0) {\
        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_ERROR, "%s: Player with pid \'%d\' not found\n", __PRETTY_FUNCTION__, pid);\
        /*ScriptFunctions::StopServer(1);*/ \
        return retvalue;\
}


class ScriptFunctions
{
public:

    static void MakePublic(ScriptFunc _public, const char *name, char ret_type, const char *def) noexcept;
    static boost::any CallPublic(const char *name, va_list args) noexcept;

     /**
     * \brief Create a timer that will run a script function after a certain interval.
     *
     * \param callback The Lua script function.
     * \param msec The interval in miliseconds.
     * \return The ID of the timer thus created.
     */
    static int CreateTimer(ScriptFunc callback, int msec) noexcept;

    /**
    * \brief Create a timer that will run a script function after a certain interval and pass
    *        certain arguments to it.
    *
    * Example usage:
    * - tes3mp.CreateTimerEx("OnTimerTest1", 250, "i", 90)
    * - tes3mp.CreateTimerEx("OnTimerTest2", 500, "sif", "Test string", 60, 77.321)
    *
    * \param callback The Lua script function.
    * \param msec The interval in miliseconds.
    * \param types The argument types.
    * \param args The arguments.
    * \return The ID of the timer thus created.
    */
    static int CreateTimerEx(ScriptFunc callback, int msec, const char *types, va_list args) noexcept;

    /**
    * \brief Start the timer with a certain ID.
    *
    * \param timerId The timer ID.
    * \return void
    */
    static void StartTimer(int timerId) noexcept;

    /**
    * \brief Stop the timer with a certain ID.
    *
    * \param timerId The timer ID.
    * \return void
    */
    static void StopTimer(int timerId) noexcept;

    /**
    * \brief Restart the timer with a certain ID for a certain interval.
    *
    * \param timerId The timer ID.
    * \param msec The interval in miliseconds.
    * \return void
    */
    static void RestartTimer(int timerId, int msec) noexcept;

    /**
    * \brief Free the timer with a certain ID.
    *
    * \param timerId The timer ID.
    * \return void
    */
    static void FreeTimer(int timerId) noexcept;

    /**
    * \brief Check whether a timer is elapsed.
    *
    * \param timerId The timer ID.
    * \return Whether the timer is elapsed.
    */
    static bool IsTimerElapsed(int timerId) noexcept;


    static constexpr ScriptFunctionData functions[]{
            {"CreateTimer",         ScriptFunctions::CreateTimer},
            {"CreateTimerEx",       ScriptFunctions::CreateTimerEx},
            {"MakePublic",          ScriptFunctions::MakePublic},
            {"CallPublic",          ScriptFunctions::CallPublic},

            {"StartTimer",          ScriptFunctions::StartTimer},
            {"StopTimer",           ScriptFunctions::StopTimer},
            {"RestartTimer",        ScriptFunctions::RestartTimer},
            {"FreeTimer",           ScriptFunctions::FreeTimer},
            {"IsTimerElapsed",      ScriptFunctions::IsTimerElapsed},

            ACTORAPI,
            BOOKAPI,
            CELLAPI,
            CHARCLASSAPI,
            CHATAPI,
            DIALOGUEAPI,
            FACTIONAPI,
            GUIAPI,
            ITEMAPI,
            MECHANICSAPI,
            MISCELLANEOUSAPI,
            POSITIONAPI,
            QUESTAPI,
            RECORDSDYNAMICAPI,
            SHAPESHIFTAPI,
            SERVERAPI,
            SETTINGSAPI,
            SPELLAPI,
            STATAPI,
            OBJECTAPI,
            WORLDSTATEAPI
    };

    static constexpr ScriptCallbackData callbacks[]{
            {"OnServerInit",             Callback<>()},
            {"OnServerPostInit",         Callback<>()},
            {"OnServerExit",             Callback<bool>()},
            {"OnServerScriptCrash",      Callback<const char*>()},
            {"OnPlayerConnect",          Callback<unsigned short>()},
            {"OnPlayerDisconnect",       Callback<unsigned short>()},
            {"OnPlayerDeath",            Callback<unsigned short>()},
            {"OnPlayerResurrect",        Callback<unsigned short>()},
            {"OnPlayerCellChange",       Callback<unsigned short>()},
            {"OnPlayerAttribute",        Callback<unsigned short>()},
            {"OnPlayerSkill",            Callback<unsigned short>()},
            {"OnPlayerLevel",            Callback<unsigned short>()},
            {"OnPlayerBounty",           Callback<unsigned short>()},
            {"OnPlayerReputation",       Callback<unsigned short>()},
            {"OnPlayerEquipment",        Callback<unsigned short>()},
            {"OnPlayerInventory",        Callback<unsigned short>()},
            {"OnPlayerJournal",          Callback<unsigned short>()},
            {"OnPlayerFaction",          Callback<unsigned short>()},
            {"OnPlayerShapeshift",       Callback<unsigned short>()},
            {"OnPlayerSpellbook",        Callback<unsigned short>()},
            {"OnPlayerQuickKeys",        Callback<unsigned short>()},
            {"OnPlayerTopic",            Callback<unsigned short>()},
            {"OnPlayerDisposition",      Callback<unsigned short>()},
            {"OnPlayerBook",             Callback<unsigned short>()},
            {"OnPlayerItemUse",          Callback<unsigned short>()},
            {"OnPlayerMiscellaneous",    Callback<unsigned short>()},
            {"OnPlayerInput",            Callback<unsigned short>()},
            {"OnPlayerRest",             Callback<unsigned short>()},
            {"OnRecordDynamic",          Callback<unsigned short>()},
            {"OnCellLoad",               Callback<unsigned short, const char*>()},
            {"OnCellUnload",             Callback<unsigned short, const char*>()},
            {"OnCellDeletion",           Callback<const char*>()},
            {"OnContainer",              Callback<unsigned short, const char*>()},
            {"OnDoorState",              Callback<unsigned short, const char*>()},
            {"OnObjectActivate",         Callback<unsigned short, const char*>()},
            {"OnObjectPlace",            Callback<unsigned short, const char*>()},
            {"OnObjectState",            Callback<unsigned short, const char*>()},
            {"OnObjectSpawn",            Callback<unsigned short, const char*>()},
            {"OnObjectDelete",           Callback<unsigned short, const char*>()},
            {"OnObjectLock",             Callback<unsigned short, const char*>()},
            {"OnObjectScale",            Callback<unsigned short, const char*>()},
            {"OnObjectTrap",             Callback<unsigned short, const char*>()},
            {"OnVideoPlay",              Callback<unsigned short, const char*>()},
            {"OnActorList",              Callback<unsigned short, const char*>()},
            {"OnActorEquipment",         Callback<unsigned short, const char*>()},
            {"OnActorAI",                Callback<unsigned short, const char*>()},
            {"OnActorDeath",             Callback<unsigned short, const char*>()},
            {"OnActorCellChange",        Callback<unsigned short, const char*>()},
            {"OnActorTest",              Callback<unsigned short, const char*>()},
            {"OnPlayerSendMessage",      Callback<unsigned short, const char*>()},
            {"OnPlayerEndCharGen",       Callback<unsigned short>()},
            {"OnGUIAction",              Callback<unsigned short, int, const char*>()},
            {"OnWorldKillCount",         Callback<unsigned short>()},
            {"OnWorldMap",               Callback<unsigned short>()},
            {"OnWorldWeather",           Callback<unsigned short>() },
            {"OnMpNumIncrement",         Callback<int>()},
            {"OnRequestDataFileList",    Callback<>()}
    };
};

#endif //SCRIPTFUNCTIONS_HPP
