//
// Created by koncord on 05.01.16.
//

#ifndef OPENMW_PLAYER_HPP
#define OPENMW_PLAYER_HPP

#include <map>
#include <string>
#include <chrono>
#include <memory>
#include <RakNetTypes.h>

#include <components/esm/npcstats.hpp>
#include <components/esm/cellid.hpp>
#include <components/esm/loadnpc.hpp>
#include <components/esm/loadcell.hpp>

#include <components/openmw-mp/Log.hpp>
#include <components/openmw-mp/Base/BasePlayer.hpp>
#include <components/openmw-mp/Packets/Player/PlayerPacket.hpp>
#include <apps/openmw-mp/Script/LuaState.hpp>
#include "Cell.hpp"
#include "CellController.hpp"
#include "CharClass.hpp"
#include "Inventory.hpp"
#include "Settings.hpp"
#include "Books.hpp"
#include "GUI.hpp"
#include "Dialogue.hpp"
#include "Factions.hpp"
#include "Cells.hpp"
#include "Quests.hpp"
#include "Spells.hpp"
#include "NetActor.hpp"
#include "CellState.hpp"

class Player : public mwmp::BasePlayer, public NetActor
{
    friend class Cell;
    friend class Players;
    unsigned short id;
    uint64_t getGUID() const {return guid.g;}
public:
    static void Init(LuaState &lua);
public:

    enum
    {
        NOTLOADED=0,
        LOADED,
        POSTLOADED
    };
    explicit Player(RakNet::RakNetGUID guid);

    unsigned short getId();
    void setId(unsigned short id);

    bool isHandshaked();
    void setHandshake();

    void setLoadState(int state);
    int getLoadState();

    virtual ~Player();

    CellController::TContainer *getCells();
    void sendToLoaded(mwmp::PlayerPacket *myPacket);

    void forEachLoaded(std::function<void(Player *pl, Player *other)> func);

    void update();

public:
    void kick() const;
    void ban() const;
    void cleanChat();

    int getAvgPing();

    void setName(const std::string &name);
    std::string getName();
    void setCharGenStage(int start, int end);
    void message(const std::string &message, bool toAll = false);
    bool isMale() const;
    void setIsMale(bool male);
    void setRace(const std::string &race);
    std::string getRace() const;
    void setHead(const std::string &head);
    std::string getHead() const;
    void setHair(const std::string &hair);
    std::string getHair() const;
    std::string getBirthsign() const;
    void setBirthsign(const std::string &sign);

    int getBounty() const;
    void setBounty(int bounty);
    void setLevel(int level);
    int getLevel() const;
    void setLevelProgress(int progress);
    int getLevelProgress() const;

    /**
     * \brief Send a PlayerResurrect packet about a player.
     *
     * This sends the packet to all players connected to the server.
     *
     * \param type The type of resurrection (0 for REGULAR, 1 for IMPERIAL_SHRINE, 2 for TRIBUNAL_TEMPLE).
     */
    void resurrect(unsigned int type);

    /**
    * \brief Send a PlayerJail packet about a player.
    *
    * This is similar to the player being jailed by a guard, but provides extra parameters for
    * increased flexibility.
    *
    * It is only sent to the player being jailed, as the other players will be informed of the
    * jailing's actual consequences via other packets sent by the affected client.
    *
    * \param jailDays The number of days to spend jailed, where each day affects one skill point.
    * \param ignoreJailTeleportation Whether the player being teleported to the nearest jail
    *                                marker should be overridden.
    * \param ignoreJailSkillIncrease Whether the player's Sneak and Security skills should be
    *                                prevented from increasing as a result of the jailing,
    *                                overriding default behavior.
    * \param jailProgressText The text that should be displayed while jailed.
    * \param jailEndText The text that should be displayed once the jailing period is over.
    */
    void jail(int jailDays, bool ignoreJailTeleportation, bool ignoreJailSkillIncreases,
              const std::string &jailProgressText, const std::string &jailEndText);

    void setWerewolfState(bool state);
    bool getWerewolfState() const;


    void setCreatureModel(const std::string &model);
    std::string getCreatureModel() const;
    void creatureName(bool useName);
    bool isCreatureName() const;

    std::string getIP() const;

    /**
     *
     * @return  x, y, z
     */
    std::tuple<float, float, float> getPreviousCellPos() const;

    /**
     *
     * @return base, current
     */
    std::tuple<int, int> getAttribute(unsigned short id) const;
    void setAttribute(unsigned short id, int base, int current);

    /**
     *
     * @return base, current, progress, increase
     */
    std::tuple<int, int, float, int> getSkill(unsigned short id) const;
    void setSkill(unsigned short id, int base, int current, float progress, int increase);

    CellState getCellState(int i);
    size_t cellStateSize() const;
    void addCellExplored(const std::string &cellDescription);

    CharClass &getCharClass(sol::this_state thisState);
    GameSettings &getSettings();
    Books &getBooks();
    GUI &getGUI();
    Dialogue &getDialogue();
    Factions &getFactions();
    Quests &getQuests();
    Spells &getSpells();

    void setAuthority();

private:
    CellController::TContainer cells;
    bool handshakeState;
    int loadState;
    bool /*statsChanged, attributesChanged, skillsChanged, baseInfoChanged, positionChanged,*/ changedMap;
    CharClass cClass;
    GameSettings settings;
    Books books;
    GUI gui;
    Dialogue dialogue;
    Factions factions;
    Quests quests;
    Spells spells;
};

#endif //OPENMW_PLAYER_HPP
