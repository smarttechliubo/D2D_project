/**********************************************************
* @file phy_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "messageDefine.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"
#include "interface_mac_phy.h"
#include "mytask.h"
#include "phy_sim.h"
#include "emac_enum_def.h"
#include "mac_defs.h"

phy_tx_info g_phyTx;
bool g_timing_sync_phyTx = false;

extern uint32_t get_syncTiming();
uint16_t g_phy_tx_mode = 0;//0:source, 1:destination


void syncTimePhyTx()//TODO: sync
{
	uint32_t time = 0;

	// 1. get timing sync
	if (g_timing_sync_phyTx == false)
	{
		time = get_syncTiming();// TODO: sync

		if (time != 0xFFFFFFFF)
		{
			g_phyTx.frame = time / 10;
			g_phyTx.subframe = time % MAX_SUBSFN;
			g_timing_sync_phyTx = true;
		}
		else
		{
			LOG_WARN(PHY, "Timing sync fail!!");
			g_timing_sync_phyTx = false;
		}
	}
	else if (g_timing_sync_phyTx == true)
	{
		g_phyTx.subframe++;

		if (g_phyTx.subframe == MAX_SUBSFN)
		{
			g_phyTx.subframe = 0;
			g_phyTx.frame = (g_phyTx.frame+1)%MAX_SFN;
		}
		
		if ((g_phyTx.frame*10 + g_phyTx.subframe)%TIMING_SYNC_PERIOD == 0)
		{
			time = get_syncTiming();

			if (time != 0xFFFFFFFF)
			{
				if (time != g_phyTx.frame*10 + g_phyTx.subframe)
				{
					LOG_WARN(PHY, "Timing sync loast!! time:%u, frame:%u, subframe:%u",
						time, g_phyTx.frame, g_phyTx.subframe);
					g_phyTx.frame = time / 10;
					g_phyTx.subframe = time % MAX_SUBSFN;
				}
			}
			else
			{
				LOG_WARN(PHY, "Timing sync failed with PHY");
			}
		}
	}
}

uint32_t init_phy_tx_sim()
{	
	g_phy_tx_mode = 0;

	void* pTimer;
	int32_t ret;

	pTimer = OSP_timerCreateSim(TASK_D2D_PHY_TX, 1, 4,0);
	ret = OSP_timerStart(pTimer);

	LOG_INFO(MAC,"init_phy_tx_sim pTimer is %p, ret:%u\r\n", pTimer, ret);

	return 0;
}

void phyTxMsgHandler(msgDef* msg)
{
	//msgDef* msg = NULL;
	//uint32_t msg_len = 0;
	msgId msg_id = 0;

	//while (1)
	{ 
		//msg_len = message_receive(TASK_D2D_PHY_TX, msg);

		//if (msg_len == 0)
		//{
		//	continue;
		//}

		msg_id = get_msgId(msg);

		switch (msg_id)
		{
			case MAC_PHY_PBCH_TX_REQ:
			{
				PHY_PBCHSendReq* req = (PHY_PBCHSendReq*)message_ptr(msg);

				LOG_INFO(PHY, "MAC_PHY_PBCH_TX_REQ received. frame:%u, subframe:%u", req->frame, req->subframe);

				g_phyTx.flag_pbch = true;
				memcpy(&g_phyTx.pbch, req, sizeof(PHY_PBCHSendReq));

				break;
			}
			case MAC_PHY_PDCCH_SEND:
			{				
				PHY_PdcchSendReq* req = (PHY_PdcchSendReq*)message_ptr(msg);

				g_phyTx.flag_pdcch = true;
				memcpy(&g_phyTx.pdcch, req, sizeof(PHY_PdcchSendReq));
		
				break;
			}
			case MAC_PHY_PUSCH_SEND:
			{				
				PHY_PuschSendReq* req = (PHY_PuschSendReq*)message_ptr(msg);

				g_phyTx.flag_pusch = true;
				memcpy(&g_phyTx.pdcch, req, sizeof(PHY_PuschSendReq));
		
				break;
			}
			default:
			{
				LOG_ERROR(PHY, "unknown msgId:%u", msg_id);
				break;
			}
		}

		message_free(msg);
	}
}

void handle_phy_tx(const      frame_t frame, const sub_frame_t subframe)
{
	mode_e mode = g_phy_tx_mode;
	task_id taskId = (mode == EMAC_DEST) ? INTERFACE_TASK_A : INTERFACE_TASK_B;

	msgDef* msg = NULL;
	msgSize msg_size = 0;

	if (g_phyTx.flag_pbch && 
		(g_phyTx.pbch.frame == frame && g_phyTx.pbch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PBCHSendReq);
		
		msg = new_message(MAC_PHY_PBCH_TX_REQ, TASK_D2D_PHY_TX, taskId, msg_size);

		if (msg != NULL)
		{
			memcpy(message_ptr(msg), &g_phyTx.pbch, msg_size);

			message_free(msg);
			//if (message_send(taskId, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PBCH_TX_REQ send");
			}
		}

		g_phyTx.flag_pbch = false;
	}

	if (g_phyTx.flag_pdcch && 
		(g_phyTx.pdcch.frame == frame && g_phyTx.pdcch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PdcchSendReq);
		
		msg = new_message(MAC_PHY_PDCCH_SEND, TASK_D2D_PHY_TX, taskId, msg_size);

		if (msg != NULL)
		{
			memcpy(message_ptr(msg), &g_phyTx.pdcch, msg_size);

			message_free(msg);
			//if (message_send(taskId, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PDCCH_SEND send");
			}
		}

		g_phyTx.flag_pdcch = false;
	}

	if (g_phyTx.flag_pusch && 
		(g_phyTx.pusch.frame == frame && g_phyTx.pusch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PuschSendReq);
		
		msg = new_message(MAC_PHY_PUSCH_SEND, TASK_D2D_PHY_TX, taskId, msg_size);

		if (msg != NULL)
		{
			memcpy(message_ptr(msg), &g_phyTx.pusch, msg_size);

			message_free(msg);
			//if (message_send(taskId, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PUSCH_SEND send");
			}
		}

		g_phyTx.flag_pusch = false;
	}
}

void phy_tx_sim_thread(msgDef* msg)
{
	frame_t frame;
	sub_frame_t subframe;

	syncTimePhyTx();

	if (g_timing_sync_phyTx != false)
	{
		frame = g_phyTx.frame;
		subframe = g_phyTx.subframe;
		
		//frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
		//subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;
		if (!is_timer(msg))
		{
			phyTxMsgHandler(msg);
		}
		else
		{
			handle_phy_tx(frame, subframe);
		}
	}
}

/*
void *phy_tx_thread()
{
	frame_t frame;
	sub_frame_t subframe;

	while(thread_wait(TASK_D2D_MAC, PERIODIC_4MS))
	{
		syncTimePhyTx();

		if (g_timing_sync_phyTx == false)
			continue;
		
		frame = g_phyTx.frame;
		subframe = g_phyTx.subframe;
		
		//frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
		//subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;

		phyTxMsgHandler();
		handle_phy_tx(frame, subframe);
	}

	return 0;
}
*/
