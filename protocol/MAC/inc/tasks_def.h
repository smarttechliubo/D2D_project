/**********************************************************
* @file tasks_def.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/10/31
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_TASKS_DEF_H_
#define _SMARTLOGICTECH_PROTOCOL_TASKS_DEF_H_

typedef enum
{
	PHY_TASK,
	PHY_TX_TASK,
	PHY_RX_TASK,
	MAC_TASK,
	MAC_PRE_TASK,
	MAC_MAIN_TASK,
	RLC_TASK,
	RRC_TASK,
	INTERFACE_TASK_A,//source receive
	INTERFACE_TASK_B,//dest receive
	MAX_TASK
}task_id;

#endif /* _SMARTLOGICTECH_PROTOCOL_TASKS_DEF_H_ */
