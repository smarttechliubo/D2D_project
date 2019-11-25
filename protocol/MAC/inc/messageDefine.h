/**********************************************************
* @file messageDefine.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/07
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MESSAGEDEFINE_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MESSAGEDEFINE_H_

#include "typedef.h"
#include "tasks_def.h"
#include "osp_ex.h"

typedef uint32_t msgId;
typedef uint32_t msgSize;

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
}MessageDefsssss;

typedef Osp_Msg_Head msgDef;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MESSAGEDEFINE_H_ */
