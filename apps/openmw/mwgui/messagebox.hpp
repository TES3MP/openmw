#ifndef MWGUI_MESSAGE_BOX_H
#define MWGUI_MESSAGE_BOX_H

#include <memory>

#include "windowbase.hpp"

namespace MyGUI
{
    class Widget;
    class Button;
    class EditBox;
}

namespace MWGui
{
    class InteractiveMessageBox;
    class MessageBoxManager;
    class MessageBox;
    class MessageBoxManager
    {
<<<<<<< HEAD
        public:
            MessageBoxManager (float timePerChar);
            ~MessageBoxManager ();
            void onFrame (float frameDuration);
            void createMessageBox (const std::string& message, bool stat = false);
            void removeStaticMessageBox ();
            /*
                Start of tes3mp change (major)

                Add a hasServerOrigin boolean to the list of arguments so those messageboxes
                can be differentiated from client-only ones
            */
            bool createInteractiveMessageBox (const std::string& message, const std::vector<std::string>& buttons, bool hasServerOrigin = false);
            /*
                End of tes3mp change (major)
            */
            bool isInteractiveMessageBox ();
=======
    public:
        MessageBoxManager(float timePerChar);
        ~MessageBoxManager();
        void onFrame(float frameDuration);
        void createMessageBox(std::string_view message, bool stat = false);
        void removeStaticMessageBox();
        bool createInteractiveMessageBox(std::string_view message, const std::vector<std::string>& buttons);
        bool isInteractiveMessageBox();
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        int getMessagesCount();

        const InteractiveMessageBox* getInteractiveMessageBox() const { return mInterMessageBoxe.get(); }

        /// Remove all message boxes
        void clear();

        bool removeMessageBox(MessageBox* msgbox);

        /// @param reset Reset the pressed button to -1 after reading it.
        int readPressedButton(bool reset = true);

        typedef MyGUI::delegates::CMultiDelegate1<int> EventHandle_Int;

        // Note: this delegate unassigns itself after it was fired, i.e. works once.
        EventHandle_Int eventButtonPressed;

        void onButtonPressed(int button)
        {
            eventButtonPressed(button);
            eventButtonPressed.clear();
        }

        void setVisible(bool value);

        const std::vector<std::unique_ptr<MessageBox>>& getActiveMessageBoxes() const;

    private:
        std::vector<std::unique_ptr<MessageBox>> mMessageBoxes;
        std::unique_ptr<InteractiveMessageBox> mInterMessageBoxe;
        MessageBox* mStaticMessageBox;
        float mMessageBoxSpeed;
        int mLastButtonPressed;
        bool mVisible = true;
    };

    class MessageBox : public Layout
    {
    public:
        MessageBox(MessageBoxManager& parMessageBoxManager, std::string_view message);
        const std::string& getMessage() { return mMessage; }
        int getHeight();
        void update(int height);
        void setVisible(bool value);

        float mCurrentTime;
        float mMaxTime;

    protected:
        MessageBoxManager& mMessageBoxManager;
        std::string mMessage;
        MyGUI::EditBox* mMessageWidget;
        int mBottomPadding;
        int mNextBoxPadding;
    };

    class InteractiveMessageBox : public WindowModal
    {
    public:
        InteractiveMessageBox(MessageBoxManager& parMessageBoxManager, const std::string& message,
            const std::vector<std::string>& buttons);
        void mousePressed(MyGUI::Widget* _widget);
        int readPressedButton();

        MyGUI::Widget* getDefaultKeyFocus() override;

        bool exit() override { return false; }

        bool mMarkedToDelete;

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Track whether the message box has a server origin
            */
            bool mHasServerOrigin = false;
            /*
                End of tes3mp addition
            */

        private:
            void buttonActivated (MyGUI::Widget* _widget);
=======
    private:
        void buttonActivated(MyGUI::Widget* _widget);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        MessageBoxManager& mMessageBoxManager;
        MyGUI::EditBox* mMessageWidget;
        MyGUI::Widget* mButtonsWidget;
        std::vector<MyGUI::Button*> mButtons;

        int mButtonPressed;
    };

}

#endif
