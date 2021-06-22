#ifndef MSG_EVENT_OBJECTS_H
#define MSG_EVENT_OBJECTS_H

#include <memory>
#include <map>
#include <condition_variable>

using namespace std;

using EVENT_OBJECT_S = struct event_object_s
{
	int handle;
	uint32_t cmd;
	uint8_t *msg_stru_data;
	uint32_t msg_stru_data_len;
	mutex mtx;
	condition_variable cv;
	event_object_s(int _handle, uint32_t _cmd, uint8_t *_msg_stru_data)
	{
		handle = _handle;
		cmd = _cmd;
		msg_stru_data = _msg_stru_data;
		msg_stru_data_len = 0;
	}
	~event_object_s()
	{
		if (msg_stru_data != nullptr)
		{
			delete[] msg_stru_data;
		}
	}
};

using EVENT_OBJECTS_MAP = map<int, EVENT_OBJECT_S *>;

class msg_event_objects
{
public:
	msg_event_objects();
	~msg_event_objects();

public:
	EVENT_OBJECT_S *add(int handle, uint32_t cmd);
	EVENT_OBJECT_S *get(int handle);
	void remove(int handle);

private:
	mutex event_objects_map_mutex;
	EVENT_OBJECTS_MAP *event_objects_map;
};
#endif