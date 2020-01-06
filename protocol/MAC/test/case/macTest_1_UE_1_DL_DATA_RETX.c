/**********************************************************
* @file macTest_Single_UE_DL_DATA_RETX.c
* 
* @brief  case: ue send dl data
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
#include "rrc_sim.h"
#include "mac_testconfig.h"

mac_testConfig g_TEST_CONFIG =
{
	//mac init
	{
		0,                 //uint16_t cellId; //value: 0..503, default: 0
		1,                 //uint16_t bandwith; // value: 0..4,  0: 1.5M, 1: 3M, 2: 6M, 3: 12M, 4:20M, default: 1
		0,                 //uint16_t subframe_config;// value: 0..1, 0: DDUUDDUU, 1: not define
		0,                 //uint16_t mode; //value: 0..1, 0：Source，1：Destination
		{
			//pdcch config
			2,                 //uint16_t rb_num;// value: 1..2, default: 2
			2                 //uint16_t rb_start_index; // value: 2..3, default: 2
		}
	},

	//MIB&SIB
	{
		1,                 //bool flag;// value: 0..2, 0: invalid, 1: mib, 2: sib
		{
			//mib
			0,                 //frame_t systemFrameNumber;// value: 0
			{
				2,                 //uint16_t rb_num_;// value: 1..2, default: 2
				2                  //uint16_t rb_start_index_; // value: 2..3, default: 2
			}
		},
		{
			//sib
			0,				   //uint16_t size;
			0				   //uint8_t *sib_pdu; //tdd-Config
		}
	}
};

mac_testUeConfig g_TEST_UE_CONFIG =
{
	1,				   //uint32_t ue_num;

	//struct ue_config
	{
		{//ue1
			0,			   //uint16_t mode;//value: 0..1, 0：Source，1：Destination
			INVALID_U16,   //rnti_t rnti;// 0: source, 1:destination
			0,   //uint16_t ue_index;// value: 0..16
			4,			   //uint16_t maxHARQ_Tx;// value: 0..8, default: 4
			4,			   //uint16_t max_out_sync;// value: 0..8, default: 4

			1,			   //uint16_t logical_channel_num;

			//logical_channel_config_s logical_channel_config[MAX_LOGICCHAN_NUM];
			{
				{
					1,		//channel_type_e chan_type; 0:CCCH, 1: DTCH
					15, 	   //uint16_t priority;// value:0..16
					1		   //uint16_t logical_channel_id;// value:0..16
				},
				{
					1,		//channel_type_e chan_type; 0:CCCH, 1: DTCH
					14, 	   //uint16_t priority;// value:0..16
					2		   //uint16_t logical_channel_id;// value:0..16
				},
				{
					1,		//channel_type_e chan_type; 0:CCCH, 1: DTCH
					13, 	   //uint16_t priority;// value:0..16
					3		   //uint16_t logical_channel_id;// value:0..16
				},
				{
					1,		//channel_type_e chan_type; 0:CCCH, 1: DTCH
					12, 	   //uint16_t priority;// value:0..16
					4		   //uint16_t logical_channel_id;// value:0..16
				}
			}
		},
		{//ue2

		}
	}
};

mac_testPolicy g_TEST_POLICY = 
{
	true,
	false,
	true,
	false
};

/*OSP_TASKMSG_REG obj name must be "TaskRegTbl" */
OSP_TASKMSG_REG TaskRegTbl[]=
{
	{TASK_D2D_RRC,			"task_rrc",			RT_MSG_PRI(60), (OSP_FUNCPTR)init_rrc_sim,	    (OSP_FUNCPTR)rrc_sim_thread,1},
	{TASK_D2D_RLC,			"task_rlc",			RT_MSG_PRI(60), (OSP_FUNCPTR)init_rlc_sim,	    (OSP_FUNCPTR)rlc_sim_thread,1},
	{TASK_D2D_PHY_TX,		"task_phy_tx",		RT_MSG_PRI(71), (OSP_FUNCPTR)init_phy_tx_sim,	(OSP_FUNCPTR)phy_tx_sim_thread,1},
	{TASK_D2D_PHY_RX,		"task_phy_rx",		RT_MSG_PRI(70), (OSP_FUNCPTR)init_phy_rx_sim,	(OSP_FUNCPTR)phy_rx_sim_thread,1},
	{TASK_D2D_MAC,		    "task_mac",		    RT_MSG_PRI(69), (OSP_FUNCPTR)init_mac_period,	    (OSP_FUNCPTR)run_period,1},
	{TASK_D2D_MAC_SCH,		"task_mac_schedule",RT_MSG_PRI(69), (OSP_FUNCPTR)init_mac_scheduler,	(OSP_FUNCPTR)run_scheduler,1},
};
/*OSP_TASKMSG_REG num name must be "TegTaskNum" */
U32 TegTaskNum =sizeof(TaskRegTbl)/sizeof(TaskRegTbl[0]);

