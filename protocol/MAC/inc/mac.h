/**********************************************************
* @file mac.h
* @brief  define structure for MAC
*
* @author	guicheng.liu
* @date 	2019/07/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_MAC_H
#define _SMARTLOGICTECH_MAC_H

#include "messageDefine.h"

void init_mac();

void mac_reset();
void run_period(msgDef* msg);
void run_scheduler();
int32_t pre_check(const sub_frame_t subframe);


#endif //_SMARTLOGICTECH_MAC_H
