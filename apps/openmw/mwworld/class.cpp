#include "class.hpp"

#include <stdexcept>

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/TimedLog.hpp>
/*
    End of tes3mp addition
*/

#include <components/esm/defs.hpp>
#include <components/esm3/loadench.hpp>
#include <components/esm3/loadmgef.hpp>
#include <components/esm3/loadsoun.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"
#include "../mwworld/esmstore.hpp"

#include "actiontake.hpp"
#include "containerstore.hpp"
#include "failedaction.hpp"
#include "inventorystore.hpp"
#include "nullaction.hpp"
#include "ptr.hpp"
#include "worldmodel.hpp"

#include "../mwgui/tooltips.hpp"

#include "../mwmechanics/npcstats.hpp"

namespace MWWorld
{
    std::map<unsigned, Class*>& Class::getClasses()
    {
        static std::map<unsigned, Class*> values;
        return values;
    }

    void Class::insertObjectRendering(
        const Ptr& ptr, const std::string& mesh, MWRender::RenderingInterface& renderingInterface) const
    {
    }

    void Class::insertObject(
        const Ptr& ptr, const std::string& mesh, const osg::Quat& rotation, MWPhysics::PhysicsSystem& physics) const
    {
    }

    void Class::insertObjectPhysics(
        const Ptr& ptr, const std::string& mesh, const osg::Quat& rotation, MWPhysics::PhysicsSystem& physics) const
    {
    }

    bool Class::consume(const MWWorld::Ptr& consumable, const MWWorld::Ptr& actor) const
    {
        return false;
    }

    void Class::skillUsageSucceeded(const MWWorld::Ptr& ptr, ESM::RefId skill, int usageType, float extraFactor) const
    {
        throw std::runtime_error("class does not represent an actor");
    }

    bool Class::canSell(const MWWorld::ConstPtr& item, int npcServices) const
    {
        return false;
    }

    int Class::getServices(const ConstPtr& actor) const
    {
        throw std::runtime_error("class does not have services");
    }

    MWMechanics::CreatureStats& Class::getCreatureStats(const Ptr& ptr) const
    {
<<<<<<< HEAD
        /*
            Start of tes3mp addition

            This is a common error in multiplayer, so additional logging has been added for it
        */
        LOG_MESSAGE_SIMPLE(TimedLog::LOG_ERROR, "Attempt at getting creatureStats for %s %i-%i which is a %s!",
            ptr.getCellRef().getRefId().c_str(), ptr.getCellRef().getRefNum().mIndex, ptr.getCellRef().getMpNum(),
            ptr.getClass().getTypeName().c_str());
        /*
            End of tes3mp addition
        */

        throw std::runtime_error ("class does not have creature stats");
=======
        throw std::runtime_error("class does not have creature stats");
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    }

    MWMechanics::NpcStats& Class::getNpcStats(const Ptr& ptr) const
    {
        throw std::runtime_error("class does not have NPC stats");
    }

    bool Class::hasItemHealth(const ConstPtr& ptr) const
    {
        return false;
    }

    int Class::getItemHealth(const ConstPtr& ptr) const
    {
        if (ptr.getCellRef().getCharge() == -1)
            return getItemMaxHealth(ptr);
        else
            return ptr.getCellRef().getCharge();
    }

    float Class::getItemNormalizedHealth(const ConstPtr& ptr) const
    {
        if (getItemMaxHealth(ptr) == 0)
        {
            return 0.f;
        }
        else
        {
            return getItemHealth(ptr) / static_cast<float>(getItemMaxHealth(ptr));
        }
    }

    int Class::getItemMaxHealth(const ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not have item health");
    }

    bool Class::evaluateHit(const Ptr& ptr, Ptr& victim, osg::Vec3f& hitPosition) const
    {
        throw std::runtime_error("class cannot hit");
    }

    void Class::hit(const Ptr& ptr, float attackStrength, int type, const Ptr& victim, const osg::Vec3f& hitPosition,
        bool success) const
    {
        throw std::runtime_error("class cannot hit");
    }

    void Class::block(const Ptr& ptr) const
    {
        throw std::runtime_error("class cannot block");
    }

    void Class::onHit(const Ptr& ptr, float damage, bool ishealth, const Ptr& object, const Ptr& attacker,
        const osg::Vec3f& hitPosition, bool successful) const
    {
        throw std::runtime_error("class cannot be hit");
    }

    std::unique_ptr<Action> Class::activate(const Ptr& ptr, const Ptr& actor) const
    {
        return std::make_unique<NullAction>();
    }

    std::unique_ptr<Action> Class::use(const Ptr& ptr, bool force) const
    {
        return std::make_unique<NullAction>();
    }

    ContainerStore& Class::getContainerStore(const Ptr& ptr) const
    {
        throw std::runtime_error("class does not have a container store");
    }

    InventoryStore& Class::getInventoryStore(const Ptr& ptr) const
    {
        throw std::runtime_error("class does not have an inventory store");
    }

<<<<<<< HEAD
    /*
        Start of tes3mp addition

        Make it possible to check whether a class has a container store
    */
    bool Class::hasContainerStore(const Ptr &ptr) const
    {
        return false;
    }
    /*
        End of tes3mp addition
    */

    bool Class::hasInventoryStore(const Ptr &ptr) const
=======
    bool Class::hasInventoryStore(const Ptr& ptr) const
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    {
        return false;
    }

<<<<<<< HEAD
    /*
        Start of tes3mp addition

        Make it possible to check whether a class can be harvested
    */
    bool Class::canBeHarvested(const ConstPtr& ptr) const
    {
        return false;
    }
    /*
        End of tes3mp addition
    */

    bool Class::canLock(const ConstPtr &ptr) const
=======
    bool Class::canLock(const ConstPtr& ptr) const
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    {
        return false;
    }

    void Class::setRemainingUsageTime(const Ptr& ptr, float duration) const
    {
        throw std::runtime_error("class does not support time-based uses");
    }

    float Class::getRemainingUsageTime(const ConstPtr& ptr) const
    {
        return -1;
    }

    ESM::RefId Class::getScript(const ConstPtr& ptr) const
    {
        return ESM::RefId();
    }

    float Class::getMaxSpeed(const Ptr& ptr) const
    {
        return 0;
    }

    float Class::getCurrentSpeed(const Ptr& ptr) const
    {
        return 0;
    }

    float Class::getJump(const Ptr& ptr) const
    {
        return 0;
    }

    int Class::getEnchantmentPoints(const MWWorld::ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not support enchanting");
    }

    MWMechanics::Movement& Class::getMovementSettings(const Ptr& ptr) const
    {
        throw std::runtime_error("movement settings not supported by class");
    }

    osg::Vec3f Class::getRotationVector(const Ptr& ptr) const
    {
        return osg::Vec3f(0, 0, 0);
    }

    std::pair<std::vector<int>, bool> Class::getEquipmentSlots(const ConstPtr& ptr) const
    {
        return std::make_pair(std::vector<int>(), false);
    }

    ESM::RefId Class::getEquipmentSkill(const ConstPtr& ptr) const
    {
        return {};
    }

    int Class::getValue(const ConstPtr& ptr) const
    {
        throw std::logic_error("value not supported by this class");
    }

    float Class::getCapacity(const MWWorld::Ptr& ptr) const
    {
        throw std::runtime_error("capacity not supported by this class");
    }

    float Class::getWeight(const ConstPtr& ptr) const
    {
        throw std::runtime_error("weight not supported by this class");
    }

    float Class::getEncumbrance(const MWWorld::Ptr& ptr) const
    {
        throw std::runtime_error("encumbrance not supported by class");
    }

    bool Class::isEssential(const MWWorld::ConstPtr& ptr) const
    {
        return false;
    }

    float Class::getArmorRating(const MWWorld::Ptr& ptr) const
    {
        throw std::runtime_error("Class does not support armor rating");
    }

    const Class& Class::get(unsigned int key)
    {
        const auto& classes = getClasses();
        auto iter = classes.find(key);

        if (iter == classes.end())
            throw std::logic_error("Class::get(): unknown class key: " + std::to_string(key));

        return *iter->second;
    }

    bool Class::isPersistent(const ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not support persistence");
    }

    void Class::registerClass(Class& instance)
    {
        getClasses().emplace(instance.getType(), &instance);
    }

    const ESM::RefId& Class::getUpSoundId(const ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not have an up sound");
    }

    const ESM::RefId& Class::getDownSoundId(const ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not have an down sound");
    }

    ESM::RefId Class::getSoundIdFromSndGen(const Ptr& ptr, std::string_view type) const
    {
        throw std::runtime_error("class does not support soundgen look up");
    }

    const std::string& Class::getInventoryIcon(const MWWorld::ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not have any inventory icon");
    }

    MWGui::ToolTipInfo Class::getToolTipInfo(const ConstPtr& ptr, int count) const
    {
        throw std::runtime_error("class does not have a tool tip");
    }

    bool Class::showsInInventory(const ConstPtr& ptr) const
    {
        // NOTE: Don't show WerewolfRobe objects in the inventory, or allow them to be taken.
        // Vanilla likely uses a hack like this since there's no other way to prevent it from
        // being shown or taken.
        return (ptr.getCellRef().getRefId() != "werewolfrobe");
    }

    bool Class::hasToolTip(const ConstPtr& ptr) const
    {
        return true;
    }

    ESM::RefId Class::getEnchantment(const ConstPtr& ptr) const
    {
        return ESM::RefId();
    }

    void Class::adjustScale(const MWWorld::ConstPtr& ptr, osg::Vec3f& scale, bool rendering) const {}

    std::string Class::getModel(const MWWorld::ConstPtr& ptr) const
    {
        return {};
    }

    bool Class::useAnim() const
    {
        return false;
    }

    void Class::getModelsToPreload(const Ptr& ptr, std::vector<std::string>& models) const
    {
        std::string model = getModel(ptr);
        if (!model.empty())
            models.push_back(model);
    }

    const ESM::RefId& Class::applyEnchantment(
        const MWWorld::ConstPtr& ptr, const ESM::RefId& enchId, int enchCharge, const std::string& newName) const
    {
        throw std::runtime_error("class can't be enchanted");
    }

    std::pair<int, std::string_view> Class::canBeEquipped(const MWWorld::ConstPtr& ptr, const MWWorld::Ptr& npc) const
    {
        return { 1, {} };
    }

    void Class::adjustPosition(const MWWorld::Ptr& ptr, bool force) const {}

    std::unique_ptr<Action> Class::defaultItemActivate(const Ptr& ptr, const Ptr& actor) const
    {
        if (!MWBase::Environment::get().getWindowManager()->isAllowed(MWGui::GW_Inventory))
            return std::make_unique<NullAction>();

        if (actor.getClass().isNpc() && actor.getClass().getNpcStats(actor).isWerewolf())
        {
            const MWWorld::ESMStore& store = *MWBase::Environment::get().getESMStore();
            auto& prng = MWBase::Environment::get().getWorld()->getPrng();
            const ESM::Sound* sound = store.get<ESM::Sound>().searchRandom("WolfItem", prng);

            std::unique_ptr<MWWorld::Action> action = std::make_unique<MWWorld::FailedAction>("#{sWerewolfRefusal}");
            if (sound)
                action->setSound(sound->mId);

            return action;
        }

        std::unique_ptr<MWWorld::Action> action = std::make_unique<ActionTake>(ptr);
        action->setSound(getUpSoundId(ptr));

        return action;
    }

    MWWorld::Ptr Class::copyToCellImpl(const ConstPtr& ptr, CellStore& cell) const
    {
        throw std::runtime_error("unable to copy class to cell");
    }

    MWWorld::Ptr Class::copyToCell(const ConstPtr& ptr, CellStore& cell, int count) const
    {
        Ptr newPtr = copyToCellImpl(ptr, cell);
        newPtr.getCellRef().unsetRefNum(); // This RefNum is only valid within the original cell of the reference
        newPtr.getRefData().setCount(count);
        newPtr.getRefData().setLuaScripts(nullptr);
        MWBase::Environment::get().getWorldModel()->registerPtr(newPtr);
        if (hasInventoryStore(newPtr))
            getInventoryStore(newPtr).setActor(newPtr);
        return newPtr;
    }

    MWWorld::Ptr Class::moveToCell(const Ptr& ptr, CellStore& cell) const
    {
        Ptr newPtr = copyToCellImpl(ptr, cell);
        ptr.getRefData().setLuaScripts(nullptr);
        MWBase::Environment::get().getWorldModel()->registerPtr(newPtr);
        if (hasInventoryStore(newPtr))
            getInventoryStore(newPtr).setActor(newPtr);
        return newPtr;
    }

    MWWorld::Ptr Class::copyToCell(const ConstPtr& ptr, CellStore& cell, const ESM::Position& pos, int count) const
    {
        Ptr newPtr = copyToCell(ptr, cell, count);
        newPtr.getRefData().setPosition(pos);
        return newPtr;
    }

    bool Class::isBipedal(const ConstPtr& ptr) const
    {
        return false;
    }

    bool Class::canFly(const ConstPtr& ptr) const
    {
        return false;
    }

    bool Class::canSwim(const ConstPtr& ptr) const
    {
        return false;
    }

    bool Class::canWalk(const ConstPtr& ptr) const
    {
        return false;
    }

    bool Class::isPureWaterCreature(const ConstPtr& ptr) const
    {
        return canSwim(ptr) && !isBipedal(ptr) && !canFly(ptr) && !canWalk(ptr);
    }

    bool Class::isPureFlyingCreature(const ConstPtr& ptr) const
    {
        return canFly(ptr) && !isBipedal(ptr) && !canSwim(ptr) && !canWalk(ptr);
    }

    bool Class::isPureLandCreature(const Ptr& ptr) const
    {
        return canWalk(ptr) && !isBipedal(ptr) && !canFly(ptr) && !canSwim(ptr);
    }

    bool Class::isMobile(const MWWorld::Ptr& ptr) const
    {
        return canSwim(ptr) || canWalk(ptr) || canFly(ptr);
    }

    float Class::getSkill(const MWWorld::Ptr& ptr, ESM::RefId id) const
    {
        throw std::runtime_error("class does not support skills");
    }

    int Class::getBloodTexture(const MWWorld::ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not support gore");
    }

    void Class::readAdditionalState(const MWWorld::Ptr& ptr, const ESM::ObjectState& state) const {}

    void Class::writeAdditionalState(const MWWorld::ConstPtr& ptr, ESM::ObjectState& state) const {}

    int Class::getBaseGold(const MWWorld::ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not support base gold");
    }

    bool Class::isClass(const MWWorld::ConstPtr& ptr, std::string_view className) const
    {
        return false;
    }

    MWWorld::DoorState Class::getDoorState(const MWWorld::ConstPtr& ptr) const
    {
        throw std::runtime_error("this is not a door");
    }

    void Class::setDoorState(const MWWorld::Ptr& ptr, MWWorld::DoorState state) const
    {
        throw std::runtime_error("this is not a door");
    }

    float Class::getNormalizedEncumbrance(const Ptr& ptr) const
    {
        float capacity = getCapacity(ptr);
        float encumbrance = getEncumbrance(ptr);

        if (encumbrance == 0)
            return 0.f;

        if (capacity == 0)
            return 1.f;

        return encumbrance / capacity;
    }

    ESM::RefId Class::getSound(const MWWorld::ConstPtr&) const
    {
        return ESM::RefId();
    }

    int Class::getBaseFightRating(const ConstPtr& ptr) const
    {
        throw std::runtime_error("class does not support fight rating");
    }

    ESM::RefId Class::getPrimaryFaction(const MWWorld::ConstPtr& ptr) const
    {
        return ESM::RefId();
    }
    int Class::getPrimaryFactionRank(const MWWorld::ConstPtr& ptr) const
    {
        return -1;
    }

    float Class::getEffectiveArmorRating(const ConstPtr& armor, const Ptr& actor) const
    {
        throw std::runtime_error("class does not support armor ratings");
    }

    osg::Vec4f Class::getEnchantmentColor(const MWWorld::ConstPtr& item) const
    {
        osg::Vec4f result(1, 1, 1, 1);
        const ESM::RefId& enchantmentName = item.getClass().getEnchantment(item);
        if (enchantmentName.empty())
            return result;

        const ESM::Enchantment* enchantment
            = MWBase::Environment::get().getESMStore()->get<ESM::Enchantment>().search(enchantmentName);
        if (!enchantment)
            return result;

        assert(enchantment->mEffects.mList.size());

        const ESM::MagicEffect* magicEffect = MWBase::Environment::get().getESMStore()->get<ESM::MagicEffect>().search(
            enchantment->mEffects.mList.front().mEffectID);
        if (!magicEffect)
            return result;

        result.x() = magicEffect->mData.mRed / 255.f;
        result.y() = magicEffect->mData.mGreen / 255.f;
        result.z() = magicEffect->mData.mBlue / 255.f;
        return result;
    }

    void Class::setBaseAISetting(const ESM::RefId&, MWMechanics::AiSetting setting, int value) const
    {
        throw std::runtime_error("class does not have creature stats");
    }

    void Class::modifyBaseInventory(const ESM::RefId& actorId, const ESM::RefId& itemId, int amount) const
    {
        throw std::runtime_error("class does not have an inventory store");
    }

    float Class::getWalkSpeed(const Ptr& /*ptr*/) const
    {
        return 0;
    }

    float Class::getRunSpeed(const Ptr& /*ptr*/) const
    {
        return 0;
    }

    float Class::getSwimSpeed(const Ptr& /*ptr*/) const
    {
        return 0;
    }
}
