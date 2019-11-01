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
#include "mytask.h"

typedef uint32_t msgId;
typedef uint32_t msgSize;

typedef struct
{
	msgId msgId;
	task_id_t source;
	task_id_t destination;
	msgSize msgSize;
}msgHeader;

typedef struct
{
	msgHeader header;
	uint8_t *data;
}MessageDef;

typedef MessageDef msgDef;

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_TEST_MESSAGEDEFINE_H_ */
