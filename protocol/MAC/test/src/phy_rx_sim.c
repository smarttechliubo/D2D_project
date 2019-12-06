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
#include "msg_queue.h"
#include "mac_osp_interface.h"

phy_rx_info g_phyRx;
bool g_timing_sync_phyRx = false;

extern void set_sysSfn(frame_t frame, sub_frame_t subframe);
extern int get_sysSfn();
extern uint16_t g_testing_mode;//0:source, 1:destination

void syncTimePhyRx()//TODO: sync
{
	int time = get_sysSfn();

	g_phyRx.frame = time >> 2;
	g_phyRx.subframe = time % MAX_SUBSFN;
/*
	uint32_t time = 0;

	// 1. get timing sync
	if (g_timing_sync_phyRx == false)
    {
		time = get_syncTiming();// TODO: sync

		if (time != 0xFFFFFFFF)
		{
			g_phyRx.frame = time / 10;
			g_phyRx.subframe = time % MAX_SUBSFN;
			g_timing_sync_phyRx = true;
		}
		else
	    {
			LOG_WARN(MAC, "Timing sync fail!!");
			g_timing_sync_phyRx = false;
		}
	}
	else if (g_timing_sync_phyRx == true)
	{
	    g_phyRx.subframe++;

		if (g_phyRx.subframe == MAX_SUBSFN)
		{
		    g_phyRx.subframe = 0;
			g_phyRx.frame = (g_phyRx.frame+1)%MAX_SFN;
		}
		
		if ((g_phyRx.frame*10 + g_phyRx.subframe)%TIMING_SYNC_PERIOD == 0)
		{
			time = get_syncTiming();

			if (time != 0xFFFFFFFF)
			{
				if (time != g_phyRx.frame*10 + g_phyRx.subframe)
				{
					LOG_WARN(PHY, "Timing sync loast!! time:%u, frame:%u, subframe:%u",
						time, g_phyRx.frame, g_phyRx.subframe);

					g_phyRx.frame = time / 10;
					g_phyRx.subframe = time % MAX_SUBSFN;
				}
			}
			else
		    {
				LOG_WARN(MAC, "Timing sync failed with PHY");
			}
		}
	}*/
}

uint32_t init_phy_rx_sim()
{
	void* pTimer;
	int32_t ret;

	pTimer = _timerCreate(TASK_D2D_PHY_RX, 1, 4,0);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_phy_rx_sim pTimer is %p, ret:%u\r\n", pTimer, ret);

	for (uint32_t i = 0; i < MAX_TX_UE; i++)
	{
		g_phyRx.pusch.pusch[i].data = (uint8_t *)mem_alloc(1024);
	}

	return 0;
}

void phyRxMsgHandler(msgDef *msg)
{
	msgId msg_id = 0;
	//mode_e mode = g_testing_mode;

#ifdef MAC_MQ_TEST
	msgHeader* msg;
	task_id taskId = ETASK_A;
	uint32_t msg_len = 0;

	msg = (msgHeader*)mem_alloc(MQ_MSGSIZE);

	while (1)
	{
		msg_len = msgRecv(taskId, (char*)msg, MQ_MSGSIZE);

		if (msg_len <= 0)
		{
			mem_free((char*)msg);
			return;
		}

		msg_id = msg->msgId;
#else
		msg_id = get_msgId(msg);
#endif

		switch (msg_id)
		{
			case MAC_PHY_PBCH_TX_REQ:
			{
				PHY_PBCHSendReq* req = (PHY_PBCHSendReq*)MQ_MSG_CONTENT_PTR(msg);

				g_phyRx.flag_pbch = true;
				memcpy(&g_phyRx.pbch, req, sizeof(PHY_PBCHSendReq));

				LOG_INFO(PHY, "MAC_PHY_PBCH_TX_REQ received, frame:%u,subframe:%u, current frame:%u,subframe:%u",
					req->frame, req->subframe, g_phyRx.frame,g_phyRx.subframe);

				//set_sysSfn(req->frame, req->subframe);

				if (g_timing_sync_phyRx == false)
				{
					syncTimePhyRx();
					g_timing_sync_phyRx = true;
				}

				break;
			}
			case MAC_PHY_PDCCH_SEND:
			{				
				PHY_PdcchSendReq* req = (PHY_PdcchSendReq*)MQ_MSG_CONTENT_PTR(msg);

				g_phyRx.flag_pdcch = true;
				memcpy(&g_phyRx.pdcch, req, sizeof(PHY_PdcchSendReq));
		
				break;
			}
			case MAC_PHY_PUSCH_SEND:
			{				
				PHY_PuschSendReq* req = (PHY_PuschSendReq*)MQ_MSG_CONTENT_PTR(msg);

				g_phyRx.flag_pusch = true;

				g_phyRx.pusch.frame = req->frame;
				g_phyRx.pusch.subframe = req->subframe;
				g_phyRx.pusch.num = req->num;

				for (uint32_t i = 0; i < g_phyRx.pusch.num ; i++)
				{
					memcpy(g_phyRx.pusch.pusch[i].data, req->pusch[i].data, req->pusch[i].pdu_len);
				}
				//memcpy(&g_phyRx.pusch, req, sizeof(PHY_PuschSendReq));
		
				break;
			}
			default:
			{
				LOG_ERROR(PHY, "unknown msgId:%u", msg_id);
				break;
			}
		}
#ifdef MAC_MQ_TEST
	}
	mem_free((char*)msg);
#endif
}

void handle_pusch(const      frame_t frame, const sub_frame_t subframe)
{
	frame_t pusch_frame = g_phyRx.pusch.frame;
	sub_frame_t pusch_subframe = g_phyRx.pusch.subframe;

	pusch_frame = (pusch_frame + (pusch_frame + 1) / MAX_SUBSFN) % MAX_SFN;
	pusch_frame = (pusch_frame + 1) % MAX_SUBSFN;

	msgDef* msg = NULL;
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
		(pusch_frame == frame && pusch_subframe == subframe))
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

			msg = new_message(PHY_MAC_PBCH_PDU_RPT, TASK_D2D_PHY_RX, TASK_D2D_MAC_SCH, msg_size);

			if (msg != NULL)
			{
				memcpy(message_ptr(msg), &ackInd, msg_size);
			
				if (message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
				{
					LOG_INFO(PHY, "LGC: PHY_MAC_PBCH_PDU_RPT send");
				}
			}
		}

		if (puschInd.num_ue > 0)
		{
			msg_size = sizeof(PHY_PuschReceivedInd);

			msg = new_message(PHY_MAC_DECOD_DATA_RPT, TASK_D2D_PHY_RX, TASK_D2D_MAC_SCH, msg_size);

			if (msg != NULL)
			{
				memcpy(message_ptr(msg), &puschInd, msg_size);
			
				if (message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
				{
					LOG_INFO(PHY, "LGC: PHY_MAC_DECOD_DATA_RPT send");
				}
			}
		}

		if (cqi.num > 0)
		{
			msg_size = sizeof(PHY_CQIInd);

			msg = new_message(PHY_MAC_CQI_IND, TASK_D2D_PHY_RX, TASK_D2D_MAC_SCH, msg_size);

			if (msg != NULL)
			{
				memcpy(message_ptr(msg), &cqi, msg_size);

				if (message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
				{
					LOG_INFO(PHY, "LGC: PHY_MAC_CQI_IND send");
				}
			}
		}

		g_phyRx.flag_pbch = false;
		g_phyRx.flag_pdcch = false;
	}

	if (pdcch.num_dci != pusch.num)
	{
		LOG_ERROR(PHY, "phy msg missing, num_dci:%u, pusch.num:%u", pdcch.num_dci, pusch.num);
	}

	if ((g_phyRx.flag_pusch && !g_phyRx.flag_pdcch) || (!g_phyRx.flag_pusch && g_phyRx.flag_pdcch))
	{
		LOG_ERROR(PHY, "phy msg missing, flag_pusch:%u, flag_pdcch:%u", g_phyRx.flag_pusch, g_phyRx.flag_pdcch);
	}

}

void handle_pbch(const      frame_t frame, const sub_frame_t subframe)
{
	frame_t pbch_frame = g_phyRx.pbch.frame;
	sub_frame_t pbch_subframe = g_phyRx.pbch.subframe;

	pbch_frame = (pbch_frame + (pbch_subframe + 1) / MAX_SUBSFN) % MAX_SFN;
	pbch_subframe = (pbch_subframe + 1) % MAX_SUBSFN;

	msgDef* msg = NULL;
	msgSize msg_size = 0;

	if (g_phyRx.flag_pbch && 
		(pbch_frame == frame && pbch_subframe == subframe))
	{
		msg_size = sizeof(PHY_PBCHReceivedInd);
		
		msg = new_message(PHY_MAC_PBCH_PDU_RPT, TASK_D2D_PHY_RX, TASK_D2D_MAC_SCH, msg_size);

		if (msg != NULL)
		{
			PHY_PBCHReceivedInd* ind = (PHY_PBCHReceivedInd*)message_ptr(msg);

			ind->cellId = 0;
			ind->frame = g_phyRx.pbch.frame;
			ind->subframe = g_phyRx.pbch.subframe;
			ind->flag = 1;

			if (message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: PHY_MAC_PBCH_PDU_RPT send");
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

void phy_rx_sim_thread(msgDef *msg)
{
	frame_t frame;
	sub_frame_t subframe;
	bool isTimer = is_timer(msg);

	if (isTimer)
		syncTimePhyRx();

	frame = g_phyRx.frame;
	subframe = g_phyRx.subframe;
	
	//frame = (frame + (subframe + 1) / MAX_SUBSFN) % MAX_SFN;
	//subframe = (subframe + 1) % MAX_SUBSFN;
	LOG_INFO(PHY, "phy_rx_sim_thread frame:%u, subframe:%u, isTimer:%u", frame, subframe, isTimer);

	if (!isTimer)
	{
		phyRxMsgHandler(msg);
	}
	else
	{
		handle_phy_rx(frame, subframe);
	}

	message_free(msg);
}

/*
void *phy_rx_thread()
{
	frame_t frame;
	sub_frame_t subframe;

	while(thread_wait(TASK_D2D_MAC, PERIODIC_4MS))
	{
		syncTimePhyRx();

		if (g_timing_sync_phyRx == false)
			continue;

		frame = g_phyRx.frame;
		subframe = g_phyRx.subframe;
		
		//frame = (frame + (subframe + 1) / MAX_SUBSFN) % MAX_SFN;
		//subframe = (subframe + 1) % MAX_SUBSFN;

		phyRxMsgHandler();
		handle_phy_rx(frame, subframe);
	}

	return 0;
}
*/

