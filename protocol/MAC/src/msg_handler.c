/**********************************************************
* @file msg_handler.c
* 
* @brief  mac msg interface
* @author   guicheng.liu
* @date     2019/10/31
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "log.h"
#include "typedef.h"
#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

#if 0
msgq_type get_msgq_type(const task_id taskId)
{
	msgq_type type = MAX_TASK;

	switch (taskId)
	{
		case PHY_TASK:
		{
			type = PHY_QUEUE;
			break;
		}
		case MAC_PRE_TASK:
		{
			type = MAC_PRE_QUEUE;
			break;
		}
		case MAC_MAIN_TASK:
		{
			type = MAC_MAIN_QUEUE;
			break;
		}
		case RLC_TASK:
		{
			type = RLC_QUEUE;
			break;
		}
		case RRC_TASK:
		{
			type = RRC_QUEUE;
			break;
		}
		case INTERFACE_TASK:
		{
			type = INTERFACE_QUEUE;
			break;
		}
		case MAX_TASK:
		default:
		{
			LOG_ERROR(MAC, "get msgq type wrong, taskId:%u", taskId);
			break;
		}
	}

	return type;
}
#endif
msgId get_msgId(const msgDef* msg)
{
	return msg->header.msgId;
}

void message_int(const task_id taskId, const msg_mode mode)
{
	//msgq_type type = get_msgq_type(taskId);

	msgq_init(taskId, mode);
}

bool new_message(msgDef* msg, const int32_t msgId, const task_id source, const task_id dest, msgSize msg_size)
{
	msg->data = (uint8_t*)msg_malloc(msg_size);

	if (msg->data != NULL)
	{
		msg->header.msgId = msgId;
		msg->header.source = source;
		msg->header.destination = dest;
		msg->header.msgSize = msg_size;

		return true;
	}
	else
	{
		LOG_ERROR(MAC, "new mac message fail! msgId:%u", msgId);
		return false;
	}

	return false;
}


bool message_send(const task_id taskId, char *msg_ptr, int msg_len)
{
	//msgq_type type = get_msgq_type(taskId);

	if (msgSend(taskId, msg_ptr, msg_len))
	{
		return true;
	}

	return false;
}

uint32_t message_receive(const task_id taskId, char *msg, int msg_len)
{
	//uint32_t msg_len = 0;
	//msgq_type type = get_msgq_type(taskId);

	msg_len = msgRecv(taskId, (char *)&msg, MQ_MSGSIZE);

	return msg_len;
}

void message_free(void *ptr)
{
	msg_free(ptr);
}

