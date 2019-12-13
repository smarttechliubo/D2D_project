/**********************************************************
* @file rlc_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include <stdlib.h>

#include "rlc_sim.h"
#include "mytask.h"
#include "messageDefine.h"
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"
#include "interface_mac_rlc.h"
#include "rrc_sim.h"
#include "mac_header.h"
#include "mac_osp_interface.h"

rlc_info g_rlc;
//static uint32_t g_runtime = 0;

uint32_t generate_mac_header(uint32_t num_sdus,
	uint32_t sdu_lcids[MAX_LOGICCHAN_NUM],
	uint32_t sdu_sizes[MAX_LOGICCHAN_NUM],
	uint8_t *dataptr,
	uint32_t padding)
{
	mac_header_fixed *mac_header_ptr = (mac_header_fixed *) dataptr;
	uint32_t pre_sub_header = 0;
	uint32_t pre_sub_header_size = 0;

	if ((padding == 1) || (padding == 2)) 
	{
		mac_header_ptr->R = 0;
		mac_header_ptr->E = 0;
		mac_header_ptr->LCID = SHORT_PADDING;
		pre_sub_header = 1;
		pre_sub_header_size = 1;
	}

	if (padding == 2) 
	{
		mac_header_ptr->E = 1;
		mac_header_ptr++;
		mac_header_ptr->R = 0;
		mac_header_ptr->E = 0;
		mac_header_ptr->LCID = SHORT_PADDING;
	}

	for (uint32_t i = 0; i < num_sdus; i++)
	{
	
		if (pre_sub_header > 0) 
		{
			mac_header_ptr->E = 1;
			mac_header_ptr += pre_sub_header_size;
		}
		else 
		{
			pre_sub_header = 1;
		}
	
		if (sdu_sizes[i] < 128) 
		{
			((mac_header_short *) mac_header_ptr)->R = 0;
			((mac_header_short *) mac_header_ptr)->E = 0;
			((mac_header_short *) mac_header_ptr)->LCID = sdu_lcids[i];

			if ((i < (num_sdus - 1)) || 
				((i == (num_sdus - 1)) && (padding >= 3)))
			{
				((mac_header_short *) mac_header_ptr)->F = 0;
				((mac_header_short *) mac_header_ptr)->L = (unsigned char) sdu_sizes[i];

				pre_sub_header_size = 2;
			}
			else
			{
				pre_sub_header_size = 1;
			}
		} 
		else
		{
			((mac_header_long *) mac_header_ptr)->R = 0;
			((mac_header_long *) mac_header_ptr)->E = 0;
			((mac_header_long *) mac_header_ptr)->LCID = sdu_lcids[i];
			((mac_header_long *) mac_header_ptr)->F = 1;

			if ((i < (num_sdus - 1)) || 
				((i == (num_sdus - 1)) && (padding >= 3)))
			{
				((mac_header_long *) mac_header_ptr)->L_MSB = ((unsigned short) sdu_sizes[i] >> 8) & 0x7f;
				((mac_header_long *) mac_header_ptr)->L_LSB = (unsigned short) sdu_sizes[i] & 0xff;
				((mac_header_long *) mac_header_ptr)->padding = 0x00;

				pre_sub_header_size = 3;
			}
			else
			{
				pre_sub_header_size = 1;
			}
		}
	  }

	if (padding >= 3) 
	{
		mac_header_ptr->E = 1;
		mac_header_ptr += pre_sub_header_size;
		mac_header_ptr->R = 0;
		mac_header_ptr->E = 0;
		mac_header_ptr->LCID = SHORT_PADDING;
		mac_header_ptr++;
	}
	else 
	{
		mac_header_ptr++;
	}

	return ((uint8_t *) mac_header_ptr - (uint8_t *)dataptr);

}


uint32_t init_rlc_sim()
{
	//g_runtime = 0;
	g_rlc.num_ue = 0;

	LOG_INFO(MAC,"init_rlc_sim \r\n");

	memset(&g_rlc.ue[0], 0, sizeof(rlc_ue_info)*D2D_MAX_USER_NUM);

	return 0;
}

rlc_ue_info* find_rlc_user(const uint16_t ueId, const uint16_t mode)
{
	rlc_ue_info* ue = &g_rlc.ue[0];

	for (uint16_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rlc.ue[i];

		if (ue->valid != 0 && ue->ueId == ueId && ue->mode == mode)
		{
			return ue;
		}
	}

	return NULL;
}

rlc_ue_info* find_rlc_user_by_rnti(const rnti_t rnti, const uint16_t mode)
{
	rlc_ue_info* ue = NULL;

	for (uint16_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rlc.ue[i];

		if (ue->valid != 0 && ue->rnti == rnti && ue->mode == mode)
		{
			return ue;
		}
	}

	return NULL;
}

void rlc_remove_user(rlc_ue_info* ue)
{
	ue->valid = 0;

	ue->ccch_data_size = 0;
	ue->lc_num = 0;
	
	//for (uint32_t i = 0; i < MAX_LOGICCHAN_NUM; i++)
	{
		mem_free((char*)ue->data_ptr);
	}
}

rlc_ue_info* rlc_add_new_user(ccch_info* ccch)
{
	rlc_ue_info* ue = NULL;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rlc.ue[i];

		if (ue->valid == 0)
		{
			break;
		}
		else if (i == D2D_MAX_USER_NUM)
		{
			LOG_ERROR(RLC, "rlc_add_new_user reach max ue number!");
			return NULL;
		}
	}

	if (ccch->flag == 0)
	{
		ue->mode = 1;
	}
	else if (ccch->flag == 1)
	{
		ue->mode = 0;
	}

	ue->valid = 1;
	ue->ueId = ccch->ueId;
	ue->rnti = ccch->rnti;

	ue->ccch_data_size = 0;
	//ue->ccch_data = NULL;
	ue->lc_num = MAX_LOGICCHAN_NUM;

	for (uint32_t i = 0; i < MAX_LOGICCHAN_NUM; i++)
	{
		ue->data_size[i] = 0;
	}

	ue->data_ptr = (uint32_t  *)mem_alloc(5120);

	LOG_INFO(RLC, "rlc_add_new_user UE ueId:%u, rnti:%u!", ue->ueId, ue->rnti);

	return ue;
}

void handle_mac_rlc_buf_status_req(const mac_rlc_buf_status_req *req)
{
	uint16_t mode = (req->sub_sfn == 0 || req->sub_sfn == 1) ? 0 : 1;

	msgDef* msg = NULL;
	rlc_mac_buf_status_rpt *rpt;
	msgSize msg_size = sizeof(rlc_mac_buf_status_rpt);

	rlc_ue_info* ue = NULL;
	uint32_t  valid_ue_num = 0; 
	uint8_t logic_chan_num = 0;

	msg = new_message(RLC_MAC_BUF_STATUS_RPT, TASK_D2D_RLC, TASK_D2D_MAC_SCH, msg_size);

	if (msg != NULL)
	{
		rpt = (rlc_mac_buf_status_rpt*)message_ptr(msg);
		rpt->sfn = req->sfn;
		rpt->sub_sfn = req->sub_sfn;
		rpt->valid_ue_num = 0;

		valid_ue_num = 0;

		for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
		{
			ue = &g_rlc.ue[i];

			if (!ue->valid || ue->mode != mode)
				continue;

			logic_chan_num = 0;

			rpt->rlc_buffer_rpt[valid_ue_num].valid_flag = 1;
			rpt->rlc_buffer_rpt[valid_ue_num].logic_chan_num = 0;
			rpt->rlc_buffer_rpt[valid_ue_num].rnti = ue->rnti;

			if (ue->hasCCCH)
			{
				rpt->rlc_buffer_rpt[valid_ue_num].rlc_mode_for_logicchan[logic_chan_num] = RLC_MODE_TM;
				rpt->rlc_buffer_rpt[valid_ue_num].logicchannel_id[logic_chan_num] = 0;
				rpt->rlc_buffer_rpt[valid_ue_num].buffer_byte_size[logic_chan_num] = ue->ccch_data_size;
				logic_chan_num++;

				memcpy(ue->data_ptr, &ue->ccch_data[0], ue->ccch_data_size);

				ue->hasCCCH = false;
			}

			for (uint32_t i = 1; i < ue->lc_num; i++)
			{
				if (ue->data_size[i] > 0)
				{
					rpt->rlc_buffer_rpt[valid_ue_num].rlc_mode_for_logicchan[logic_chan_num] = RLC_MODE_UM;
					rpt->rlc_buffer_rpt[valid_ue_num].logicchannel_id[logic_chan_num] = i;
					rpt->rlc_buffer_rpt[valid_ue_num].buffer_byte_size[logic_chan_num] = ue->data_size[i];
					logic_chan_num++;
				}
			}

			rpt->rlc_buffer_rpt[valid_ue_num].logic_chan_num = logic_chan_num;

			if (logic_chan_num > 0)
			{
				valid_ue_num++;
			}
		}

		rpt->valid_ue_num = valid_ue_num;

		if (rpt->valid_ue_num > 0)
		{
			if(message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
			{
				LOG_INFO(RLC, "LGC: RLC_MAC_BUF_STATUS_RPT, send msg!, rnti:%u, lcId:%u, buffSize:%u,rlcHeader:%u", 
				rpt->rlc_buffer_rpt[0].rnti, rpt->rlc_buffer_rpt[0].logicchannel_id[0],
				rpt->rlc_buffer_rpt[0].buffer_byte_size[0], rpt->rlc_buffer_rpt[0].rlc_header_byte_size[0]);
			}
		}
		else
		{
			message_free(msg);
		}
	}
}

void fill_mac_sdus(rlc_mac_data_ind* ind, mac_rlc_data_req* req)
{
	uint16_t mode = (req->sub_sfn == 0 || req->sub_sfn == 1) ? 0 : 1;

	rlc_data_req* dataReq = NULL;
	//rnti_t rnti = 0;
	rlc_ue_info* ue = NULL;
	uint16_t  tb_size = 0;
	uint8_t logicchannel_id = 0;
	//uint32_t data_size = 0;
	uint32_t dataReqSize = 0;

	uint32_t num_sdus = 0;
	uint32_t sdu_lcids[MAX_LOGICCHAN_NUM];
	uint32_t sdu_sizes[MAX_LOGICCHAN_NUM];

	uint32_t padding = 0;//0:no padding, 1: 1byte padding, 2: 2byte padding, >= 3: long padding
	uint32_t offset = 0;
	//bool hasCCCH = false;
	uint32_t header_size = 0;
	uint32_t header_last = 0;
	uint32_t total_sdu_size = 0;
	uint8_t* ptr = NULL;

	for (uint32_t i = 0; i < req->ue_num; i++)
	{
		dataReq = &req->rlc_data_request[i];

		ue = find_rlc_user_by_rnti(dataReq->rnti, mode);

		if (ue == NULL)
		{
			LOG_ERROR(RLC, "data req, ue->rnti:%u does not exist", dataReq->rnti);
			continue;
		}

		ptr = (uint8_t*)ue->data_ptr;

		tb_size = dataReq->tb_size;
		//data_size = 0;
		num_sdus = 0;
		header_size = 0;
		header_last = 0;
		total_sdu_size = 0;

		for (uint32_t j = 0; j < dataReq->logic_chan_num; j++)
		{
			logicchannel_id = dataReq->logicchannel_id[j];
			dataReqSize = dataReq->mac_pdu_byte_size[j];

			header_last = dataReqSize > 130 ? 3 : 2;// TODO: not accurate, should be modify later
			header_size += header_last;

			sdu_lcids[num_sdus] = logicchannel_id;
			sdu_sizes[num_sdus] = dataReqSize - header_last;

			total_sdu_size += sdu_sizes[num_sdus];
			num_sdus++;

			if (ue->data_size[logicchannel_id] >= dataReqSize)
			{
				//ue->data_size[logicchannel_id] -= dataReqSize;
			}
			else
			{
				//LOG_ERROR(RLC, "req data size too small");
			}

		}

		if (num_sdus > 0)
		{
			LOG_INFO(RLC, "lcNUM:%u, tb_size:%u, total_sdu_size:%u, header_size:%u",
				dataReq->logic_chan_num, tb_size , total_sdu_size , header_size);

			padding = tb_size - total_sdu_size - (header_size - (header_last == 2 ? 1 : 2));

			//memset((uint8_t*)ue->data_ptr, 0X12,  dataReqSize);

			offset = generate_mac_header(num_sdus, sdu_lcids, sdu_sizes, ptr, padding);

			for (uint32_t j = 0; j < num_sdus; j++)
			{
				if (sdu_lcids[j] == CCCH_)
				{
					ccch_info* ccch = (ccch_info*)&ue->ccch_data[0];
					memcpy(ptr + offset, &ue->ccch_data[0], ue->ccch_data_size);
				
					LOG_INFO(MAC, "size:%u, flag:%u,cause:%u,ueId:%u,rnti:%u",
						ue->ccch_data_size,ccch->flag,ccch->cause,ccch->ueId,ccch->rnti);
				
					offset = offset + ue->ccch_data_size;
					ue->ccch_data_size = 0;
				}
				else
				{
					memcpy(ptr + offset, ue->data[j], ue->data_size[j]);
				
					offset = offset + ue->data_size[j];
				}
			}


			LOG_DEBUG(RLC, "MAC mac_header, offset:%u, padding:%u, data:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",offset, padding,
				ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9]);
		}

		ind->sdu_pdu_info[ind->ue_num].valid_flag = 1;
		ind->sdu_pdu_info[ind->ue_num].rnti = ue->rnti;
		ind->sdu_pdu_info[ind->ue_num].tb_byte_size = dataReqSize;
		ind->sdu_pdu_info[ind->ue_num].data_buffer_adder_ptr = ue->data_ptr;
		ind->ue_num++;
	}
}


void handle_mac_rlc_data_req(mac_rlc_data_req* req)
{
	msgDef* msg = NULL;
	rlc_mac_data_ind* ind;
	msgSize msg_size = sizeof(rlc_mac_data_ind);

	msg = new_message(RLC_MAC_DATA_IND, TASK_D2D_RLC, TASK_D2D_MAC_SCH, msg_size);

	if (msg != NULL)
	{
		ind = (rlc_mac_data_ind*)message_ptr(msg);
		ind->ue_num = 0;

		fill_mac_sdus(ind, req);
		
		if (message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
		{
			LOG_INFO(RLC, "LGC: RLC_MAC_DATA_IND send");
		}
	}
}

void handle_rrc_data_ind(rrc_rlc_data_ind *ind)
{
	ccch_info* ccch = NULL;
	rlc_ue_info* ue = NULL;
	uint16_t mode = 0;

	if (ind->rb_type != RB_TYPE_SRB0)
	{
		LOG_ERROR(RLC, "unexpect msg rb_type:%u",ind->rb_type);
		return;
	}

	ccch = (ccch_info*)ind->data_addr_ptr;

	if (ccch->flag == 0 || ccch->flag == 2 || ccch->flag == 4)
	{
		mode = 1;
	}
	else if (ccch->flag == 1 || ccch->flag == 3)
	{
		mode = 0;
	}

	ue  = find_rlc_user(ccch->ueId, mode);

	if (ue == NULL)
	{
		ue = rlc_add_new_user(ccch);
	}

	if (ue == NULL)
		return;

	if (ccch->flag == 2)
	{
		ue->rnti = ccch->rnti;
	}

	if (ccch->flag == 4)
	{
		ue->data[1] = "I am destination UE!";
		ue->data_size[1] = sizeof(ue->data);
	}

	if (ccch->flag == 3)
	{
		ue->data[1] = "I am source UE!";
		ue->data_size[1] = sizeof(ue->data);
	}

	if (ccch->flag == 0 || ccch->flag == 1 || ccch->flag == 2)
	{
		ue->hasCCCH = true;
		ue->ccch_data_size = ind->data_size;
		ue->data_size[0] = ind->data_size;

		memcpy(&ue->ccch_data[0], ind->data_addr_ptr, ue->ccch_data_size);

		LOG_INFO(RLC, "RLC CCCH received, flag:%u, cause:%u, ueId:%u, rnti:%u", 
			ccch->flag, ccch->cause, ccch->ueId, ccch->rnti);
	}


	mem_free((char*)ind->data_addr_ptr);
}

void handle_mac_rlc_data_rpt(mac_rlc_data_rpt* req)
{
    uint32_t  ue_num = req->ue_num; 
	mac_rlc_data_info* data_ind = NULL;
	rnti_t   rnti = 0; 
	
	uint16_t  logic_chan_num = 0; 
	uint16_t  logicchannel_id = 0;
	uint32_t  mac_pdu_size = 0;
	char  *mac_pdu_buffer_ptr = NULL;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		data_ind = &req->sdu_data_rpt[i];

		if (data_ind->valid_flag)
			continue;

		rnti = data_ind->rnti;
		logic_chan_num = data_ind->logic_chan_num;

		for (uint32_t j = 0; j < logic_chan_num; j++)
		{
			logicchannel_id = data_ind->logicchannel_id[j];
			mac_pdu_size = data_ind->mac_pdu_size[j];
			mac_pdu_buffer_ptr = (char  *)data_ind->mac_pdu_buffer_ptr[j];

			LOG_DEBUG(RLC, "Received MAC data report, rnti:%u, lcId:%u, pduSize:%u, pdu:%s", 
				rnti, logicchannel_id, mac_pdu_size, mac_pdu_buffer_ptr);
		}

		mem_free((char*)data_ind->mac_pdu_buffer_ptr[0]);
	}
}

void rlcMsgHandler(msgDef* msg)
{
	//msgDef* msg = NULL;
	//uint32_t msg_len = 0;
	msgId msg_id = 0;

	//while (1)
	{ 
		//msg_len = message_receive(TASK_D2D_RLC, msg);

		//if (msg_len == 0)
		//{
		//	continue;
		//}

		if (is_timer(msg))
			return;
		
		msg_id = get_msgId(msg);

		switch (msg_id)
		{
			case RRC_RLC_DATA_IND:
			{
				rrc_rlc_data_ind *ind = (rrc_rlc_data_ind *)message_ptr(msg);

				LOG_INFO(RLC, "RRC_RLC_DATA_IND");
				
				handle_rrc_data_ind(ind);
				break;
			}
			case MAC_RLC_BUF_STATUS_REQ:
			{
				mac_rlc_buf_status_req *req = (mac_rlc_buf_status_req *)message_ptr(msg);

				LOG_INFO(RLC, "MAC_RLC_BUF_STATUS_REQ, sfn:%u, sunsfn:%u", req->sfn, req->sub_sfn);

				handle_mac_rlc_buf_status_req(req);
				break;
			}
			case MAC_RLC_DATA_REQ:
			{
				mac_rlc_data_req *req = (mac_rlc_data_req *)message_ptr(msg);

				LOG_INFO(RLC, "MAC_RLC_DATA_REQ");

				handle_mac_rlc_data_req(req);
				break;
			}
			case MAC_RLC_DATA_RPT:
			{
				mac_rlc_data_rpt *req = (mac_rlc_data_rpt *)message_ptr(msg);

				LOG_INFO(RLC, "MAC_RLC_DATA_RPT");

				handle_mac_rlc_data_rpt(req);

				break;
			}
			default:
			{
				LOG_ERROR(RLC, "UNknown RLC msg, msgId:%u", msg_id);
				break;
			}
		}

	}
}

void rlc_sim_thread(msgDef* msg)
{
	LOG_INFO(RLC, "rlc_sim_thread ");
	rlcMsgHandler(msg);

	message_free(msg);
}

/*
void *rlc_thread()
{

	//rlcMsgHandler();

	for (uint16_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		 rlc_ue_info* ue = &g_rlc.ue[i];

		//if (ue->valid)
		rlc_remove_user(ue );
	}

	return 0;
}

*/
