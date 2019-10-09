//
// Created by koncord on 04.03.16.
//

#include "GUIChat.hpp"

#include <MyGUI_EditBox.h>
#include "apps/openmw/mwbase/environment.hpp"
#include "apps/openmw/mwgui/windowmanagerimp.hpp"
#include "apps/openmw/mwinput/inputmanagerimp.hpp"
#include <MyGUI_InputManager.h>
#include <components/openmw-mp/MWMPLog.hpp>

#include "../Networking.hpp"
#include "../Main.hpp"
#include "../LocalPlayer.hpp"

#include "../GUIController.hpp"


namespace mwmp
{
    GUIChat::GUIChat(int x, int y, int w, int h)
            : WindowBase("tes3mp_chat.layout")
    {
        setCoord(x, y, w, h);

        getWidget(mCommandLine, "edit_Command");
        getWidget(mHistory, "list_History");

        // Set up the command line box
        mCommandLine->eventEditSelectAccept +=
                newDelegate(this, &GUIChat::acceptCommand);
        mCommandLine->eventKeyButtonPressed +=
                newDelegate(this, &GUIChat::keyPress);

        setTitle("Chat");

        mHistory->setOverflowToTheLeft(true);
        mHistory->setEditWordWrap(true);
        mHistory->setTextShadow(true);
        mHistory->setTextShadowColour(MyGUI::Colour::Black);

        mHistory->setNeedKeyFocus(false);

        windowState = 0;
        mCommandLine->setVisible(0);
        delay = 3; // 3 sec.
    }

    void GUIChat::onOpen()
    {
        // Give keyboard focus to the combo box whenever the console is
        // turned on
        setEditState(0);
        windowState = CHAT_ENABLED;
    }

    void GUIChat::onClose()
    {
        // Apparently, hidden widgets can retain key focus
        // Remove for MyGUI 3.2.2
        windowState = CHAT_DISABLED;
        setEditState(0);
    }

    bool GUIChat::exit()
    {
        //WindowBase::exit();
        return true;
    }

    void GUIChat::acceptCommand(MyGUI::EditBox *_sender)
    {
        const std::string &cm =  MyGUI::TextIterator::toTagsString(mCommandLine->getCaption());
        
        // If they enter nothing, then it should be canceled.
        // Otherwise, there's no way of closing without having text.
        if (cm.empty())
        {
            mCommandLine->setCaption("");
            setEditState(0);
            return;
        }

        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Player: %s", cm.c_str());

        // Add the command to the history, and set the current pointer to
        // the end of the list
        if (mCommandHistory.empty() || mCommandHistory.back() != cm)
            mCommandHistory.push_back(cm);
        mCurrent = mCommandHistory.end();
        mEditString.clear();

        // Reset the command line before the command execution.
        // It prevents the re-triggering of the acceptCommand() event for the same command
        // during the actual command execution
        mCommandLine->setCaption("");
        setEditState(0);
        send (cm);
    }

    void GUIChat::onResChange(int width, int height)
    {
        setCoord(10,10, width-10, height/2);
    }

    void GUIChat::setFont(const std::string &fntName)
    {
        mHistory->setFontName(fntName);
        mCommandLine->setFontName(fntName);
    }

    void GUIChat::print(const std::string &msg, const std::string &color)
    {
        if (windowState == 2 && !isVisible())
        {
            setVisible(true);
        }

        if(msg.size() == 0)
        {
            clean();
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "Chat cleaned");
        }
        else
        {
            mHistory->addText(color + msg);
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_INFO, "%s", msg.c_str());
        }
    }

    void GUIChat::printOK(const std::string &msg)
    {
        print(msg + "\n", "#FF00FF");
    }

    void GUIChat::printError(const std::string &msg)
    {
        print(msg + "\n", "#FF2222");
    }

    void GUIChat::send(const std::string &str)
    {
        LocalPlayer *localPlayer = Main::get().getLocalPlayer();

        Networking *networking = Main::get().getNetworking();

        localPlayer->chatMessage = str;

        networking->getPlayerPacket(ID_CHAT_MESSAGE)->setPlayer(localPlayer);
        networking->getPlayerPacket(ID_CHAT_MESSAGE)->Send();
    }

    void GUIChat::clean()
    {
        mHistory->setCaption("");
    }

    void GUIChat::pressedChatMode()
    {
        windowState++;
        if (windowState == 3) windowState = 0;

        std::string chatMode = windowState == CHAT_DISABLED ? "Chat hidden" :
                               windowState == CHAT_ENABLED ? "Chat visible" :
                               "Chat appearing when needed";

        LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Switch chat mode to %s", chatMode.c_str());
        MWBase::Environment::get().getWindowManager()->messageBox(chatMode);

        switch (windowState)
        {
            case CHAT_DISABLED:
                this->mMainWidget->setVisible(false);
                setEditState(0);
                break;
            case CHAT_ENABLED:
                this->mMainWidget->setVisible(true);
                break;
            default: //CHAT_HIDDENMODE
                this->mMainWidget->setVisible(true);
                curTime = 0;
        }
    }

    void GUIChat::setEditState(bool state)
    {
        editState = state;
        mCommandLine->setVisible(editState);
        MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(editState ? mCommandLine : nullptr);
    }

    void GUIChat::pressedSay()
    {
        if (windowState == CHAT_DISABLED)
            return;

        if (!mCommandLine->getVisible())
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Opening chat.");

        if (windowState == CHAT_HIDDENMODE)
        {
            setVisible(true);
            curTime = 0;
        }

        setEditState(true);
    }

    void GUIChat::keyPress(MyGUI::Widget *_sender, MyGUI::KeyCode key, MyGUI::Char _char)
    {
        if (mCommandHistory.empty()) return;

        // Traverse history with up and down arrows
        if (key == MyGUI::KeyCode::ArrowUp)
        {
            // If the user was editing a string, store it for later
            if (mCurrent == mCommandHistory.end())
                mEditString = mCommandLine->getOnlyText();

            if (mCurrent != mCommandHistory.begin())
            {
                --mCurrent;
                mCommandLine->setCaption(*mCurrent);
            }
        }
        else if (key == MyGUI::KeyCode::ArrowDown)
        {
            if (mCurrent != mCommandHistory.end())
            {
                ++mCurrent;

                if (mCurrent != mCommandHistory.end())
                    mCommandLine->setCaption(*mCurrent);
                else
                    // Restore the edit string
                    mCommandLine->setCaption(mEditString);
            }
        }

    }

    void GUIChat::update(float dt)
    {
        if (windowState == CHAT_HIDDENMODE && !editState && isVisible())
        {
            curTime += dt;
            if (curTime >= delay)
            {
                setEditState(false);
                this->mMainWidget->setVisible(false);
            }
        }
    }

    void GUIChat::setDelay(float delay)
    {
        this->delay = delay;
    }
}
