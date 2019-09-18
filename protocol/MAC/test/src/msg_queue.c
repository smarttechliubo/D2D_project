/**********************************************************
* @file msg_queue.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/06
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include <limits.h>
#include <sys/types.h> 
#include <mqueue.h>
#include <sys/mman.h>
#include "fcntl.h"

#include <errno.h>
#include "log.h"
#include "msg_queue.h"
//typedef unsigned long long int uint64_t;


static const char* s_msgq_file[] = {
    [PHY_QUEUE] = "/phyQ",
    [MAC_QUEUE] = "/macQ",
	[RRC_MAC_QUEUE] = "/rrc-macQ",
	[RLC_MAC_QUEUE] = "/rlc-macQ",
	[PHY_MAC_QUEUE] = "/phy-macQ",
    [RLC_QUEUE] = "/rlcQ",
    [RRC_QUEUE] = "/rrcQ"
};

typedef struct
{
	msgq_type type;
	mqd_t msgq_id;
}msgq_info;

msgq_info q_info[MAX_QUEUE];

mqd_t msgq_init(msgq_type type)
{
	if (type >= MAX_QUEUE)
		return -1;

	const char *file = s_msgq_file[type];
    struct mq_attr msgq_attr;
 
	mqd_t msgq_id;
	
    msgq_attr.mq_maxmsg = QUE_DEP;
    msgq_attr.mq_msgsize = MQ_MSGSIZE;

    msgq_id = mq_open(file, O_RDWR | O_CREAT | O_NONBLOCK , S_IRWXU | S_IRWXG, &msgq_attr);
    if(msgq_id == (mqd_t)-1)
    {
        perror("mq_open");
		printf("errno:%d\n",errno);
        return -1;
    }
    /* getting the attributes from the queue        --  mq_getattr() */
    if(mq_getattr(msgq_id, &msgq_attr) == -1)
    {
        perror("mq_getattr");
        return -1;
    }

    printf("Queue \"%s\":\n\t- stores at most %ld messages\n\t- "
        "large at most %ld bytes each\n\t- currently holds %ld messages\n",
        file, msgq_attr.mq_maxmsg, msgq_attr.mq_msgsize, msgq_attr.mq_curmsgs);

	q_info[type].msgq_id = msgq_id;
	q_info[type].type = type;

    return msgq_id;
}

bool msgSend(msgq_type type, const char *msg_ptr, int msg_len)
{
	int ret;
	mqd_t msgq_id = q_info[type].msgq_id;

	if (type >= MAX_QUEUE)
	{
		LOG_ERROR(MAC, "Wrong msgType:%u,msg_len:%u",type,msg_len);
		return false;
	}

	ret = mq_send(msgq_id, msg_ptr, msg_len, 1);
	if (ret == -1)
	{
		perror("mq_send");
		LOG_ERROR(MAC, "msgSend fail msgType:%u,msg_len:%u,errno:%d",type,msg_len, errno);
		return false;
	}

	return true;
}

uint32_t msgRecv(msgq_type type, char *msg_ptr, int msg_len)
{
	uint32_t ret;
	//unsigned msg_prio = 1;
	mqd_t msgq_id = q_info[type].msgq_id;

	if (type >= MAX_QUEUE || msg_ptr == NULL)
		return 0;

	ret = mq_receive(msgq_id, msg_ptr, msg_len, NULL);
	
	if (ret == -1)
	{
		if (errno != EAGAIN)// TODO: how to handle O_NONBLOCK
		{
			perror("mq_receive");
			LOG_ERROR(MAC, "msgSend fail msgType:%u,msg_len:%u,errno:%d",type,msg_len, errno);
		}
		return 0;
	}

	return ret;
}

void *msg_malloc(uint32_t size)
{
	void *ptr = NULL;
	AssertFatal ((ptr=malloc (size)) != NULL, MAC, "Memory allocation failed");
	return ptr;
}

int msg_free(void *ptr)
{
	AssertFatal (ptr != NULL, MAC, "Trying to free a NULL pointer");
	free (ptr);
	return (EXIT_SUCCESS);
}

msgDef *new_msg(msgSize msg_size)
{
	int size = sizeof(msgHeader) + msg_size;

	msgDef *msg = (msgDef *)msg_malloc(size);
	return msg;
}


