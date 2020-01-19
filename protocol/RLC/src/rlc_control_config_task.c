/******************************************************************
 * @file  rlc_config_task.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月5日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月5日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>
#include <intertask_interface.h>
#include <d2d_message_type.h>
#include <rrc_global_def.h>
#include <pthread.h>
#include <osp_ex.h>

rlc_op_status_t rrc_rlc_remove_rlc   (
  const protocol_ctxt_t* const ctxt_pP,
  const srb_flag_t  srb_flagP,
  const MBMS_flag_t MBMS_flagP,
  const rb_id_t     rb_idP)
{
  //-----------------------------------------------------------------------------
  logical_chan_id_t      lcid            = 0;
  hash_key_t             key             = HASHTABLE_NOT_A_KEY_VALUE;
  hashtable_rc_t         h_rc;
  hash_key_t             key_lcid        = HASHTABLE_NOT_A_KEY_VALUE;
  hashtable_rc_t         h_lcid_rc;
  rlc_union_t           *rlc_union_p = NULL;

  /* for no gcc warnings */
  (void)lcid;


    key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
  

  //!生成key,从hash table 中获取rlc union 
  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

  if (h_rc == HASH_TABLE_OK) {
    // also remove the hash-key created by LC-id
    switch (rlc_union_p->mode) {
#ifdef AM_ENABLE
    case RLC_MODE_AM:
      lcid = rlc_union_p->rlc.am.channel_id;
      break;
#endif
    case RLC_MODE_UM:
      lcid = rlc_union_p->rlc.um.channel_id;
      break;
    case RLC_MODE_TM:
      lcid = rlc_union_p->rlc.tm.channel_id;
      break;
    default:
      LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"[%s %u] RLC mode is unknown!\n",
            PROTOCOL_CTXT_ARGS(ctxt_pP),
            (srb_flagP) ? "SRB" : "DRB",
            rb_idP);
    }
    key_lcid = RLC_COLL_KEY_LCID_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, lcid, srb_flagP);
    h_lcid_rc = hashtable_get(rlc_coll_p, key_lcid, (void**)&rlc_union_p);
  } else {
    h_lcid_rc = HASH_TABLE_KEY_NOT_EXISTS;
  }

  if ((h_rc == HASH_TABLE_OK) && (h_lcid_rc == HASH_TABLE_OK)) {
  	//!从hashtable中remove 节点
    h_lcid_rc = hashtable_remove(rlc_coll_p, key_lcid);
    h_rc = hashtable_remove(rlc_coll_p, key);
    LOG_DEBUG(RLC, PROTOCOL_CTXT_FMT"[%s %u LCID %d] RELEASED %s\n",
          PROTOCOL_CTXT_ARGS(ctxt_pP),
          (srb_flagP) ? "SRB" : "DRB",
          rb_idP, lcid,
          (srb_flagP) ? "SRB" : "DRB");
  } else if ((h_rc == HASH_TABLE_KEY_NOT_EXISTS) || (h_lcid_rc == HASH_TABLE_KEY_NOT_EXISTS)) {
    LOG_DEBUG(RLC, PROTOCOL_CTXT_FMT"[%s %u LCID %d] RELEASE : RLC NOT FOUND %s, by RB-ID=%d, by LC-ID=%d\n",
          PROTOCOL_CTXT_ARGS(ctxt_pP),
          (srb_flagP) ? "SRB" : "DRB",
          rb_idP, lcid,
          (srb_flagP) ? "SRB" : "DRB",
          h_rc, h_lcid_rc);
  } else {
    LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"[%s %u LCID %d] RELEASE : INTERNAL ERROR %s\n",
          PROTOCOL_CTXT_ARGS(ctxt_pP),
          (srb_flagP) ? "SRB" : "DRB",
          rb_idP, lcid,
          (srb_flagP) ? "SRB" : "DRB");
  }

  return RLC_OP_STATUS_OK;
}
//----------------------------------


rlc_union_t *rrc_rlc_add_rlc   (const protocol_ctxt_t* const ctxt_pP,
									const srb_flag_t        srb_flagP,
									const MBMS_flag_t       MBMS_flagP,
									const rb_id_t           rb_idP,
									const logical_chan_id_t chan_idP,
									const rlc_mode_e        rlc_modeP)
{
	hashtable_rc_t         h_lcid_rc;
	hashtable_rc_t         h_rc;
	rlc_union_t            *rlc_union_p = NULL;
	hash_key_t             key         = HASHTABLE_NOT_A_KEY_VALUE;
	hash_key_t             key_lcid    = HASHTABLE_NOT_A_KEY_VALUE;

	if(chan_idP >= MAX_LOGICCHAN_NUM) {
		LOG_ERROR(RLC, "LC id is too high (%u/%d)!\n", chan_idP, MAX_LOGICCHAN_NUM);
		return NULL;
	}

	key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
	key_lcid = RLC_COLL_KEY_LCID_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, chan_idP, srb_flagP);


	h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

	if ( HASH_TABLE_OK == h_rc) {
		LOG_WARN(RLC, PROTOCOL_CTXT_FMT"[%s %u] rrc_rlc_add_rlc,rlc_union_p already exist %s\n",
					PROTOCOL_CTXT_ARGS(ctxt_pP),
				  (srb_flagP) ? "SRB" : "DRB",
					rb_idP);
				
		      
		AssertFatal(rlc_union_p->mode == rlc_modeP,RLC, "Error rrc_rlc_add_rlc , already exist but RLC mode differ");

		return rlc_union_p;
	} 
	else if (h_rc == HASH_TABLE_KEY_NOT_EXISTS) {
		rlc_union_p = calloc(1, sizeof(rlc_union_t));
		//! 在RRC 信令中，当需要增加RLC 实体时，进行hashtable 的insert, 使得插入key对应的节点
		pthread_mutex_init(&rlc_union_p->rlc_union_mtex ,NULL);
		
		h_rc = hashtable_insert(rlc_coll_p, key, rlc_union_p);  //!rb_id生成的h_rc
		h_lcid_rc = hashtable_insert(rlc_coll_p, key_lcid, rlc_union_p); //！logic chid 生成的h_lcid_rc

		if ((h_rc == HASH_TABLE_OK) && (h_lcid_rc == HASH_TABLE_OK)) {  //!<如果插入成功
		  //!<打印log
			LOG_WARN(RLC, PROTOCOL_CTXT_FMT"[%s %u] rrc_rlc_add_rlc:key =%lld,lc_key=%lld, insert rlc entity\n",
			      PROTOCOL_CTXT_ARGS(ctxt_pP),
			      (srb_flagP) ? "SRB" : "DRB",
			      rb_idP,
			      key,
			      key_lcid);

		      rlc_union_p->mode = rlc_modeP; 	//!<更新mode
		      
		      return rlc_union_p;
		}
		else {
		      LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"[%s %u] rrc_rlc_add_rlc FAILED (add by RB_id=%d,add by LC_id=%d)\n",
		            PROTOCOL_CTXT_ARGS(ctxt_pP),
		            (srb_flagP) ? "SRB" : "DRB",
		            rb_idP,
		            rb_idP,
					chan_idP);
		      free(rlc_union_p);  //!< 如果插入到hash表中失败时，则将其free掉。
		      rlc_union_p = NULL;
		      return NULL;
	    }
	}
	else {
	    LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"%s %u rrc_rlc_add_rlc , INTERNAL ERROR %s\n",
	          PROTOCOL_CTXT_ARGS(ctxt_pP),
	          (srb_flagP) ? "SRB" : "DRB",
	          rb_idP);
  }
  return NULL;
}



rlc_op_status_t rrc_rlc_config_req	 (const protocol_ctxt_t* const ctxt_pP,
											const srb_flag_t		srb_flagP,
											const MBMS_flag_t	mbms_flagP,
											const config_action_t actionP,
											const rb_id_t		rb_idP,
											const logical_chan_id_t logic_ch_id,
											const rlc_info_t		rlc_infoP)
{
	//-----------------------------------------------------------------------------
	//rlc_op_status_t status;
	switch (actionP) {

		case CONFIG_ACTION_ADD:
		{
			
			LOG_INFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] ADD RLC MODE: %s\n",
						   PROTOCOL_CTXT_ARGS(ctxt_pP),
						   rb_idP,
						   g_rlc_mode_str[ rlc_infoP.rlc_mode]);
			rrc_rlc_add_rlc(ctxt_pP, srb_flagP, MBMS_FLAG_NO, rb_idP, logic_ch_id , rlc_infoP.rlc_mode); 
			break; 
		}
	// no break, fall to next case
	case CONFIG_ACTION_MODIFY:
	{
		switch (rlc_infoP.rlc_mode) {
	#ifdef AM_ENABLE 
			case RLC_MODE_AM:
			  LOG_INFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] MODIFY RB AM\n",
					PROTOCOL_CTXT_ARGS(ctxt_pP),
					rb_idP);

			  config_req_rlc_am(
				ctxt_pP,
				srb_flagP,
				&rlc_infoP.rlc.rlc_am_info,
				rb_idP, rb_idP);
			  break;
   #endif 
			case RLC_MODE_UM:
			  LOG_INFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] MODIFY RB UM\n",
					PROTOCOL_CTXT_ARGS(ctxt_pP),
					rb_idP);
			  config_req_rlc_um(
				ctxt_pP,
				srb_flagP,
				&rlc_infoP.rlc.rlc_um_info,
				rb_idP, logic_ch_id);
			  break;

			case RLC_MODE_TM:
			  LOG_INFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] MODIFY RB TM\n",
					PROTOCOL_CTXT_ARGS(ctxt_pP),
					rb_idP);
			  config_req_rlc_tm(
					ctxt_pP,
					srb_flagP,
					&rlc_infoP.rlc.rlc_tm_info,
					rb_idP, logic_ch_id);
			  break;

			default:
			  return RLC_OP_STATUS_BAD_PARAMETER;
			}

		break;
	}

	case CONFIG_ACTION_REMOVE:
	{
		return rrc_rlc_remove_rlc(ctxt_pP, srb_flagP, mbms_flagP, rb_idP);
		break;
	}
	default:
		return RLC_OP_STATUS_BAD_PARAMETER;
	}

	return RLC_OP_STATUS_OK;
}





void  rlc_rrc_config_process(void *message, MessagesIds         msg_type)
{

	rrc_rlc_initial_req  *intial_req; 
	rrc_rlc_bcch_para_cfg  *bcch_cfg_req; 
	rrc_rlc_connect_setup_cfg  *connect_req; 
	rlc_info_t     temp_rlc_info; 
	uint32_t       rb_index;  
	rb_id_t        rb_id; 
	logical_chan_id_t  logic_ch_id; 
	uint16_t         dl_sn_field; 
	uint16_t         dl_t_recorder; 
	uint16_t         ul_sn_field; 


	LOG_INFO(RLC, "RLC_RRC_TASK receive message = %d\n",msg_type);
	switch (msg_type)
	{
		
		case RRC_RLC_INITIAL_REQ: 
		{
			intial_req = (rrc_rlc_initial_req * )message;
				
            //! constitue  protocol_ctxt_t; 
            if (D2D_MODE_TYPE_SOURCE  == intial_req->mode)
            {
            	g_rlc_protocol_ctxt.enb_flag = 1; 
            	g_rlc_protocol_ctxt.eNB_index = 0; 
            }
            else
            {
				g_rlc_protocol_ctxt.enb_flag = 0;
            }
            g_rlc_protocol_ctxt.module_id = 0; 
            g_rlc_protocol_ctxt.rnti = 0xffff;  //!initial value ; 
            g_rlc_protocol_ctxt.configured = FALSE; 
            //! response to rrc 
            rlc_Rrc_Configure_Cfm(RLC_RRC_INITIAL_CFM); 
            
			break;   
		}
		case RRC_RLC_RELEASE_REQ: 
		{
			
 			
			//! TODO response to rrc 
			rlc_Rrc_Configure_Cfm(RLC_RRC_RELEASE_CFM); 
			break; 

		}
		case RRC_RLC_BCCH_PARA_CFG_REQ: 
		{
		    //!source and destination do the SRB0 config 
			bcch_cfg_req = (rrc_rlc_bcch_para_cfg * )message; 
			//!do SRB0 establish 
            AssertFatal(bcch_cfg_req->srb_cfg_req.rb_type <= RB_TYPE_SRB2,RLC,"RLC reveive BCCH_CFG_REQ rb_type error\n"); 
			for (rb_index = 0; rb_index < bcch_cfg_req->srb_cfg_req.srb_count; rb_index++)
			{
			    
				temp_rlc_info.rlc_mode = bcch_cfg_req->srb_cfg_req.srb_list[rb_index].rlc_mode;
				if(RLC_MODE_TM  == temp_rlc_info.rlc_mode)
				{	
					temp_rlc_info.rlc.rlc_tm_info.is_uplink_downlink = 	1; 

				}
				rb_id = bcch_cfg_req->srb_cfg_req.srb_list[rb_index].rb_id; 
				logic_ch_id = bcch_cfg_req->srb_cfg_req.srb_list[rb_index].logicchannel_id; 
				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_YES,
									MBMS_FLAG_NO,
									CONFIG_ACTION_ADD,
									rb_id,
									logic_ch_id,
									temp_rlc_info); 

				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_YES,
									MBMS_FLAG_NO,
									CONFIG_ACTION_MODIFY,
									rb_id,
									logic_ch_id,
									temp_rlc_info); 
			    LOG_INFO(RLC,"RRC_RLC_BCCH_PARA_CFG_REQ: SRB add and modify, rb_indx = %d,rb_id = %d, lc_id = %d,\
			     			rlc_mode = %s\n", \
			     			rb_index,rb_id,logic_ch_id,g_rlc_mode_str[temp_rlc_info.rlc_mode]); 
			}

            //! response to rrc 
            rlc_Rrc_Configure_Cfm(RLC_RRC_BCCH_PARA_CFG_CFM); 
            
			break;
		}
		case RRC_RLC_CONNECT_SETUP_CFG_REQ: 
		{
			connect_req = (rrc_rlc_connect_setup_cfg *)message; 
 
			 //! update ue's rnti 
			g_rlc_protocol_ctxt.rnti = connect_req->ue_rnit; 
			for (rb_index = 0; rb_index < connect_req->srb_cfg_req.srb_count; rb_index++)
			{
				
				temp_rlc_info.rlc_mode = connect_req->srb_cfg_req.srb_list[rb_index].rlc_mode;
				if(RLC_MODE_TM	== temp_rlc_info.rlc_mode)
				{	
					temp_rlc_info.rlc.rlc_tm_info.is_uplink_downlink =	1; 

				}
				rb_id = connect_req->srb_cfg_req.srb_list[rb_index].rb_id; 
				logic_ch_id = connect_req->srb_cfg_req.srb_list[rb_index].logicchannel_id; 
				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_YES,
									MBMS_FLAG_NO,
									CONFIG_ACTION_ADD,
									rb_id,
									logic_ch_id,
									temp_rlc_info); 
				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_YES,
									MBMS_FLAG_NO,
									CONFIG_ACTION_MODIFY,
									rb_id,
									logic_ch_id,
									temp_rlc_info); 
				LOG_INFO(RLC,"RRC_RLC_CONNECT_SETUP_CFG_REQ :SRB add and modify, rb_indx = %d,rb_id = %d, lc_id = %d,\
							rlc_mode = %s\n",  \
			     			rb_index,rb_id,logic_ch_id,g_rlc_mode_str[temp_rlc_info.rlc_mode]); 					
			}

			
			for (rb_index = 0; rb_index < connect_req->drb_cfg_req.drb_count; rb_index++)
			{
				
				temp_rlc_info.rlc_mode = connect_req->drb_cfg_req.drb_list[rb_index].rlc_mode;
				if(RLC_MODE_TM	== temp_rlc_info.rlc_mode)
				{	
					temp_rlc_info.rlc.rlc_tm_info.is_uplink_downlink =	1; 

				}
				else 
				{
					AssertFatal(temp_rlc_info.rlc_mode == RLC_MODE_UM, RLC, "RRC_RLC_CONNECT_SETUP_CFG_REQ \
					mesasge with error rlc mode \n "); 
					temp_rlc_info.rlc.rlc_um_info.is_uplink_downlink = 1; 
					temp_rlc_info.rlc.rlc_um_info.is_mXch = 0; 

					dl_sn_field = connect_req->drb_cfg_req.drb_list[rb_index].rlc_mode_cfg.dl_um_cfg.sn_field; 
					ul_sn_field =  connect_req->drb_cfg_req.drb_list[rb_index].rlc_mode_cfg.ul_um_cfg.sn_field;

					AssertFatal(dl_sn_field ==ul_sn_field, RLC, "RRC_RLC_CONNECT_SETUP_CFG_REQ:SRB sn_field error\n") 
			
					if(1 == dl_sn_field) {
					
						temp_rlc_info.rlc.rlc_um_info.sn_field_length = 10; 
					}else {
						temp_rlc_info.rlc.rlc_um_info.sn_field_length = 5; 
					}
					
					dl_t_recorder = connect_req->drb_cfg_req.drb_list[rb_index].rlc_mode_cfg.dl_um_cfg.t_recordering; 
					temp_rlc_info.rlc.rlc_um_info.timer_reordering = t_Reordering_tab[dl_t_recorder]; 
					


				}
				rb_id = connect_req->drb_cfg_req.drb_list[rb_index].rb_id; 
				logic_ch_id = connect_req->drb_cfg_req.drb_list[rb_index].logicchannel_id; 
				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_NO,
									MBMS_FLAG_NO,
									CONFIG_ACTION_ADD,
									rb_id,
									logic_ch_id,
									temp_rlc_info); 
				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_NO,
									MBMS_FLAG_NO,
									CONFIG_ACTION_MODIFY,
									rb_id,
									logic_ch_id,
									temp_rlc_info); 

				LOG_INFO(RLC,"RRC_RLC_CONNECT_SETUP_CFG_REQ :DRB add and modify, rb_indx = %d,rb_id = %d, lc_id = %d,\
							rlc_mode = %s\n",  \
			     			rb_index,rb_id,logic_ch_id,g_rlc_mode_str[temp_rlc_info.rlc_mode]); 					
			}
			rlc_Rrc_Configure_Cfm(RLC_RRC_CONNECT_SETUP_CFG_CFM); 	
			break; 
		}
		default: break; 
	}

}




 /*!   
 * @brief: rlc receive rrc config message  
 * @author:  bo.liu
 * @Date:  2019年9月5日
 */
void *rlc_rrc_config_task(MessageDef *recv_msg)
{

	rlc_rrc_config_process(MSG_HEAD_TO_COMM(recv_msg), recv_msg->ittiMsgHeader.MsgType);
	itti_free_message(recv_msg);

}
 


 
 
/**************************function******************************/
