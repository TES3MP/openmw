//
// Created by koncord on 18.04.17.
//

#ifndef OPENMW_BASEOBJECTPROCESSOR_HPP
#define OPENMW_BASEOBJECTPROCESSOR_HPP

#include "../ObjectProcessor.hpp"
#include "apps/openmw/mwmp/Main.hpp"
#include "apps/openmw/mwmp/CellController.hpp"
#include "apps/openmw/mwworld/cellstore.hpp"

namespace mwmp
{
    class BaseObjectProcessor : public ObjectProcessor
    {
    public:
        virtual void Do(ObjectPacket &packet, ObjectList &objectList)
        {
            ptrCellStore = Main::get().getCellController()->getCellStore(objectList.cell);

            if (!ptrCellStore) return;

            LOG_MESSAGE_SIMPLE(MWMPLog::LOG_VERBOSE, "Received %s about %s", strPacketID.c_str(), objectList.cell.getDescription().c_str());
        }
    protected:
        MWWorld::CellStore *ptrCellStore;

    };
}

#endif //OPENMW_BASEOBJECTPROCESSOR_HPP
