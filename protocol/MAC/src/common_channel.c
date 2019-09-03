/**********************************************************
* @file common_channel.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "log.h"
#include "smac_info.h"
#include "smac_schedule_result.h"
#include "mac_vars.h"
#include "mac_cce.h"

void schedule_mib(const frame_t frame, mac_info_s *mac)
{
	common_channel_s *common_channel = &mac->common_channel;
	uint8_t sfn = (uint8_t)((frame>>2)&0xFF);
	uint8_t rb_num = (uint8_t)(common_channel->bch_info.rb_num);
	uint8_t pdcch_index = (uint8_t)(common_channel->bch_info.rb_start_index);

	uint8_t pdcch = (rb_num<<4) | (pdcch_index<<1);
	
	//sfn: 8bit, pdcch: 7bit, padding: 9bit
	common_channel->mib_pdu[0] = sfn;
	common_channel->mib_pdu[1] = pdcch&0xFE;
	common_channel->mib_pdu[2] = 0;
}

void schedule_sib(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
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

void schedule_common(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac)
{
	if ((mac->common_channel.mib_size > 0) && ((subframe == 0) && (frame % 4) == 0))
	{
		schedule_mib(frame, mac);
	}
	if ((mac->common_channel.sib_size > 0) && ((subframe == 1) && (frame % 2) == 0))
	{
		schedule_sib(frame, subframe, mac);
	}
}

