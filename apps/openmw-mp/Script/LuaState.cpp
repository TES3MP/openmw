//
// Created by koncord on 01.08.17.
//

#include <iostream>
#include <cstdlib>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <components/misc/stringops.hpp>
#include <apps/openmw/mwworld/inventorystore.hpp>
#include <apps/openmw-mp/Networking.hpp>
#include <components/openmw-mp/Version.hpp>
#include <apps/openmw-mp/GUI.hpp>
#include <apps/openmw-mp/Dialogue.hpp>
#include <apps/openmw-mp/Factions.hpp>
#include <apps/openmw-mp/Quests.hpp>
#include <apps/openmw-mp/Spells.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>
#include "LuaState.hpp"
#include "EventController.hpp"
#include "luaUtils.hpp"
#include "CommandController.hpp"
#include "../Inventory.hpp"
#include "../Settings.hpp"
#include "../Players.hpp"
#include "../Networking.hpp"
#include "../MasterClient.hpp"

using namespace std;

LuaState::LuaState()
{
    lua = make_shared<sol::state>();
    lua->open_libraries();
    LuaUtils::Init(*this);

    actorCtrl = make_unique<ActorController>();
    ActorController::Init(*this);

    eventCtrl = make_unique<EventController>(this);
    EventController::Init(*this);

    objectCtrl = make_unique<ObjectController>();
    ObjectController::Init(*this);

    cmdCtrl = make_unique<CommandController>();
    Player::Init(*this);
    CharClass::Init(*this);
    Inventory::Init(*this);
    GameSettings::Init(*this);
    Books::Init(*this);
    GUI::Init(*this);
    Dialogue::Init(*this);
    Factions::Init(*this);
    Faction::Init(*this);
    JournalItem::Init(*this);
    Quests::Init(*this);
    Effect::Init(*this);
    Spell::Init(*this);
    Spells::Init(*this);

    Players::Init(*this);

    timerCtrl = make_unique<TimerController>();
    TimerController::Init(*this);
    Timer::Init(*this);

    CommandController::Init(*this);

    dataEnv = sol::environment(*lua, sol::create, lua->globals());
    lua->set("Data", dataEnv); // plain global environment for communicating between mods
    auto coreTable = dataEnv.create("Core");
    coreTable["VERSION"] = TES3MP_VERSION;
    coreTable["PROTOCOL"] = TES3MP_PROTO_VERSION;
    coreTable["loadedMods"] = coreTable.create();

    configEnv = sol::environment(*lua, sol::create, lua->globals());
    lua->set("Config", configEnv); // plain global environment for mod configuration

    // errors in sol::functions catches only in debug build for better performance
#ifdef SOL_SAFE_FUNCTIONS
    lua->set_function("ErrorHandler", [](sol::object error_msg) {
        LOG_MESSAGE_SIMPLE(Log::LOG_ERROR, ("Lua: " + error_msg.as<string>()).c_str());
    });

    sol::reference errHandler = (*lua)["ErrorHandler"];
    sol::protected_function::set_default_handler(errHandler);
#endif

    sol::table Constants = lua->create_named_table("Constants");
    
    Constants.set_function("getAttributeCount", []() {
        return ESM::Attribute::Length;
    });
    
    Constants.set_function("getSkillCount", []() {
        return ESM::Skill::Length;
    });

    Constants.set_function("getAttributeId", [](const string &name) {
        for (int x = 0; x < ESM::Attribute::Length; x++)
            if (Misc::StringUtils::ciEqual(name, ESM::Attribute::sAttributeNames[x]))
                return x;
        return -1;
    });

    Constants.set_function("getSkillId", [](const string &name) {
        for (int x = 0; x < ESM::Skill::Length; x++)
            if (Misc::StringUtils::ciEqual(name, ESM::Skill::sSkillNames[x]))
                return x;
        return -1;
    });

    Constants.set_function("getAttributeName", [](unsigned short attribute) -> const string {
        if (attribute >= ESM::Attribute::Length)
            return "invalid";

        return ESM::Attribute::sAttributeNames[attribute];
    });

    Constants.set_function("getSkillName", [](unsigned short skill)  -> const string {
        if (skill >= ESM::Skill::Length)
            return "invalid";

        return ESM::Skill::sSkillNames[skill];
    });

    Constants.set_function("getEquipmentSize", []() {
        return MWWorld::InventoryStore::Slots;
    });


    lua->set_function("getCurrentMpNum", []() {
        return mwmp::Networking::getPtr()->getCurrentMpNum();
    });
    lua->set_function("setCurrentMpNum", [](int num) {
        mwmp::Networking::getPtr()->setCurrentMpNum(num);
    });

    lua->set_function("logMessage", [](unsigned short level, const char *message) {
        LOG_MESSAGE_SIMPLE(level, "[Script]: %s", message);
    });
    lua->set_function("logAppend", [](unsigned short level, const char *message) {
        LOG_APPEND(level, "%s", message);
    });

    lua->set_function("stopServer", [](int code) {
        mwmp::Networking::getPtr()->stopServer(code);
    });

    lua->set_function("banAddress", [](const char *ipAddress) {
        mwmp::Networking::getPtr()->banAddress(ipAddress);
    });

    lua->set_function("unbanAddress", [](const char *ipAddress) {
        mwmp::Networking::getPtr()->unbanAddress(ipAddress);
    });

    lua->set_function("setRuleValue", [](const std::string &key, sol::object value) {
        if (!value.valid())
            return;

        auto mc = mwmp::Networking::getPtr()->getMasterClient();
        if (mc)
        {
            sol::type type = value.get_type();
            if (type == sol::type::string)
                mc->SetRuleString(key, value.as<string>());
            if (type == sol::type::number)
                mc->SetRuleValue(key, value.as<double>());
        }
    });

    lua->set_function("setModname", [](const std::string &modname) {
        auto mc = mwmp::Networking::getPtr()->getMasterClient();
        if (mc)
            mc->SetModname(modname);
    });

    lua->set_function("setHostname", [](const std::string &hostname) {
        auto mc = mwmp::Networking::getPtr()->getMasterClient();
        if (mc)
            mc->SetHostname(hostname);
    });

    lua->set_function("setServerPassword", [](const std::string &passw) {
        mwmp::Networking::getPtr()->setServerPassword(passw);
    });

    lua->set_function("setHour", [](double hour) {
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GAME_TIME);
        Players::for_each([&hour, &packet](std::shared_ptr<Player> player){
            player->hour = hour;
            player->month = -1;
            player->day = -1;

            packet->setPlayer(player.get());
            packet->Send(false);
        });
    });

    lua->set_function("setMonth", [](int month) {

        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GAME_TIME);
        Players::for_each([&month, &packet](std::shared_ptr<Player> player){
            player->hour = -1;
            player->month = month;
            player->day = -1;

            packet->setPlayer(player.get());
            packet->Send(false);
        });
    });

    lua->set_function("setDay", [](int day) {
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GAME_TIME);
        Players::for_each([&day, &packet](std::shared_ptr<Player> player){
            player->hour = -1;
            player->month = -1;
            player->day = day;

            packet->setPlayer(player.get());
            packet->Send(false);
        });
    });
}

sol::environment LuaState::openScript(std::string homePath, std::string modname)
{
    cout << "Loading file: " << homePath + "/mods/" + modname + "/main.lua" << endl;
    sol::environment env(*lua, sol::create, lua->globals());
    std::string package_path = env["package"]["path"];
    env["package"]["path"] = Utils::convertPath(homePath + "/mods/" + modname + "/?.lua") + ";" + package_path;
    package_path = env["package"]["path"];
    //env["ModInfo"] = sol::create;

    lua->set_function("getDataFolder", [homePath, modname]() -> const string {
        return homePath + "/data/" + modname + '/';
    });

    lua->set_function("getModFolder", [homePath, modname]() -> const string{
        return homePath + "/mods/" + modname + '/';
    });

    lua->script_file(homePath + "/mods/" + modname + "/main.lua", env);

    sol::table modinfo = env["ModInfo"];
    if (!modinfo.valid())
        throw runtime_error("ModInfo table for \"" + modname + "\" not found");

    if (modinfo["name"].get_type() != sol::type::string)
        throw runtime_error(modname + R"(: ModInfo["name"] undefined or not string")");

    if (modinfo["version"].get_type() != sol::type::string)
        throw runtime_error(modname + R"(: ModInfo["version"] undefined or not string")");

    sol::table dependencies = modinfo["dependencies"];

    if (dependencies.get_type() != sol::type::table)
        throw runtime_error(modname + R"(: ModInfo["dependencies"] undefined or not table")");

    string name = env["ModInfo"]["name"];

    mods.emplace(name, env);
    sol::table loaded = dataEnv["Core"]["loadedMods"];
    loaded.add(name);

    cout << "modname: " << name << endl;

    return env;
}

void LuaState::addGlobalPackagePath(const std::string &path)
{
    std::string package_path = (*lua)["package"]["path"];
    (*lua)["package"]["path"] = Utils::convertPath(path) + ";" + package_path;
}

void LuaState::addGlobalCPath(const std::string &path)
{
    std::string cpath = (*lua)["package"]["cpath"];
    (*lua)["package"]["cpath"] = Utils::convertPath(path) + ";" + cpath;
}

CommandController &LuaState::getCmdCtrl()
{
    return *cmdCtrl;
}

EventController &LuaState::getEventCtrl()
{
    return *eventCtrl;
}

TimerController &LuaState::getTimerCtrl()
{
    return *timerCtrl;
}

ActorController &LuaState::getActorCtrl()
{
    return *actorCtrl;
}

ObjectController &LuaState::getObjectCtrl()
{
    return *objectCtrl;
}

int CompVersion(const string &wishVersion, const string &depVersionFound)
{
    unsigned startVer = 0;

    if (wishVersion[0] == '>')
    {
        startVer = 1;

        if (wishVersion[1] == '=')
            startVer = 2;
    }

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(".");
    tokenizer tokensWish(wishVersion.begin() + startVer, wishVersion.end(), sep), tokensFound(depVersionFound, sep);

    auto wishIter = tokensWish.begin();
    auto founditer = tokensFound.begin();
    int bellowExpected = 0;

    for (int i = 0; i < 3; ++i)
    {
        auto v1 = boost::lexical_cast<int>(*(wishIter++));
        auto v2 = boost::lexical_cast<int>(*(founditer++));

        if (v2 > v1 && startVer > 0)
            return 0; // found applicable version

        if (v2 <= v1)
        {
            if (v2 == v1 && startVer == 1)
                bellowExpected++;
            else if (v2 < v1)
                return -1; // found version below expected
            continue;
        }

        return 1; // version higher than expected
    }

    if (bellowExpected == 3)
        return -1; // found version below expected

    return 0;
}

void LuaState::loadMods()
{
    using namespace boost::filesystem;

    auto funcLoadMods = [this](path path) {
        const auto mainScript = "main.lua";
        if (!is_directory(path / "mods"))
            throw runtime_error(path.string() + ": No such directory.");
        for (const auto &modDir : directory_iterator(path / "mods"))
        {
            if (is_directory(modDir.status()) && exists(modDir.path() / mainScript))
                openScript(path.string(), modDir.path().filename().string());
        }
    };


#ifdef _WIN32
    const char* libExt = ".dll";
#else
    const char* libExt = ".so";
#endif


    path envServerDir = std::getenv("TES3MP_SERVER_DIR");
    const char *envServerUserDir = std::getenv("TES3MP_SERVER_USERDIR");

    if (envServerDir.empty())
    {
        envServerDir = current_path();
        setenv("TES3MP_SERVER_DIR", envServerDir.string().c_str(), 1);
    }

    addGlobalPackagePath(envServerDir.string() + "/lib/lua/?/init.lua;" + envServerDir.string() + "/lib/lua/?.lua");
    addGlobalCPath(envServerDir.string() + "lib/?" + libExt);
    funcLoadMods(envServerDir);


    if (envServerUserDir != nullptr)
    {
        funcLoadMods(envServerUserDir);
        addGlobalPackagePath(string(envServerUserDir) + "/lib/lua/?/init.lua;" + string(envServerUserDir) + "/lib/lua/?.lua");

    }

    for (auto &mod : mods)
    {
        const string &modName = mod.first;
        sol::environment &modEnv = mod.second;
        sol::table dependencies = modEnv["ModInfo"]["dependencies"];
        for (auto &dependency : dependencies)
        {
            int idx = dependency.first.as<int>();
            sol::table dep = dependency.second;
            const std::string depNameRequest = dep[1];
            const std::string depVersionRequest = dep[2];

            cout << "\"" << idx << "\": \"" << depNameRequest << "\" \"" << depVersionRequest << "\"" << endl;
            const auto &depEnvIt = mods.find(depNameRequest);
            if (depEnvIt != mods.end())
            {
                string depVersionFound = depEnvIt->second["ModInfo"]["version"];
                if (CompVersion(depVersionRequest, depVersionFound) != 0)
                {
                    stringstream sstr;
                    sstr << depNameRequest << ": version \"" << depVersionFound
                         << "\" is not applicable for \"" << modName << "\" expected \""
                         << depVersionRequest + "\"";
                    throw runtime_error(sstr.str());
                }
            }
            else
            {
                stringstream sstr;
                sstr << depNameRequest + " \"" << depVersionRequest << "\" not found.";
                throw runtime_error(sstr.str());
            }
        }
    }

    dataEnv["Core"]["LOADED_MODS"] = mods.size();
}