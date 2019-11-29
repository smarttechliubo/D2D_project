/**********************************************************
* @file macTest.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/19
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "typedef.h"

//#include "threads.h"

#include "mac_defs.h"
#include "mac.h"
#include "log.h"
#include "mytask.h"
#include "msg_handler.h"
#include "mac_osp_interface.h"

extern uint32_t init_rrc_sim();
extern uint32_t init_rlc_sim();
extern uint32_t init_phy_tx_sim();
extern uint32_t init_phy_rx_sim();
extern void rrc_sim_thread(msgDef* msg);
extern void rlc_sim_thread(msgDef* msg);
extern void phy_tx_sim_thread(msgDef* msg);
extern void phy_rx_sim_thread(msgDef *msg);

uint32_t init_mac_sim()
{
	void* pTimer;
	int32_t ret;
	
	pTimer = _timerCreate(TASK_D2D_MAC, 1, 4,0);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_mac_sim, pTimer is %p,ret:%d\r\n", pTimer, ret);

	init_mac();

	return 0;
}

void sim_run_period(msgDef *msg)
{
	run_period(msg);
}

uint32_t init_mac_sch_sim()
{
	void* pTimer;
	int32_t ret;
	
	pTimer = _timerCreate(TASK_D2D_MAC_SCH, 1, 4,1);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_mac_sch_sim, pTimer is %p，ret:%d\r\n", pTimer, ret);

	return 0;
}

void sim_run_scheduler(msgDef *msg)
{
	run_scheduler(msg);
}

/*OSP_TASKMSG_REG obj name must be "TaskRegTbl" */
OSP_TASKMSG_REG TaskRegTbl[]=
{
	{TASK_D2D_RRC,			"task_rrc",			RT_MSG_PRI(60), (OSP_FUNCPTR)init_rrc_sim,	    (OSP_FUNCPTR)rrc_sim_thread,0},
	{TASK_D2D_RLC,			"task_rlc",			RT_MSG_PRI(60), (OSP_FUNCPTR)init_rlc_sim,	    (OSP_FUNCPTR)rlc_sim_thread,0},
	{TASK_D2D_PHY_TX,		"task_phy_tx",		RT_MSG_PRI(75), (OSP_FUNCPTR)init_phy_tx_sim,	(OSP_FUNCPTR)phy_tx_sim_thread,1},
	{TASK_D2D_PHY_RX,		"task_phy_rx",		RT_MSG_PRI(75), (OSP_FUNCPTR)init_phy_rx_sim,	(OSP_FUNCPTR)phy_rx_sim_thread,1},
	{TASK_D2D_MAC,		    "task_mac",		    RT_MSG_PRI(70), (OSP_FUNCPTR)init_mac_sim,	    (OSP_FUNCPTR)sim_run_period,0},
	{TASK_D2D_MAC_SCH,		"task_mac_schedule",RT_MSG_PRI(70), (OSP_FUNCPTR)init_mac_sch_sim,	(OSP_FUNCPTR)sim_run_scheduler,0},
};
/*OSP_TASKMSG_REG num name must be "TegTaskNum" */
U32 TegTaskNum =sizeof(TaskRegTbl)/sizeof(TaskRegTbl[0]);

