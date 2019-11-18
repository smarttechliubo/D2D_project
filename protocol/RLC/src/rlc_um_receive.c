/******************************************************************
 * @file  rlc_um_receive.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月10日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月10日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>



void rlc_um_init_timer_reordering(
  const protocol_ctxt_t* const ctxt_pP,
  rlc_um_entity_t * const rlc_pP,
  const uint32_t  ms_durationP)
{
  rlc_pP->t_reordering.running         = 0;
  rlc_pP->t_reordering.ms_time_out     = 0;
  rlc_pP->t_reordering.ms_start        = 0;
  rlc_pP->t_reordering.ms_duration     = ms_durationP;
  rlc_pP->t_reordering.timed_out       = 0;
}




signed int rlc_um_get_pdu_infos(const protocol_ctxt_t* const ctxt_pP,
									  const rlc_um_entity_t* const rlc_pP,
									  rlc_um_pdu_sn_10_t   * const header_pP,
									  const sdu_size_t             total_sizeP,
									  rlc_um_pdu_info_t    * const pdu_info_pP,
									  const uint8_t                sn_lengthP)
{
  sdu_size_t         sum_li = 0;
  memset(pdu_info_pP, 0, sizeof (rlc_um_pdu_info_t));

  

  AssertFatal( total_sizeP > 0 , RLC,"RLC UM PDU LENGTH %d\n", total_sizeP);
  AssertFatal( sn_lengthP  == 10 , RLC,"RLC UM SN length error %d\n", sn_lengthP);

  //! header中解析出来PDU 的info
  if (sn_lengthP == 10) {
    pdu_info_pP->fi           = (header_pP->b1 >> 3) & 0x03;
    pdu_info_pP->e            = (header_pP->b1 >> 2) & 0x01;
    pdu_info_pP->sn           = header_pP->b2 + (((uint16_t)(header_pP->b1 & 0x03)) << 8);
    pdu_info_pP->header_size  = 2;
    pdu_info_pP->payload      = &header_pP->data[0]; //！固定偏移2个byte之后的地址
  } else if (sn_lengthP == 5) {
    AssertFatal(0, RLC, "RLC sn length error!,%s,%d\n",__FILE__, __LINE__); 
    pdu_info_pP->fi           = (header_pP->b1 >> 6) & 0x03;
    pdu_info_pP->e            = (header_pP->b1 >> 5) & 0x01;
    pdu_info_pP->sn           = header_pP->b1 & 0x1F;
    pdu_info_pP->header_size  = 1;
    pdu_info_pP->payload      = &header_pP->b2;
  } else {
    AssertFatal(0, RLC, "RLC UM SN LENGTH %d", sn_lengthP);
  }

  pdu_info_pP->num_li = 0;
  pdu_info_pP->hidden_size = 0;
  if (pdu_info_pP->e) {  //E表示后面还有E+LI
    rlc_am_e_li_t      *e_li_p;
    unsigned int li_length_in_bytes  = 1;
    unsigned int li_to_read          = 1;

    e_li_p = (rlc_am_e_li_t*)(pdu_info_pP->payload); 

    while (li_to_read)  {
      li_length_in_bytes = li_length_in_bytes ^ 3; //！= 2 OR =1

        //！这里的读取分成2种，先读2byte，然后再看情况是否有后面的LI 需要读取
      if (li_length_in_bytes  == 2) { //！偶数个LI ，占2个byte 

		//将LI 从b1,b2中提出来，b2中包含了4bit的padding ,b1中包含了E的值 
        pdu_info_pP->li_list[pdu_info_pP->num_li] = ((uint16_t)(e_li_p->b1 << 4)) & 0x07F0;  //!取7bit,并左移4bit
        pdu_info_pP->li_list[pdu_info_pP->num_li] |= (((uint8_t)(e_li_p->b2 >> 4)) & 0x000F);//!取高4bit,并组装成11bit的LI
        li_to_read = e_li_p->b1 & 0x80; //是否还需要继续读LI 
        pdu_info_pP->header_size  += 2;
      } 
      else 
      { //!如果还有下一个E+LI,则从b2开始读取
		//取b2的低3bit, 并且右移8bit,和b3或，得到LI+E
        pdu_info_pP->li_list[pdu_info_pP->num_li] = ((uint16_t)(e_li_p->b2 << 8)) & 0x0700; //!取b2的低3bit,和b3的8bit组成11BIT的LI
        pdu_info_pP->li_list[pdu_info_pP->num_li] |=  e_li_p->b3;
        li_to_read = e_li_p->b2 & 0x08;
        e_li_p++; //!每次更新3个byte 
        pdu_info_pP->header_size  += 1;
      }

   
      sum_li += pdu_info_pP->li_list[pdu_info_pP->num_li]; //每一个segment 的Length相加 
      pdu_info_pP->num_li = pdu_info_pP->num_li + 1; //最后一个segment没有LI 

   
    }

    AssertFatal(pdu_info_pP->num_li < 128, RLC, "number of LI exceed the limit\n"); 

    if (2 == li_length_in_bytes) {
      pdu_info_pP->payload = &e_li_p->b3; //表示最后一次是只占2个byte的LI,data 从b3开始 
    } else {
      pdu_info_pP->payload = &e_li_p->b1; //!表示最后一次是占3个byte的LI,所以data 从下一个b1开始
    }
  }
  //减去header就是data field的size 
  pdu_info_pP->payload_size = total_sizeP - pdu_info_pP->header_size;
   //data field size > LI之和，则说明data filed中还有hidden size ,可能有padding
  if ((pdu_info_pP->num_li  != 0) && (pdu_info_pP->payload_size > sum_li)) {
    pdu_info_pP->hidden_size = pdu_info_pP->payload_size - sum_li;
  }



  LOG_INFO(RLC_RX,"RLC rx header info:SN:%d, headersize:%d, LI number:%d,sum of LI:%d, tbsize:%d,hiddensize :%d \n", 
  							 pdu_info_pP->sn,
							pdu_info_pP->header_size,
							pdu_info_pP->num_li,
							pdu_info_pP->payload_size, 
							total_sizeP,
							pdu_info_pP->hidden_size);
  return 0;
} 


//-----------------------------------------------------------------------------
// returns -2 if lower_bound	> sn
// returns -1 if higher_bound < sn
// returns  0 if lower_bound	< sn < higher_bound
// returns  1 if lower_bound	== sn
// returns  2 if higher_bound == sn
// returns  3 if higher_bound == sn == lower_bound
//！这个函数并不是按照协议去一次性判断SN 是否在接收窗内的，
//！而是自定义了窗的下边界和上边界，将协议里的接收窗分成了几部分进行处理

signed int   rlc_um_in_window(const protocol_ctxt_t* const ctxt_pP,
									rlc_um_entity_t * const rlc_pP,
									rlc_sn_t lower_boundP,	//! UH - window_size是窗的下边界 
									rlc_sn_t snP,			//！当前要处理的SN号
									rlc_sn_t higher_boundP) 
{
	rlc_sn_t modulus = (rlc_sn_t)rlc_pP->vr_uh - rlc_pP->rx_um_window_size;  //!根据协议，uh-window_size 是模的Base
#if TRACE_RLC_UM_RX
	rlc_sn_t	 lower_bound  = lower_boundP;
	rlc_sn_t	 higher_bound = higher_boundP;
	rlc_sn_t	 sn 		  = snP;
#endif

	//！本次这个分段接收窗和上下边界和SNP 都分别和真正的下边界做减法，归一化。 
	lower_boundP  = (lower_boundP  - modulus) % rlc_pP->rx_sn_modulo;  //！rx_sn_modulo = 1024
	higher_boundP = (higher_boundP - modulus) % rlc_pP->rx_sn_modulo;
	snP 		  = (snP		   - modulus) % rlc_pP->rx_sn_modulo; 

	if ( lower_boundP > snP) {	//SN 不在接收窗内
#if TRACE_RLC_UM_RX
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" %d not in WINDOW[%03d:%03d] (SN<LOWER BOUND)\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			sn,
			lower_bound,
			higher_bound);
#endif
	  return -2;
	}

	if ( higher_boundP < snP) { //!< 说明本次处理的PDU 不在这段接收窗内
#if TRACE_RLC_UM_RX
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" %d not in WINDOW[%03d:%03d] (SN>HIGHER BOUND) <=> %d not in WINDOW[%03d:%03d]\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			sn,
			lower_bound,
			higher_bound,
			snP,
			lower_boundP,
			higher_boundP);
#endif
	  return -1;
	}
	 //!PDU 处于本段接收窗的下边界，返回1，如果上下边界相等了，那么说明窗不合适，返回3 
	if ( lower_boundP == snP) {
	  if ( higher_boundP == snP) {
#if TRACE_RLC_UM_RX
		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" %d  in WINDOW[%03d:%03d] (SN=HIGHER BOUND=LOWER BOUND)\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  sn,
			  lower_bound,
			  higher_bound);
#endif
		return 3;
	  }

#if TRACE_RLC_UM_RX
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" %d  in WINDOW[%03d:%03d] (SN=LOWER BOUND)\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			sn,
			lower_bound,
			higher_bound);
#endif
	  return 1;
	}

	//！SN 在上边界了，则返回2
	if ( higher_boundP == snP) {
#if TRACE_RLC_UM_RX
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" %d  in WINDOW[%03d:%03d] (SN=HIGHER BOUND)\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			sn,
			lower_bound,
			higher_bound);
#endif
	  return 2;
	}

	return 0; //!正常返回0

}
//-----------------------------------------------------------------------------


void   rlc_um_clear_rx_sdu (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t* rlc_pP)
{
  rlc_pP->output_sdu_size_to_write = 0;
}


//！UM 发送SDU 
//! 这个函数要做到将SDU 上报给上层之后，释放申请的内存。
void  rlc_um_send_sdu (const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t *rlc_pP)
{
  if ((rlc_pP->output_sdu_in_construction)) {
   LOG_ERROR(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" SEND_SDU SN:%d to upper layers %d bytes sdu %p\n",
          PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
          rlc_pP->last_reassemblied_sn,
          rlc_pP->output_sdu_size_to_write,
          rlc_pP->output_sdu_in_construction);

    if (rlc_pP->output_sdu_size_to_write > 0) {
		//!已经有SDU了， 并且其中已经存放了PDU的数据， 
      rlc_pP->stat_rx_pdcp_sdu += 1;
      rlc_pP->stat_rx_pdcp_bytes += rlc_pP->output_sdu_size_to_write;
       
      // msg("[RLC] DATA IND ON MOD_ID %d RB ID %d, size %d\n",rlc_pP->module_id, rlc_pP->rb_id, ctxt_pP->frame,rlc_pP->output_sdu_size_to_write);
       //! //!正式代码：将RLC的数据上报给PDCP 
       #if 0
      rlc_data_ind (
        ctxt_pP,
        BOOL_NOT(rlc_pP->is_data_plane), //！是SRB,还是DRB 的flag
        rlc_pP->is_mxch,
        rlc_pP->rb_id,
        rlc_pP->output_sdu_size_to_write,
        rlc_pP->output_sdu_in_construction); //!在此函数中释放memory
       #endif 
      rlc_pP->output_sdu_in_construction = NULL;  //！释放指针
    } else {
      LOG_ERROR(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"[SEND_SDU] ERROR SIZE <= 0 ... DO NOTHING, SET SDU SIZE TO 0\n",
            PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
    }

    rlc_pP->output_sdu_size_to_write = 0; //！清空当前实体的SDU size 
  }
}


		
int rlc_um_read_length_indicators(unsigned char**data_ppP, 
											rlc_um_e_li_t* e_liP, 
											unsigned int* li_array_pP, 
											unsigned int *num_li_pP, 
											sdu_size_t *data_size_pP)
{
  int		   continue_loop = 1;
  unsigned int e1  = 0;
  unsigned int li1 = 0;
  unsigned int e2  = 0;
  unsigned int li2 = 0;
  *num_li_pP = 0;

  while ((continue_loop)) {
	//msg("[RLC_UM] e_liP->b1 = %02X\n", e_liP->b1);
	//msg("[RLC_UM] e_liP->b2 = %02X\n", e_liP->b2);

	//!1bit的E,11bit的LI 
	e1 = ((unsigned int)e_liP->b1 & 0x00000080) >> 7;
	li1 = (((unsigned int)e_liP->b1 & 0x0000007F) << 4) + (((unsigned int)e_liP->b2 & 0x000000F0) >> 4);
	li_array_pP[*num_li_pP] = li1; //！第一个data segment 的长度 
	*data_size_pP = *data_size_pP - li1 - 2;  //！减去第一个data segment ，还要减去2，是减去E+LI的字节数 
	*num_li_pP = *num_li_pP +1;

	if ((e1)) { //！如果E1表示后面还有一个E+LI
	  e2 = ((unsigned int)e_liP->b2 & 0x00000008) >> 3; //!第二个E
	  li2 = (((unsigned int)e_liP->b2 & 0x00000007) << 8) + ((unsigned int)e_liP->b3 & 0x000000FF);
	  li_array_pP[*num_li_pP] = li2;
	  *data_size_pP = *data_size_pP - li2 - 1; //!这里再减去1byte,这样当有2个E+LI时，减去的就是3个BYTE 
	  *num_li_pP = *num_li_pP +1;

	  if ((*data_size_pP < 0)) 
	  {
		  LOG_ERROR(RLC_RX, "Invalid data_size=%d! (pdu_size=%d loop=%d e1=%d e2=%d li2=%d e_liP=%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
			  *data_size_pP, *data_size_pP, continue_loop, e1, e2, li2,
			  (e_liP-(continue_loop-1)+0)->b1,
			  (e_liP-(continue_loop-1)+0)->b2,
			  (e_liP-(continue_loop-1)+0)->b3,
			  (e_liP-(continue_loop-1)+1)->b1,
			  (e_liP-(continue_loop-1)+1)->b2,
			  (e_liP-(continue_loop-1)+1)->b3,
			  (e_liP-(continue_loop-1)+2)->b1,
			  (e_liP-(continue_loop-1)+2)->b2,
			  (e_liP-(continue_loop-1)+2)->b3);
	  }
	  // AssertFatal(*data_size_pP >= 0, "Invalid data_size!");

	  if (e2 == 0) {
		continue_loop = 0;	//!后面没有扩展部分了，停止读取LI
	  } else {	//!如果e2后面还有，则继续
		e_liP++;
		continue_loop++;
	  }
	} 
	else {
	  //如果E1后面没有扩展部分了，那么后面就是数据了 
	  if ((*data_size_pP < 0)) 
	  {
	    LOG_ERROR(RLC_RX, "Invalid data_size=%d! (pdu_size=%d loop=%d e1=%d li1=%d e_liP=%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
		  *data_size_pP, *data_size_pP, continue_loop, e1, li1,
		  (e_liP-(continue_loop-1)+0)->b1,
		  (e_liP-(continue_loop-1)+0)->b2,
		  (e_liP-(continue_loop-1)+0)->b3,
		  (e_liP-(continue_loop-1)+1)->b1,
		  (e_liP-(continue_loop-1)+1)->b2,
		  (e_liP-(continue_loop-1)+1)->b3,
		  (e_liP-(continue_loop-1)+2)->b1,
		  (e_liP-(continue_loop-1)+2)->b2,
		  (e_liP-(continue_loop-1)+2)->b3);
	  	  continue_loop = 0;
	  }
	  // AssertFatal(*data_size_pP >= 0, "Invalid data_size!");
	}

	if (*num_li_pP > RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU) {
	  return -1;
	}
  }

  *data_ppP = *data_ppP + (((*num_li_pP*3) +1) >> 1); //!偏移掉LI,来到data field
  if (*data_size_pP > 0) {  //!最后一个data segment 还有数据
	return 0;
  } else if (*data_size_pP == 0) { //！最后没有数据了
	LOG_WARN(RLC, "Last RLC SDU size is zero!\n");
	return -1;
  } else {
	LOG_WARN(RLC, "Last RLC SDU size is negative %d!\n", *data_size_pP);
	return -1;
  }
}



//!从mac buffer中拿出DATA来
void  rlc_um_reassembly (const protocol_ctxt_t* const ctxt_pP, 
							   rlc_um_entity_t *rlc_pP, 
							   uint8_t * src_pP, 
							   int32_t lengthP)
{
  sdu_size_t      sdu_max_size;

  LOG_DEBUG(RLC_RX,PROTOCOL_RLC_UM_CTXT_FMT"REASSEMBLY reassembly()  %d bytes %d bytes start to reassemble\n",
        PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
        lengthP,
        rlc_pP->output_sdu_size_to_write);

  if (lengthP <= 0) {
    return;
  }
  
  if ((rlc_pP->is_data_plane)) {
    sdu_max_size = RLC_SDU_MAX_SIZE_DATA_PLANE;
  } else {
    sdu_max_size = RLC_SDU_MAX_SIZE_CONTROL_PLANE;
  }
   //！如果SDU 还没有申请BUFFER,则从MEM中申请一块SDU buffer,用来承载PDU的data filed 
  if (rlc_pP->output_sdu_in_construction == NULL) {
    //    msg("[RLC_UM_LITE] Getting mem_block ...\n");
    //! 从memory block 中获取一块free的memory ,获取的size大小为max_size 
    rlc_pP->output_sdu_in_construction = get_free_mem_block (sdu_max_size, __func__);
    rlc_pP->output_sdu_size_to_write = 0;
  }
  //！如果之前已经申请过buffer了，现在从偏移地址开始，把数据copy进去 
  if ((rlc_pP->output_sdu_in_construction)) {
    // check if no overflow in size
    if ((rlc_pP->output_sdu_size_to_write + lengthP) <= sdu_max_size) {
	 	
	 //！将PDU中的data field copy到SDU的buffer中 
      memcpy (&rlc_pP->output_sdu_in_construction->data[rlc_pP->output_sdu_size_to_write], src_pP, lengthP);
      rlc_pP->output_sdu_size_to_write += lengthP;
#if TRACE_RLC_UM_DISPLAY_ASCII_DATA
      rlc_pP->output_sdu_in_construction->data[rlc_pP->output_sdu_size_to_write] = 0;
      LOG_T(RLC, PROTOCOL_RLC_UM_CTXT_FMT"[REASSEMBLY] DATA :",
            PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
      rlc_util_print_hex_octets(RLC, (unsigned char*)rlc_pP->output_sdu_in_construction->data, rlc_pP->output_sdu_size_to_write);
#endif
    } else {

	//! MAC 上报的PDU 太大了，超出了SDU 的最大size 

      AssertFatal(0, RLC,PROTOCOL_RLC_UM_CTXT_FMT" RLC_UM_DATA_IND, SDU TOO BIG, DROPPED\n",
                  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
#if 0
      LOG_ERROR(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"[REASSEMBLY] [max_sdu size %d] ERROR  SDU SIZE OVERFLOW SDU GARBAGED\n",
            PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP),
            sdu_max_size);
#endif 
      // erase  SDU
      rlc_pP->output_sdu_size_to_write = 0;
    }
  } 
  else {
   //！申请SDU buffer 失败
    AssertFatal(0, RLC,PROTOCOL_RLC_UM_CTXT_FMT" RLC_UM_DATA_IND, SDU DROPPED, OUT OF MEMORY\n",
                PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));

  }

}

//-----------------------------------------------------------------------------
//!将满足接收条件，并且不在recording窗内的PDU 进行去掉Header，
//!并重新组装，按照SN 的顺序发给上层

void   rlc_um_try_reassembly(const protocol_ctxt_t* const ctxt_pP,
									  rlc_um_entity_t * 		   rlc_pP,
									  rlc_sn_t					   start_snP,
									  rlc_sn_t					   end_snP)
{
	mem_block_t		 *pdu_mem_p 			 = NULL;
	struct mac_tb_ind  *tb_ind_p				 = NULL;
	rlc_um_e_li_t 	 *e_li_p				 = NULL;
	unsigned char 	 *data_p				 = NULL;
	int				  e 					 = 0;
	int				  fi					 = 0;
	sdu_size_t		  size					 = 0;
	rlc_sn_t			  sn					 = 0;
	unsigned int		  continue_reassembly	 = 0;
	unsigned int		  num_li				 = 0;
	unsigned int		  li_array[RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU];
	int				  i 					 = 0;
	int				  reassembly_start_index = 0;

    LOG_DEBUG(RLC_RX, "%s, start_snp:%d-----end_snp:%d\n", __func__, start_snP,end_snP);

	if (end_snP < 0)	 {
		end_snP   = end_snP   + rlc_pP->rx_sn_modulo;
	}

	if (start_snP < 0) {
		start_snP = start_snP + rlc_pP->rx_sn_modulo;
	}

	LOG_DEBUG(RLC_RX,  PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY FROM PDU SN=%03d+1  TO  PDU SN=%03d,SN Length = %d bits\n",
		PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
		rlc_pP->last_reassemblied_sn,
		end_snP,
		rlc_pP->rx_sn_length);


	// nothing to be reassemblied
	if (start_snP == end_snP) {
		return;
	}

	continue_reassembly = 1;
	
	sn = start_snP;


	while (continue_reassembly) {
	//!之前已经把数据存放在dar_buffer中了，按照SN的顺序
		if ((pdu_mem_p = rlc_pP->dar_buffer[sn])) {
		   //如果SN 不等于之前最后一次组包的SN+1,那么说明有PDU 丢失
		  if ((rlc_pP->last_reassemblied_sn+1)%rlc_pP->rx_sn_modulo != sn) {

		LOG_WARN(RLC,
			  PROTOCOL_RLC_UM_CTXT_FMT" FINDING a HOLE in RLC UM SN: CLEARING OUTPUT SDU BECAUSE NEW SN (%03d) TO REASSEMBLY NOT \ 
CONTIGUOUS WITH LAST REASSEMBLIED SN (%03d) \n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  sn,
			  rlc_pP->last_reassemblied_sn);

		//！将 output_sdu_size_to_write 设置为0 
		rlc_um_clear_rx_sdu(ctxt_pP, rlc_pP);
	  }

	  rlc_pP->last_reassemblied_sn = sn;  //!更新最新的组包SN号
	  tb_ind_p = (struct mac_tb_ind *)(pdu_mem_p->data);
	  
	   //!按照SN = 10进行处理，得到e和FI,以及扩展部分的E+LI的部分 
	  if (rlc_pP->rx_sn_length == 10) {
	
		e  = (((rlc_um_pdu_sn_10_t*)(tb_ind_p->data_ptr))->b1 & 0x04) >> 2; //bit2
		fi = (((rlc_um_pdu_sn_10_t*)(tb_ind_p->data_ptr))->b1 & 0x18) >> 3; //bit3,bit4 
		
		e_li_p = (rlc_um_e_li_t*)((rlc_um_pdu_sn_10_t*)(tb_ind_p->data_ptr))->data; //!指向E_LI的首地址地址
		size   = tb_ind_p->size - 2; //！减去固定header 
		data_p = &tb_ind_p->data_ptr[2]; //!指向固定header之后的地址
	  } else {	
	  	AssertFatal(0, RLC,"parameter error, %s,%d",__FILE__, __LINE__);
	  }
	  //AssertFatal(size >= 0, "invalid size!");
	  //AssertFatal((e==0) || (e==1), "invalid e!");
	  //AssertFatal((fi >= 0) && (fi <= 3), "invalid fi!");
	  //!如果当前的SN 的PDU 中的header参数异常，或者size 不对，则sn +1,处理下一个sn 
	  if((size < 0) || ((e!=0) && (e!=1)) || ((fi < 0) || (fi > 3))){
			LOG_ERROR(RLC_RX, "sn;%d, invalid size %d, e %d, fi %d\n", sn,size, e, fi);
			sn = (sn + 1) % rlc_pP->rx_sn_modulo;
			if ((sn == rlc_pP->vr_uh) || (sn == end_snP)) {
			  continue_reassembly = 0;
			}
			continue;
	  }

	  //!如果header之后是data field 
	  if (e == RLC_E_FIXED_PART_DATA_FIELD_FOLLOW) {
		switch (fi) {
			//!bit[1]表示fist byte， 0表示是，1表示不是
			//!bit[0]表示last byte,  0表示是，1表示不是 
			
			case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
			{

				  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU NO E_LI FI= (00) \n",
						PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

				  // one complete SDU
				  //LGrlc_um_send_sdu(rlc_pP,ctxt_pP->frame,ctxt_pP->enb_flag); // may be not necessary
				  //! 这包PDU中的数据中，PDU是整个的SDU 
				  rlc_um_clear_rx_sdu(ctxt_pP, rlc_pP);
				  //！这里由于没有扩展部分，因此data_p 指向的就是data field 
				  //! 将data_p的数据往SDU 的接收buffer中放
				  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size);
				  
				  rlc_um_send_sdu(ctxt_pP, rlc_pP); //!将当前SDU中的数据发送给PDCP 
				  rlc_pP->reassembly_missing_sn_detected = 0;

				  break;
	        }
			case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
			{

				  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU NO E_LI FI= (01) \n",
						PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

				  // one beginning segment of SDU in PDU
				  //LG rlc_um_send_sdu(rlc_pP,ctxt_pP->frame,ctxt_pP->enb_flag); // may be not necessary
				  //!当前PDU中的first byte是SDU的first byte,但是结尾不是last,因此这个PDU只是一个SDU的开头
				  //!由于E =0表示后面是数据域，因此这里的PDU 只能是一个SDU的部分数据 
				  rlc_um_clear_rx_sdu(ctxt_pP, rlc_pP);
				  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size); 
				  //！这里只完成data 的copy，但是不向上层发送
				  rlc_pP->reassembly_missing_sn_detected = 0;
				  break;
	        }
			case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
			{

				  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU NO E_LI FI= (10) \n",
						PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));


				  // one last segment of SDU
				  if (rlc_pP->reassembly_missing_sn_detected == 0) {
					//！最后一个byte接收到，则上报SDU 
					rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size);
					rlc_um_send_sdu(ctxt_pP, rlc_pP);
				  } else {
					//clear sdu already done
					rlc_pP->stat_rx_data_pdu_dropped += 1;
					rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;
				  }

				  rlc_pP->reassembly_missing_sn_detected = 0;
				  break;
	         }
			case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
			{

				  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU NO E_LI FI= (11)\n",
						PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));


				  if (rlc_pP->reassembly_missing_sn_detected == 0) {
					// one whole segment of SDU in PDU
					//！过来的是一个SDU 的中间部分数据，直接copy 
					rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size);
				  } else {
				   //！如果reassembly_missing_sn_detected = 1,说明之前的包丢失了，没有收到包含有数据头的包

					LOG_WARN(RLC, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU NO E_LI FI=00 (11) MISSING SN DETECTED\n",
						  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

					//LOG_DEBUG(RLC_RX, "[MSC_NBOX][FRAME %05u][%s][RLC_UM][MOD %u/%u][RB %u][Missing SN detected][RLC_UM][MOD %u/%u][RB %u]\n",
					//		ctxt_pP->frame, rlc_pP->module_id,rlc_pP->rb_id, rlc_pP->module_id,rlc_pP->rb_id);
					rlc_pP->reassembly_missing_sn_detected = 1; // not necessary but for readability of the code
					rlc_pP->stat_rx_data_pdu_dropped += 1;
					rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;

					AssertFatal( rlc_pP->reassembly_missing_sn_detected == 1,RLC,
								 PROTOCOL_RLC_UM_CTXT_FMT" MISSING PDU DETECTED (%s:%u)\n",
								 PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
								 __FILE__,
								 __LINE__);

				  }

				  break;
	        }
			default:
			{
				  LOG_ERROR(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" fi=%d! TRY REASSEMBLY SHOULD NOT GO HERE\n",
							 PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP), fi);
			      break; 
		    }	  
		 }
	  }
	  else {
	   //!固定header 之后跟着的是扩展部分E+LI
	   //!这里的size 是去掉了固定header 2byte之后的size 
	   //！将LI存放再li_array中, 
	   //! 运行完此函数之后，data_p  指向了SDU data filed 
		if (rlc_um_read_length_indicators(&data_p, e_li_p, li_array, &num_li, &size ) >= 0) {
			  switch (fi) {
			  case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
			  {

					LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU FI=11 (00) Li=",
						  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

					for (i=0; i < num_li; i++) {
					  LOG_DEBUG(RLC_RX, "%d ",li_array[i]);
					}

					LOG_DEBUG(RLC_RX, " remaining size %d\n",size);

					// N complete SDUs
					//LGrlc_um_send_sdu(rlc_pP,ctxt_pP->frame,ctxt_pP->enb_flag);
					rlc_um_clear_rx_sdu(ctxt_pP, rlc_pP);

					//！此时第一个byte是一个SDU的first byte
					//！ 并且最后一个Byte是一个SDU的last byte 
					//!说明存在多个完整的SDU,将多个SDU 逐个的上报
					for (i = 0; i < num_li; i++) {
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, li_array[i]);
					  rlc_um_send_sdu(ctxt_pP, rlc_pP);
					  data_p = &data_p[li_array[i]]; //!更新地址
					}
					//！最后一个data segment 没有LI,所以直接处理，
					//！这里的size 是已经从总的大小中减去了所有LI 的值
					if (size > 0) { // normally should always be > 0 but just for help debug
					  // data_p is already ok, done by last loop above
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size);
					  rlc_um_send_sdu(ctxt_pP, rlc_pP);  //!最后一个data segment 也是完整的PDU 
					}

					rlc_pP->reassembly_missing_sn_detected = 0;
					break;
			  }

			  case RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
			  {

					LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU FI=10 (01) Li=",
						  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

					for (i=0; i < num_li; i++) {
					  LOG_DEBUG(RLC_RX, "%d ",li_array[i]);
					}

					LOG_DEBUG(RLC_RX, " remaining size %d\n",size);

					// N complete SDUs + one segment of SDU in PDU
					//LG rlc_um_send_sdu(rlc_pP,ctxt_pP->frame,ctxt_pP->enb_flag);
					rlc_um_clear_rx_sdu(ctxt_pP, rlc_pP);

					 //!前面的SDU都是完整的，直接整理并发送
					for (i = 0; i < num_li; i++) {
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, li_array[i]);
					  rlc_um_send_sdu(ctxt_pP, rlc_pP);
					  data_p = &data_p[li_array[i]];
					}
					//！最后一个data segment是不完整的SDU，所以只copy数据，不上报
					if (size > 0) { // normally should always be > 0 but just for help debug
					  // data_p is already ok, done by last loop above
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size);
					}

					rlc_pP->reassembly_missing_sn_detected = 0; //！这里要清0
					break;
	          }
			  case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU:
			  {

					LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU FI=01 (10) Li=",
						  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

					for (i=0; i < num_li; i++) {
					  LOG_DEBUG(RLC_RX, "%d ",li_array[i]);
					}

					LOG_DEBUG(RLC_RX, " remaining size %d\n",size);

					//!因为这里有多个segment ,也就是在一个PDU上有多个SDU,并且最后一个byte是SDU的last byte
					//!因此这里只能是第一个segment出现了丢包，其他的SDU 包都是完整的。
					if (rlc_pP->reassembly_missing_sn_detected) {  //如果丢包过，则丢弃这个SDU 
					  reassembly_start_index = 1;
					  data_p = &data_p[li_array[0]];
					  //rlc_pP->stat_rx_data_pdu_dropped += 1;
					  rlc_pP->stat_rx_data_bytes_dropped += li_array[0];
					} else {
					  reassembly_start_index = 0;
					}

					// one last segment of SDU + N complete SDUs in PDU
					for (i = reassembly_start_index; i < num_li; i++) {
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, li_array[i]);
					  rlc_um_send_sdu(ctxt_pP, rlc_pP);
					  data_p = &data_p[li_array[i]];
					}
				   
					if (size > 0) { // normally should always be > 0 but just for help debug
					  // data_p is already ok, done by last loop above
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size);
					  rlc_um_send_sdu(ctxt_pP, rlc_pP);
					}

					rlc_pP->reassembly_missing_sn_detected = 0;
					break;
	          }
			  case RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU:
			  {

					LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TRY REASSEMBLY PDU FI=00 (11) Li=",
						  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
						  num_li);

					for (i=0; i < num_li; i++) {
					  LOG_DEBUG(RLC_RX, "%d ",li_array[i]);
					}

					LOG_DEBUG(RLC_RX, " remaining size %d\n",size);

					 //！这种情况，说明第一个包是个半截包，最后一个包也是个半截包，中间的包是完整的
					 //! 如果出现了丢包，那么只能是第一个包丢了
					if (rlc_pP->reassembly_missing_sn_detected) {

					  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" DISCARD FIRST LI %d \n",
							PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
							li_array[0]);

					  reassembly_start_index = 1;
					  data_p = &data_p[li_array[0]];
					  //rlc_pP->stat_rx_data_pdu_dropped += 1;
					  rlc_pP->stat_rx_data_bytes_dropped += li_array[0];
					} else {
					  reassembly_start_index = 0;
					}
					 //！将中间的包都上报
					for (i = reassembly_start_index; i < num_li; i++) {
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, li_array[i]);
					  rlc_um_send_sdu(ctxt_pP, rlc_pP);
					  data_p = &data_p[li_array[i]];
					}

					if (size > 0) { // normally should always be > 0 but just for help debug
					  // data_p is already ok, done by last loop above
					  rlc_um_reassembly (ctxt_pP, rlc_pP, data_p, size); //！最后一个包是半截包 
					} 
					else 
					{
					  AssertFatal( 0 , RLC, PROTOCOL_RLC_UM_CTXT_FMT" size=%d! SHOULD NOT GO HERE \n",
					  			 PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
					  			 size);
					  
					  
					  //rlc_pP->stat_rx_data_pdu_dropped += 1;
					  rlc_pP->stat_rx_data_bytes_dropped += size;
					}

					rlc_pP->reassembly_missing_sn_detected = 0;
					break;
	          }
			  default:
			  {

						LOG_WARN(RLC, PROTOCOL_RLC_UM_CTXT_FMT" Missing SN detected \n",
							  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

						rlc_pP->stat_rx_data_pdu_dropped += 1;
						rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;

						rlc_pP->reassembly_missing_sn_detected = 1;

						AssertFatal( rlc_pP->reassembly_missing_sn_detected == 1,RLC, 
									 PROTOCOL_RLC_UM_CTXT_FMT" MISSING PDU DETECTED (%s:%u)\n",
									 PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
									 __FILE__,
									 __LINE__);
								

				}
			  }
		} 
		else 
		{ //!HEAER 信息解析出来发现有错误，直接将整个SN包丢弃
			  rlc_pP->stat_rx_data_pdu_dropped += 1;
			  rlc_pP->stat_rx_data_bytes_dropped += tb_ind_p->size;
			  rlc_pP->reassembly_missing_sn_detected = 1;
			   //！header错了
			  LOG_ERROR(RLC_RX, "[SN %d] Bad RLC header! Discard this RLC PDU (size=%d)\n", sn, size);
		}
	  }

	   //！将这个SN对应的memory 从dar_buffer中释放
	  free_mem_block(rlc_pP->dar_buffer[sn], __func__);
	  rlc_pP->dar_buffer[sn] = NULL;
	} 
	else
	{
		 //！如果从dar_buffer中没有找到SN对应的data,则认为丢失了data 
		  rlc_pP->last_reassemblied_missing_sn = sn;
		  rlc_pP->reassembly_missing_sn_detected = 1;  //!置位flag 
		  rlc_um_clear_rx_sdu(ctxt_pP, rlc_pP);

	}

	
	LOG_ERROR(RLC_RX, "func:%s, reassembly sdu sn:%d finished\n",
    			__func__, 
    		     sn );

	//!SN 向上加，
	sn = (sn + 1) % rlc_pP->rx_sn_modulo;

	if ((sn == rlc_pP->vr_uh) || (sn == end_snP)) {
	  continue_reassembly = 0;
	}
  }

}
//-----------------------------------------------------------------------------
void   rlc_um_stop_and_reset_timer_reordering(const protocol_ctxt_t* const ctxt_pP,
													rlc_um_entity_t * 		   rlc_pP)
{

	LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" [T-REORDERING] STOPPED AND RESET\n",
		PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));

	rlc_pP->t_reordering.running		   = 0;
	rlc_pP->t_reordering.ms_time_out	   = 0;
	rlc_pP->t_reordering.ms_start 	   = 0;
	rlc_pP->t_reordering.timed_out	   = 0;
}
//-----------------------------------------------------------------------------
void   rlc_um_start_timer_reordering(const protocol_ctxt_t* const ctxt_pP,
										rlc_um_entity_t * 		   rlc_pP)
{
	rlc_pP->t_reordering.timed_out	   = 0;

	if (rlc_pP->t_reordering.ms_duration > 0) {
		rlc_pP->t_reordering.running		   = 1;
		//!设置durtion 之后超时
		rlc_pP->t_reordering.ms_time_out	  = PROTOCOL_CTXT_TIME_MILLI_SECONDS(ctxt_pP) + rlc_pP->t_reordering.ms_duration;
		 //！设置起始时刻从当前帧开始
		rlc_pP->t_reordering.ms_start		 = PROTOCOL_CTXT_TIME_MILLI_SECONDS(ctxt_pP);

		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" [T-REORDERING] STARTED (TIME-OUT = FRAME %05u)\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			rlc_pP->t_reordering.ms_time_out);

	} else {
		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"[T-REORDERING] NOT STARTED, CAUSE CONFIGURED 0 ms\n",
		     PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,rlc_pP));
	}
}
//-----------------------------------------------------------------------------
mem_block_t* rlc_um_get_pdu_from_dar_buffer(const protocol_ctxt_t* const ctxt_pP, rlc_um_entity_t * const rlc_pP, rlc_usn_t snP)
{
	return rlc_pP->dar_buffer[snP];
}
//-----------------------------------------------------------------------------
//!
//
void rlc_um_check_timer_dar_time_out(const protocol_ctxt_t* const ctxt_pP,
											   rlc_um_entity_t * const rlc_pP)
{
	signed int	 in_window;
	rlc_usn_t 	 old_vr_ur;




	if ((rlc_pP->t_reordering.running)) {
		if (
		//!下面这两种情况都算超时 
	  // CASE 1:		  start 			 time out
	  //		+-----------+------------------+----------+
	  //		|			|******************|		  |
	  //		+-----------+------------------+----------+
	  //FRAME # 0									  FRAME MAX
	  ((rlc_pP->t_reordering.ms_start < rlc_pP->t_reordering.ms_time_out) &&
	   ((PROTOCOL_CTXT_TIME_MILLI_SECONDS(ctxt_pP) >= rlc_pP->t_reordering.ms_time_out) ||
		(PROTOCOL_CTXT_TIME_MILLI_SECONDS(ctxt_pP) < rlc_pP->t_reordering.ms_start)))	   ||
	  // CASE 2:		time out			start
	  //		+-----------+------------------+----------+
	  //		|***********|				   |**********|
	  //		+-----------+------------------+----------+
	  //FRAME # 0									  FRAME MAX VALUE
	  ((rlc_pP->t_reordering.ms_start > rlc_pP->t_reordering.ms_time_out) &&
	   (PROTOCOL_CTXT_TIME_MILLI_SECONDS(ctxt_pP) < rlc_pP->t_reordering.ms_start) &&
	   (PROTOCOL_CTXT_TIME_MILLI_SECONDS(ctxt_pP) >= rlc_pP->t_reordering.ms_time_out))
	) {

	  //if ((uint32_t)((uint32_t)rlc_pP->timer_reordering  + (uint32_t)rlc_pP->timer_reordering_init)	<= ctxt_pP->frame) {
	  // 5.1.2.2.4	 Actions when t-Reordering expires
	  //  When t-Reordering expires, the receiving UM RLC entity shall:
	  //  -update VR(UR) to the SN of the first UMD PDU with SN >= VR(UX) that has not been received;
	  //  -reassemble RLC SDUs from any UMD PDUs with SN < updated VR(UR), remove RLC headers when doing so and deliver the reassembled 
	   //!RLC SDUs to upper layer in ascending order of the RLC SN if not delivered before;
	  //  -if VR(UH) > VR(UR):
	  //	  -start t-Reordering;
	  //	  -set VR(UX) to VR(UH).
	  rlc_pP->stat_timer_reordering_timed_out += 1;
#if TRACE_RLC_PAYLOAD
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"*****************************************************\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"*    T I M E	-  O U T							  *\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"*****************************************************\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TIMER t-Reordering expiration\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP));
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" timer_reordering=%d frame=%d expire ms %d\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			rlc_pP->t_reordering.ms_duration,
			ctxt_pP->frame,
			rlc_pP->t_reordering.ms_time_out);
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" set VR(UR)=%03d to",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			rlc_pP->vr_ur);
#endif

  if (pthread_mutex_trylock(&rlc_pP->lock_dar_buffer) == 0) {
	//！超时后的处理，将Ux之前的PDU都不再接收了，要么上报，要么丢弃
	old_vr_ur	= rlc_pP->vr_ur;

	//从ux开始，往上一直找一个没有接收到的SN，作为新的UR,更新UR,后续就处理新的
	//!UR以下的SDU 
	rlc_pP->vr_ur = rlc_pP->vr_ux; 
	while (rlc_um_get_pdu_from_dar_buffer(ctxt_pP, rlc_pP, rlc_pP->vr_ur)) {
	  rlc_pP->vr_ur = (rlc_pP->vr_ur+1)%rlc_pP->rx_sn_modulo;
	}

#if TRACE_RLC_PAYLOAD
	LOG_DEBUG(RLC_RX, " %d", rlc_pP->vr_ur);
	LOG_DEBUG(RLC_RX, "\n");
#endif
	//！开始处理更新后的UR以下的SDU ，从旧的UR 开始一直处理到新的UR 
	rlc_um_try_reassembly(ctxt_pP, rlc_pP ,old_vr_ur, rlc_pP->vr_ur);

	//!这里需要判断uh>ur, 并且uh = uh时就会返回2，所以根据协议，要重启timer,并更新ux = uh 
	in_window = rlc_um_in_window(ctxt_pP, rlc_pP, rlc_pP->vr_ur,  rlc_pP->vr_uh,  rlc_pP->vr_uh);
	 //!
	if (in_window == 2) {
	  rlc_um_start_timer_reordering(ctxt_pP, rlc_pP); //！重启timer 
	  rlc_pP->vr_ux = rlc_pP->vr_uh; 
#if TRACE_RLC_PAYLOAD
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" restarting t-Reordering set VR(UX) to %d (VR(UH)>VR(UR))\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			rlc_pP->vr_ux);
#endif
	} else {
#if TRACE_RLC_PAYLOAD
	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" STOP t-Reordering VR(UX) = %03d\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			rlc_pP->vr_ux);
#endif
	  rlc_um_stop_and_reset_timer_reordering(ctxt_pP, rlc_pP);
	}

	RLC_UM_MUTEX_UNLOCK(&rlc_pP->lock_dar_buffer);
  }
}
}

}
//-----------------------------------------------------------------------------
mem_block_t*    rlc_um_remove_pdu_from_dar_buffer(const protocol_ctxt_t* const ctxt_pP,
															  rlc_um_entity_t * const rlc_pP,
															  rlc_usn_t snP)
{
	mem_block_t * pdu_p	  = rlc_pP->dar_buffer[snP];

	LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" REMOVE PDU FROM DAR BUFFER  SN=%03d\n",
		PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
		snP);

	rlc_pP->dar_buffer[snP] = NULL;
	return pdu_p;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void   rlc_um_store_pdu_in_dar_buffer(const protocol_ctxt_t* const ctxt_pP,
											rlc_um_entity_t * const rlc_pP,
											mem_block_t *pdu_pP,
											rlc_usn_t snP)
{

	LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" STORE PDU IN DAR BUFFER:SN= %d  VR(UR)= %d VR(UX)= %d VR(UH)= %d\n",
		PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
		snP,
		rlc_pP->vr_ur,
		rlc_pP->vr_ux,
		rlc_pP->vr_uh);

	rlc_pP->dar_buffer[snP] = pdu_pP;
}


//! recoding window, 按照协议，[UH -window_size: UH]

signed int   rlc_um_in_reordering_window(const protocol_ctxt_t* const ctxt_pP,
									  	  rlc_um_entity_t * const rlc_pP,
										  const rlc_sn_t snP)
{
	rlc_sn_t   modulus = (signed int)rlc_pP->vr_uh - rlc_pP->rx_um_window_size;
	rlc_sn_t   sn_mod = (snP - modulus) % rlc_pP->rx_sn_modulo;

	if ( 0 <= sn_mod) {  //在recoding window的下边界之上
	//在recoding window 以内，返回0 
		if (sn_mod < rlc_pP->rx_um_window_size) {

		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" %d IN REORDERING WINDOW[%03d:%03d) [SN %d IN [%03d:%03d) VR(UR)=%03d VR(UH)=%03d\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  sn_mod,
			  0,
			  rlc_pP->rx_um_window_size,
			  snP,
			  (signed int)rlc_pP->vr_uh - rlc_pP->rx_um_window_size,
			  rlc_pP->vr_uh,
			  rlc_pP->vr_ur,
			  rlc_pP->vr_uh);

		return 0;
		}
	}


if (modulus < 0) { //！uh 还小于512，这里的接收窗是个循环窗，所以此时的接收窗的边界应该是：[uh + 512,...uh]
	LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"modules < 0, sn_mod: %d NOT IN REORDERING WINDOW[%03d:%03d) [ SN %d NOT IN [%03d:%03d) VR(UR)=%03d VR(UH)=%03d\n",
		PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
		sn_mod,
		0,
		(rlc_pP->vr_uh - modulus)%1024,
		snP,
		modulus ,
		rlc_pP->vr_uh,
		rlc_pP->vr_ur,
		rlc_pP->vr_uh);
	} else { //！uh 大于512了，此时的接收窗的大小是[uh-512,uh]
	LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT"modules >0, sn_mod: %d NOT IN REORDERING WINDOW[%03d:%03d) [SN %d NOT IN [%03d:%03d) VR(UR)=%03d VR(UH)=%03d\n",
		PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
		sn_mod,
		modulus,
		rlc_pP->rx_um_window_size,
		snP,
		modulus ,
		rlc_pP->vr_uh,
		rlc_pP->vr_ur,
		rlc_pP->vr_uh);
	}


	return -1;
}



void   rlc_um_receive_process_dar (const protocol_ctxt_t* const ctxt_pP,
										rlc_um_entity_t * const 	 rlc_pP,
										mem_block_t *				 pdu_mem_pP,
										rlc_um_pdu_sn_10_t * const	 pdu_pP,
										const sdu_size_t			 tb_sizeP)
{
// 36.322v9.3.0 section 5.1.2.2.1:
// The receiving UM RLC entity shall maintain a reordering window according to state variable VR(UH) as follows:
//		-a SN falls within the reordering window if (VR(UH) – UM_Window_Size) <= SN < VR(UH);
//		-a SN falls outside of the reordering window otherwise.
// When receiving an UMD PDU from lower layer, the receiving UM RLC entity shall:
//		-either discard the received UMD PDU or place it in the reception buffer (see sub clause 5.1.2.2.2);
//		-if the received UMD PDU was placed in the reception buffer:
//			-update state variables, reassemble and deliver RLC SDUs to upper layer and start/stop t-Reordering as needed (see sub clause 5.1.2.2.3);
// When t-Reordering expires, the receiving UM RLC entity shall:
// -   update state variables, reassemble and deliver RLC SDUs to upper layer and start t-Reordering as needed (see sub clause 5.1.2.2.4).



// When an UMD PDU with SN = x is received from lower layer, the receiving UM RLC entity shall:
// -if VR(UR) < x < VR(UH) and the UMD PDU with SN = x has been received before; or
// -if (VR(UH) – UM_Window_Size) <= x < VR(UR):
//		-discard the received UMD PDU;
// -else:
//		-place the received UMD PDU in the reception buffer.

	rlc_sn_t sn = -1;
	signed int in_window = 0;
	uint32_t  state_index = 0; 
    char *sdu_handle_state[3] = {
		"sn < vr_ur,droped", 
		"vr_ur <= sn < vr_uh, but duplicated",
		"sn == vr_ur, reassmbly handled "
    };


	if (rlc_pP->rx_sn_length == 10) {
	  sn = ((pdu_pP->b1 & 0x00000003) << 8) + pdu_pP->b2; //!获取SN 
	} else if (rlc_pP->rx_sn_length == 5) {
	  sn = pdu_pP->b1 & 0x1F;
	} else {
	  free_mem_block(pdu_mem_pP, __func__);
	}

//	RLC_UM_MUTEX_LOCK(&rlc_pP->lock_dar_buffer, ctxt_pP, rlc_pP); //!加线程锁
	 //！vr_ur 表示的是还没有收到PDU 的最小SN号
	 //! 这里调用rlc_um_in_window 是用来判断： PDU 是否在[uh-window_size,ur] 这个范围内,
	in_window = rlc_um_in_window(ctxt_pP, rlc_pP, rlc_pP->vr_uh - rlc_pP->rx_um_window_size, sn, rlc_pP->vr_ur);

	LOG_DEBUG(RLC_RX, "judge 1: SN:%d, in window[uh - winsize:ur][%d--%d], result = %d \n", 
					sn,
					rlc_pP->vr_uh - rlc_pP->rx_um_window_size,
					rlc_pP->vr_ur,
					in_window);

#if TRACE_RLC_PAYLOAD
	rlc_util_print_hex_octets(RLC, &pdu_pP->b1, tb_sizeP);
#endif

    AssertFatal(in_window != -2, RLC, "SN is lower than vr_uh - windowssize, the SDU maybe retransmited SDU,drop it\n"); 
    
    // rlc_um_in_window() returns -2 if lower_bound  > sn
	// rlc_um_in_window() returns -1 if higher_bound < sn
	// rlc_um_in_window() returns  0 if lower_bound  < sn < higher_bound
	// rlc_um_in_window() returns  1 if lower_bound  == sn
	// rlc_um_in_window() returns  2 if higher_bound == sn
	// rlc_um_in_window() returns  3 if higher_bound == sn == lower_bound
	if ((in_window == 1) || (in_window == 0)) { 

	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" RX PDU  VR(UH) – UM_Window_Size) <= SN %d < VR(UR) -> GARBAGE\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			sn);
			
	  //! 这里说明SN <= UR, 按照协议，UR 以下的PDU 应该不用再接收了， 
	  // 可能是收齐了，也可能是丢弃了，因此不再接收了，将buffer 释放掉即可，可能是对端重传了
	  rlc_pP->stat_rx_data_pdu_out_of_window   += 1; //！记录出窗的个数
	  rlc_pP->stat_rx_data_bytes_out_of_window += tb_sizeP;
	  free_mem_block(pdu_mem_pP, __func__);
	  pdu_mem_pP = NULL;
	  RLC_UM_MUTEX_UNLOCK(&rlc_pP->lock_dar_buffer);
	  state_index = 0; 

	  LOG_INFO(RLC_RX, "func:%s, received SN:%d, vr_vh:%d, vr_ur:%d, ur_ux:%d, handle_status:%s",
    			__func__, 
    		     sn, 
    		     rlc_pP->vr_uh, 
    		     rlc_pP->vr_ur,
    		     rlc_pP->vr_ux, 
    		     sdu_handle_state[state_index]);
	  
	  return;
	}

     //!从接收Buffer中获取到data ,说明之前已经收到过SN 的数据了
	if ((rlc_um_get_pdu_from_dar_buffer(ctxt_pP, rlc_pP, sn))) {

	  	LOG_DEBUG(RLC_RX, "judge 2: SN:%d, is a duplicate PDU or not:%d \n", 
						sn,
						1);
	   //!如果能从dar buffer 中获取到，并且PDU在[UR, UH ]这段窗内，说明之前已经收到过了，这次收到的是重复的PDU
	  in_window = rlc_um_in_window(ctxt_pP, rlc_pP, rlc_pP->vr_ur, sn, rlc_pP->vr_uh);
	  	 LOG_DEBUG(RLC_RX, "judge 3: SN:%d in window[ur--uh][%d--%d], result = %d", 
						sn,
						rlc_pP->vr_ur,
						rlc_pP->vr_uh,
						in_window);

	  if (in_window == 0) {  //如果在，说明重复了，也要丢弃

		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" RX PDU  VR(UR) < SN %d < VR(UH) and RECEIVED BEFORE-> GARBAGE\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  sn);

		//!discard the PDU
		rlc_pP->stat_rx_data_pdus_duplicate  += 1;	//！记录重复的个数
		rlc_pP->stat_rx_data_bytes_duplicate += tb_sizeP;
		free_mem_block(pdu_mem_pP, __func__);
		pdu_mem_pP = NULL;
		RLC_UM_MUTEX_UNLOCK(&rlc_pP->lock_dar_buffer);
        state_index = 1; 
		LOG_INFO(RLC_RX, "func:%s, received SN:%d, vr_vh:%d, vr_ur:%d, ur_ux:%d, handle_status:%s",
    			__func__, 
    		     sn, 
    		     rlc_pP->vr_uh, 
    		     rlc_pP->vr_ur,
    		     rlc_pP->vr_ux, 
    		     sdu_handle_state[state_index]);
		
		return;
	  }

	   //如果不在[UR,UH]这段内，但是能从dar_buffer中获取，那么也是重复的，

	  // 2 lines to avoid memory leaks
	  rlc_pP->stat_rx_data_pdus_duplicate  += 1;
	  rlc_pP->stat_rx_data_bytes_duplicate += tb_sizeP;

	  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" RX PDU SN %03d REMOVE OLD PDU BEFORE STORING NEW PDU\n",
			PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			sn);

	  mem_block_t *pdu = rlc_um_remove_pdu_from_dar_buffer(ctxt_pP, rlc_pP, sn); //！将重复的PDU的old 数据丢弃
	  free_mem_block(pdu, __func__);
	}

    //!前面已经判断了两种场景： 
    //! case 1: SN 属于[VH- windowssize, Vr), 释放内存，直接return
    //! case 2: SN 属于[VR,VH],并且已经收到过了，则释放内存，直接return; 
    //! 排除掉上述情况，则相当于SN 属于[VR, VH],并且是第一次收到，则将数据放入buffer中
	rlc_um_store_pdu_in_dar_buffer(ctxt_pP, rlc_pP, pdu_mem_pP, sn);

    /*****************************完成了对接收SDU 的判断，是否应该丢弃还是应该存入到接收buffer中*********************************/

    /*****************************下面开始对接收Buffer中的SDU 判断哪些应该处理并上传了*********************************/

	// -if x falls outside of the reordering window:
	//		-update VR(UH) to x + 1;
	//		-reassemble RLC SDUs from any UMD PDUs with SN that falls outside of
	//		 the reordering window, remove RLC headers when doing so and deliver
	//		 the reassembled RLC SDUs to upper layer in ascending order of the
	//		 RLC SN if not delivered before;
	//
	//		-if VR(UR) falls outside of the reordering window:
	//			-set VR(UR) to (VR(UH) – UM_Window_Size);

	 //!前面已经验证了SN 是否在UR 以下，是否是重复的 
	 //!如果SN 不在recording 窗内，返回-1，否则返回0
	 //!当初始化开始后，当sn =0,vh = 0，返回-1，然后更新vr_uh + 1,
	 //! 一直到sn = 512时，uh = 512,此时rlc_um_in_reordering_window 才能返回0 
	 //! 下面这段代码用于更新uh
	LOG_DEBUG(RLC_RX, "judge 4: whether SN in recording window or not \n"); 
	if (rlc_um_in_reordering_window(ctxt_pP, rlc_pP, sn) < 0) {

		//！这里也需要判断SN 在接收窗的上边界才能更新UH，但这里只需要判断不在窗内? 
		//！更新UH
		rlc_pP->vr_uh = (sn + 1) % rlc_pP->rx_sn_modulo;
        
        LOG_DEBUG(RLC_RX, "judge 4.1: SN:%d in not recording window,update UH = sn + 1: UH = %d  \n",
						sn,
						rlc_pP->vr_uh);

		LOG_DEBUG(RLC_RX, "judge 4.2: whether vr_ur:%d in  recording window or not after UH updated   \n",
						rlc_pP->vr_ur);
						
		//!<如果ur 在recording 窗外，则表示需要处理窗外的PDU了,处理vr_ur以下的SDU 。
		if (rlc_um_in_reordering_window(ctxt_pP, rlc_pP, rlc_pP->vr_ur) != 0) {
		  //如果UR 也被移出到窗外了，那么更新UR = UH - WINDOWSIZE 
		  //这里相当于是下边界
		  LOG_DEBUG(RLC_RX, "judge 4.3: vr_ur:%d in not recording window \n",rlc_pP->vr_ur); 
		 in_window = rlc_pP->vr_uh - rlc_pP->rx_um_window_size;

		if (in_window < 0) {
		  in_window = in_window + rlc_pP->rx_sn_modulo;
		}
		 //！处理从ur开始，依次SN 递增的处理
		 //!当初始时，ur = 0, in_window = 513,则滑动窗是[513--1023:0]
		 //!这里处理 ur 之下的SDU, 不包括UR           
		  LOG_DEBUG(RLC_RX, "judge 4.4: start to handle sn < UR:%d \n",rlc_pP->vr_ur); 
		rlc_um_try_reassembly(ctxt_pP, rlc_pP, in_window,rlc_pP->vr_ur);
	  }

	  //!判断vr 是否在windows中,注意此时的vh 已经更新了，所以此时这个函数vr_ur可能在窗内了。
	  if (rlc_um_in_reordering_window(ctxt_pP, rlc_pP, rlc_pP->vr_ur) < 0) {

		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" VR(UR) %d OUTSIDE REORDERING WINDOW AFTER UH update, SET UR TO VR(UH) – UM_Window_Size = \
%d, ur_uh = %d \n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  rlc_pP->vr_ur,
			  in_window,
			   rlc_pP->vr_uh);

		rlc_pP->vr_ur = in_window; //！更新ur = uh - window-size
	  }
	}

	// -if the reception buffer contains an UMD PDU with SN = VR(UR):
	//		-update VR(UR) to the SN of the first UMD PDU with SN > current
	//			VR(UR) that has not been received;
	//		-reassemble RLC SDUs from any UMD PDUs with SN < updated VR(UR),
	//			remove RLC headers when doing so and deliver the reassembled RLC
	//			SDUs to upper layer in ascending order of the RLC SN if not
	//			delivered before;

	//！如果sn =ur,并且ur的PDU已经收到过，则更新vr_ur 
	if ((sn == rlc_pP->vr_ur) && rlc_um_get_pdu_from_dar_buffer(ctxt_pP, rlc_pP, rlc_pP->vr_ur)) {
		

		
	  do { //！从ur 开始依次往上找，直到找到一个没有收到PDU的SN号，并且不能等于UH 
		rlc_pP->vr_ur = (rlc_pP->vr_ur+1) % rlc_pP->rx_sn_modulo;
	  } while (rlc_um_get_pdu_from_dar_buffer(ctxt_pP, rlc_pP, rlc_pP->vr_ur));  //! &&(rlc_pP->vr_ur != rlc_pP->vr_uh)

	  LOG_DEBUG(RLC_RX, "judge 5: sn == UR, and SN have stored in buffer,update the UR = %d ,and the handle PDU < UR \n", rlc_pP->vr_ur); 

	   //!将SN < 更新后的ur 的PDU,进行去header处理，从SN 往上处理，处理到更新后的UR结束。
	  rlc_um_try_reassembly(ctxt_pP, rlc_pP, sn, rlc_pP->vr_ur);

	  state_index = 2; 
	  LOG_ERROR(RLC_RX, "func:%s, received SN:%d, vr_vh:%d, vr_ur:%d, ur_ux:%d, handle_status:%s",
    			__func__, 
    		     sn, 
    		     rlc_pP->vr_uh, 
    		     rlc_pP->vr_ur,
    		     rlc_pP->vr_ux, 
    		     sdu_handle_state[state_index]);
	}

	// -if t-Reordering is running:
	//		-if VR(UX) <= VR(UR); or
	//		-if VR(UX) falls outside of the reordering window and VR(UX) is not
	//			equal to VR(UH)::
	//			-stop and reset t-Reordering;

	if (rlc_pP->t_reordering.running) {
	  // 如果ux != uh,并且在ux 在recording windows之外，那么停止并重置timer 
	  if (rlc_pP->vr_uh != rlc_pP->vr_ux) {
		in_window = rlc_um_in_reordering_window(ctxt_pP, rlc_pP, rlc_pP->vr_ux);

		if (in_window < 0) {

		  LOG_DEBUG(RLC_RX,
				PROTOCOL_RLC_UM_CTXT_FMT" STOP and RESET t-Reordering because VR(UX) falls outside of the reordering window and VR(UX)=%d \
	is not equal to VR(UH)=%d -or- VR(UX) <= VR(UR)\n",
				PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
				rlc_pP->vr_ux,
				rlc_pP->vr_uh);

		  rlc_um_stop_and_reset_timer_reordering(ctxt_pP, rlc_pP);
		}
	  }
	}

	if (rlc_pP->t_reordering.running) {
	  in_window = rlc_um_in_window(ctxt_pP, rlc_pP, rlc_pP->vr_ur,	rlc_pP->vr_ux,	rlc_pP->vr_uh);
	   //！ -2： ux < ur, 丢失ux,不用再运行timer 
	   //! 1:	 ux = ur ,表示已经收到了，也不用再运行了
	  if ((in_window == -2) ) {

		LOG_DEBUG(RLC_RX,
			  PROTOCOL_RLC_UM_CTXT_FMT" STOP and RESET t-Reordering because VR(UX) = %d falls outside of the reordering window[%d:%d) \n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  rlc_pP->vr_ux,
			  rlc_pP->vr_uh - rlc_pP->rx_um_window_size,
			  rlc_pP->vr_uh);
			  rlc_um_stop_and_reset_timer_reordering(ctxt_pP, rlc_pP);
	  }
	  else if (in_window == 1)
	  {
			LOG_DEBUG(RLC_RX,
			  PROTOCOL_RLC_UM_CTXT_FMT" STOP and RESET t-Reordering because VR(UX) =%d is equal to VR(UH)=%d\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  rlc_pP->vr_ux,
			  rlc_pP->vr_ur);
		      rlc_um_stop_and_reset_timer_reordering(ctxt_pP, rlc_pP);
	  }
	}

	// -if t-Reordering is not running (includes the case when t-Reordering is
	//		stopped due to actions above):
	//		-if VR(UH) > VR(UR):
	//			-start t-Reordering;
	//			-set VR(UX) to VR(UH).

	if (rlc_pP->t_reordering.running == 0) {
	  in_window = rlc_um_in_window(ctxt_pP, rlc_pP, rlc_pP->vr_ur,	rlc_pP->vr_uh,	rlc_pP->vr_uh);

	  if (in_window >= 2) {
		//!启动timer 
		rlc_um_start_timer_reordering(ctxt_pP, rlc_pP);
		rlc_pP->vr_ux = rlc_pP->vr_uh;	//！更新ux = uh
		LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" RESTART t-Reordering set VR(UX) to VR(UH) =%d\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP, rlc_pP),
			  rlc_pP->vr_ux);
		
	  }
	}

    
//	RLC_UM_MUTEX_UNLOCK(&rlc_pP->lock_dar_buffer); //！解锁

}


void   rlc_um_receive (const protocol_ctxt_t* const ctxt_pP,
							rlc_um_entity_t * const rlc_pP,
							struct mac_data_ind data_indP)
{

	mem_block_t 	      *tb_p  = NULL;
	uint8_t 			  *first_byte_p 	= NULL;
	uint16_t			   tb_size_in_bytes = 0;

	while ((tb_p = list_remove_head (&data_indP.data))) { //！按照链表从head开始依次处理

	  first_byte_p = ((struct mac_tb_ind *) (tb_p->data))->data_ptr;  //！数据首地址 
	  tb_size_in_bytes = ((struct mac_tb_ind *) (tb_p->data))->size;  //！tb size,unit:byte

	  rlc_pP->stat_rx_data_bytes += tb_size_in_bytes; //！统计接收Byte
	  rlc_pP->stat_rx_data_pdu	 += 1; //!接收到的PDU ++

	  if (tb_size_in_bytes > 0) {
		  rlc_um_receive_process_dar (ctxt_pP, rlc_pP, tb_p, (rlc_um_pdu_sn_10_t*)first_byte_p, tb_size_in_bytes);

	  }
	}
}
 


void	rlc_um_rx (const protocol_ctxt_t *const ctxt_pP, void *argP, struct mac_data_ind data_indP) 
{
	rlc_um_entity_t    *l_rlc_p = (rlc_um_entity_t *) argP;

	mem_block_t 	   *tb_p;
	int16_t 			  tb_size_in_bytes;

	rlc_um_pdu_info_t	pdu_info;



	switch (l_rlc_p->protocol_state) {
	  case RLC_NULL_STATE:
		// from 3GPP TS 25.322 V9.2.0 p43
		// In the NULL state the RLC entity does not exist and therefore it is
		// not possible to transfer any data through it.
		// Upon reception of a CRLC-CONFIG-Req from upper layer indicating
		// establishment, the RLC entity:
		//	 - is created; and
		//	 - enters the DATA_TRANSFER_READY state.
		LOG_INFO(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" ERROR MAC_DATA_IND IN RLC_NULL_STATE\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,l_rlc_p));

		if (data_indP.data.nb_elements > 0) {
		  tb_p = data_indP.data.head;

		  while (tb_p != NULL) 
		  { 
			   //！对每一个MAC 给过来的PDU 都进行同样的处理，得到PDU header信息
				tb_size_in_bytes   = ((struct mac_tb_ind *) (tb_p->data))->size;  //!TB块的大小,byte:unit
				// 从MAC 给的Buffer中获取PDU haader信息，包括E,FI,LI+E,等header信息，还包括data field size等。 
				//!(struct mac_tb_ind *) (tb_p->data))->data_ptr 指向的是MAC 上报的SDU 的数据位置
				rlc_um_get_pdu_infos(ctxt_pP,
									 l_rlc_p,
									(rlc_um_pdu_sn_10_t *) ((struct mac_tb_ind *) (tb_p->data))->data_ptr,
									tb_size_in_bytes, 
									&pdu_info, 
									l_rlc_p->rx_sn_length);
				
			
			}/*MESSAGE_CHART_GENERATOR*/
		}

		list_free (&data_indP.data); //将整个链表释放
		break;

	  case RLC_DATA_TRANSFER_READY_STATE:
		// from 3GPP TS 25.322 V9.2.0 p43-44
		// In the DATA_TRANSFER_READY state, unacknowledged mode data can be
		// exchanged between the entities according to subclause 11.2.
		// Upon reception of a CRLC-CONFIG-Req from upper layer indicating
		// release, the RLC entity:
		// -enters the NULL state; and
		// -is considered as being terminated.
		// Upon reception of a CRLC-CONFIG-Req from upper layer indicating
		// modification, the RLC entity:
		// - stays in the DATA_TRANSFER_READY state;
		// - modifies only the protocol parameters and timers as indicated by
		// upper layers.
		// Upon reception of a CRLC-SUSPEND-Req from upper layers, the RLC
		// entity:
		// - enters the LOCAL_SUSPEND state.
		data_indP.tb_size = data_indP.tb_size >> 3;//!按照byte 

		if (data_indP.data.nb_elements > 0) {
		  LOG_DEBUG(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" MAC_DATA_IND %d TBs\n",
				PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,l_rlc_p),
				data_indP.data.nb_elements);
		  tb_p = data_indP.data.head;

		  while (tb_p != NULL) {
			//!<每一个节点都进行header的解析
				tb_size_in_bytes   = ((struct mac_tb_ind *) (tb_p->data))->size; //TB 块的大小
				rlc_um_get_pdu_infos(ctxt_pP,
									 l_rlc_p,(rlc_um_pdu_sn_10_t *) ((struct mac_tb_ind *) (tb_p->data))->data_ptr,
									 tb_size_in_bytes,
									 &pdu_info,
									 l_rlc_p->rx_sn_length);
				

				tb_p = tb_p->next;
		  }
		}

		rlc_um_receive (ctxt_pP, l_rlc_p, data_indP);
		break;

	  case RLC_LOCAL_SUSPEND_STATE:
		// from 3GPP TS 25.322 V9.2.0 p44
		// In the LOCAL_SUSPEND state, the RLC entity is suspended, i.e. it does
		// not send UMD PDUs with "Sequence Number" greater than or equal to a
		// certain specified value (see subclause 9.7.5).
		// Upon reception of a CRLC-CONFIG-Req from upper layer indicating
		// release, the RLC entity:
		// - enters the NULL state; and
		// - is considered as being terminated.
		// Upon reception of a CRLC-RESUME-Req from upper layers, the RLC entity:
		// - enters the DATA_TRANSFER_READY state; and
		// - resumes the data transmission.
		// Upon reception of a CRLC-CONFIG-Req from upper layer indicating
		// modification, the RLC entity:
		// - stays in the LOCAL_SUSPEND state;
		// - modifies only the protocol parameters and timers as indicated by
		//	 upper layers.
		LOG_INFO(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" RLC_LOCAL_SUSPEND_STATE\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,l_rlc_p));
		list_free (&data_indP.data);
		break;

	  default:
		LOG_ERROR(RLC_RX, PROTOCOL_RLC_UM_CTXT_FMT" TX UNKNOWN PROTOCOL STATE %02X hex\n",
			  PROTOCOL_RLC_UM_CTXT_ARGS(ctxt_pP,l_rlc_p),
			  l_rlc_p->protocol_state);
		list_free (&data_indP.data);
	}
}





void  rlc_um_mac_data_indication(const protocol_ctxt_t *const ctxt_pP, void *rlc_pP, struct mac_data_ind data_indP)
{
  //！对MAC的PDU 进行接收窗处理，并处理可以上传的PDU到上层，将PDU去掉header把数据copy到上层buffer 中
  //！这里的上层是PDCP
  rlc_um_rx (ctxt_pP, rlc_pP, data_indP);
  //！检查是否timer超时，如果超时，根据协议处理，更新ux,ur,uh等
  rlc_um_check_timer_dar_time_out(ctxt_pP, rlc_pP);

}

