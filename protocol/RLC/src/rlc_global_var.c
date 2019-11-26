/******************************************************************
 * @file  rlc_global_var.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月5日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月5日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc_type.h> 
#include <pthread.h>
#include <rlc.h>
 
 
protocol_ctxt_t    g_rlc_protocol_ctxt; 

pthread_mutex_t    g_rlc_buffer_mutex = PTHREAD_MUTEX_INITIALIZER; 



rlc_buffer_status   g_rlc_buffer_status[D2D_MAX_USER_NUM+1]; 


uint8_t            g_rlc_pdu_buffer[D2D_MAX_USER_NUM *  MAX_DLSCH_PAYLOAD_BYTES]; 

//!(4 logic chanenl  + 1pading) * max 3byte's header
uint8_t            g_rlc_mac_subheader[D2D_MAX_USER_NUM * ((MAX_LOGICCHAN_NUM  + 1)* 3)]; 
mac_pdu_size_para  g_rlc_pdu_size_para[D2D_MAX_USER_NUM]; 

struct mac_data_req g_rlc_mac_data_req;

#ifdef RLC_UT_DEBUG 

uint32_t   g_rlc_no_data_transfer = 0; 

#endif 

 
/**************************function******************************/
