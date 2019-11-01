/**********************************************************
* @file msg_handler.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/10/30
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MSG_HADNLER_H_
#define _SMARTLOGICTECH_PROTOCOL_MSG_HADNLER_H_

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

msgId get_msgId(const msgDef* msg);
bool new_message(msgDef* msg, int32_t msgId, const task_id_t source, const task_id_t dest, msgSize msg_size);
bool message_send(const task_id_t dest, char *msg_ptr, int msg_len);
uint32_t message_receive(const task_id_t dest, char *msg, int msg_len);
void message_free(void *ptr);

#endif //_SMARTLOGICTECH_PROTOCOL_MSG_HADNLER_H_
