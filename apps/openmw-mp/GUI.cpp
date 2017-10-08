//
// Created by koncord on 15.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include "GUI.hpp"
#include "Player.hpp"
#include "Networking.hpp"

void GUI::Init(LuaState &lua)
{
    lua.getState()->new_usertype<GUI>("GUI",
                                      "messageBox", &GUI::messageBox,
                                      "customMessageBox", &GUI::customMessageBox,
                                      "inputDialog", &GUI::inputDialog,
                                      "passwordDialog", &GUI::passwordDialog,
                                      "listBox", &GUI::listBox,
                                      "setMapVisibility", &GUI::setMapVisibility,
                                      "setMapVisibilityAll", &GUI::setMapVisibilityAll
    );
}

GUI::GUI(Player *player): player(player), changed(false)
{

}

GUI::~GUI()
{

}

void GUI::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GUI_MESSAGEBOX);
    packet->setPlayer(player);
    packet->Send(false);
}

void GUI::messageBox(int id, const char *label)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.type = Player::GUIMessageBox::MessageBox;

    changed = true;
}

void GUI::customMessageBox(int id, const char *label, const char *buttons)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.buttons = buttons;
    player->guiMessageBox.type = Player::GUIMessageBox::CustomMessageBox;

    changed = true;
}

void GUI::inputDialog(int id, const char *label)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.type = Player::GUIMessageBox::InputDialog;

    changed = true;
}

void GUI::passwordDialog(int id, const char *label, const char *note)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.note = note;
    player->guiMessageBox.type = Player::GUIMessageBox::PasswordDialog;

    changed = true;
}

void GUI::listBox(int id, const char *label, const char *items)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.data = items;
    player->guiMessageBox.type = Player::GUIMessageBox::ListBox;

    changed = true;
}

void GUI::setMapVisibility(unsigned short targetPID, unsigned short affectedPID, unsigned short state)
{
    LOG_MESSAGE(Log::LOG_WARN, "stub");
}

void GUI::setMapVisibilityAll(unsigned short targetPID, unsigned short state)
{
    LOG_MESSAGE(Log::LOG_WARN, "stub");
}
