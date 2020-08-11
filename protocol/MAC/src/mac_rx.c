/**********************************************************
* @file mac_rx.c
* @brief  mac rx function
*
* @author	guicheng.liu
* @date 	2019/07/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#include "typedef.h"
#include "interface_mac_phy.h"
#include "interface_rrc_mac.h"
#include "d2d_message_type.h"
#include "mac_ra.h"
#include "mac_vars.h"
#include "log.h"
#include "mac_header.h"
#include "mac_ue.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_handler.h"
<<<<<<< HEAD
=======
#include "osp_ex.h"
>>>>>>> master

void init_mac_rx()
{

}

void mac_rrc_bcch_received(const frame_t frame, const sub_frame_t subframe)
{
	msgDef* msg = NULL;
	mac_rrc_bcch_mib_rpt *bcch;
	msgSize msg_size = sizeof(mac_rrc_bcch_mib_rpt);

	msg = new_message(MAC_RRC_BCCH_MIB_RPT, TASK_D2D_MAC_SCH, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{
		bcch = (mac_rrc_bcch_mib_rpt*)message_ptr(msg);
		bcch->SFN = frame;
		bcch->subsfn = subframe;
		bcch->mib_receive_flag = 1;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_BCCH_MIB_RPT send");
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "new mac message fail!");
	}
}

void mac_rrc_data_ind(const frame_t frame, const sub_frame_t subframe, const uint16_t rx_length, const uint8_t* payload)
{
	msgDef* msg = NULL;
	mac_rrc_ccch_rpt *ind;
	msgSize msg_size = sizeof(mac_rrc_ccch_rpt);

	msg = new_message(MAC_RRC_CCCH_RPT, TASK_D2D_MAC_SCH, TASK_D2D_RRC, msg_size);

	if (msg != NULL)
	{
		ind = (mac_rrc_ccch_rpt*)message_ptr(msg);
		ind->sfn = frame;
		ind->subsfn = subframe;
		ind->data_size = rx_length;
		ind->data_ptr = (uint32_t*)payload;

		if (message_send(TASK_D2D_RRC, msg, sizeof(msgDef)))
		{
			LOG_INFO(MAC, "LGC: MAC_RRC_CCCH_RPT send");
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "mac_rrc_data_ind, new mac message fail!");
	}
}

void fill_rlc_data_ind(const uint16_t rx_length, 
	const uint8_t rx_lcId,
<<<<<<< HEAD
	const uint8_t* payload,
=======
	const uint32_t offset,
>>>>>>> master
	mac_rlc_data_info *ind)
{
	ind->logicchannel_id[ind->logic_chan_num] = rx_lcId;
	ind->mac_pdu_size[ind->logic_chan_num] = rx_length;
<<<<<<< HEAD
	ind->mac_pdu_buffer_ptr[ind->logic_chan_num] = (uint32_t*)payload;
	ind->logic_chan_num++;
}

void mac_rlc_data_ind(msgDef* msg)
{
}

=======
	ind->logic_chan_data_offset[ind->logic_chan_num] = offset;
	ind->logic_chan_num++;
}

>>>>>>> master
uint8_t *parse_mac_header(uint8_t *mac_header,
                                  uint8_t *num_ce,
                                  uint8_t *num_sdu,
                                  uint8_t *rx_ces,
                                  uint8_t *rx_lcids,
                                  uint16_t *rx_lengths,
                                  uint16_t tb_length) 
{
	bool not_done = true;
	uint8_t num_ces = 0;
	uint8_t num_sdus = 0;
	uint8_t lcid;
	uint8_t num_sdu_cnt;
	uint8_t *mac_header_ptr = mac_header;
	uint16_t length, ce_len = 0;

<<<<<<< HEAD
	LOG_INFO(MAC, "parse_mac_header, mac_header:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",
=======
	LOG_ERROR(MAC, "parse_mac_header, mac_header:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",
>>>>>>> master
		mac_header_ptr[0],mac_header_ptr[1],mac_header_ptr[2],
		mac_header_ptr[3],mac_header_ptr[4],mac_header_ptr[5],
		mac_header_ptr[6],mac_header_ptr[7],mac_header_ptr[8],mac_header_ptr[9]);

	while (not_done)
	{
		if (((mac_header_fixed *) mac_header_ptr)->E == 0)
		{
			not_done = false;
		}

		lcid = ((mac_header_fixed *) mac_header_ptr)->LCID;

		if (lcid < MAX_LCID) // MAC logical channel
		{
			if (not_done == false) // last MAC SDU, length is implicit
			{
				mac_header_ptr++;
				length = tb_length - (mac_header_ptr - mac_header) - ce_len;

				for (num_sdu_cnt = 0; num_sdu_cnt < num_sdus;num_sdu_cnt++)
				{
					length -= rx_lengths[num_sdu_cnt];
				}
			} 
			else 
			{
				if (((mac_header_short *) mac_header_ptr)->F == 0)
				{
					length = ((mac_header_short *) mac_header_ptr)->L;
					mac_header_ptr += 2; 
				} 
				else
				{  // F = 1
					length =((((mac_header_long *) mac_header_ptr)->L_MSB &	0x7f) << 8) | (((mac_header_long *)mac_header_ptr)->L_LSB & 0xff);
					mac_header_ptr += 3;  //sizeof(SCH_SUBHEADER_LONG);
				}
			}

			rx_lcids[num_sdus] = lcid;
			rx_lengths[num_sdus] = length;
			num_sdus++;
		}
		else //control element subheader POWER_HEADROOM, BSR and CRNTI
		{
			if (lcid == SHORT_PADDING)
			{
				mac_header_ptr++;
			}
#if 0 // not support, TODO: add mac ce here
			else
			{
				rx_ces[num_ces] = lcid;
				num_ces++;
				mac_header_ptr++;
				if (lcid == LONG_BSR)
				{
					ce_len += 3;
				}
				else if (lcid == CRNTI)
				{
					ce_len += 2;
				} 
				else if ((lcid == POWER_HEADROOM)
					|| (lcid == TRUNCATED_BSR)
					|| (lcid == SHORT_BSR))
				{
					ce_len++;
				}
				else
				{
					return NULL;
				}
			}
#endif
		}
	}

	*num_ce = num_ces;
	*num_sdu = num_sdus;

	return (mac_header_ptr);
}

//mac ce not support now.
void handle_mac_ce(const uint8_t num_ce, const uint8_t rx_ceIds[MAX_NUM_CE])
{
	for (uint32_t i = 0; i < num_ce; i++)
	{
		switch (rx_ceIds[i])
		{
			case PHR:
			case CRNTI:
			case CONRES:
			case TA:
			{
				break;
			}
			default:
				break;
		}
	}
}
<<<<<<< HEAD

=======
#if 0
>>>>>>> master
void handlePuschSdu(const frame_t frame, const sub_frame_t subframe, 
	const pusch_result* result, mac_rlc_data_rpt *rpt)
{
	uint8_t * payload = result->dataptr;
	//mac_header_info header;
	uint16_t tb_length = result->dataSize;
	uint8_t num_ce = 0;
	uint8_t num_sdu = 0;
	uint8_t rx_ceIds[MAX_NUM_CE];
	uint8_t rx_lcIds[MAX_LOGICCHAN_NUM];
	uint16_t rx_lengths[MAX_LOGICCHAN_NUM];
	uint16_t offset = 0;
	mac_rlc_data_info* data_ind = &rpt->sdu_data_rpt[rpt->ue_num];
	bool hasData = false;

	payload = parse_mac_header(payload, &num_ce, &num_sdu, rx_ceIds, rx_lcIds, rx_lengths, tb_length);

	if (payload == NULL)
	{
		LOG_ERROR(MAC, "parse mac header error! rnti:%u",
			result->rnti);
		return;
	}

	for (uint32_t i = 0; i < num_sdu; i++)
	{
		if (rx_lcIds[i] < MAX_LCID)
		{
			if (rx_lcIds[i] == CCCH_)
			{
#if 0
				if(result.rnti == RA_RNTI)//source ue
				{
					if (!add_ra(cellId, EMAC_SRC))
					{
						LOG_ERROR(MAC, "add new ra ue fail! cellId:%u", cellId);
					}

					//return;
				}
				else if(update_ra_state(result.rnti))
				{

				}
#endif
				//CCCH
				mac_rrc_data_ind(frame, subframe, rx_lengths[i], payload + offset);
				offset = offset + rx_lengths[i];
			}
			else
			{
				//DTCH
				//mac_rlc_data_ind(frame, subframe, rx_lengths[i], payload + offset, rpt);
				fill_rlc_data_ind(rx_lengths[i], rx_lcIds[i], payload, data_ind);
				offset = offset + rx_lengths[i];
				hasData = true;
			}
		}
		else
		{
			LOG_WARN(MAC, "not support now lc Id:%u", rx_lcIds[i]);
		}

		if (hasData)
		{
			rpt->ue_num++;
		}
	}
}
<<<<<<< HEAD

=======
#endif
>>>>>>> master
void handle_cqi(const PHY_CQIInd* ind)
{
	frame_t frame = ind->frame;
	sub_frame_t subframe = ind->subframe;
	uint16_t num = ind->num;
	uint16_t cqi = 0;
	rnti_t rnti = INVALID_U16;

	for (uint32_t i = 0; i < num; i++)
	{
		cqi = ind->cqiInfo[i].rnti;
		rnti = ind->cqiInfo[i].rnti;

		if (!update_ue_cqi(rnti, cqi))
		{
			LOG_WARN(MAC, "handle cqi fail, frame:%u, subframe:%u", frame, subframe);
		}
	}
}
/*
void handleCrcFail(const frame_t frame, const sub_frame_t subframe, const pusch_result* result)
{
	rnti_t rnti = result->rnti;
	uint16_t crc = result->crc;

	if (crc != 0)
	{
		LOG_ERROR(MAC, "unknow crc result, frame:%u, subframe:%u, crc:%u", frame, subframe, crc);
		return;
	}

	update_crc_result(rnti, crc);
	
}

void handleCrcOK(const frame_t frame, const sub_frame_t subframe, 
	const pusch_result* result, mac_rlc_data_rpt *rpt)
{
	rnti_t rnti = result->rnti;
	uint16_t crc = result->crc;

	if (crc != 1)
	{
		LOG_ERROR(MAC, "unknow crc result, crc:%u", crc);
		return;
	}

	update_crc_result(rnti, crc);
	handlePuschSdu(frame, subframe, result, rpt);
}
*/
<<<<<<< HEAD
=======


>>>>>>> master
void handle_ack(const PHY_ACKInd* ind)
{
	//frame_t frame = ind->frame;
	sub_frame_t subframe = ind->subframe;
	uint32_t num = ind->num;
	rnti_t rnti = INVALID_U16;
<<<<<<< HEAD
	uint16_t ack = 0;
=======
	uint8_t ack_num = 0;
	uint8_t ack_bits;//0:NACK, 1:ACK
>>>>>>> master

	for (uint32_t i = 0; i < num; i++)
	{
		rnti = ind->ack[i].rnti;
<<<<<<< HEAD
		ack = ind->ack[i].ack;

		update_harq_info(subframe, rnti, ack);
=======
		ack_num = ind->ack[i].ack_num;
		ack_bits = ind->ack[i].ack_bits;

		handle_ack_result(rnti, ack_num, ack_bits);

		//update_harq_info(subframe, rnti, ack);
>>>>>>> master
	}
}

void handle_linkStatusReport(const PHY_LinkStatusReportInd* ind)
{
	uint32_t num = ind->num;
	rnti_t rnti = 0;
	uint16_t status = 0;

	for (uint32_t i = 0; i < num; i++)
	{
		rnti = ind->status[i].rnti;
		status = ind->status[i].status;

		update_ue_status(rnti, status);
	}
}

void handlePuschReceivedInd(PHY_PuschReceivedInd *pusch)
{
	//uint16_t cellId = 0;//g_sch.cellId;//TODO:
	frame_t frame = pusch->frame;
	sub_frame_t subframe = pusch->subframe;
	//uint16_t ueIndex = 0;
	rnti_t rnti = 0;
	uint16_t crc = 0;
	uint16_t num_ue = pusch->num_ue;
	pusch_result* result = NULL;
	uint8_t * payload = NULL;
<<<<<<< HEAD
=======
	uint8_t * data = NULL;
>>>>>>> master
	//mac_header_info header;
	uint16_t tb_length = 0;
	uint8_t num_ce = 0;
	uint8_t num_sdu = 0;
	uint8_t rx_ceIds[MAX_NUM_CE];
	uint8_t rx_lcIds[MAX_LOGICCHAN_NUM];
	uint16_t rx_lengths[MAX_LOGICCHAN_NUM];
<<<<<<< HEAD
	uint16_t offset = 0;
=======
	uint32_t offset = 0;
>>>>>>> master
	mac_rlc_data_info* data_ind = NULL;
	bool hasData = false;

	msgDef* msg = NULL;
	msgSize msg_size = sizeof(mac_rlc_data_rpt);
	mac_rlc_data_rpt *rpt = NULL;

<<<<<<< HEAD
	msg = new_message(MAC_RLC_DATA_RPT, TASK_D2D_MAC_SCH, TASK_D2D_RLC, msg_size);
=======
	msg = new_message(MAC_RLC_DATA_RPT, TASK_D2D_MAC_SCH, TASK_D2D_RLC_RX, msg_size);
>>>>>>> master

	if (msg != NULL)
	{
		rpt = (mac_rlc_data_rpt *)message_ptr(msg);
	}
	else
	{
		LOG_ERROR(MAC, "NEW msg fail");
		return;
	}

	rpt->ue_num = 0;

	for (uint32_t i = 0; i < num_ue; i++)
	{
		result = &pusch->result[i];
		rnti = result->rnti;
		crc = result->crc;
<<<<<<< HEAD
		payload = result->dataptr;
		tb_length = result->dataSize;

=======

		if (result->buffer_id != 0 && result->buffer_id != 1)
		{
			LOG_ERROR(MAC, "pusch buffer id error! buffer_id:%u",result->buffer_id);
			return ;
		}

		payload = (uint8_t *)OspGetApeRDateAddr(result->buffer_id);

		tb_length = result->dataSize;

		data_ind = &rpt->sdu_data_rpt[rpt->ue_num];

		data_ind->logic_chan_num = 0;
		data_ind->ue_tb_size = tb_length;
		data_ind->mac_pdu_buffer_ptr = (uint32_t *)payload;

		handle_crc_result(subframe, rnti, crc);
#if 0
>>>>>>> master
		if (rnti == RA_RNTI)
		{
			if(crc == 0)
			{
				LOG_WARN(MAC, "ue rnti:%u attach fail!", rnti);
				//update_temp_ue_crc_result(subframe, rnti, crc);

				continue; // no retx for msg1
			}
		}
		else
		{
			update_crc_result(subframe, rnti, crc);
		}

		if (crc == 0)//crc = NACK
		{
			continue;
		}
<<<<<<< HEAD
	
		payload = parse_mac_header(payload, &num_ce, &num_sdu, rx_ceIds, rx_lcIds, rx_lengths, tb_length);
	
		if (payload == NULL)
=======
#endif
		data = parse_mac_header(payload, &num_ce, &num_sdu, rx_ceIds, rx_lcIds, rx_lengths, tb_length);
	
		if (data == NULL)
>>>>>>> master
		{
			LOG_ERROR(MAC, "parse mac header error! rnti:%u",
				result->rnti);
			return;
		}
<<<<<<< HEAD
	
		data_ind = &rpt->sdu_data_rpt[rpt->ue_num];

		data_ind->logic_chan_num = 0;
=======

		offset = data - payload;
>>>>>>> master

		for (uint32_t i = 0; i < num_sdu; i++)
		{
			if (rx_lcIds[i] < MAX_LCID)
			{
				if (rx_lcIds[i] == CCCH_)
				{
					//CCCH
<<<<<<< HEAD
					mac_rrc_data_ind(frame, subframe, rx_lengths[i], payload + offset);
=======
					mac_rrc_data_ind(frame, subframe, rx_lengths[i], data + offset);
>>>>>>> master
					offset = offset + rx_lengths[i];
				}
				else
				{
					//DTCH
<<<<<<< HEAD
					fill_rlc_data_ind(rx_lengths[i], rx_lcIds[i], payload + offset, data_ind);
=======
					fill_rlc_data_ind(rx_lengths[i], rx_lcIds[i], offset, data_ind);
>>>>>>> master
					offset = offset + rx_lengths[i];
					hasData = true;
				}
			}
			else
			{
				LOG_ERROR(MAC, "not support now lc Id:%u", rx_lcIds[i]);
			}
	
		}

		if (hasData)
		{
			hasData = false;
			rpt->sdu_data_rpt[rpt->ue_num].valid_flag = 1;
			rpt->sdu_data_rpt[rpt->ue_num].rnti = rnti;
			rpt->ue_num++;
		}

	}

	if (rpt->ue_num > 0)
	{
<<<<<<< HEAD
		if (message_send(TASK_D2D_RLC, msg, sizeof(msgDef)))
=======
		if (message_send(TASK_D2D_RLC_RX, msg, sizeof(msgDef)))
>>>>>>> master
		{
			LOG_INFO(MAC, "LGC: MAC_RLC_DATA_RPT send, ue_num:%u", rpt->ue_num);
		}
		//mac_rlc_data_ind(msg);
	}
	else
	{
		message_free(msg);
	}
}

void handlePbchReceivedInd(const PHY_PBCHReceivedInd* ind)
{
	LOG_DEBUG(MAC, "PBCH received, frame:%u, subframe:%u", ind->frame, ind->subframe);

	mac_rrc_bcch_received(ind->frame, ind->subframe);
}

void handle_phy_msg(msgDef* msg)
{
	msgId msgid = get_msgId(msg);

	switch (msgid)
	{
		case PHY_MAC_PBCH_PDU_RPT:
		{
			PHY_PBCHReceivedInd* ind = (PHY_PBCHReceivedInd *)message_ptr(msg); //TODO:

			handlePbchReceivedInd(ind);
			break;
		}
		case PHY_MAC_DECOD_DATA_RPT:
		{
<<<<<<< HEAD
=======
			LOG_INFO(MAC, "PHY_MAC_DECOD_DATA_RPT");
>>>>>>> master
			PHY_PuschReceivedInd* req = (PHY_PuschReceivedInd *)message_ptr(msg); //TODO:

			handlePuschReceivedInd(req);
			break;
		}
		case PHY_MAC_ACK_RPT:
		{
			PHY_ACKInd* ind = (PHY_ACKInd *)message_ptr(msg); //TODO:

			handle_ack(ind);
			break;
		}
		case PHY_MAC_CQI_IND:
		{
			PHY_CQIInd* ind = (PHY_CQIInd*) message_ptr(msg);

			handle_cqi(ind);
			break;
		}
		case PHY_MAC_LINK_STATUS_IND:
		{
			PHY_LinkStatusReportInd* ind = (PHY_LinkStatusReportInd*) message_ptr(msg);

			handle_linkStatusReport(ind);
			break;
		}
		default:
		{
			LOG_ERROR(MAC, "unknow PHY msg id:%u", get_msgId(msg));
			break;
		}
	}
}



