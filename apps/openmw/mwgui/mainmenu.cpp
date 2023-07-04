#include "mainmenu.hpp"

#include <MyGUI_Gui.h>
#include <MyGUI_RenderManager.h>
#include <MyGUI_TextBox.h>

#include <components/settings/settings.hpp>
#include <components/vfs/manager.hpp>
#include <components/widgets/imagebutton.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/statemanager.hpp"
#include "../mwbase/windowmanager.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/globals.hpp"

#include "backgroundimage.hpp"
#include "confirmationdialog.hpp"
#include "savegamedialog.hpp"
#include "videowidget.hpp"

namespace MWGui
{

    MainMenu::MainMenu(int w, int h, const VFS::Manager* vfs, const std::string& versionDescription)
        : WindowBase("openmw_mainmenu.layout")
        , mWidth(w)
        , mHeight(h)
        , mVFS(vfs)
        , mButtonBox(nullptr)
        , mBackground(nullptr)
        , mVideoBackground(nullptr)
        , mVideo(nullptr)
    {
        getWidget(mVersionText, "VersionText");
        mVersionText->setCaption(versionDescription);

        mHasAnimatedMenu = mVFS->exists("video/menu_background.bik");

        updateMenu();
    }

    void MainMenu::onResChange(int w, int h)
    {
        mWidth = w;
        mHeight = h;

        updateMenu();
    }

    void MainMenu::setVisible(bool visible)
    {
        if (visible)
            updateMenu();

        bool isMainMenu = MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_MainMenu)
            && MWBase::Environment::get().getStateManager()->getState() == MWBase::StateManager::State_NoGame;

        showBackground(isMainMenu);

        if (visible)
        {
            if (isMainMenu)
            {
                if (mButtons["loadgame"]->getVisible())
                    MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mButtons["loadgame"]);
                else
                    MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mButtons["newgame"]);
            }
            else
                MWBase::Environment::get().getWindowManager()->setKeyFocusWidget(mButtons["return"]);
        }

        Layout::setVisible(visible);
    }

    void MainMenu::onNewGameConfirmed()
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_MainMenu);
        MWBase::Environment::get().getStateManager()->newGame();
    }

    void MainMenu::onExitConfirmed()
    {
        MWBase::Environment::get().getStateManager()->requestQuit();
    }

    void MainMenu::onButtonClicked(MyGUI::Widget* sender)
    {
        MWBase::WindowManager* winMgr = MWBase::Environment::get().getWindowManager();

        const std::string& name = *sender->getUserData<std::string>();
        winMgr->playSound(ESM::RefId::stringRefId("Menu Click"));
        if (name == "return")
        {
            winMgr->removeGuiMode(GM_MainMenu);
        }
        else if (name == "options")
            winMgr->pushGuiMode(GM_Settings);
        else if (name == "credits")
            winMgr->playVideo("mw_credits.bik", true);
        else if (name == "exitgame")
        {
            if (MWBase::Environment::get().getStateManager()->getState() == MWBase::StateManager::State_NoGame)
                onExitConfirmed();
            else
            {
                ConfirmationDialog* dialog = winMgr->getConfirmationDialog();
                dialog->askForConfirmation("#{OMWEngine:QuitGameConfirmation}");
                dialog->eventOkClicked.clear();
                dialog->eventOkClicked += MyGUI::newDelegate(this, &MainMenu::onExitConfirmed);
                dialog->eventCancelClicked.clear();
            }
        }
        else if (name == "newgame")
        {
            if (MWBase::Environment::get().getStateManager()->getState() == MWBase::StateManager::State_NoGame)
                onNewGameConfirmed();
            else
            {
                ConfirmationDialog* dialog = winMgr->getConfirmationDialog();
                dialog->askForConfirmation("#{OMWEngine:NewGameConfirmation}");
                dialog->eventOkClicked.clear();
                dialog->eventOkClicked += MyGUI::newDelegate(this, &MainMenu::onNewGameConfirmed);
                dialog->eventCancelClicked.clear();
            }
        }

        else
        {
            if (!mSaveGameDialog)
                mSaveGameDialog = std::make_unique<SaveGameDialog>();
            if (name == "loadgame")
                mSaveGameDialog->setLoadOrSave(true);
            else if (name == "savegame")
                mSaveGameDialog->setLoadOrSave(false);
            mSaveGameDialog->setVisible(true);
        }
    }

    void MainMenu::showBackground(bool show)
    {
        if (mVideo && !show)
        {
            MyGUI::Gui::getInstance().destroyWidget(mVideoBackground);
            mVideoBackground = nullptr;
            mVideo = nullptr;
        }
        if (mBackground && !show)
        {
            MyGUI::Gui::getInstance().destroyWidget(mBackground);
            mBackground = nullptr;
        }

        if (!show)
            return;

        bool stretch = Settings::Manager::getBool("stretch menu background", "GUI");

        if (mHasAnimatedMenu)
        {
            if (!mVideo)
            {
                // Use black background to correct aspect ratio
                mVideoBackground = MyGUI::Gui::getInstance().createWidgetReal<MyGUI::ImageBox>(
                    "ImageBox", 0, 0, 1, 1, MyGUI::Align::Default, "MainMenuBackground");
                mVideoBackground->setImageTexture("black");

                mVideo = mVideoBackground->createWidget<VideoWidget>(
                    "ImageBox", 0, 0, 1, 1, MyGUI::Align::Stretch, "MainMenuBackground");
                mVideo->setVFS(mVFS);

                mVideo->playVideo("video\\menu_background.bik");
            }

            MyGUI::IntSize viewSize = MyGUI::RenderManager::getInstance().getViewSize();
            int screenWidth = viewSize.width;
            int screenHeight = viewSize.height;
            mVideoBackground->setSize(screenWidth, screenHeight);

            mVideo->autoResize(stretch);

            mVideo->setVisible(true);
        }
        else
        {
            if (!mBackground)
            {
                mBackground = MyGUI::Gui::getInstance().createWidgetReal<BackgroundImage>(
                    "ImageBox", 0, 0, 1, 1, MyGUI::Align::Stretch, "MainMenuBackground");
                mBackground->setBackgroundImage("textures\\menu_morrowind.dds", true, stretch);
            }
            mBackground->setVisible(true);
        }
    }

    void MainMenu::onFrame(float dt)
    {
        if (mVideo)
        {
            if (!mVideo->update())
            {
                // If finished playing, start again
                mVideo->playVideo("video\\menu_background.bik");
            }
        }
    }

    bool MainMenu::exit()
    {
        return MWBase::Environment::get().getStateManager()->getState() == MWBase::StateManager::State_Running;
    }

    void MainMenu::updateMenu()
    {
        setCoord(0, 0, mWidth, mHeight);

        if (!mButtonBox)
            mButtonBox
                = mMainWidget->createWidget<MyGUI::Widget>({}, MyGUI::IntCoord(0, 0, 0, 0), MyGUI::Align::Default);

        int curH = 0;

        MWBase::StateManager::State state = MWBase::Environment::get().getStateManager()->getState();

        mVersionText->setVisible(state == MWBase::StateManager::State_NoGame);

        std::vector<std::string> buttons;

        if (state == MWBase::StateManager::State_Running)
            buttons.emplace_back("return");

        /*
            Start of tes3mp change (major)

            In multiplayer, the main menu should not have options for starting or loading the game,
            so they have been removed

            Saving the game should still be possible, as long as it's clear that the resulting
            save is singleplayer-only; this will prevent players from completely losing their
            characters and houses on servers if those servers ever go down
        */

        //buttons.emplace_back("newgame");

        if (state == MWBase::StateManager::State_Running
            && MWBase::Environment::get().getWorld()->getGlobalInt(MWWorld::Globals::sCharGenState) == -1
            && MWBase::Environment::get().getWindowManager()->isSavingAllowed())
            buttons.emplace_back("savegame");

<<<<<<< HEAD
        /*
        if (MWBase::Environment::get().getStateManager()->characterBegin()!=
            MWBase::Environment::get().getStateManager()->characterEnd())
=======
        if (MWBase::Environment::get().getStateManager()->characterBegin()
            != MWBase::Environment::get().getStateManager()->characterEnd())
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
            buttons.emplace_back("loadgame");
        */

        /*
            End of tes3mp change (major)
        */

        buttons.emplace_back("options");

        if (state == MWBase::StateManager::State_NoGame)
            buttons.emplace_back("credits");

        buttons.emplace_back("exitgame");

        // Create new buttons if needed
        for (std::string_view id : { "return", "newgame", "savegame", "loadgame", "options", "credits", "exitgame" })
        {
            if (mButtons.find(id) == mButtons.end())
            {
                Gui::ImageButton* button = mButtonBox->createWidget<Gui::ImageButton>(
                    "ImageBox", MyGUI::IntCoord(0, curH, 0, 0), MyGUI::Align::Default);
                const std::string& buttonId = mButtons.emplace(id, button).first->first;
                button->setProperty("ImageHighlighted", "textures\\menu_" + buttonId + "_over.dds");
                button->setProperty("ImageNormal", "textures\\menu_" + buttonId + ".dds");
                button->setProperty("ImagePushed", "textures\\menu_" + buttonId + "_pressed.dds");
                button->eventMouseButtonClick += MyGUI::newDelegate(this, &MainMenu::onButtonClicked);
                button->setUserData(buttonId);
            }
        }

        // Start by hiding all buttons
        int maxwidth = 0;
        for (const auto& buttonPair : mButtons)
        {
            buttonPair.second->setVisible(false);
            MyGUI::IntSize requested = buttonPair.second->getRequestedSize();
            if (requested.width > maxwidth)
                maxwidth = requested.width;
        }

        // Now show and position the ones we want
        for (const std::string& buttonId : buttons)
        {
            auto it = mButtons.find(buttonId);
            assert(it != mButtons.end());
            Gui::ImageButton* button = it->second;
            button->setVisible(true);

            // By default, assume that all menu buttons textures should have 64 height.
            // If they have a different resolution, scale them.
            MyGUI::IntSize requested = button->getRequestedSize();
            float scale = requested.height / 64.f;

            button->setImageCoord(MyGUI::IntCoord(0, 0, requested.width, requested.height));
            // Trim off some of the excessive padding
            // TODO: perhaps do this within ImageButton?
            int height = requested.height;
            button->setImageTile(MyGUI::IntSize(requested.width, requested.height - 16 * scale));
            button->setCoord(
                (maxwidth - requested.width / scale) / 2, curH, requested.width / scale, height / scale - 16);
            curH += height / scale - 16;
        }

        if (state == MWBase::StateManager::State_NoGame)
        {
            // Align with the background image
            int bottomPadding = 24;
            mButtonBox->setCoord(mWidth / 2 - maxwidth / 2, mHeight - curH - bottomPadding, maxwidth, curH);
        }
        else
            mButtonBox->setCoord(mWidth / 2 - maxwidth / 2, mHeight / 2 - curH / 2, maxwidth, curH);
    }
}
