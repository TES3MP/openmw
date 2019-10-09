#ifndef OPENMW_PROCESSORMUSICPLAY_HPP
#define OPENMW_PROCESSORMUSICPLAY_HPP

#include "../ObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorMusicPlay : public ObjectProcessor
    {
    public:
        ProcessorMusicPlay()
        {
            BPP_INIT(ID_MUSIC_PLAY)
        }

        virtual void Do(ObjectPacket &packet, ObjectList &objectList)
        {
            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Received %s", strPacketID.c_str());
            objectList.playMusic();
        }
    };
}

#endif //OPENMW_PROCESSORSCRIPTGLOBALSHORT_HPP
