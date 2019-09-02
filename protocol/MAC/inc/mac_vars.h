/**********************************************************
* @file mac_var.h
* @brief  define variables for mac 
*
* @author	guicheng.liu
* @date 	2019/07/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_MAC_VARS_H
#define _SMARTLOGICTECH_MAC_VARS_H

#include "mac.h"

context_s g_context;
schedule_result_s sch;

uint32_t get_rb_num(uint16_t bandwith);
uint32_t get_rb_start(const uint16_t bandwith);
uint32_t get_tbs(const uint8_t mcs, const uint8_t num_rb);
uint32_t get_rbg_size(const uint16_t bandwith);


#endif  //_SMARTLOGICTECH_MAC_VARS_H
