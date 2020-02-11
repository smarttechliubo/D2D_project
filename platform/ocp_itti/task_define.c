/******************************************************************
 * @file  task_define.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年11月19日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年11月19日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>

#include <task_declare.h>
 
/*OSP_TASKMSG_REG obj name must be "TaskRegTbl" */
OSP_TASKMSG_REG TaskRegTbl[]=
{
	{TASK_D2D_RRC,			"task_rrc",			RT_MSG_PRI(74), 	(OSP_FUNCPTR)rrc_module_Initial,	(OSP_FUNCPTR)rrc_Sche_Task,0},
	{TASK_D2D_RLC,			"task_rlc",			RT_MSG_PRI(70), 	(OSP_FUNCPTR)rlc_module_initial,	(OSP_FUNCPTR)rlc_rrc_config_task,0},
	{TASK_D2D_RLC_TX,		"task_rlc_tx",		RT_MSG_PRI(75), 	NULL,	(OSP_FUNCPTR)rlc_tx_task,0},
	{TASK_D2D_RLC_RX,		"task_rlc_rx",		RT_MSG_PRI(76), 	NULL,	(OSP_FUNCPTR)rlc_rx_task,1},
	{TASK_D2D_DUMMY,        "task_dummy_test",	RT_MSG_PRI(60), 	(OSP_FUNCPTR)dummy_init,	(OSP_FUNCPTR)dummy_test_task,0},
	{TASK_D2D_IP_UDP,           "task_ip_udp",       	RT_NOMSG_PRI(72), 	NULL,	(OSP_FUNCPTR)ip_udp_task,0},
	{TASK_D2D_IP_MSG,           "task_ip_msg",       	RT_MSG_PRI(77), 	(OSP_FUNCPTR)ip_rlc_sdu_udp_initial,	(OSP_FUNCPTR)ip_msg_task,1},

	{TASK_D2D_DUMMY_INT,        "task_timer_int",	RT_MSG_PRI(80), 	(OSP_FUNCPTR)timer_int_init,	(OSP_FUNCPTR)timer_int_task,0},

	{TASK_D2D_PHY_TX,		"task_phy_tx",		RT_MSG_PRI(82), (OSP_FUNCPTR)init_phy_tx_sim,	(OSP_FUNCPTR)phy_tx_sim_thread,1},
	{TASK_D2D_PHY_RX,		"task_phy_rx",		RT_MSG_PRI(81), (OSP_FUNCPTR)init_phy_rx_sim,	(OSP_FUNCPTR)phy_rx_sim_thread,1},
	{TASK_D2D_MAC,			"task_mac", 		RT_MSG_PRI(79), (OSP_FUNCPTR)init_mac_period,		(OSP_FUNCPTR)run_period,1},
	{TASK_D2D_MAC_SCH,		"task_mac_schedule",RT_MSG_PRI(80), (OSP_FUNCPTR)init_mac_scheduler,	(OSP_FUNCPTR)run_scheduler,1}

};
/*OSP_TASKMSG_REG num name must be "TegTaskNum" */
U32 TegTaskNum =sizeof(TaskRegTbl)/sizeof(TaskRegTbl[0]);
 
 
 
/**************************function******************************/
