#ifndef OPENMW_PROCESSORSCRIPTGLOBALSHORT_HPP
#define OPENMW_PROCESSORSCRIPTGLOBALSHORT_HPP

#include "../ObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorScriptGlobalShort : public ObjectProcessor
    {
    public:
        ProcessorScriptGlobalShort()
        {
            BPP_INIT(ID_SCRIPT_GLOBAL_SHORT)
        }

        virtual void Do(ObjectPacket &packet, ObjectList &objectList)
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Received %s", strPacketID.c_str());
            objectList.setGlobalShorts();
        }
    };
}

#endif //OPENMW_PROCESSORSCRIPTGLOBALSHORT_HPP
