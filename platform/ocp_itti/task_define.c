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
	{TASK_D2D_RRC,			"task_rrc",			RT_MSG_PRI(60), 	(OSP_FUNCPTR)rrc_module_Initial,	(OSP_FUNCPTR)rrc_Sche_Task,0},
	{TASK_D2D_RLC,			"task_rlc",			RT_MSG_PRI(70), 	(OSP_FUNCPTR)rlc_module_initial,	(OSP_FUNCPTR)rlc_rrc_config_task,0},
	{TASK_D2D_RLC_TX,		"task_rlc_tx",		RT_MSG_PRI(75), 	NULL,	(OSP_FUNCPTR)rlc_tx_task,0},
	{TASK_D2D_RLC_RX,		"task_rlc_rx",		RT_MSG_PRI(72), 	NULL,	(OSP_FUNCPTR)rlc_rx_task,0},
#if 0	
	{TestTask2, 			"osptask2",			RT_MSG_PRI(80), 	NULL,			            (OSP_FUNCPTR)OspTeskMain2,1},   
	{TestTask3,			    "osptask3",			RT_NOMSG_PRI(80),	NULL,			            (OSP_FUNCPTR)OspTeskMain3,0},   
	{TestTask4,				"osptask4",			RT_MSG_PRI(90), 	NULL,	                    (OSP_FUNCPTR)OspTeskMain4,1},
	{TestTask5, 			"osptask5",			RT_MSG_PRI(80), 	NULL,			            (OSP_FUNCPTR)OspTeskMain5,0},   
	{TestTask6,			    "osptask6",			RT_MSG_PRI(80),	    NULL,			            (OSP_FUNCPTR)OspTeskMain6,1}, 
#endif 	
};
/*OSP_TASKMSG_REG num name must be "TegTaskNum" */
U32 TegTaskNum =sizeof(TaskRegTbl)/sizeof(TaskRegTbl[0]);
 
 
 
/**************************function******************************/
