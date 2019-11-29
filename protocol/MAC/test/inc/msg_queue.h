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
#include "mytask.h"


#define QUE_DEP 10
#define MQ_MSGSIZE 2048

typedef enum
{
	EMSG_NULL,
	EMSG_TIMER,
	EMSG_RRC,
	EMSG_RLC,
	EMSG_PHY
}MMsg_type;

typedef enum
{
	EBLOCK,
	ENONBLOCK
}msg_mode;

typedef struct
{
	msgId msgId;
	task_id source;
	task_id destination;
	msgSize msgSize;
}msgHeader;

typedef struct
{
	msgHeader header;
	uint8_t *data;
}MsgqDef;

#define MQ_MSG_HEADER_SIZE sizeof(msgHeader)
#define MQ_MSG_CONTENT_PTR(x) (char *)(x + MQ_MSG_HEADER_SIZE)

mqd_t msgq_init(task_id_sim type, msg_mode mode);
void msgq_free_msg(task_id_sim taskId);
void msgq_close();
bool msgSend(task_id_sim type, char *msg_ptr, int msg_len);
uint32_t msgRecv(task_id_sim type, char *msg_ptr, int msg_len);
msgHeader *new_msg(const int32_t msgId, const task_id source, const task_id dest, msgSize msg_size);
int msg_free(void *ptr);
void *msg_malloc(uint32_t size);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MSG_QUEUE_H_ */
