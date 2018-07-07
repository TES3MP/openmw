#ifndef OPENMW_QUESTAPI_HPP
#define OPENMW_QUESTAPI_HPP

#define QUESTAPI \
    {"InitializeJournalChanges",  QuestFunctions::InitializeJournalChanges},\
    {"InitializeKillChanges",     QuestFunctions::InitializeKillChanges},\
    \
    {"GetJournalChangesSize",     QuestFunctions::GetJournalChangesSize},\
    {"GetKillChangesSize",        QuestFunctions::GetKillChangesSize},\
    \
    {"AddJournalEntry",           QuestFunctions::AddJournalEntry},\
    {"AddJournalIndex",           QuestFunctions::AddJournalIndex},\
    {"AddKill",                   QuestFunctions::AddKill},\
    \
    {"SetReputation",             QuestFunctions::SetReputation},\
    \
    {"GetJournalItemQuest",       QuestFunctions::GetJournalItemQuest},\
    {"GetJournalItemIndex",       QuestFunctions::GetJournalItemIndex},\
    {"GetJournalItemType",        QuestFunctions::GetJournalItemType},\
    {"GetJournalItemActorRefId",  QuestFunctions::GetJournalItemActorRefId},\
    {"GetKillRefId",              QuestFunctions::GetKillRefId},\
    {"GetKillNumber",             QuestFunctions::GetKillNumber},\
    \
    {"GetReputation",             QuestFunctions::GetReputation},\
    \
    {"SendJournalChanges",        QuestFunctions::SendJournalChanges},\
    {"SendKillChanges",           QuestFunctions::SendKillChanges},\
    {"SendReputation",            QuestFunctions::SendReputation}

class QuestFunctions
{
public:

    /**
    * \brief Clear the last recorded journal changes for a player.
    *
    * This is used to initialize the sending of new PlayerJournal packets.
    *
    * \param pid The player ID whose journal changes should be used.
    * \return void
    */
    static void InitializeJournalChanges(unsigned short pid) noexcept;

    /**
    * \brief Clear the last recorded kill count changes for a player.
    *
    * This is used to initialize the sending of new PlayerKillCount packets.
    *
    * \param pid The player ID whose kill count changes should be used.
    * \return void
    */
    static void InitializeKillChanges(unsigned short pid) noexcept;

    /**
    * \brief Get the number of indexes in a player's latest journal changes.
    *
    * \param pid The player ID whose journal changes should be used.
    * \return The number of indexes.
    */
    static unsigned int GetJournalChangesSize(unsigned short pid) noexcept;

    /**
    * \brief Get the number of indexes in a player's latest kill count changes.
    *
    * \param pid The player ID whose kill count changes should be used.
    * \return The number of indexes.
    */
    static unsigned int GetKillChangesSize(unsigned short pid) noexcept;

    /**
    * \brief Add a new journal item of type ENTRY to the journal changes for a player.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param quest The quest of the journal item.
    * \param index The quest index of the journal item.
    * \param actorRefId The actor refId of the journal item.
    * \return void
    */
    static void AddJournalEntry(unsigned short pid, const char* quest, unsigned int index, const char* actorRefId) noexcept;

    /**
    * \brief Add a new journal item of type INDEX to the journal changes for a player.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param quest The quest of the journal item.
    * \param index The quest index of the journal item.
    * \return void
    */
    static void AddJournalIndex(unsigned short pid, const char* quest, unsigned int index) noexcept;

    /**
    * \brief Add a new kill count to the kill count changes for a player.
    *
    * \param pid The player ID whose kill count changes should be used.
    * \param refId The refId of the kill count.
    * \param number The number of kills in the kill count.
    * \return void
    */
    static void AddKill(unsigned short pid, const char* refId, int number) noexcept;

    /**
    * \brief Set the reputation of a certain player.
    *
    * \param pid The player ID.
    * \param value The reputation.
    * \return void
    */
    static void SetReputation(unsigned short pid, int value) noexcept;

    /**
    * \brief Get the quest at a certain index in a player's latest journal changes.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param i The index of the journalItem.
    * \return The quest.
    */
    static const char *GetJournalItemQuest(unsigned short pid, unsigned int i) noexcept;

    /**
    * \brief Get the quest index at a certain index in a player's latest journal changes.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param i The index of the journalItem.
    * \return The quest index.
    */
    static int GetJournalItemIndex(unsigned short pid, unsigned int i) noexcept;

    /**
    * \brief Get the journal item type at a certain index in a player's latest journal changes.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param i The index of the journalItem.
    * \return The type (0 for ENTRY, 1 for INDEX).
    */
    static int GetJournalItemType(unsigned short pid, unsigned int i) noexcept;

    /**
    * \brief Get the actor refId at a certain index in a player's latest journal changes.
    *
    * Every journal change has an associated actor, which is usually the quest giver.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param i The index of the journalItem.
    * \return The actor refId.
    */
    static const char *GetJournalItemActorRefId(unsigned short pid, unsigned int i) noexcept;

    /**
    * \brief Get the refId at a certain index in a player's latest kill count changes.
    *
    * \param pid The player ID whose kill count changes should be used.
    * \param i The index of the kill count.
    * \return The refId.
    */
    static const char *GetKillRefId(unsigned short pid, unsigned int i) noexcept;

    /**
    * \brief Get the number of kills at a certain index in a player's latest kill count changes.
    *
    * \param pid The player ID whose kill count changes should be used.
    * \param i The index of the kill count.
    * \return The number of kills.
    */
    static int GetKillNumber(unsigned short pid, unsigned int i) noexcept;

    /**
    * \brief Get the a certain player's reputation.
    *
    * \param pid The player ID.
    * \return The reputation.
    */
    static int GetReputation(unsigned short pid) noexcept;

    /**
    * \brief Send a PlayerJournal packet with a player's recorded journal changes.
    *
    * \param pid The player ID whose journal changes should be used.
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param sendToAttachedPlayer Whether the packet should be sent to the player attached
    *                             to the packet (true by default).
    * \return void
    */
    static void SendJournalChanges(unsigned short pid, bool sendToOtherPlayers = false, bool sendToAttachedPlayer = true) noexcept;

    /**
    * \brief Send a PlayerKillCount packet with a player's recorded kill count changes.
    *
    * \param pid The player ID whose kill count changes should be used.
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param sendToAttachedPlayer Whether the packet should be sent to the player attached
    *                             to the packet (true by default).
    * \return void
    */
    static void SendKillChanges(unsigned short pid, bool sendToOtherPlayers = false, bool sendToAttachedPlayer = true) noexcept;

    /**
    * \brief Send a PlayerReputation packet with a player's recorded reputation.
    *
    * \param pid The player ID whose reputation should be used.
    * \param sendToOtherPlayers Whether this packet should be sent to players other than the
    *                           player attached to the packet (false by default).
    * \param sendToAttachedPlayer Whether the packet should be sent to the player attached
    *                             to the packet (true by default).
    * \return void
    */
    static void SendReputation(unsigned short pid, bool sendToOtherPlayers = false, bool sendToAttachedPlayer = true) noexcept;

private:

};

#endif //OPENMW_QUESTAPI_HPP
