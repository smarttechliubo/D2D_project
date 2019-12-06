/**********************************************************
* @file ue_dest_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "d2d_sim.h"
#include "rrc_sim.h"
#include "interface_mac_phy.h"
#include "d2d_message_type.h"
#include "log.h"

d2d_sim_info g_d2d_dst;
uint32_t g_d2d_ueId = 0;

void init_d2d_dst()
{
	g_d2d_dst.frame = 0;
	g_d2d_dst.subframe = 0;
	g_d2d_dst.timeSync = false;
	g_d2d_dst.mode = 1;
	g_d2d_dst.num_ue = 0;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		g_d2d_dst.ue[i].status = ERRC_UE_INVALID;
	}
}

uint16_t get_d2d_new_user_id()
{
	return g_d2d_ueId++;
}

bool d2d_dst_add_new_user(const uint16_t ueId)
{
	ue_sim_info* ue = NULL;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_d2d_dst.ue[i];

		if (ue->status == ERRC_UE_INVALID)
		{
			ue->ueId = ueId;
			ue->rnti = INVALID_U16;
			ue->mode = g_d2d_dst.mode;
			ue->setup_timer = 0;
			ue->status = ERRC_UE_SETUP;

			return true;
		}
	}

	return false;
}

bool d2d_dst_fill_pusch(pusch_info* pusch, ue_sim_info* ue)
{

	if (ue->status == ERRC_UE_SETUP || ue->status == ERRC_UE_SETUP_COMPLETE)
	{
		pusch->rnti = ue->rnti;
		
		pusch->rb_start = 4;
		pusch->rb_num = 80;
		pusch->mcs = 2;
		pusch->data_ind = 2;// 1:ack/nack, 2:data, 3:ack/nack + data

		pusch->modulation = 3;//QPSK
		pusch->rv = 0;
		pusch->harqId = 1;
		pusch->ack = 1;

		pusch->padding_len = 0;
		pusch->pdu_len = 8;
		//pusch->data = ue->ccch;
	}
	return true;
}

void d2d_dst_send_pusch()
{
	ue_sim_info* ue = NULL;
	pusch_info* pusch = NULL;


	msgDef* msg = NULL;
	PHY_PuschSendReq *req;
	msgSize msg_size = sizeof(PHY_PuschSendReq);

	msg = new_message(MAC_PHY_PUSCH_SEND, TASK_D2D_UE, TASK_D2D_PHY_RX, msg_size);

	if (msg != NULL)
	{
		req = (PHY_PuschSendReq*)message_ptr(msg);
		req->frame = g_d2d_dst.frame;
		req->subframe = g_d2d_dst.subframe;
		req->num = 0;

		for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
		{
			ue = &g_d2d_dst.ue[i];

			if (ue->status == ERRC_UE_INVALID)
				continue;

			pusch = &req->pusch[req->num];

			if (d2d_dst_fill_pusch(pusch, ue))
			{
				req->num++;
			}
		}


		if (message_send(TASK_D2D_PHY_RX, msg, sizeof(msgDef)))
		{
			LOG_INFO(RRC, "LGC: rrc_rlc_data_ind send");
		}
	}
	else
	{
		LOG_ERROR(RRC, "[TEST]: src_user_setup new rrc message fail!");
	}

}
/*
void d2d_dst_user_setup_req(rrc_ue_info* ue)
{
	ccch_info ccch;;
	ccch.flag = 0;// 0:setup req, 1:setup, 2:setup complete, 
	ccch.ueId = ueId;
	ccch.rnti = INVALID_U16;
}
*/
void d2dDstcStatusHandler()
{

}

void d2dDstUserStatusHandler(const frame_t frame, const sub_frame_t subframe)
{
	if (g_d2d_dst.timeSync == false)
	{
		return;
	}

	uint16_t ueId = 0;

	if (g_d2d_dst.num_ue == 0)
	{
		ueId = get_d2d_new_user_id();
		d2d_dst_add_new_user(ueId);
	}
}

void d2dDstMain()
{
	frame_t frame = 0;
	sub_frame_t subframe = 0;

	g_d2d_dst.subframe++;

	if (g_d2d_dst.subframe == MAX_SUBSFN)
	{
	    g_d2d_dst.subframe = 0;
		g_d2d_dst.frame = (g_d2d_dst.frame+1)%MAX_SFN;
	}

	frame = g_d2d_dst.frame;
	subframe = g_d2d_dst.subframe;

	if (subframe == 2 || subframe == 3)
	{
		d2dDstcStatusHandler();
		d2dDstUserStatusHandler(frame, subframe);
	}
}

void d2dDstMsgHandler(msgDef* msg)
{
	msgId msg_id = 0;
		
	msg_id = get_msgId(msg);

	switch (msg_id)
	{
		case MAC_PHY_PBCH_TX_REQ:
		{
			PHY_PBCHSendReq *req = (PHY_PBCHSendReq *)message_ptr(msg);

			LOG_INFO(MAC, "[UE D]: MAC_PHY_PBCH_TX_REQ, frame:%u, subframe:%u", 
				req->frame, req->subframe);

			if (g_d2d_dst.timeSync == false)
			{
				g_d2d_dst.frame = req->frame;
				g_d2d_dst.subframe = req->subframe;
				g_d2d_dst.timeSync = true;
			}
			else
			{
				
			}

			break;
		}
		case MAC_PHY_PDCCH_SEND:
		{
			//PHY_PdcchSendReq* req = (PHY_PdcchSendReq*)message_ptr(msg);
			break;
		}
		case MAC_PHY_PUSCH_SEND:
		{
			break;
		}
		default:
		{
			LOG_ERROR(MAC, "[UE]: unknown msg Id:%u", msg_id);
			break;
		}
	}
}
