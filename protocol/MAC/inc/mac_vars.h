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

#include "smac_context.h"
#include "smac_schedule_result.h"

extern context_s g_context;
extern schedule_result_s g_sch;

uint32_t get_rb_num(uint16_t bandwith);
uint32_t get_rb_start(const uint16_t bandwith);
uint32_t get_tbs(const uint8_t mcs, const uint8_t num_rb);
uint32_t get_rbg_size(const uint16_t bandwith);
uint32_t get_first_rb(uint16_t bandwith);
uint8_t get_harqId(const sub_frame_t subframe);
uint16_t cqi_to_mcs(const uint16_t cqi);
bool pre_check(const sub_frame_t subframe);


#endif  //_SMARTLOGICTECH_MAC_VARS_H
