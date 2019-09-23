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
	uint16_t cellId = g_sch.cellId;

	for (uint32_t i = 0; i < ue_num; i++)
	{
		rlc_buffer_rpt buffer = rpt->rlc_buffer_rpt[i];

		if (buffer.rnti == RA_RNTI)// dest
		{
			if (!add_ra(cellId, MAC_SRC))
			{
				LOG_ERROR(MAC, "add new ra ue fail! cellId:%u", cellId);
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

}
