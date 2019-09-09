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
#if 0
rlc_union_t* rrc_rlc_add_rlc   (
  const protocol_ctxt_t* const ctxt_pP,
  const srb_flag_t        srb_flagP,
  const MBMS_flag_t       MBMS_flagP,
  const rb_id_t           rb_idP,
  const logical_chan_id_t chan_idP,
  const rlc_mode_e        rlc_modeP)
{

  //-----------------------------------------------------------------------------
  hash_key_t             key         = HASHTABLE_NOT_A_KEY_VALUE;
  hashtable_rc_t         h_rc;
  hash_key_t             key_lcid    = HASHTABLE_NOT_A_KEY_VALUE;
  hashtable_rc_t         h_lcid_rc;
  rlc_union_t           *rlc_union_p = NULL;


  	if(chan_idP >= RLC_MAX_LC) {
  		LOG_ERROR(RLC, "LC id is too high (%u/%d)!\n", chan_idP, RLC_MAX_LC);
  		return NULL;
  	}

 

  
    key = RLC_COLL_KEY_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, rb_idP, srb_flagP);
    key_lcid = RLC_COLL_KEY_LCID_VALUE(ctxt_pP->module_id, ctxt_pP->rnti, ctxt_pP->enb_flag, chan_idP, srb_flagP);
  

  h_rc = hashtable_get(rlc_coll_p, key, (void**)&rlc_union_p);

  if (h_rc == HASH_TABLE_OK) {
    LOG_W(RLC, PROTOCOL_CTXT_FMT"[%s %u] rrc_rlc_add_rlc , already exist %s\n",
          PROTOCOL_CTXT_ARGS(ctxt_pP),
          (srb_flagP) ? "SRB" : "DRB",
          rb_idP,
          (srb_flagP) ? "SRB" : "DRB");
    //AssertFatal(rlc_union_p->mode == rlc_modeP, "Error rrc_rlc_add_rlc , already exist but RLC mode differ");
  	if(rlc_union_p->mode != rlc_modeP){
  		LOG_ERROR(RLC, "Error rrc_rlc_add_rlc , already exist but RLC mode differ\n");
  		return NULL;
  	}
  	return rlc_union_p;
  } else if (h_rc == HASH_TABLE_KEY_NOT_EXISTS) {
    rlc_union_p = calloc(1, sizeof(rlc_union_t));
	//! 在RRC 信令中，当需要增加RLC 实体时，进行hashtable 的insert, 使得插入key对应的节点
    h_rc = hashtable_insert(rlc_coll_p, key, rlc_union_p);  //!rb_id生成的h_rc
    h_lcid_rc = hashtable_insert(rlc_coll_p, key_lcid, rlc_union_p); //logic chid 生成的h_lcid_rc 

	if ((h_rc == HASH_TABLE_OK) && (h_lcid_rc == HASH_TABLE_OK)) {  //!<如果插入成功
	{  //!<打印log 
	LOG_INFO(RLC, PROTOCOL_CTXT_FMT" [%s %u] rrc_rlc_add_rlc  %s\n",
	      PROTOCOL_CTXT_ARGS(ctxt_pP),
	      (srb_flagP) ? "SRB" : "DRB",
	      rb_idP,
	      (srb_flagP) ? "SRB" : "DRB");
	

      rlc_union_p->mode = rlc_modeP; //!<更新mode, 
      return rlc_union_p;  //!<并将这个union 返回。
    } else {
      LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"[%s %u] rrc_rlc_add_rlc FAILED %s (add by RB_id=%d; add by LC_id=%d)\n",
            PROTOCOL_CTXT_ARGS(ctxt_pP),
            (srb_flagP) ? "SRB" : "DRB",
            rb_idP,
            (srb_flagP) ? "SRB" : "DRB",
            h_rc, h_lcid_rc);
      free(rlc_union_p);  //!< 如果插入到hash表中失败时，则将其free掉。
      rlc_union_p = NULL;
      return NULL;
    }
  } else {
    LOG_ERROR(RLC, PROTOCOL_CTXT_FMT"[%s %u] rrc_rlc_add_rlc , INTERNAL ERROR %s\n",
          PROTOCOL_CTXT_ARGS(ctxt_pP),
          (srb_flagP) ? "SRB" : "DRB",
          rb_idP,
          (srb_flagP) ? "SRB" : "DRB");
  }
  return NULL;
}


rlc_op_status_t rrc_rlc_config_req	 (
											const protocol_ctxt_t* const ctxt_pP,
											const srb_flag_t		srb_flagP,
											const MBMS_flag_t 	mbms_flagP,
											const config_action_t actionP,
											const rb_id_t 		rb_idP,
											const rlc_info_t		rlc_infoP)
{
	//-----------------------------------------------------------------------------
	//rlc_op_status_t status;

	LOG_DEBUG(RLC, PROTOCOL_CTXT_FMT" CONFIG_REQ for RAB %u\n",
		PROTOCOL_CTXT_ARGS(ctxt_pP),
		rb_idP);

	AssertFatal (rb_idP < MAX_DRB_COUNT, "RB id is too high (%u/%d)!\n", rb_idP, MAX_DRB_COUNT);
	
	switch (actionP) {

	case CONFIG_ACTION_ADD:
	if (rrc_rlc_add_rlc(ctxt_pP, srb_flagP, MBMS_FLAG_NO, rb_idP, rb_idP, rlc_infoP.rlc_mode
#if (LTE_RRC_VERSION >= MAKE_VERSION(14, 0, 0))
						,0,
						0
#endif
	) != NULL) {
	  return RLC_OP_STATUS_INTERNAL_ERROR;
	}

	// no break, fall to next case
	case CONFIG_ACTION_MODIFY:
	switch (rlc_infoP.rlc_mode) {
	case RLC_MODE_AM:
	  LOG_INFONFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] MODIFY RB AM\n",
			PROTOCOL_CTXT_ARGS(ctxt_pP),
			rb_idP);

	  config_req_rlc_am(
		ctxt_pP,
		srb_flagP,
		&rlc_infoP.rlc.rlc_am_info,
		rb_idP, rb_idP);
	  break;

	case RLC_MODE_UM:
	  LOG_INFONFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] MODIFY RB UM\n",
			PROTOCOL_CTXT_ARGS(ctxt_pP),
			rb_idP);
	  config_req_rlc_um(
		ctxt_pP,
		srb_flagP,
		&rlc_infoP.rlc.rlc_um_info,
		rb_idP, rb_idP);
	  break;

	case RLC_MODE_TM:
	  LOG_INFONFO(RLC, PROTOCOL_CTXT_FMT"[RB %u] MODIFY RB TM\n",
			PROTOCOL_CTXT_ARGS(ctxt_pP),
			rb_idP);
	  config_req_rlc_tm(
			ctxt_pP,
			srb_flagP,
			&rlc_infoP.rlc.rlc_tm_info,
			rb_idP, rb_idP);
	  break;

	default:
	  return RLC_OP_STATUS_BAD_PARAMETER;
	}

	break;

	case CONFIG_ACTION_REMOVE:
		return rrc_rlc_remove_rlc(ctxt_pP, srb_flagP, mbms_flagP, rb_idP);
		break;

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
	uint32_t       rb_id; 
	switch (msg_type)
	{
		case RRC_RLC_INITIAL_REQ: 
		{
			intial_req = (rrc_rlc_initial_req * )message; 
			
			AssertFatal(intial_req->mode == D2D_MODE_TYPE_SOURCE, RLC,
									"only SOURCE can receive RLC INITIAL message\n"); 
				
            //! constitue  protocol_ctxt_t; 
            g_rlc_protocol_ctxt.enb_flag = 1; 
            g_rlc_protocol_ctxt.eNB_index = 0; 
            g_rlc_protocol_ctxt.module_id = 0; 
            g_rlc_protocol_ctxt.rnti = 0xffff;  //!initial value ; 
            g_rlc_protocol_ctxt.configured = FALSE; 
            //! response to rrc 
            rlc_Rrc_Configure_Cfm(RLC_RRC_INITIAL_CFM); 
            
			break; 
		}
		case RRC_RLC_RELEASE_REQ: 
		{
			
 			
			//! response to rrc 
			
			break; 

		}
		case RRC_RLC_BCCH_PARA_CFG_REQ: 
		{
		    //!source and destination do the SRB0 config 
			bcch_cfg_req = (rrc_rlc_bcch_para_cfg * )message; 
			//!do SRB0 establish 
            AssertFatal(bcch_cfg_req->srb_cfg_req.rb_type <=RB_TYPE_DRB,RLC,"RLC reveive BCCH_CFG_REQ rb_type error\n"); 
			for (rb_index = 0; rb_index < bcch_cfg_req->srb_cfg_req.srb_count; rb_index++)
			{
			    
				temp_rlc_info.rlc_mode = bcch_cfg_req->srb_cfg_req.srb_list[rb_index].rlc_mode;
				if(RLC_MODE_TM  == temp_rlc_info.rlc_mode)
				{	
					temp_rlc_info.rlc.rlc_tm_info.is_uplink_downlink = 	1; 

				}
				rb_id = bcch_cfg_req->srb_cfg_req.srb_list[rb_index].rb_id; 
				rrc_rlc_config_req(&g_rlc_protocol_ctxt,
									SRB_FLAG_YES,
									MBMS_FLAG_NO,
									CONFIG_ACTION_ADD,
									rb_id,
									temp_rlc_info); 
			}

            //! response to rrc 
			break;
		}
		case RRC_RLC_CONNECT_SETUP_CFG_REQ: 
		{
			connect_req = (rrc_rlc_connect_setup_cfg *)message; 
			connect_req.

			//!response to rrc 
		}
		default: break; 
	}

}




void rlc_tx_process(void *message, MessagesIds      msg_type)
{

	switch (msg_type)
	{
		case RRC_RLC_BUF_STATUS_REQ: 
		{



			break; 
		}
		case RRC_RLC_DATA_IND: 
		{



			break; 
		}
		//!TODO  IP DATA transfer message 
		default: break
	}

}

 /*!   
 * @brief: rlc receive rrc config message  
 * @author:  bo.liu
 * @Date:  2019年9月5日
 */
void rlc_rrc_config_task( )
{

	MessageDef *recv_msg;
	rlc_module_initial();

	while(1)
	{

        if (0 == itti_receive_msg(TASK_D2D_RLC, &recv_msg))
        {
			rlc_rrc_config_process(recv_msg->message_ptr, recv_msg->ittiMsgHeader.messageId);
			itti_free_message(recv_msg);
        }






	}







}
 
#endif

 
 
/**************************function******************************/
