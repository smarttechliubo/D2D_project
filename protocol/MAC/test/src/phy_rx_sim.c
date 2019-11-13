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

phy_rx_info g_phyRx;
bool g_timing_sync_phyRx = false;

extern mode_e get_mac_mode();
extern uint32_t syncT();

void syncTimePhyRx()//TODO: sync
{
    // 1. get timing sync
	//if (g_timing_sync_phyRx == false)
    {
		uint32_t time = syncT();// TODO: sync
		if (time != 0xFFFFFFFF)
		{
			g_phyRx.frame = time >> 16;
			g_phyRx.subframe = time&0xFFFF;
			g_timing_sync_phyRx = true;
		}
		else
	    {
			LOG_WARN(MAC, "Timing sync fail!!");
			g_timing_sync_phyRx = false;
		}
	}
}

void phyRxMsgHandler()
{
	msgDef msg;
	uint32_t msg_len = 0;
	msgId msg_id = 0;
	mode_e mode = get_mac_mode();
	task_id taskId = (mode == EMAC_SRC) ? INTERFACE_TASK_A : INTERFACE_TASK_B;

	while (1)
	{ 
		msg_len = message_receive(taskId, (char *)&msg, msg_len);

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

				g_phyRx.flag_pbch = true;
				memcpy(&g_phyRx.pbch, req, sizeof(PHY_PBCHSendReq));
				message_free(req);

				break;
			}
			case MAC_PHY_PDCCH_SEND:
			{				
				PHY_PdcchSendReq* req = (PHY_PdcchSendReq*)msg.data;

				g_phyRx.flag_pdcch = true;
				memcpy(&g_phyRx.pdcch, req, sizeof(PHY_PdcchSendReq));
				message_free(req);
		
				break;
			}
			case MAC_PHY_PUSCH_SEND:
			{				
				PHY_PuschSendReq* req = (PHY_PuschSendReq*)msg.data;

				g_phyRx.flag_pusch = true;
				memcpy(&g_phyRx.pdcch, req, sizeof(PHY_PuschSendReq));
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

void handle_pusch(const      frame_t frame, const sub_frame_t subframe)
{
	msgDef msg;
	msgSize msg_size = 0;
	PHY_PdcchSendReq pdcch = g_phyRx.pdcch;
	PHY_PuschSendReq pusch = g_phyRx.pusch;
	//dci_s dci;
	pusch_result* result;
	uint8_t data_ind = 0;

	PHY_ACKInd ackInd;
	ackInd.frame = frame;
	ackInd.subframe = subframe;
	ackInd.num = 0;

	PHY_PuschReceivedInd puschInd;
	puschInd.frame = frame;
	puschInd.subframe = subframe;
	puschInd.num_ue = 0;

	PHY_CQIInd cqi;
	cqi.frame = frame;
	cqi.subframe = subframe;
	cqi.num = 0;

	if (g_phyRx.flag_pusch && g_phyRx.flag_pdcch &&
		(g_phyRx.pusch.frame == frame && g_phyRx.pusch.subframe == subframe))
	{
		for (uint32_t i = 0; i < pdcch.num_dci; i++)
		{
			//dci = (dci_s)pdcch.dci[i].data[0];
			data_ind = (pdcch.dci[i].data[i] & 0X03) >> 3;

			if (data_ind == 1 || data_ind == 3)//ACK/NACK
			{
				ackInd.ack[ackInd.num].rnti = pdcch.dci[i].rnti;
				ackInd.ack[ackInd.num].ack = 1;
				ackInd.num++;
			}

			if (data_ind == 2 || data_ind == 3)//DATA
			{
				result = &puschInd.result[puschInd.num_ue];
				
				result->rnti = pusch.pusch[i].rnti;
				result->crc = 1;
				result->dataSize = pusch.pusch[i].pdu_len;
				result->dataptr = pusch.pusch[i].data;
				puschInd.num_ue++;
			}

			if (frame%4 == 0)
			{
				cqi.cqiInfo[cqi.num].rnti = pusch.pusch[i].rnti;
				cqi.cqiInfo[cqi.num].cqi = 15;
				cqi.num++;
			}
		}

		if (ackInd.num > 0)
		{
			msg_size = sizeof(PHY_ACKInd);

			if (new_message(&msg, PHY_MAC_PBCH_PDU_RPT, PHY_RX_TASK, MAC_MAIN_TASK, msg_size))
			{
				memcpy(msg.data, &ackInd, msg_size);
			
				if (message_send(MAC_MAIN_TASK, (char *)&msg, sizeof(msgDef)))
				{
			
				}
			}
		}

		if (puschInd.num_ue > 0)
		{
			msg_size = sizeof(PHY_PuschReceivedInd);

			if (new_message(&msg, PHY_MAC_DECOD_DATA_RPT, PHY_RX_TASK, MAC_MAIN_TASK, msg_size))
			{
				memcpy(msg.data, &puschInd, msg_size);
			
				if (message_send(MAC_MAIN_TASK, (char *)&msg, sizeof(msgDef)))
				{
			
				}
			}
		}

		if (cqi.num > 0)
		{
			msg_size = sizeof(PHY_CQIInd);

			if (new_message(&msg, PHY_MAC_CQI_IND, PHY_RX_TASK, MAC_MAIN_TASK, msg_size))
			{
				memcpy(msg.data, &cqi, msg_size);

				if (message_send(MAC_MAIN_TASK, (char *)&msg, sizeof(msgDef)))
				{

				}
			}
		}
	}

	if (pdcch.num_dci != pusch.num)
	{
		LOG_ERROR(PHY, "phy msg missing, num_dci:%u, pusch.num:%u", pdcch.num_dci, pusch.num);
	}

	if ((g_phyRx.flag_pusch && !g_phyRx.flag_pdcch) || (!g_phyRx.flag_pusch && g_phyRx.flag_pdcch))
	{
		LOG_ERROR(PHY, "phy msg missing, flag_pusch:%u, flag_pdcch:%u", g_phyRx.flag_pusch, g_phyRx.flag_pdcch);
	}

	g_phyRx.flag_pbch = false;
	g_phyRx.flag_pdcch = false;
}

void handle_pbch(const      frame_t frame, const sub_frame_t subframe)
{
	msgDef msg;
	msgSize msg_size = 0;

	if (g_phyRx.flag_pbch && 
		(g_phyRx.pbch.frame == frame && g_phyRx.pbch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PBCHReceivedInd);
		
		if (new_message(&msg, PHY_MAC_PBCH_PDU_RPT, PHY_RX_TASK, MAC_MAIN_TASK, msg_size))
		{
			PHY_PBCHReceivedInd* ind = (PHY_PBCHReceivedInd*)msg.data;

			ind->cellId = 0;
			ind->frame = g_phyRx.pbch.frame;
			ind->subframe = g_phyRx.pbch.subframe;
			ind->flag = 1;

			if (message_send(MAC_MAIN_TASK, (char *)&msg, sizeof(msgDef)))
			{

			}
		}

		g_phyRx.flag_pbch = false;
		
	}
}

void handle_phy_rx(const      frame_t frame, const sub_frame_t subframe)
{
	handle_pbch(frame, subframe);

	handle_pusch(frame, subframe);	
}

void *phy_rx_thread()
{
	frame_t frame;
	sub_frame_t subframe;

	while(thread_wait(MAC_TASK, PERIODIC_4MS))
	{
		syncTimePhyRx();

		if (g_timing_sync_phyRx == false)
			continue;

		frame = g_phyRx.frame;
		subframe = g_phyRx.subframe;
		
		frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
		subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;

		handle_phy_rx(frame, subframe);
		phyRxMsgHandler();
	}

	return 0;
}


