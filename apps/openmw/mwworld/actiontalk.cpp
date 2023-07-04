#include "actiontalk.hpp"

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"

#include "../mwmechanics/actorutil.hpp"

namespace MWWorld
{
    ActionTalk::ActionTalk(const Ptr& actor)
        : Action(false, actor)
    {
    }

    void ActionTalk::executeImp(const Ptr& actor)
    {
        /*
            Start of tes3mp change (major)

            We need to be able to make actors start conversations with players, so reverse the check added
            by 4118b20608b630b8d166d060a34c1234b80e378d here
        */
        MWBase::Environment::get().getWindowManager()->pushGuiMode(MWGui::GM_Dialogue, getTarget());
        /*
            End of tes3mp change (major)    
        */
    }
}
