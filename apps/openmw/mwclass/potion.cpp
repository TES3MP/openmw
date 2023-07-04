#include "potion.hpp"

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/Utils.hpp>
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
/*
    End of tes3mp addition
*/

#include <MyGUI_TextIterator.h>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"
#include <components/esm3/loadalch.hpp>
#include <components/esm3/loadnpc.hpp>

#include "../mwworld/actionapply.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwgui/tooltips.hpp"
#include "../mwgui/ustring.hpp"

#include "../mwrender/objects.hpp"
#include "../mwrender/renderinginterface.hpp"

#include "../mwmechanics/alchemy.hpp"

#include "classmodel.hpp"

namespace MWClass
{
    Potion::Potion()
        : MWWorld::RegisteredClass<Potion>(ESM::Potion::sRecordId)
    {
    }

    void Potion::insertObjectRendering(
        const MWWorld::Ptr& ptr, const std::string& model, MWRender::RenderingInterface& renderingInterface) const
    {
        if (!model.empty())
        {
            renderingInterface.getObjects().insertModel(ptr, model);
        }
    }

    std::string Potion::getModel(const MWWorld::ConstPtr& ptr) const
    {
        // TODO: add option somewhere to enable collision for placeable objects

        /*
            Start of tes3mp addition

            Make it possible to enable collision for this object class from a packet
        */
        if (!model.empty())
        {
            mwmp::BaseWorldstate *worldstate = mwmp::Main::get().getNetworking()->getWorldstate();

            if (worldstate->hasPlacedObjectCollision || Utils::vectorContains(worldstate->enforcedCollisionRefIds, ptr.getCellRef().getRefId()))
            {
                if (worldstate->useActorCollisionForPlacedObjects)
                    physics.addObject(ptr, model, MWPhysics::CollisionType_Actor);
                else
                    physics.addObject(ptr, model, MWPhysics::CollisionType_World);
            }
        }
        /*
            End of tes3mp addition
        */

        return getClassModel<ESM::Potion>(ptr);
    }

    std::string_view Potion::getName(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();
        const std::string& name = ref->mBase->mName;

        return !name.empty() ? name : ref->mBase->mId.getRefIdString();
    }

    std::unique_ptr<MWWorld::Action> Potion::activate(const MWWorld::Ptr& ptr, const MWWorld::Ptr& actor) const
    {
        return defaultItemActivate(ptr, actor);
    }

    ESM::RefId Potion::getScript(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();

        return ref->mBase->mScript;
    }

    int Potion::getValue(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();

        return ref->mBase->mData.mValue;
    }

    const ESM::RefId& Potion::getUpSoundId(const MWWorld::ConstPtr& ptr) const
    {
        static const auto sound = ESM::RefId::stringRefId("Item Potion Up");
        return sound;
    }

    const ESM::RefId& Potion::getDownSoundId(const MWWorld::ConstPtr& ptr) const
    {
        static const auto sound = ESM::RefId::stringRefId("Item Potion Down");
        return sound;
    }

    const std::string& Potion::getInventoryIcon(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();

        return ref->mBase->mIcon;
    }

    MWGui::ToolTipInfo Potion::getToolTipInfo(const MWWorld::ConstPtr& ptr, int count) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();

        MWGui::ToolTipInfo info;
        std::string_view name = getName(ptr);
        info.caption
            = MyGUI::TextIterator::toTagsString(MWGui::toUString(name)) + MWGui::ToolTips::getCountString(count);
        info.icon = ref->mBase->mIcon;

        std::string text;

        text += "\n#{sWeight}: " + MWGui::ToolTips::toString(ref->mBase->mData.mWeight);
        text += MWGui::ToolTips::getValueString(ref->mBase->mData.mValue, "#{sValue}");

        info.effects = MWGui::Widgets::MWEffectList::effectListFromESM(&ref->mBase->mEffects);

        // hide effects the player doesn't know about
        MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayerPtr();
        for (size_t i = 0; i < info.effects.size(); ++i)
            info.effects[i].mKnown = MWMechanics::Alchemy::knownEffect(i, player);

        info.isPotion = true;

        if (MWBase::Environment::get().getWindowManager()->getFullHelp())
        {
            text += MWGui::ToolTips::getCellRefString(ptr.getCellRef());
            text += MWGui::ToolTips::getMiscString(ref->mBase->mScript.getRefIdString(), "Script");
        }

        info.text = text;

        return info;
    }

    std::unique_ptr<MWWorld::Action> Potion::use(const MWWorld::Ptr& ptr, bool force) const
    {
        MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();

        auto action = std::make_unique<MWWorld::ActionApply>(ptr, ref->mBase->mId);

        action->setSound(ESM::RefId::stringRefId("Drink"));

        return action;
    }

    MWWorld::Ptr Potion::copyToCellImpl(const MWWorld::ConstPtr& ptr, MWWorld::CellStore& cell) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();

        return MWWorld::Ptr(cell.insert(ref), &cell);
    }

    bool Potion::canSell(const MWWorld::ConstPtr& item, int npcServices) const
    {
        return (npcServices & ESM::NPC::Potions) != 0;
    }

    float Potion::getWeight(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Potion>* ref = ptr.get<ESM::Potion>();
        return ref->mBase->mData.mWeight;
    }
}
