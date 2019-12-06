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
#include "mac_osp_interface.h"

static const char* s_msgq_file[] = {
	[ETASK_A] = "/taskA",
	[ETASK_B] = "/taskB"
};

typedef struct
{
	bool open;
	task_id_sim taskId;
	mqd_t msgq_id;
}msgq_info;

static msgq_info q_info[ETASK_MAX];

static uint16_t g_msgq_mode = 0;//0:source, 1:destination

void msgInit(const uint16_t mode)
{
	g_msgq_mode = mode;
}

void msgq_unlink(const task_id_sim id)
{
	if (id  >= ETASK_MAX)
		return;

	const char *file = s_msgq_file[id];

	mq_unlink(file);//remove mq if it exist
}

mqd_t msgq_init(const task_id_sim id, const msg_mode mode)
{
	if (id  >= ETASK_MAX)
		return -1;

	const char *file = s_msgq_file[id];
    struct mq_attr msgq_attr;
 
	mqd_t msgq_id;
	
    msgq_attr.mq_maxmsg = QUE_DEP;
    msgq_attr.mq_msgsize = MQ_MSGSIZE;

	//mq_unlink(file);//remove mq if it exist

	if (mode == EBLOCK)
		msgq_id = mq_open(file, O_RDWR | O_CREAT , S_IRWXU | S_IRWXG, &msgq_attr);
	else
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

	q_info[id].open = true;
	q_info[id].msgq_id = msgq_id;
	q_info[id].taskId = id;

	//msgq_free_msg(msgq_id);

    return msgq_id;
}

void msgq_close()
{
	for (uint32_t i = 0; i < ETASK_MAX; i++)
	{
		if (q_info[i].open == true)
		{
			mq_close(q_info[i].msgq_id);

			LOG_INFO(MAC, "msgq_close, msgq_id:%u",q_info[i].msgq_id);
		}
	}
}

bool msgSend(task_id_sim id, char *msg_ptr, int msg_len)
{
	int ret;

	if (id >= ETASK_MAX)
	{
		LOG_ERROR(MAC, "Wrong msgType:%u,msg_len:%u",id,msg_len);
		msg_free(msg_ptr);
		return false;
	}

	mqd_t msgq_id = q_info[id].msgq_id;

	ret = mq_send(msgq_id, msg_ptr, msg_len, 1);

	if (ret == -1)
	{
		LOG_ERROR(MAC, "msgSend fail taskId:%u,msg_len:%u,errno:%d",id, msg_len, errno);

		perror("mq_send");
	}

	msg_free(msg_ptr);

	return true;
}

uint32_t msgRecv(task_id_sim id, char *msg_ptr, int msg_len)
{
	uint32_t ret;

	if (id >= ETASK_MAX)
	{
		LOG_ERROR(MAC, "Wrong msgType:%u,msg_len:%u",id,msg_len);
		return false;
	}

	mqd_t msgq_id = q_info[id].msgq_id;

	if (msg_ptr == NULL)
		return 0;

	ret = mq_receive(msgq_id, msg_ptr, msg_len, NULL);
	
	if (ret == -1)
	{
		if (errno != EAGAIN)// TODO: how to handle O_NONBLOCK
		{
			perror("mq_receive");
			LOG_ERROR(MAC, "msgRecv fail taskId:%u,msg_len:%u,errno:%d",id,msg_len, errno);
		}
		return 0;
	}

	return ret;
}

void *msg_malloc(uint32_t size)
{
	void *ptr = NULL;
	AssertFatal ((ptr=mem_alloc (size)) != NULL, MAC, "Memory allocation failed");
	return ptr;
}

int msg_free(void *ptr)
{
	AssertFatal (ptr != NULL, MAC, "Trying to free a NULL pointer");
	mem_free (ptr);
	return (EXIT_SUCCESS);
}

msgHeader *new_msg(const int32_t msgId, const task_id_sim source, const task_id_sim dest, msgSize msg_size)
{
	int size = sizeof(msgHeader) + msg_size;

	msgHeader *msg = (msgHeader *)msg_malloc(size);

	if (msg != NULL)
	{
		msg->msgId = msgId;
		msg->source = source;
		msg->destination = dest;
		msg->msgSize = msg_size;
	}
	else
	{
		LOG_ERROR(MAC, "new mac message fail! msgId:%u", msgId);
	}

	return msg;
}

