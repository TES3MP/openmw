#include <components/debug/debugging.hpp>
#include <components/fallback/fallback.hpp>
#include <components/fallback/validate.hpp>
#include <components/files/configurationmanager.hpp>
#include <components/misc/rng.hpp>
#include <components/platform/platform.hpp>
#include <components/version/version.hpp>

#include "mwgui/debugwindow.hpp"

#include "engine.hpp"
#include "options.hpp"

#include <boost/program_options/variables_map.hpp>

/*
    Start of tes3mp addition

    Include the header of the multiplayer's Main class
*/
#include "mwmp/Main.hpp"
/*
    End of tes3mp addition
*/

#if defined(_WIN32)
#include <components/windows.hpp>
// makes __argc and __argv available on windows
#include <cstdlib>

extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
#endif

#include <filesystem>

#if (defined(__APPLE__) || defined(__linux) || defined(__unix) || defined(__posix))
#include <unistd.h>
#endif

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/ErrorMessages.hpp>
#include <components/openmw-mp/TimedLog.hpp>
#include <components/openmw-mp/Utils.hpp>
#include <components/openmw-mp/Version.hpp>
/*
    End of tes3mp addition
*/


using namespace Fallback;

/**
 * \brief Parses application command line and calls \ref Cfg::ConfigurationManager
 * to parse configuration files.
 *
 * Results are directly written to \ref Engine class.
 *
 * \retval true - Everything goes OK
 * \retval false - Error
 */
bool parseOptions(int argc, char** argv, OMW::Engine& engine, Files::ConfigurationManager& cfgMgr)
{
    // Create a local alias for brevity
    namespace bpo = boost::program_options;
    typedef std::vector<std::string> StringsVector;

    bpo::options_description desc = OpenMW::makeOptionsDescription();
    /*
        Start of tes3mp addition

        Parse options added by multiplayer
    */
    mwmp::Main::optionsDesc(&desc);
    /*
        End of tes3mp addition
    */
    bpo::variables_map variables;

    Files::parseArgs(argc, argv, variables, desc);
    bpo::notify(variables);

    if (variables.count("help"))
    {
        getRawStdout() << desc << std::endl;
        return false;
    }

    if (variables.count("version"))
    {
        cfgMgr.readConfiguration(variables, desc, true);

        Version::Version v
            = Version::getOpenmwVersion(variables["resources"]
                                            .as<Files::MaybeQuotedPath>()
                                            .u8string()); // This call to u8string is redundant, but required to build
                                                          // on MSVC 14.26 due to implementation bugs.
        getRawStdout() << v.describe() << std::endl;
        return false;
    }

    cfgMgr.readConfiguration(variables, desc);

    setupLogging(cfgMgr.getLogPath(), "OpenMW");

    Version::Version v
        = Version::getOpenmwVersion(variables["resources"]
                                        .as<Files::MaybeQuotedPath>()
                                        .u8string()); // This call to u8string is redundant, but required to build on
                                                      // MSVC 14.26 due to implementation bugs.
    Log(Debug::Info) << v.describe();

    /*
        Start of tes3mp addition

        Print the multiplayer version first
    */
    Log(Debug::Info) << Utils::getVersionInfo("TES3MP client", TES3MP_VERSION, v.mCommitHash, TES3MP_PROTO_VERSION);
    /*
        End of tes3mp addition
    */

    /*
        Start of tes3mp change (minor)

        Because there is no need to print the commit hash again, only print OpenMW's version
    */
    Log(Debug::Info) << "OpenMW version " << v.mVersion;
    /*
        End of tes3mp change (minor)
    */

    Settings::Manager::load(cfgMgr);

    MWGui::DebugWindow::startLogRecording();

    engine.setGrabMouse(!variables["no-grab"].as<bool>());

    // Font encoding settings
    std::string encoding(variables["encoding"].as<std::string>());
    Log(Debug::Info) << ToUTF8::encodingUsingMessage(encoding);
    engine.setEncoding(ToUTF8::calculateEncoding(encoding));

    Files::PathContainer dataDirs(asPathContainer(variables["data"].as<Files::MaybeQuotedPathContainer>()));

    Files::PathContainer::value_type local(variables["data-local"]
                                               .as<Files::MaybeQuotedPathContainer::value_type>()
                                               .u8string()); // This call to u8string is redundant, but required to
                                                             // build on MSVC 14.26 due to implementation bugs.
    if (!local.empty())
        dataDirs.push_back(local);

    cfgMgr.filterOutNonExistingPaths(dataDirs);

    engine.setResourceDir(variables["resources"]
                              .as<Files::MaybeQuotedPath>()
                              .u8string()); // This call to u8string is redundant, but required to build on MSVC 14.26
                                            // due to implementation bugs.
    engine.setDataDirs(dataDirs);

    // fallback archives
    StringsVector archives = variables["fallback-archive"].as<StringsVector>();
    for (StringsVector::const_iterator it = archives.begin(); it != archives.end(); ++it)
    {
        engine.addArchive(*it);
    }

    StringsVector content = variables["content"].as<StringsVector>();
    if (content.empty())
    {
        Log(Debug::Error) << "No content file given (esm/esp, nor omwgame/omwaddon). Aborting...";
        return false;
    }
    std::set<std::string> contentDedupe;
    for (const auto& contentFile : content)
    {
        if (!contentDedupe.insert(contentFile).second)
        {
            Log(Debug::Error) << "Content file specified more than once: " << contentFile << ". Aborting...";
            return false;
        }
    }

    for (auto& file : content)
    {
        engine.addContentFile(file);
    }

    StringsVector groundcover = variables["groundcover"].as<StringsVector>();
    for (auto& file : groundcover)
    {
        engine.addGroundcoverFile(file);
    }

    if (variables.count("lua-scripts"))
    {
        Log(Debug::Warning) << "Lua scripts have been specified via the old lua-scripts option and will not be loaded. "
                               "Please update them to a version which uses the new omwscripts format.";
    }

    // startup-settings
    engine.setCell(variables["start"].as<std::string>());
    engine.setSkipMenu(variables["skip-menu"].as<bool>(), variables["new-game"].as<bool>());
    if (!variables["skip-menu"].as<bool>() && variables["new-game"].as<bool>())
        Log(Debug::Warning) << "Warning: new-game used without skip-menu -> ignoring it";

    // scripts
    engine.setCompileAll(variables["script-all"].as<bool>());
    engine.setCompileAllDialogue(variables["script-all-dialogue"].as<bool>());
    engine.setScriptConsoleMode(variables["script-console"].as<bool>());

    /*
        Start of tes3mp change (major)

        Clients should not be allowed to set any of these unilaterally in multiplayer, so
        disable them
    */
    // engine.setStartupScript(variables["script-run"].as<std::string>());
    // engine.setWarningsMode(variables["script-warn"].as<int>());
    // std::vector<ESM::RefId> scriptBlacklist;
    // auto& scriptBlacklistString = variables["script-blacklist"].as<StringsVector>();
    // for (const auto& blacklistString : scriptBlacklistString)
    // {
    //     scriptBlacklist.push_back(ESM::RefId::stringRefId(blacklistString));
    // }
    // engine.setScriptBlacklist(scriptBlacklist);
    // engine.setScriptBlacklistUse(variables["script-blacklist-use"].as<bool>());
    // engine.setSaveGameFile(variables["load-savegame"].as<Files::MaybeQuotedPath>().u8string());
    /*
        End of tes3mp change (major)
    */

    // other settings
    Fallback::Map::init(variables["fallback"].as<FallbackMap>().mMap);
    engine.setSoundUsage(!variables["no-sound"].as<bool>());
    engine.setActivationDistanceOverride(variables["activate-dist"].as<int>());
    engine.setRandomSeed(variables["random-seed"].as<unsigned int>());

    /*
        Start of tes3mp addition

        Configure multiplayer using parsed variables
    */
    mwmp::Main::configure(&variables);
    /*
        End of tes3mp addition
    */

    return true;
}

namespace
{
    class OSGLogHandler : public osg::NotifyHandler
    {
        void notify(osg::NotifySeverity severity, const char* msg) override
        {
            // Copy, because osg logging is not thread safe.
            std::string msgCopy(msg);
            if (msgCopy.empty())
                return;

            Debug::Level level;
            switch (severity)
            {
                case osg::ALWAYS:
                case osg::FATAL:
                    level = Debug::Error;
                    break;
                case osg::WARN:
                case osg::NOTICE:
                    level = Debug::Warning;
                    break;
                case osg::INFO:
                    level = Debug::Info;
                    break;
                case osg::DEBUG_INFO:
                case osg::DEBUG_FP:
                default:
                    level = Debug::Debug;
            }
            std::string_view s(msgCopy);
            if (s.size() < 1024)
                Log(level) << (s.back() == '\n' ? s.substr(0, s.size() - 1) : s);
            else
            {
                while (!s.empty())
                {
                    size_t lineSize = 1;
                    while (lineSize < s.size() && s[lineSize - 1] != '\n')
                        lineSize++;
                    Log(level) << s.substr(0, s[lineSize - 1] == '\n' ? lineSize - 1 : lineSize);
                    s = s.substr(lineSize);
                }
            }
        }
    };
}

int runApplication(int argc, char* argv[])
{
    Platform::init();

#ifdef __APPLE__
    std::filesystem::path binary_path = std::filesystem::absolute(std::filesystem::path(argv[0]));
    std::filesystem::current_path(binary_path.parent_path());
    setenv("OSG_GL_TEXTURE_STORAGE", "OFF", 0);
#endif

    osg::setNotifyHandler(new OSGLogHandler());
    Files::ConfigurationManager cfgMgr;
    std::unique_ptr<OMW::Engine> engine = std::make_unique<OMW::Engine>(cfgMgr);

    if (parseOptions(argc, argv, *engine, cfgMgr))
    {
        engine->go();
    }

    return 0;
}

#ifdef ANDROID
extern "C" int SDL_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
{
    /*
        Start of tes3mp addition

        Initialize the logger added for multiplayer
    */
    LOG_INIT(TimedLog::LOG_INFO);
    /*
        End of tes3mp addition
    */

    /*
        Start of tes3mp change (major)

        Instead of logging information in openmw.log, use a more descriptive filename
        that includes a timestamp
    */
    return wrapApplication(&runApplication, argc, argv, "/tes3mp-client-" + TimedLog::getFilenameTimestamp());
    /*
        End of tes3mp change (major)
    */
}

// Platform specific for Windows when there is no console built into the executable.
// Windows will call the WinMain function instead of main in this case, the normal
// main function is then called with the __argc and __argv parameters.
#if defined(_WIN32) && !defined(_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    return main(__argc, __argv);
}
#endif
