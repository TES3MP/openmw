#ifndef OPENMW_PROCESSORGAMESETTINGS_HPP
#define OPENMW_PROCESSORGAMESETTINGS_HPP

#include "apps/openmw/mwbase/environment.hpp"
#include "apps/openmw/mwworld/worldimp.hpp"
#include "apps/openmw/mwgui/windowmanagerimp.hpp"

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorGameSettings : public PlayerProcessor
    {
    public:
        ProcessorGameSettings()
        {
            BPP_INIT(ID_GAME_SETTINGS)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            static const int initialLogLevel = MWMPLog::GetLevel();

            if (isLocal())
            {
                LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Received ID_GAME_SETTINGS");

                if (MWBase::Environment::get().getWindowManager()->isGuiMode())
                {
                    if (MWBase::Environment::get().getWindowManager()->getMode() == MWGui::GM_Console && !player->consoleAllowed)
                        MWBase::Environment::get().getWindowManager()->popGuiMode();
                    else if (MWBase::Environment::get().getWindowManager()->getMode() == MWGui::GM_Rest &&
                        (!player->bedRestAllowed || !player->wildernessRestAllowed || !player->waitAllowed))
                        MWBase::Environment::get().getWindowManager()->popGuiMode();
                }

                if (player->enforcedLogLevel > -1)
                {
                    LOG_APPEND(MWMPLog::LOG_INFO, "- server is enforcing log level %i", player->enforcedLogLevel);
                    MWMPLog::SetLevel(player->enforcedLogLevel);
                }
                else if (initialLogLevel != MWMPLog::GetLevel())
                {
                    LOG_APPEND(MWMPLog::LOG_INFO, "- log level has been reset to initial value %i", initialLogLevel);
                    MWMPLog::SetLevel(initialLogLevel);
                }

                MWBase::Environment::get().getWorld()->setPhysicsFramerate(player->physicsFramerate);
            }
        }
    };
}

#endif //OPENMW_PROCESSORGAMESETTINGS_HPP
