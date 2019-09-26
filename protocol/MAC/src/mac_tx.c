/**********************************************************
* @file mac_tx.c
* @brief  mac tx function
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "d2d_message_type.h"
#include "mac_defs.h"
#include "mac.h"
#include "mac_vars.h"
#include "mac_ra.h"
#include "log.h"
#include "interface_mac_rlc.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

void init_mac_tx(uint16_t cellId)
{
	g_sch.cellId = cellId;
	g_sch.frame = INVALID_U32;
	g_sch.subframe = INVALID_U32;
}

void handle_buffer_status(const frame_t frame, const sub_frame_t subframe, const rlc_mac_buf_status_rpt *rpt)
{
	uint32_t ue_num = rpt->valid_ue_num; 
	rlc_buffer_rpt buffer;
	//uint16_t cellId = g_sch.cellId;
	uint8_t logic_chan_num = 0;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		buffer = rpt->rlc_buffer_rpt[i];
		logic_chan_num = buffer.logic_chan_num;

		for(uint8_t i = 0; i < logic_chan_num; i++)
		{
			if (buffer.logicchannel_id[i] == CCCH_)// TODO: no data should be transmit before ue get connection setup.
			{
				update_ra_buffer(buffer);
				break;
			}
		}
	}
}

void handle_buffer_status_ind(const frame_t frame, const sub_frame_t subframe)
{
	//MAC_TEST
	msgDef msg;
	uint32_t msg_len = 0;
	//while (1)
	{
		msg_len = msgRecv(RLC_MAC_QUEUE, (char *)&msg, MQ_MSGSIZE);

		if (msg_len == 0)
		{
			return;
		}

		switch (msg.header.msgId)
		{
			case RLC_MAC_BUF_STATUS_RPT:
			{
				rlc_mac_buf_status_rpt *rpt = (rlc_mac_buf_status_rpt *)msg.data;

				handle_buffer_status(frame, subframe, rpt);
				msg_free(rpt);
				break;
			}
		}
	}
}

void mac_scheduler()
{
	frame_t frame = g_sch.frame;
	sub_frame_t subframe = g_sch.subframe;

	handle_buffer_status_ind(frame, subframe);

	schedule_ra(frame, subframe);
}
#if 0
void resource_alloc()
{
	mac_info_s *mac = g_context.mac;
	common_channel_s *common_channel = &mac->common_channel;
	mac_tx_req *tx_req = &g_sch.tx_req;
	uint32_t sib_len = common_channel->sib_size;
	uint32_t rb_max = get_rb_num(mac->bandwith);
	uint32_t rb_start_index = get_rb_start(mac->bandwith);
	uint32_t rbg_size = get_rbg_size(mac->bandwith);
	uint32_t rbs_req = 0;
	uint16_t aggregation_level = 2;
	int32_t cce_offset = -1;

	uint8_t mcs = 2; // for sib
	uint32_t tbs = get_tbs(mcs, rbg_size);
	
	while (tbs < sib_len)
	{
	  rbs_req += rbg_size;
	
	  if (rbs_req > mac->max_rbs_per_ue || rbs_req > rb_max) 
	  {
		rbs_req = MIN(mac->max_rbs_per_ue,rb_max);
		tbs = get_tbs(mcs, rbs_req);
		break;
	  }
	  tbs = get_tbs(mcs, rbs_req);
	} // end of while

	tx_req->tx_info[tx_req->num_tx].sch.rb_start = rb_start_index;
	tx_req->tx_info[tx_req->num_tx].sch.rb_num = rbs_req;
	tx_req->tx_info[tx_req->num_tx].sch.modulation = 2;//QPSK
	tx_req->tx_info[tx_req->num_tx].sch.rv = 0;
	tx_req->tx_info[tx_req->num_tx].sch.data_ind = 2;
	tx_req->tx_info[tx_req->num_tx].sch.ack = INVALID_U8;
	tx_req->tx_info[tx_req->num_tx].sch.pdu_len = sib_len;
	tx_req->tx_info[tx_req->num_tx].sch.data = common_channel->sib_pdu;

	cce_offset = allocate_CCE(aggregation_level);

	if (cce_offset >= 0)
	{
		tx_req->tx_info[tx_req->num_tx].rnti = SI_RNTI;
		tx_req->tx_info[tx_req->num_tx].ueIndex = INVALID_U16;

		tx_req->tx_info[tx_req->num_tx].dci.cce_rb_num = aggregation_level;
		tx_req->tx_info[tx_req->num_tx].dci.cce_rb = cce_offset;
		tx_req->tx_info[tx_req->num_tx].dci.rb_num = rbs_req;
		tx_req->tx_info[tx_req->num_tx].dci.rb_start = rb_start_index;
		tx_req->tx_info[tx_req->num_tx].dci.mcs = mcs;
		tx_req->tx_info[tx_req->num_tx].dci.data_ind = 2;
		tx_req->tx_info[tx_req->num_tx].dci.ndi = 0;
		tx_req->tx_info[tx_req->num_tx].dci.rv = 0;
		tx_req->num_tx++;
	}
	else
	{
		LOG_WARN(MAC, "No CCE Resoure for SIB SFN:%u", frame*4+subframe);
	}

}
#endif
