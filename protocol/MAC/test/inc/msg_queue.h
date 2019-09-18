/**********************************************************
* @file msg_queue.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/06
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MSG_QUEUE_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MSG_QUEUE_H_

#include <mqueue.h>
#include "messageDefine.h"


#define QUE_DEP 10
#define MQ_MSGSIZE 128

typedef enum
{
	PHY_QUEUE,
	MAC_QUEUE,
	RRC_MAC_QUEUE,
	RLC_MAC_QUEUE,
	PHY_MAC_QUEUE,
	RLC_QUEUE,
	RRC_QUEUE,
	MAX_QUEUE
}msgq_type;

mqd_t msgq_init(msgq_type type);
bool msgSend(msgq_type type, const char *msg_ptr, int msg_len);
uint32_t msgRecv(msgq_type type, char *msg_ptr, int msg_len);
msgDef *new_msg(msgSize msg_size);
int msg_free(void *ptr);
void *msg_malloc(uint32_t size);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MSG_QUEUE_H_ */
