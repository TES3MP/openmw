//
// Created by koncord on 07.01.16.
//

#ifndef OPENMW_BASEPLAYER_HPP
#define OPENMW_BASEPLAYER_HPP

#include <components/esm/loadcell.hpp>
#include <components/esm/loadnpc.hpp>
#include <components/esm/npcstats.hpp>
#include <components/esm/loadclas.hpp>
#include <components/esm/loadspel.hpp>
#include <components/esm/activespells.hpp>

#include <components/openmw-mp/Base/BaseStructs.hpp>
#include <components/openmw-mp/Base/BaseNetCreature.hpp>

#include <RakNetTypes.h>

namespace mwmp
{
    struct CurrentContainer
    {
        std::string refId;
        int refNumIndex;
        int mpNum;
        bool loot;
    };

    struct JournalItem
    {
        std::string quest;
        int index;
        enum JOURNAL_ITEM_TYPE
        {
            ENTRY = 0,
            INDEX = 1
        };

        std::string actorRefId;

        int type; // 0 - An entire entry, 1 - An index
    };

    struct Faction
    {
        std::string factionId;
        int rank;
        int reputation;
        bool isExpelled;
    };

    struct Topic
    {
        std::string topicId;
    };

    struct Kill
    {
        std::string refId;
        int number;
    };

    struct Book
    {
        std::string bookId;
    };

    struct CellState
    {
        ESM::Cell cell;

        enum CELL_STATE_ACTION
        {
            LOAD = 0,
            UNLOAD = 1
        };

        int type; // 0 - Cell load, 1 - Cell unload
    };

    struct JournalChanges
    {
        std::vector<JournalItem> journalItems;
        unsigned int count;
    };

    struct FactionChanges
    {
        std::vector<Faction> factions;
        unsigned int count;

        enum FACTION_ACTION
        {
            RANK = 0,
            EXPULSION = 1,
            REPUTATION = 2
        };

        int action; // 0 - Rank, 1 - Expulsion state, 2 - Both
    };

    struct TopicChanges
    {
        std::vector<Topic> topics;
        unsigned int count;
    };

    struct KillChanges
    {
        std::vector<Kill> kills;
        unsigned int count;
    };

    struct BookChanges
    {
        std::vector<Book> books;
        unsigned int count;
    };

    struct MapChanges
    {
        std::vector<ESM::Cell> cellsExplored;
        unsigned int count;
    };

    struct SpellbookChanges
    {
        std::vector<ESM::Spell> spells;
        unsigned int count;
        enum ACTION_TYPE
        {
            SET = 0,
            ADD,
            REMOVE
        };
        int action; // 0 - Clear and set in entirety, 1 - Add spell, 2 - Remove spell
    };

    struct CellStateChanges
    {
        std::vector<CellState> cellStates;
        unsigned int count;
    };

    enum RESURRECT_TYPE
    {
        REGULAR = 0,
        IMPERIAL_SHRINE,
        TRIBUNAL_TEMPLE
    };

    class BasePlayer : public mwmp::BaseNetCreature
    {
    public:

        struct CGStage
        {
            int current, end;
        };

        struct GUIMessageBox
        {
            int id;
            int type;
            enum GUI_TYPE
            {
                MessageBox = 0,
                CustomMessageBox,
                InputDialog,
                PasswordDialog,
                ListBox
            };
            std::string label;
            std::string note;
            std::string buttons;

            std::string data;
        };

        BasePlayer(RakNet::RakNetGUID guid) : guid(guid)
        {
            inventoryChanges.action = 0;
            inventoryChanges.count = 0;
            spellbookChanges.action = 0;
            spellbookChanges.count = 0;
            useCreatureName = false;
            isWerewolf = false;
        }

        BasePlayer()
        {

        }

        ~BasePlayer()
        {

        }

        RakNet::RakNetGUID guid;
        GUIMessageBox guiMessageBox;
        int month;
        int day;
        double hour;

        SpellbookChanges spellbookChanges;
        JournalChanges journalChanges;
        FactionChanges factionChanges;
        TopicChanges topicChanges;
        KillChanges killChanges;
        BookChanges bookChanges;
        MapChanges mapChanges;
        CellStateChanges cellStateChanges;

        ESM::ActiveSpells activeSpells;
        CurrentContainer currentContainer;

        bool consoleAllowed;
        int difficulty;

        bool ignorePosPacket;

        ESM::Position previousCellPosition;
        ESM::NPC npc;
        ESM::NpcStats npcStats;
        ESM::Class charClass;
        std::string birthsign;
        std::string chatMessage;
        CGStage charGenStage;
        std::string passw;

        bool isWerewolf;
        std::string creatureModel;
        bool useCreatureName;

        std::string deathReason;

        int jailDays;
        bool ignoreJailTeleportation;
        bool ignoreJailSkillIncreases;
        std::string jailProgressText;
        std::string jailEndText;

        unsigned int resurrectType;

        bool diedSinceArrestAttempt;
    };
}

#endif //OPENMW_BASEPLAYER_HPP
