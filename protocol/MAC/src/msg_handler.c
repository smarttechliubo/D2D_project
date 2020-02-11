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
//#include "msg_queue.h"

msgId get_msgId(const msgDef* msg)
{
	return msg->MsgType;
}

task_id get_SrcId(const msgDef* msg)
{
	return msg->SrcId;
}

task_id get_DstId(const msgDef* msg)
{
	return msg->DstId;
}

bool is_timer(msgDef* msg)
{
	
	if (IS_TIMER_MSG(msg))
	{
		return true;
	}

	return false;
}

void message_close()
{
	//msgq_close();
}
/*
MMsg_type message_type(msgDef* msg)
{
	
	if (IS_TIMER_MSG(msg))
	{
		return EMSG_TIMER;
	}

	if (msg->SrcId == TASK_D2D_RRC)
	{
		return EMSG_RRC;
	}

	if (msg->SrcId == TASK_D2D_RLC)
	{
		return EMSG_RLC;
	}

	if (msg->SrcId == TASK_D2D_PHY_RX)
	{
		return EMSG_PHY;
	}

	return EMSG_NULL;
}
*/
msgDef* new_message(const int32_t msgId, const task_id source, const task_id dest, msgSize msg_size)
{
	msgDef* msg = OSP_Alloc_Msg(msg_size);

	if (msg == NULL)
	{
		LOG_ERROR(MAC, "message new_message err, taskId:%u, msgId:%u", 
			source, msgId);

		return NULL;
	}
	else
	{
    	msg->SrcId = CURRENT_TASKID;
    	msg->DstId = dest;
    	msg->MsgSize = msg_size;
    	msg->MsgType = msgId;
	}

	return msg;
}


bool message_send(const task_id taskId, msgDef *msg, int msg_len)
{
	//msgq_type type = get_msgq_type(taskId);

	//if (msgSend(taskId, (char*)msg_ptr, msg_len))
	if (OSP_Send_Msg(msg) == OSP_OK)
	{
		return true;
	}
	else
	{
		LOG_ERROR(MAC, "message send err, taskId:%u, msgId:%u", taskId, msg->MsgType);
		return false;
	}

	return false;
}

msgDef * message_receive_timeout(const task_id taskId, uint32_t timeout)
{
	msgDef *msg = NULL;

	msg = OSP_Rev_MsgTimeout(timeout);

	if (msg == NULL)
	{
		LOG_INFO(MAC, "message_receive_try, timer out no msg received taskId:%u", taskId);
	}
	else
	{	
		if(is_timer(msg))
		{
			//LOG_ERROR(MAC, "IS_TIMER_MSG, taskId:%u", taskId);
		}
	}

	return msg;
}

msgDef* message_receive(const task_id taskId)
{
	msgDef *msg = NULL;
	//uint32_t msg_len = 0;
	//msg_len = msgRecv(taskId, (char *)msg, MQ_MSGSIZE);

	msg = OSP_Rev_MsgTry();

	if (msg == NULL)
	{
		//LOG_ERROR(MAC, "message receive err, taskId:%u", taskId);
	}
	else
	{	
		if(is_timer(msg))
		{
			LOG_ERROR(MAC, "IS_TIMER_MSG, taskId:%u", taskId);

			msg = NULL;
		}
		else
		{
		}
	}

	return msg;
}

void* message_ptr(msgDef* msg)
{
	return MSG_HEAD_TO_COMM(msg);
}

void message_free(void *ptr)
{
	//msg_free(ptr);
	OSP_Free_Msg(ptr);
}

