#ifndef OPENMW_PROCESSORCONTAINER_HPP
#define OPENMW_PROCESSORCONTAINER_HPP

#include "BaseObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorContainer : public BaseObjectProcessor
    {
    public:
        ProcessorContainer()
        {
            BPP_INIT(ID_CONTAINER)
        }

        virtual void Do(ObjectPacket &packet, ObjectList &objectList)
        {
            BaseObjectProcessor::Do(packet, objectList);

            LOG_APPEND(MWMPLog::LOG_VERBOSE, "- action: %i, containerSubAction: %i", objectList.action, objectList.containerSubAction);

            // If we've received a request for information, comply with it
            if (objectList.action == mwmp::BaseObjectList::REQUEST)
            {
                if (objectList.baseObjectCount == 0)
                {
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "- Request had no objects attached, so we are sending all containers in the cell %s",
                        objectList.cell.getDescription().c_str());
                    objectList.reset();
                    objectList.cell = *ptrCellStore->getCell();
                    objectList.action = mwmp::BaseObjectList::SET;
                    objectList.containerSubAction = mwmp::BaseObjectList::REPLY_TO_REQUEST;
                    objectList.addAllContainers(ptrCellStore);
                    objectList.sendContainer();
                }
                else
                {
                    LOG_APPEND(MWMPLog::LOG_VERBOSE, "- Request was for %i %s", objectList.baseObjectCount, objectList.baseObjectCount == 1 ? "object" : "objects");
                    std::vector<BaseObject> requestObjects = objectList.baseObjects;
                    objectList.reset();
                    objectList.cell = *ptrCellStore->getCell();
                    objectList.action = mwmp::BaseObjectList::SET;
                    objectList.containerSubAction = mwmp::BaseObjectList::REPLY_TO_REQUEST;
                    objectList.addRequestedContainers(ptrCellStore, requestObjects);

                    if (objectList.baseObjects.size() > 0)
                        objectList.sendContainer();
                }
            }
            // Otherwise, edit containers based on the information received
            else
            {
                LOG_APPEND(MWMPLog::LOG_VERBOSE, "- Editing container contents to match those of packet", objectList.baseObjectCount);
                objectList.editContainers(ptrCellStore);
            }
        }

    };
}

#endif //OPENMW_PROCESSORCONTAINER_HPP
