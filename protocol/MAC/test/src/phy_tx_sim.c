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

extern mode_e get_mac_mode();
extern uint32_t syncT();


void syncTimePhyTx()//TODO: sync
{
    // 1. get timing sync
	//if (g_timing_sync_phyTx == false)
    {
		uint32_t time = syncT();// TODO: sync
		if (time != 0xFFFFFFFF)
		{
			g_phyTx.frame = time >> 16;
			g_phyTx.subframe = time&0xFFFF;
			g_timing_sync_phyTx = true;
		}
		else
	    {
			LOG_WARN(MAC, "Timing sync fail!!");
			g_timing_sync_phyTx = false;
		}
	}
}

void init_phy_sim()
{	
	message_int(PHY_TASK, ENONBLOCK);
	message_int(PHY_TX_TASK, EBLOCK);
	message_int(PHY_RX_TASK, EBLOCK);
	message_int(INTERFACE_TASK_A, EBLOCK);
	message_int(INTERFACE_TASK_B, EBLOCK);
}
/*
void forward_msg(msgDef* msg, const msgId msg_id, const task_id taskId, const msgSize msg_size)
{
	if (new_message(msg, msg_id, PHY_TX_TASK, taskId, msg_size))
	{
		if (message_send(taskId, (char *)msg, sizeof(msgDef)))
		{

		}
	}
}
*/
void phyTxMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msg_id = 0;

	while (1)
	{ 
		msg_len = message_receive(PHY_TX_TASK, (char *)&msg, msg_len);

		if (msg_len == 0)
		{
			continue;
		}

		msg_id = get_msgId(&msg);

		switch (msg_id)
		{
			case MAC_PHY_PBCH_TX_REQ:
			{
				PHY_PBCHSendReq* req = (PHY_PBCHSendReq*)msg.data;

				g_phyTx.flag_pbch = true;
				memcpy(&g_phyTx.pbch, req, sizeof(PHY_PBCHSendReq));
				message_free(req);

				break;
			}
			case MAC_PHY_PDCCH_SEND:
			{				
				PHY_PdcchSendReq* req = (PHY_PdcchSendReq*)msg.data;

				g_phyTx.flag_pdcch = true;
				memcpy(&g_phyTx.pdcch, req, sizeof(PHY_PdcchSendReq));
				message_free(req);
		
				break;
			}
			case MAC_PHY_PUSCH_SEND:
			{				
				PHY_PuschSendReq* req = (PHY_PuschSendReq*)msg.data;

				g_phyTx.flag_pusch = true;
				memcpy(&g_phyTx.pdcch, req, sizeof(PHY_PuschSendReq));
				message_free(req);
		
				break;
			}
			default:
			{
				LOG_ERROR(PHY, "unknown msgId:%u", msg_id);
				break;
			}
		}
	}
}

void handle_phy_tx(const      frame_t frame, const sub_frame_t subframe)
{
	mode_e mode = get_mac_mode();
	task_id taskId = (mode == EMAC_DEST) ? INTERFACE_TASK_A : INTERFACE_TASK_B;

	msgDef msg;
	msgSize msg_size = 0;

	if (g_phyTx.flag_pbch && 
		(g_phyTx.pbch.frame == frame && g_phyTx.pbch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PBCHSendReq);
		
		if (new_message(&msg, MAC_PHY_PBCH_TX_REQ, PHY_TX_TASK, taskId, msg_size))
		{
			memcpy(msg.data, &g_phyTx.pbch, msg_size);

			if (message_send(taskId, (char *)&msg, sizeof(msgDef)))
			{

			}
		}

		g_phyTx.flag_pbch = false;
	}

	if (g_phyTx.flag_pdcch && 
		(g_phyTx.pdcch.frame == frame && g_phyTx.pdcch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PdcchSendReq);
		
		if (new_message(&msg, MAC_PHY_PBCH_TX_REQ, PHY_TX_TASK, taskId, msg_size))
		{
			memcpy(msg.data, &g_phyTx.pdcch, msg_size);

			if (message_send(taskId, (char *)&msg, sizeof(msgDef)))
			{

			}
		}

		g_phyTx.flag_pdcch = false;
	}

	if (g_phyTx.flag_pusch && 
		(g_phyTx.pusch.frame == frame && g_phyTx.pusch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PuschSendReq);
		
		if (new_message(&msg, MAC_PHY_PBCH_TX_REQ, PHY_TX_TASK, taskId, msg_size))
		{
			memcpy(msg.data, &g_phyTx.pusch, msg_size);

			if (message_send(taskId, (char *)&msg, sizeof(msgDef)))
			{

			}
		}

		g_phyTx.flag_pusch = false;
	}
}

void *phy_tx_thread()
{
	frame_t frame;
	sub_frame_t subframe;

	while(thread_wait(MAC_TASK, PERIODIC_4MS))
	{
		syncTimePhyTx();

		if (g_timing_sync_phyTx == false)
			continue;
		
		frame = g_phyTx.frame;
		subframe = g_phyTx.subframe;
		
		frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
		subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;

		handle_phy_tx(frame, subframe);
		phyTxMsgHandler();
	}

	return 0;
}

