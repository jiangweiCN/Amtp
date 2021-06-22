#include "msg_event_objects.hpp"
#include "../../jlog/jLog.h"

msg_event_objects::msg_event_objects()
{
    event_objects_map = new EVENT_OBJECTS_MAP();
}

msg_event_objects::~msg_event_objects()
{
	
}

EVENT_OBJECT_S *msg_event_objects::add(int handle, uint32_t cmd)
{
    remove(handle);
    // JLOG(INFO) << "add event objects, handle = " << handle;
    unique_lock<std::mutex> wait_lock(event_objects_map_mutex);
    EVENT_OBJECT_S *stru = new EVENT_OBJECT_S(handle, cmd, nullptr);
    (*event_objects_map)[handle] = stru;
    // JLOG(INFO) << "add objects count = " << event_objects_map->size();
    return stru;
}

EVENT_OBJECT_S *msg_event_objects::get(int handle)
{
    EVENT_OBJECT_S *stru = nullptr;
    unique_lock<std::mutex> wait_lock(event_objects_map_mutex);
    EVENT_OBJECTS_MAP::iterator it;
    it = event_objects_map->find(handle);
    if (it != event_objects_map->end())
    {
        // JLOG(INFO) << "get has objects handle = " << handle;
        stru = (*event_objects_map)[handle];
    }
    // JLOG(INFO) << "get objects count = " << event_objects_map->size();
    return stru;
}
void msg_event_objects::remove(int handle)
{
    unique_lock<std::mutex> wait_lock(event_objects_map_mutex);
    EVENT_OBJECTS_MAP::iterator it;
    it = event_objects_map->find(handle);
    if (it != event_objects_map->end())
    {
        // JLOG(INFO) << "remove has objects handle = " << handle;
        EVENT_OBJECT_S *stru = (*event_objects_map)[handle];
        if (stru != nullptr)
        {
            // JLOG(INFO) << "rstru != nullptr, handle = " << handle;
            delete stru;
        }
        event_objects_map->erase(it);
    }
    // JLOG(INFO) << "remove objects count = " << event_objects_map->size();
}
