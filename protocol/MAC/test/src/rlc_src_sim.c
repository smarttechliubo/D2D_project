/**********************************************************
* @file rlc_src_sim.c
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
#include "d2d_message_type.h"
#include "log.h"
#include "msg_handler.h"
#include "interface_mac_rlc.h"
#include "rrc_sim.h"
#include "mac_header.h"
#include "mac_osp_interface.h"

rlc_info g_rlc_src;
//static uint32_t g_runtime = 0;


void init_rlc_src_sim()
{
	//g_runtime = 0;
	g_rlc_src.num_ue = 0;
}

rlc_ue_info* find_src_rlc_src_user(const uint16_t ueId, const uint16_t mode)
{
	rlc_ue_info* ue = &g_rlc_src.ue[0];

	for (uint16_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rlc_src.ue[i];

		if (ue->valid != 0 && ue->ueId == ueId && ue->mode == mode)
		{
			return ue;
		}
	}

	return NULL;
}

rlc_ue_info* find_src_rlc_user_by_rnti(const rnti_t rnti)
{
	rlc_ue_info* ue = NULL;

	for (uint16_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rlc_src.ue[i];

		if (ue->valid != 0 && ue->rnti == rnti)
		{
			return ue;
		}
	}

	return NULL;
}

void rlc_src_remove_user(rlc_ue_info* ue)
{
	ue->valid = 0;

	ue->ccch_data_size = 0;
	ue->lc_num = 0;
	
	//for (uint32_t i = 0; i < MAX_LOGICCHAN_NUM; i++)
	{
		mem_free((char*)ue->data_ptr);
	}
}

rlc_ue_info* rlc_src_add_new_user(ccch_info* ccch)
{
	rlc_ue_info* ue = NULL;

	for (uint32_t i = 0; i < D2D_MAX_USER_NUM; i++)
	{
		ue = &g_rlc_src.ue[i];

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
	ue->lc_num = 1;

	for (uint32_t i = 0; i < MAX_LOGICCHAN_NUM; i++)
	{
		ue->data_size[i] = 0;
	}

	ue->data_ptr = (uint32_t  *)mem_alloc(5120);

	LOG_INFO(RLC, "rlc_add_new_user UE ueId:%u, rnti:%u!", ue->ueId, ue->rnti);

	return ue;
}

void handle_src_mac_rlc_buf_status_req(const mac_rlc_buf_status_req *req)
{
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
			ue = &g_rlc_src.ue[i];

			if (!ue->valid)
				continue;

			logic_chan_num = 0;

			rpt->rlc_buffer_rpt[valid_ue_num].valid_flag = 1;
			rpt->rlc_buffer_rpt[valid_ue_num].logic_chan_num = 0;
			rpt->rlc_buffer_rpt[valid_ue_num].rnti = ue->rnti;
/*
			if (ue->hasCCCH)
			{
				rpt->rlc_buffer_rpt[valid_ue_num].rlc_mode_for_logicchan[logic_chan_num] = RLC_MODE_TM;
				rpt->rlc_buffer_rpt[valid_ue_num].logicchannel_id[logic_chan_num] = 0;
				rpt->rlc_buffer_rpt[valid_ue_num].buffer_byte_size[logic_chan_num] = ue->ccch_data_size;
				logic_chan_num++;

				//memcpy(ue->data_ptr, &ue->ccch_data[0], ue->ccch_data_size);

				ue->hasCCCH = false;
			}
*/
			for (uint32_t i = 0; i < ue->lc_num; i++)
			{
				if (ue->data_size[i] > 0)
				{
					rpt->rlc_buffer_rpt[valid_ue_num].rlc_mode_for_logicchan[logic_chan_num] = (i == 0) ?  RLC_MODE_TM : RLC_MODE_UM;
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
				LOG_INFO(RLC, "LGC: RLC_MAC_BUF_STATUS_RPT, send msg!, lcNum:%u,rnti:%u, lcId:%u,%u, buffSize:%u,rlcHeader:%u", 
				rpt->rlc_buffer_rpt[0].logic_chan_num,rpt->rlc_buffer_rpt[0].rnti, rpt->rlc_buffer_rpt[0].logicchannel_id[0],
				rpt->rlc_buffer_rpt[0].logicchannel_id[1],
				rpt->rlc_buffer_rpt[0].buffer_byte_size[0], rpt->rlc_buffer_rpt[0].rlc_header_byte_size[0]);
			}
		}
		else
		{
			message_free(msg);
		}
	}
}

void fill_src_mac_sdus(rlc_mac_data_ind* ind, mac_rlc_data_req* req)
{
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

		ue = find_src_rlc_user_by_rnti(dataReq->rnti);

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
			padding = tb_size - total_sdu_size - (header_size - (header_last == 2 ? 1 : 2));

			//memset((uint8_t*)ue->data_ptr, 0X12,  dataReqSize);

			offset = generate_mac_header(num_sdus, sdu_lcids, sdu_sizes, ptr, padding);

			LOG_INFO(RLC, "SRC offset:%u, padding:%u, lcNUM:%u, num_sdus:%u,sdu_lcids[0]:%u,sdu_sizes[0]:%u,tb_size:%u, total_sdu_size:%u, header_size:%u",
				offset, padding, dataReq->logic_chan_num, num_sdus, sdu_lcids[0],sdu_sizes[0],tb_size , total_sdu_size , header_size);


			for (uint32_t j = 0; j < num_sdus; j++)
			{
			/*
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
			*/
				uint8_t lc_id = sdu_lcids[j];

				if (ue->data_size[lc_id] > 0)
				{
					memcpy(ptr + offset, ue->data[lc_id], ue->data_size[lc_id]);
				
					offset = offset + ue->data_size[lc_id];

					if (lc_id == CCCH_)
						ue->data_size[lc_id] = 0;
				}
				else
				{
					LOG_ERROR(RLC, "data of lc_id:%u is empty");
				}
			}


			LOG_DEBUG(RLC, "RLC mac_header, data:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",
				ptr[0],ptr[1],ptr[2],ptr[3],ptr[4],ptr[5],ptr[6],ptr[7],ptr[8],ptr[9]);
		}

		ind->sdu_pdu_info[ind->ue_num].valid_flag = 1;
		ind->sdu_pdu_info[ind->ue_num].rnti = ue->rnti;
		ind->sdu_pdu_info[ind->ue_num].tb_byte_size = dataReqSize;
		ind->sdu_pdu_info[ind->ue_num].data_buffer_adder_ptr = ue->data_ptr;
		ind->ue_num++;
	}
}


void handle_src_mac_rlc_data_req(mac_rlc_data_req* req)
{
	msgDef* msg = NULL;
	rlc_mac_data_ind* ind;
	msgSize msg_size = sizeof(rlc_mac_data_ind);

	msg = new_message(RLC_MAC_DATA_IND, TASK_D2D_RLC, TASK_D2D_MAC_SCH, msg_size);

	if (msg != NULL)
	{
		ind = (rlc_mac_data_ind*)message_ptr(msg);
		ind->ue_num = 0;

		fill_src_mac_sdus(ind, req);
		
		if (message_send(TASK_D2D_MAC_SCH, msg, sizeof(msgDef)))
		{
			LOG_INFO(RLC, "LGC: RLC_MAC_DATA_IND send");
		}
	}
}

void handle_src_rrc_data_ind(rrc_rlc_data_ind *ind)
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

	mode = ccch->mode;

	if (mode != 0)
		return;

	ue  = find_src_rlc_src_user(ccch->ueId, mode);

	if (ue == NULL)
	{
		ue = rlc_src_add_new_user(ccch);
	}

	if (ue == NULL)
		return;

	if (ccch->flag != 1 && ccch->flag != 3)
	{
		LOG_ERROR(RLC,"SRC RLC received dst msg flag:%u", ccch->flag);
		return;
	}

	if (ccch->flag == 1)
	{
		ue->hasCCCH = true;
		ue->ccch_data_size = ind->data_size;
		ue->data_size[0] = ind->data_size;

		memcpy(&ue->ccch_data[0], ind->data_addr_ptr, ue->ccch_data_size);

		ue->data[0] = (char*)&ue->ccch_data[0];

		LOG_INFO(RLC, "SRC RLC CCCH received, flag:%u, cause:%u, ueId:%u, rnti:%u", 
			ccch->flag, ccch->cause, ccch->ueId, ccch->rnti);
	}

	if (ccch->flag == 3)
	{
		rrc_setup* setup = (rrc_setup*)ind->data_addr_ptr;

		for (uint32_t i = 0; i < setup->setup.logical_channel_num; i++)
		{
			ue->data[ue->lc_num] = "I am source UE!";
			ue->data_size[ue->lc_num] = strlen(ue->data[ue->lc_num]);
			ue->lc_num++;
		}

		LOG_INFO(RLC, "SRC RLC CCCH received, flag:3, ue->data_size[1]:%u",ue->data_size[1]);
	}

	mem_free((char*)ind->data_addr_ptr);
}

void handle_src_mac_rlc_data_rpt(mac_rlc_data_rpt* req)
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

		if (!data_ind->valid_flag)
			continue;

		rnti = data_ind->rnti;
		logic_chan_num = data_ind->logic_chan_num;

		for (uint32_t j = 0; j < logic_chan_num; j++)
		{
			logicchannel_id = data_ind->logicchannel_id[j];
			mac_pdu_size = data_ind->mac_pdu_size[j];
			mac_pdu_buffer_ptr = (char  *)data_ind->mac_pdu_buffer_ptr[j];

			LOG_INFO(RLC, "Received MAC data report, rnti:%u, lcId:%u, pduSize:%u, pdu:%s", 
				rnti, logicchannel_id, mac_pdu_size, mac_pdu_buffer_ptr);
		}

		//mem_free((char*)data_ind->mac_pdu_buffer_ptr[0]);
	}
}

void rlcSrcMsgHandler(msgDef* msg)
{
	msgId msg_id = 0;

	msg_id = get_msgId(msg);

	switch (msg_id)
	{
		case RRC_RLC_DATA_IND:
		{
			rrc_rlc_data_ind *ind = (rrc_rlc_data_ind *)message_ptr(msg);
			
			handle_src_rrc_data_ind(ind);
			break;
		}
		case MAC_RLC_BUF_STATUS_REQ:
		{
			mac_rlc_buf_status_req *req = (mac_rlc_buf_status_req *)message_ptr(msg);

			handle_src_mac_rlc_buf_status_req(req);
			break;
		}
		case MAC_RLC_DATA_REQ:
		{
			mac_rlc_data_req *req = (mac_rlc_data_req *)message_ptr(msg);

			handle_src_mac_rlc_data_req(req);
			break;
		}
		case MAC_RLC_DATA_RPT:
		{
			mac_rlc_data_rpt *req = (mac_rlc_data_rpt *)message_ptr(msg);

			handle_src_mac_rlc_data_rpt(req);

			break;
		}
		default:
		{
			LOG_ERROR(RLC, "UNknown RLC msg, msgId:%u", msg_id);
			break;
		}
	}
}

