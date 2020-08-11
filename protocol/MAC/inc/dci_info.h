/**********************************************************
* @file dci_info.h
* @brief  define dci
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_INTERFACE_DIC_INFO_H
#define _SMARTLOGICTECH_INTERFACE_DIC_INFO_H
	
#include "typedef.h"
#include "d2d_system_global.h"

typedef struct
{
	uint32_t sfn:9;
	//uint32_t rb_num:5;
	uint32_t mcs:5;
	uint32_t data_ind:2;
	uint32_t ndi:1;
	uint32_t ack_num:2;
	uint32_t ack_bits:3;
	uint32_t dataFlag:2;//0:no data, 1:data, 2:PSS+DCI
	uint32_t padding:8;
}dci_t;

#endif //_SMARTLOGICTECH_INTERFACE_DIC_INFO_H

