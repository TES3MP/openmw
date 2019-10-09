#ifndef OPENMW_PROCESSORVIDEOPLAY_HPP
#define OPENMW_PROCESSORVIDEOPLAY_HPP

#include "../ObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorVideoPlay : public ObjectProcessor
    {
    public:
        ProcessorVideoPlay()
        {
            BPP_INIT(ID_VIDEO_PLAY)
        }

        virtual void Do(ObjectPacket &packet, ObjectList &objectList)
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Received %s", strPacketID.c_str());
            objectList.playVideo();
        }
    };
}

#endif //OPENMW_PROCESSORVIDEOPLAY_HPP
