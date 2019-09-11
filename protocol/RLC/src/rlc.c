/******************************************************************
 * @file  rlc.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <d2d_system_global.h>
#include <hashtable.h> 
#include <interface_rrc_rlc.h>
#include <rlc.h>




void rb_free_rlc_union (void *rlcu_pP)
{

  //-----------------------------------------------------------------------------
  rlc_union_t * rlcu_p;

  if (rlcu_pP) {
    rlcu_p = (rlc_union_t *)(rlcu_pP);
    LOG_DEBUG(RLC,"%s %p \n",__func__,rlcu_pP);

    switch (rlcu_p->mode) {
    #ifdef AM_ENABLE 
	    case RLC_MODE_AM:
	      rlc_am_cleanup(&rlcu_p->rlc.am);
	      break;
    #endif 
	    case RLC_MODE_UM:
	      rlc_um_cleanup(&rlcu_p->rlc.um);  //！释放UM 使用的接收和发送memory链表
	      break;

	    case RLC_MODE_TM:
	      rlc_tm_cleanup(&rlcu_p->rlc.tm); //! 释放TM 使用的接收和发送memory链表
	      break;

	    default:
	      LOG_WARN(RLC,
	            "%s %p unknown RLC type\n",
	            __func__,
	            rlcu_pP);
	      break;
    }
  }

}


 

void rlc_module_initial( )
{

	//!step1:initial HASH TABLE 
	rlc_coll_p = hashtable_create ((MAX_SRB_COUNT + MAX_DRB_COUNT + 2) *D2D_MAX_USER_NUM , NULL, rb_free_rlc_union);

	AssertFatal(rlc_coll_p != NULL, RLC, "RLC initial HASH TABLE failed\n"); 

    //!memory pool initial 
 
    pool_buffer_init(); 

    //!initial  g_rlc_protocol_ctxt as destination 
    g_rlc_protocol_ctxt.enb_flag = 0; 
    g_rlc_protocol_ctxt.eNB_index = 0; 
    g_rlc_protocol_ctxt.module_id = 0; 
    g_rlc_protocol_ctxt.rnti = 0xffff;  //!initial value ; 
    g_rlc_protocol_ctxt.configured = FALSE; 

}
 
 
/**************************function******************************/
