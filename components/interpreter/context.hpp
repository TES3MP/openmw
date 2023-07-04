#ifndef INTERPRETER_CONTEXT_H_INCLUDED
#define INTERPRETER_CONTEXT_H_INCLUDED

#include <components/esm/refid.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace Interpreter
{
    class Context
    {
    public:
        virtual ~Context() {}

<<<<<<< HEAD
            /*
                Start of tes3mp addition

                Keep an enumeration of the possible context types
            */
            enum CONTEXT_TYPE
            {
                CONSOLE = 0,
                DIALOGUE = 1,
                SCRIPT_LOCAL = 2,
                SCRIPT_GLOBAL = 3
            };
            /*
                End of tes3mp addition
            */

            virtual ~Context() {}
=======
        virtual ESM::RefId getTarget() const = 0;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        virtual int getLocalShort(int index) const = 0;

        virtual int getLocalLong(int index) const = 0;

        virtual float getLocalFloat(int index) const = 0;

        virtual void setLocalShort(int index, int value) = 0;

        virtual void setLocalLong(int index, int value) = 0;

        virtual void setLocalFloat(int index, float value) = 0;

        virtual void messageBox(std::string_view message, const std::vector<std::string>& buttons) = 0;

        void messageBox(std::string_view message)
        {
            std::vector<std::string> empty;
            messageBox(message, empty);
        }

        virtual void report(const std::string& message) = 0;

        virtual int getGlobalShort(std::string_view name) const = 0;

        virtual int getGlobalLong(std::string_view name) const = 0;

        virtual float getGlobalFloat(std::string_view name) const = 0;

        virtual void setGlobalShort(std::string_view name, int value) = 0;

        virtual void setGlobalLong(std::string_view name, int value) = 0;

        virtual void setGlobalFloat(std::string_view name, float value) = 0;

        virtual std::vector<std::string> getGlobals() const = 0;

        virtual char getGlobalType(std::string_view name) const = 0;

        virtual std::string getActionBinding(std::string_view action) const = 0;

        virtual std::string_view getActorName() const = 0;

        virtual std::string_view getNPCRace() const = 0;

        virtual std::string_view getNPCClass() const = 0;

        virtual std::string_view getNPCFaction() const = 0;

        virtual std::string_view getNPCRank() const = 0;

        virtual std::string_view getPCName() const = 0;

        virtual std::string_view getPCRace() const = 0;

        virtual std::string_view getPCClass() const = 0;

        virtual std::string_view getPCRank() const = 0;

        virtual std::string_view getPCNextRank() const = 0;

        virtual int getPCBounty() const = 0;

        virtual std::string_view getCurrentCellName() const = 0;

        virtual int getMemberShort(ESM::RefId id, std::string_view name, bool global) const = 0;

        virtual int getMemberLong(ESM::RefId id, std::string_view name, bool global) const = 0;

        virtual float getMemberFloat(ESM::RefId id, std::string_view name, bool global) const = 0;

        virtual void setMemberShort(ESM::RefId id, std::string_view name, int value, bool global) = 0;

        virtual void setMemberLong(ESM::RefId id, std::string_view name, int value, bool global) = 0;

<<<<<<< HEAD
            virtual void setMemberFloat (const std::string& id, const std::string& name, float value, bool global)
                = 0;

            /*
                Start of tes3mp addition

                Used for tracking and checking the type of this Context, as well as
                its current script
            */
            virtual unsigned short getContextType() const = 0;
            virtual std::string getCurrentScriptName() const = 0;
            virtual void trackContextType(unsigned short interpreterType) = 0;
            virtual void trackCurrentScriptName(const std::string& name) = 0;
            /*
                End of tes3mp addition
            */
=======
        virtual void setMemberFloat(ESM::RefId id, std::string_view name, float value, bool global) = 0;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
    };
}

#endif
