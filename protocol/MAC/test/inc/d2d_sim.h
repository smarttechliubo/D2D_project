/**********************************************************
* @file tasks_def.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/10/31
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_UE_SIM_H_
#define _SMARTLOGICTECH_PROTOCOL_UE_SIM_H_

#include "typedef.h"
#include "rrc_sim.h"
#include "messageDefine.h"
//#include "smac_ue_info.h"

#define TASK_D2D_UE TASK_D2D_DUMMY

void d2dDstMsgHandler(msgDef* msg);
void d2dDstMain();

void d2dSrcMain(msgDef* msg);

typedef struct
{
	uint16_t ueId;
	uint16_t rnti;
	uint16_t mode; //0:source, 1:destination
	uint16_t setup_timer; // setup waiting timer
	rrc_ue_status_e status;
	ccch_info ccch;

	//schedule_info sch_info;
	//harq_info     harq[MAX_HARQ_NUM];
}ue_sim_info;

typedef struct
{
	frame_t frame;
	sub_frame_t subframe;

	bool    timeSync;
	uint8_t mode;
	uint8_t num_ue;
	ue_sim_info ue[D2D_MAX_USER_NUM];
}d2d_sim_info;

#endif /* _SMARTLOGICTECH_PROTOCOL_UE_SIM_H_ */
