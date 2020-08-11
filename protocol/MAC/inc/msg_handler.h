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

msgId get_msgId(const msgDef* msg);
task_id get_SrcId(const msgDef* msg);
task_id get_DstId(const msgDef* msg);
bool is_timer(msgDef* msg);
void message_close();
msgDef* new_message(const int32_t msgId, const task_id source, const task_id dest, msgSize msg_size);
bool message_send(const task_id dest, msgDef *msg_ptr, int msg_len);
msgDef * message_receive_timeout(const task_id taskId, const uint32_t timeout);
uint32_t message_receive(const task_id dest, msgDef *msg);
void* message_ptr(msgDef* msg);
void message_free(void *ptr);

#endif //_SMARTLOGICTECH_PROTOCOL_MSG_HADNLER_H_
