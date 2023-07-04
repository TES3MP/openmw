#include "ingredient.hpp"

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

#include <components/esm3/loadingr.hpp>
#include <components/esm3/loadnpc.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/actioneat.hpp"
#include "../mwworld/cellstore.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/nullaction.hpp"
#include "../mwworld/ptr.hpp"

#include "../mwgui/tooltips.hpp"
#include "../mwgui/ustring.hpp"

#include "../mwrender/objects.hpp"
#include "../mwrender/renderinginterface.hpp"

#include "classmodel.hpp"

namespace MWClass
{
    Ingredient::Ingredient()
        : MWWorld::RegisteredClass<Ingredient>(ESM::Ingredient::sRecordId)
    {
    }

    void Ingredient::insertObjectRendering(
        const MWWorld::Ptr& ptr, const std::string& model, MWRender::RenderingInterface& renderingInterface) const
    {
        if (!model.empty())
        {
            renderingInterface.getObjects().insertModel(ptr, model);
        }
    }

    std::string Ingredient::getModel(const MWWorld::ConstPtr& ptr) const
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

        return getClassModel<ESM::Ingredient>(ptr);
    }

    std::string_view Ingredient::getName(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();
        const std::string& name = ref->mBase->mName;

        return !name.empty() ? name : ref->mBase->mId.getRefIdString();
    }

    std::unique_ptr<MWWorld::Action> Ingredient::activate(const MWWorld::Ptr& ptr, const MWWorld::Ptr& actor) const
    {
        return defaultItemActivate(ptr, actor);
    }

    ESM::RefId Ingredient::getScript(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();

        return ref->mBase->mScript;
    }

    int Ingredient::getValue(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();

        return ref->mBase->mData.mValue;
    }

    std::unique_ptr<MWWorld::Action> Ingredient::use(const MWWorld::Ptr& ptr, bool force) const
    {
        if (ptr.get<ESM::Ingredient>()->mBase->mData.mEffectID[0] < 0)
            return std::make_unique<MWWorld::NullAction>();
        std::unique_ptr<MWWorld::Action> action = std::make_unique<MWWorld::ActionEat>(ptr);

        action->setSound(ESM::RefId::stringRefId("Swallow"));

        return action;
    }

    const ESM::RefId& Ingredient::getUpSoundId(const MWWorld::ConstPtr& ptr) const
    {
        static auto sound = ESM::RefId::stringRefId("Item Ingredient Up");
        return sound;
    }

    const ESM::RefId& Ingredient::getDownSoundId(const MWWorld::ConstPtr& ptr) const
    {
        static auto sound = ESM::RefId::stringRefId("Item Ingredient Down");
        return sound;
    }

    const std::string& Ingredient::getInventoryIcon(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();

        return ref->mBase->mIcon;
    }

    MWGui::ToolTipInfo Ingredient::getToolTipInfo(const MWWorld::ConstPtr& ptr, int count) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();

        MWGui::ToolTipInfo info;
        std::string_view name = getName(ptr);
        info.caption
            = MyGUI::TextIterator::toTagsString(MWGui::toUString(name)) + MWGui::ToolTips::getCountString(count);
        info.icon = ref->mBase->mIcon;

        std::string text;

        text += MWGui::ToolTips::getWeightString(ref->mBase->mData.mWeight, "#{sWeight}");
        text += MWGui::ToolTips::getValueString(ref->mBase->mData.mValue, "#{sValue}");

        if (MWBase::Environment::get().getWindowManager()->getFullHelp())
        {
            text += MWGui::ToolTips::getCellRefString(ptr.getCellRef());
            text += MWGui::ToolTips::getMiscString(ref->mBase->mScript.getRefIdString(), "Script");
        }

        MWWorld::Ptr player = MWBase::Environment::get().getWorld()->getPlayerPtr();
        float alchemySkill = player.getClass().getSkill(player, ESM::Skill::Alchemy);

        static const float fWortChanceValue = MWBase::Environment::get()
                                                  .getESMStore()
                                                  ->get<ESM::GameSetting>()
                                                  .find("fWortChanceValue")
                                                  ->mValue.getFloat();

        MWGui::Widgets::SpellEffectList list;
        for (int i = 0; i < 4; ++i)
        {
            if (ref->mBase->mData.mEffectID[i] < 0)
                continue;
            MWGui::Widgets::SpellEffectParams params;
            params.mEffectID = ref->mBase->mData.mEffectID[i];
            params.mAttribute = ref->mBase->mData.mAttributes[i];
            params.mSkill = ref->mBase->mData.mSkills[i];

            params.mKnown = ((i == 0 && alchemySkill >= fWortChanceValue)
                || (i == 1 && alchemySkill >= fWortChanceValue * 2) || (i == 2 && alchemySkill >= fWortChanceValue * 3)
                || (i == 3 && alchemySkill >= fWortChanceValue * 4));

            list.push_back(params);
        }
        info.effects = list;

        info.text = text;
        info.isIngredient = true;

        return info;
    }

    MWWorld::Ptr Ingredient::copyToCellImpl(const MWWorld::ConstPtr& ptr, MWWorld::CellStore& cell) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();

        return MWWorld::Ptr(cell.insert(ref), &cell);
    }

    bool Ingredient::canSell(const MWWorld::ConstPtr& item, int npcServices) const
    {
        return (npcServices & ESM::NPC::Ingredients) != 0;
    }

    float Ingredient::getWeight(const MWWorld::ConstPtr& ptr) const
    {
        const MWWorld::LiveCellRef<ESM::Ingredient>* ref = ptr.get<ESM::Ingredient>();
        return ref->mBase->mData.mWeight;
    }
}
