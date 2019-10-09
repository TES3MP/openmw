#ifndef OPENMW_PROCESSORSCRIPTMEMBERFLOAT_HPP
#define OPENMW_PROCESSORSCRIPTMEMBERFLOAT_HPP

#include "../ObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorScriptMemberFloat : public ObjectProcessor
    {
    public:
        ProcessorScriptMemberFloat()
        {
            BPP_INIT(ID_SCRIPT_MEMBER_FLOAT)
        }

        virtual void Do(ObjectPacket &packet, ObjectList &objectList)
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Received %s", strPacketID.c_str());
            //objectList.setMemberFloats();
        }
    };
}

#endif //OPENMW_PROCESSORSCRIPTMEMBERFLOAT_HPP
