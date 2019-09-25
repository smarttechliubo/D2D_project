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

#include "messageDefine.h"//MAC_TEST
#include "msg_queue.h"

void init_mac_rx()
{

}

void mac_rrc_data_ind(const frame_t frame, const sub_frame_t subframe, const pusch_result result)
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
		ind->data_size = result.dataSize;
		ind->data_ptr = (uint32_t*)result.dataptr;

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
				} else 
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

void handlePuschSdu(const frame_t frame, const sub_frame_t subframe, const uint16_t cellId, const pusch_result result)
{
	uint8_t * payload = result.dataptr;
	//mac_header_info header;
	uint16_t tb_length = result.dataSize;
	uint8_t num_ce = 0;
	uint8_t num_sdu = 0;
	uint8_t rx_ceIds[MAX_NUM_CE];
	uint8_t rx_lcIds[MAX_LOGICCHAN_NUM];
	uint16_t rx_lengths[MAX_LOGICCHAN_NUM];

	payload = parse_mac_header(payload, &num_ce, &num_sdu, rx_ceIds, rx_lcIds, rx_lengths, tb_length);

	if (payload == NULL)
	{
		LOG_ERROR(MAC, "parse mac header error! cellId:%u, rnti:%u",cellId, result.rnti);
		return;
	}

#if 0 //TODO: mac ce handler
	for (uint32_t i = 0; i < num_ce; i++)
	{
		switch (rx_ceIds[i])
		{
			case 
		}
	}
#endif
	for (uint32_t i = 0; i < num_sdu; i++)
	{
		switch (rx_lcIds[i])
		{
			case CCCH_:
			{
				if(result.rnti == RA_RNTI)//source ue
				{
					if (!add_ra(cellId, MAC_SRC))
					{
						LOG_ERROR(MAC, "add new ra ue fail! cellId:%u", cellId);
					}

					mac_rrc_data_ind(frame, subframe, result);
					//return;
				}
				break;
			}
			default:
				break;
			}

	}
}

void handleCrcFail(const pusch_result result)
{

}

void handlePuschReceivedcInd(const PHY_PuschReceivedInd *req)
{
	uint16_t cellId = 0;//g_sch.cellId;//TODO:
	frame_t frame = req->frame;
	sub_frame_t subframe = req->subframe;
	uint16_t num_ue = req->num_ue;
	pusch_result result;

	for (uint32_t i = 0; i < num_ue; i++)
	{
		result = req->result[i];
		if (result.crc == 1)//ACK
		{
			handleCrcFail(result);
		}
		else
		{
			handlePuschSdu(cellId, frame, subframe, result);
		}
	}
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
				PHY_PuschReceivedInd *req = (PHY_PuschReceivedInd *)msg.data; //TODO:

				handlePuschReceivedcInd(req);
				msg_free(req);
				break;
			}
			case PHY_MAC_DECOD_DATA_RPT:
			{
				break;
			}
			case PHY_MAC_ACK_RPT:
			{
				break;
			}
			case PHY_MAC_CQI_IND:
			{
				break;
			}
			case PHY_MAC_LINK_STATUS_IND:
			{
				break;
			}
			default:
				break;
		}
	}
}


