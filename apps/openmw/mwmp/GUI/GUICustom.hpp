#pragma once
#ifndef OPENMW_GUICUSTOM_HPP
#define OPENMW_GUICUSTOM_HPP

#include "apps/openmw/mwgui/windowbase.hpp"
#include "../LocalPlayer.hpp"

namespace mwmp
{
    class GUIController;
    
    class GUICustom : public MWGui::WindowBase
    {
        friend class GUIController;
        public:
            GUICustom(int id, const std::string& layout);
        private:
            
            static const std::string BUTTON_PRESSED;
            static const std::string MOUSE_CLICK;
            static const std::string FIELD;
            static const std::string BIND;
            static const std::string ANCHOR;
            static const std::string RELATIVE_POSITION;

            static void log(std::string event, std::string name, std::string data);

            int id;

            void send(std::string event, std::string data);

            void updateProperties(BasePlayer::FieldList properties);
            void positionRelatively();

            void traverse(MyGUI::Widget* widget);
            template<class T>
            void attachEventHandlers(T* widget);
            template<>
            void attachEventHandlers(MyGUI::Widget* widget);
            template<>
            void attachEventHandlers(MyGUI::ListBox* widget);
            std::map<std::string, MyGUI::Widget*> fieldWidgets;
            void findFields(MyGUI::Widget* widget);
            std::map<std::string, std::pair<MyGUI::Widget*, std::string>> propertyMap;
            void findPropertyBindings(MyGUI::Widget* widget);

            void buttonPressed(MyGUI::Widget* _sender, MyGUI::KeyCode key, MyGUI::Char _char);
            void mouseClick(MyGUI::Widget* _sender);
            void listMouseItemActivate(MyGUI::ListBox* _sender, size_t _index);

            void collectFields();
    };
}
#endif //OPENMW_GUICUSTOM_HPP
