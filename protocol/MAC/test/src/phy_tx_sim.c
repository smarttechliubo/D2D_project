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

phy_tx_info g_phyTx;
bool g_timing_sync_phyTx = false;

extern uint32_t get_syncTiming();
extern uint16_t g_testing_mode;//0:source, 1:destination

int sysSFN = 0;

void sysTimingUpdate()
{
	if (sysSFN == 4095)
	{
		OSP_atomicSet(&sysSFN, 0);
	}
	else
	{
		OSP_atomicInc(&sysSFN);
	}
}

void set_sysSfn(frame_t frame, sub_frame_t subframe)
{
	uint16_t time = (frame << 2) | (subframe & 0X3);
	
	OSP_atomicSet(&sysSFN, time);

	LOG_INFO(PHY, "set_sysSfn, frame:%u, subframe:%u, time:%u, sysSFN:%u",frame, subframe, time, sysSFN);
}

int32_t get_sysSfn()
{
	int sfn = OSP_atomicGet(&sysSFN);
	return sfn;
}

void syncTimePhyTx()//TODO: sync
{
	sysTimingUpdate();

	int time = get_sysSfn();

	g_phyTx.frame = time >> 2;
	g_phyTx.subframe = time % MAX_SUBSFN;
	
/*
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
*/
}

uint32_t init_phy_tx_sim()
{	
	void* pTimer;
	int32_t ret;

	pTimer = _timerCreate(TASK_D2D_PHY_TX, 1, 4,0);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_phy_tx_sim pTimer is %p, ret:%u\r\n", pTimer, ret);

#ifdef MAC_MQ_TEST
	if(g_testing_mode == 1)
	{
		msgq_unlink(ETASK_A);
		msgq_unlink(ETASK_B);
	}

	msgq_init(ETASK_A,ENONBLOCK);
	msgq_init(ETASK_B,ENONBLOCK);
#endif

	return 0;
}

void phyTxMsgHandler(msgDef* msg)
{
	msgId msg_id = 0;

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

			LOG_INFO(PHY, "MAC_PHY_PDCCH_SEND received. frame:%u, subframe:%u", req->frame, req->subframe);

			g_phyTx.flag_pdcch = true;
			memcpy(&g_phyTx.pdcch, req, sizeof(PHY_PdcchSendReq));
	
			break;
		}
		case MAC_PHY_PUSCH_SEND:
		{				
			PHY_PuschSendReq* req = (PHY_PuschSendReq*)message_ptr(msg);

			LOG_INFO(PHY, "MAC_PHY_PUSCH_SEND received. frame:%u, subframe:%u", req->frame, req->subframe);

			g_phyTx.flag_pusch = true;
			memcpy(&g_phyTx.pusch, req, sizeof(PHY_PuschSendReq));

			break;
		}
		default:
		{
			LOG_ERROR(PHY, "unknown msgId:%u", msg_id);
			break;
		}
	}

}

void handle_phy_tx(const      frame_t frame, const sub_frame_t subframe)
{
	//mode_e mode = g_testing_mode;
#ifdef MAC_MQ_TEST
	task_id taskId = ETASK_B;

	msgHeader* msg = NULL;
#else
	msgDef* msg = NULL;
#endif
	
	msgSize msg_size = 0;

	if (g_phyTx.flag_pbch && 
		(g_phyTx.pbch.frame == frame && g_phyTx.pbch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PBCHSendReq);
#ifdef MAC_MQ_TEST
		msg = new_msg(MAC_PHY_PBCH_TX_REQ, TASK_D2D_PHY_TX, taskId, msg_size);

		if (msg != NULL)
		{
			memcpy(MQ_MSG_CONTENT_PTR(msg), &g_phyTx.pbch, msg_size);

			if (msgSend(taskId, (char*)msg, sizeof(msgHeader) + msg_size))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PBCH_TX_REQ send, pbch frame:%u, subframe:%u, current frame:%u, subframe:%u",
					g_phyTx.pbch.frame,g_phyTx.pbch.subframe,g_phyTx.frame,g_phyTx.subframe);
			}
		}
#else
		msg = new_message(MAC_PHY_PBCH_TX_REQ, TASK_D2D_PHY_TX, TASK_D2D_PHY_RX, msg_size);

		if (msg != NULL)
		{
			memcpy(MSG_HEAD_TO_COMM(msg), &g_phyTx.pbch, msg_size);

			if (message_send(TASK_D2D_PHY_RX, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PBCH_TX_REQ send, pbch frame:%u, subframe:%u, current frame:%u, subframe:%u",
					g_phyTx.pbch.frame,g_phyTx.pbch.subframe,g_phyTx.frame,g_phyTx.subframe);
			}
		}
#endif

		g_phyTx.flag_pbch = false;
	}

	if (g_phyTx.flag_pdcch && 
		(g_phyTx.pdcch.frame == frame && g_phyTx.pdcch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PdcchSendReq);

#ifdef MAC_MQ_TEST	
		msg = new_msg(MAC_PHY_PDCCH_SEND, TASK_D2D_PHY_TX, taskId, msg_size);

		if (msg != NULL)
		{
			memcpy(MQ_MSG_CONTENT_PTR(msg), &g_phyTx.pdcch, msg_size);

			if (msgSend(taskId, (char*)msg, sizeof(msgHeader) + msg_size))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PDCCH_SEND send");
			}
		}
#else
		msg = new_message(MAC_PHY_PDCCH_SEND, TASK_D2D_PHY_TX, TASK_D2D_PHY_RX, msg_size);

		if (msg != NULL)
		{
			memcpy(MSG_HEAD_TO_COMM(msg), &g_phyTx.pdcch, msg_size);

			if (message_send(TASK_D2D_PHY_RX, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PDCCH_SEND send, pdcch frame:%u, subframe:%u, current frame:%u, subframe:%u",
					g_phyTx.pdcch.frame,g_phyTx.pdcch.subframe,g_phyTx.frame,g_phyTx.subframe);
			}
		}

#endif
		g_phyTx.flag_pdcch = false;
	}

	if (g_phyTx.flag_pusch && 
		(g_phyTx.pusch.frame == frame && g_phyTx.pusch.subframe == subframe))
	{
		msg_size = sizeof(PHY_PuschSendReq);

#ifdef MAC_MQ_TEST
		msg = new_msg(MAC_PHY_PUSCH_SEND, TASK_D2D_PHY_TX, taskId, msg_size);

		if (msg != NULL)
		{
			memcpy(MQ_MSG_CONTENT_PTR(msg), &g_phyTx.pusch, msg_size);

			if (msgSend(taskId, (char*)msg, sizeof(msgHeader) + msg_size))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PUSCH_SEND send");
			}
		}
#else
		msg = new_message(MAC_PHY_PUSCH_SEND, TASK_D2D_PHY_TX, TASK_D2D_PHY_RX, msg_size);

		if (msg != NULL)
		{
			memcpy(MSG_HEAD_TO_COMM(msg), &g_phyTx.pusch, msg_size);

			if (message_send(TASK_D2D_PHY_RX, msg, sizeof(msgDef)))
			{
				LOG_INFO(PHY, "LGC: MAC_PHY_PUSCH_SEND send, pdcch frame:%u, subframe:%u, current frame:%u, subframe:%u",
					g_phyTx.pusch.frame,g_phyTx.pusch.subframe,g_phyTx.frame,g_phyTx.subframe);
			}
		}

#endif
		g_phyTx.flag_pusch = false;
	}
}

void phy_tx_sim_thread(msgDef* msg)
{
	frame_t frame;
	sub_frame_t subframe;
	bool isTimer = is_timer(msg);

	if (isTimer)
		syncTimePhyTx();

	frame = g_phyTx.frame;
	subframe = g_phyTx.subframe;

	LOG_INFO(PHY, "phy_tx_sim_thread frame:%u, subframe:%u, isTimer:%u", g_phyTx.frame,g_phyTx.subframe, isTimer);
	//frame = (frame + (subframe + TIMING_ADVANCE) / MAX_SUBSFN) % MAX_SFN;
	//subframe = (subframe + TIMING_ADVANCE) % MAX_SUBSFN;
	if (!isTimer)
	{
		phyTxMsgHandler(msg);
	}
	else
	{
		handle_phy_tx(frame, subframe);
	}

	message_free(msg);
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
