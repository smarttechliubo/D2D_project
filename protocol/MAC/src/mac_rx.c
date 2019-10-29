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
#include "log.h"
#include "mac_header.h"
#include "mac_ue.h"

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

void init_mac_rx()
{

}

void mac_rrc_bcch_received(const frame_t frame, const sub_frame_t subframe)
{
	msgDef msg;
	mac_rrc_bcch_mib_rpt *bcch;
	msgSize msg_size = sizeof(mac_rrc_bcch_mib_rpt);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_BCCH_MIB_RPT;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		bcch = (mac_rrc_bcch_mib_rpt*)msg.data;
		bcch->SFN = frame;
		bcch->subsfn = subframe;
		bcch->mib_receive_flag = 1;

		if (!msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
			LOG_ERROR(MAC, "send bcch fail!");
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
	msgDef msg;
	mac_rrc_ccch_rpt *ind;
	msgSize msg_size = sizeof(mac_rrc_ccch_rpt);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_CCCH_RPT;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		ind = (mac_rrc_ccch_rpt*)msg.data;
		ind->sfn = frame;
		ind->subsfn = subframe;
		ind->data_size = rx_length;
		ind->data_ptr = (uint32_t*)payload;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "[TEST]: new mac message fail!");
	}
}

void mac_rlc_data_ind(const frame_t frame, const sub_frame_t subframe, const uint16_t rx_length, const uint8_t* payload)
{
	msgDef msg;
	mac_rrc_ccch_rpt *ind;
	msgSize msg_size = sizeof(mac_rrc_ccch_rpt);
	msg.data = (uint8_t*)msg_malloc(msg_size);

	if (msg.data != NULL)
	{
		msg.header.msgId = MAC_RRC_CCCH_RPT;
		msg.header.source = MAC_TASK;
		msg.header.destination = RRC_TASK;
		msg.header.msgSize = msg_size;

		ind = (mac_rrc_ccch_rpt*)msg.data;
		ind->sfn = frame;
		ind->subsfn = subframe;
		ind->data_size = rx_length;
		ind->data_ptr = (uint32_t*)payload;

		if (msgSend(RRC_QUEUE, (char *)&msg, sizeof(msgDef)))
		{
		}

		//msg_free(msg);
	}
	else
	{
		LOG_ERROR(MAC, "new mac message fail!");
	}
}

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

void handlePuschSdu(const frame_t frame, const sub_frame_t subframe, const pusch_result result)
{
	uint8_t * payload = result.dataptr;
	//mac_header_info header;
	uint16_t tb_length = result.dataSize;
	uint8_t num_ce = 0;
	uint8_t num_sdu = 0;
	uint8_t rx_ceIds[MAX_NUM_CE];
	uint8_t rx_lcIds[MAX_LOGICCHAN_NUM];
	uint16_t rx_lengths[MAX_LOGICCHAN_NUM];
	uint16_t offset = 0;

	payload = parse_mac_header(payload, &num_ce, &num_sdu, rx_ceIds, rx_lcIds, rx_lengths, tb_length);

	if (payload == NULL)
	{
		LOG_ERROR(MAC, "parse mac header error! rnti:%u",
			result.rnti);
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
				mac_rlc_data_ind(frame, subframe, rx_lengths[i], payload + offset);
				offset = offset + rx_lengths[i];
			}
		}
		else
		{
			LOG_WARN(MAC, "not support now lc Id:%u", rx_lcIds[i]);
		}

	}
}

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

void handleCrcFail(const frame_t frame, const sub_frame_t subframe, const pusch_result result)
{
	rnti_t rnti = result.rnti;
	uint16_t crc = result.crc;

	if (crc != 0)
	{
		LOG_ERROR(MAC, "unknow crc result, frame:%u, subframe:%u, crc:%u", frame, subframe, crc);
		return;
	}

	update_crc_result(rnti, crc);
	
}

void handleCrcOK(const frame_t frame, const sub_frame_t subframe, const pusch_result result)
{
	rnti_t rnti = result.rnti;
	uint16_t crc = result.crc;

	if (crc != 1)
	{
		LOG_ERROR(MAC, "unknow crc result, crc:%u", crc);
		return;
	}

	update_crc_result(rnti, crc);
	handlePuschSdu(frame, subframe, result);
}

void handle_ack(const PHY_ACKInd* ind)
{
	//frame_t frame = ind->frame;
	sub_frame_t subframe = ind->subframe;
	uint32_t num = ind->num;
	rnti_t rnti = INVALID_U16;
	uint16_t ack = 0;

	for (uint32_t i = 0; i < num; i++)
	{
		rnti = ind->ack[i].rnti;
		ack = ind->ack[i].ack;

		update_harq_info(subframe, rnti, ack);
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

void handlePuschReceivedInd(const PHY_PuschReceivedInd *req)
{
	//uint16_t cellId = 0;//g_sch.cellId;//TODO:
	frame_t frame = req->frame;
	sub_frame_t subframe = req->subframe;
	uint16_t num_ue = req->num_ue;
	pusch_result result;

	for (uint32_t i = 0; i < num_ue; i++)
	{
		result = req->result[i];

		if (result.crc == 0) //NACK
		{
			handleCrcFail(frame, subframe, result);
		}
		else
		{
			handleCrcOK(frame, subframe, result);
		}
	}
}

void handlePbchReceivedInd(const PHY_PBCHReceivedInd* ind)
{
	LOG_DEBUG(MAC, "PBCH received, frame:%u, subframe:%u", ind->frame, ind->subframe);

	mac_rrc_bcch_received(ind->frame, ind->subframe);
}

void handle_phy_msg()
{
	msgDef msg;
	uint32_t msg_len = 0;

	while(1)
	{
		msg_len = msgRecv(PHY_MAC_QUEUE, (char *)&msg, MQ_MSGSIZE);

		if (msg_len == 0)
		{
			return;
		}

		switch (msg.header.msgId)
		{
			case PHY_MAC_PBCH_PDU_RPT:
			{
				PHY_PBCHReceivedInd* ind = (PHY_PBCHReceivedInd *)msg.data; //TODO:

				handlePbchReceivedInd(ind);
				msg_free(ind);
				break;
			}
			case PHY_MAC_DECOD_DATA_RPT:
			{
				PHY_PuschReceivedInd* req = (PHY_PuschReceivedInd *)msg.data; //TODO:

				handlePuschReceivedInd(req);
				msg_free(req);
				break;
			}
			case PHY_MAC_ACK_RPT:
			{
				PHY_ACKInd* ind = (PHY_ACKInd *)msg.data; //TODO:

				handle_ack(ind);
				msg_free(ind);
				break;
			}
			case PHY_MAC_CQI_IND:
			{
				PHY_CQIInd* ind = (PHY_CQIInd*) msg.data;

				handle_cqi(ind);
				msg_free(ind);
				break;
			}
			case PHY_MAC_LINK_STATUS_IND:
			{
				PHY_LinkStatusReportInd* ind = (PHY_LinkStatusReportInd*) msg.data;

				handle_linkStatusReport(ind);
				msg_free(ind);
				break;
			}
			default:
			{
				LOG_ERROR(MAC, "unknow PHY msg id:%u", msg.header.msgId);
				break;
			}
		}
	}
}


