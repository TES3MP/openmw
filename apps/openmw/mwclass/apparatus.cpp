#include "apparatus.hpp"

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

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include <components/esm3/loadappa.hpp>
#include <components/esm3/loadnpc.hpp>

#include "../mwworld/actionalchemy.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwrender/objects.hpp"
#include "../mwrender/renderinginterface.hpp"

#include "../mwgui/tooltips.hpp"
#include "../mwgui/ustring.hpp"

#include "classmodel.hpp"

namespace MWClass
{
    Apparatus::Apparatus()
        : MWWorld::RegisteredClass<Apparatus>(ESM::Apparatus::sRecordId)
    {
    }

    void Apparatus::insertObjectRendering(
        const MWWorld::Ptr& ptr, const std::string& model, MWRender::RenderingInterface& renderingInterface) const
    {
        if (!model.empty())
        {
            renderingInterface.getObjects().insertModel(ptr, model);
        }
    }

    std::string Apparatus::getModel(const MWWorld::ConstPtr& ptr) const
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
        return getClassModel<ESM::Apparatus>(ptr);
    }

    std::string_view Apparatus::getName(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();
        const std::string& name = ref->mBase->mName;

        return !name.empty() ? name : ref->mBase->mId.getRefIdString();
    }

    std::unique_ptr<MWWorld::Action> Apparatus::activate(const MWWorld::Ptr& ptr, const MWWorld::Ptr& actor) const
    {
        return defaultItemActivate(ptr, actor);
    }

    ESM::RefId Apparatus::getScript(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();

        return ref->mBase->mScript;
    }

    int Apparatus::getValue(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();

        return ref->mBase->mData.mValue;
    }

    const ESM::RefId& Apparatus::getUpSoundId(const MWWorld::ConstPtr& ptr) const
    {
        static const auto sound = ESM::RefId::stringRefId("Item Apparatus Up");
        return sound;
    }

    const ESM::RefId& Apparatus::getDownSoundId(const MWWorld::ConstPtr& ptr) const
    {
        static const auto sound = ESM::RefId::stringRefId("Item Apparatus Down");
        return sound;
    }

    const std::string& Apparatus::getInventoryIcon(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();

        return ref->mBase->mIcon;
    }

    MWGui::ToolTipInfo Apparatus::getToolTipInfo(const MWWorld::ConstPtr& ptr, int count) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();

        MWGui::ToolTipInfo info;
        std::string_view name = getName(ptr);
        info.caption
            = MyGUI::TextIterator::toTagsString(MWGui::toUString(name)) + MWGui::ToolTips::getCountString(count);
        info.icon = ref->mBase->mIcon;

        std::string text;
        text += "\n#{sQuality}: " + MWGui::ToolTips::toString(ref->mBase->mData.mQuality);
        text += MWGui::ToolTips::getWeightString(ref->mBase->mData.mWeight, "#{sWeight}");
        text += MWGui::ToolTips::getValueString(ref->mBase->mData.mValue, "#{sValue}");

        if (MWBase::Environment::get().getWindowManager()->getFullHelp())
        {
            text += MWGui::ToolTips::getCellRefString(ptr.getCellRef());
            text += MWGui::ToolTips::getMiscString(ref->mBase->mScript.getRefIdString(), "Script");
        }
        info.text = text;

        return info;
    }

    std::unique_ptr<MWWorld::Action> Apparatus::use(const MWWorld::Ptr& ptr, bool force) const
    {
        return std::make_unique<MWWorld::ActionAlchemy>(force);
    }

    MWWorld::Ptr Apparatus::copyToCellImpl(const MWWorld::ConstPtr& ptr, MWWorld::CellStore& cell) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();

        return MWWorld::Ptr(cell.insert(ref), &cell);
    }

    bool Apparatus::canSell(const MWWorld::ConstPtr& item, int npcServices) const
    {
        return (npcServices & ESM::NPC::Apparatus) != 0;
    }

    float Apparatus::getWeight(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Apparatus>* ref = ptr.get<ESM::Apparatus>();
        return ref->mBase->mData.mWeight;
    }
}
