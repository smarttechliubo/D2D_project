/** @file interface_mac_rlc.h
 *
 *  @brief
 *  @author bo.liu
 *  @date   2019/07/29
 *  COPYRIGHT NOTICE: (c) smartlogictech.
 *  All rights reserved.
 */

#ifndef INC_INTERFACE_MAC_RLC_H_
#define INC_INTERFACE_MAC_RLC_H_

#include <typedef.h>
#include <d2d_system_global.h>

/**MAC_RLC_BUF_STATUS_REQ**/
typedef struct mac_rlc_buf_status_req_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/

	//uint8_t logicchannel_id[MAX_LOGICCHAN_NUM];
	//uint16_t rlc_entity_type[MAX_LOGICCHAN_NUM]; /**1:TM;2:UM; 3:AM */

}mac_rlc_buf_status_req;

typedef  struct rlc_status_rpt_s
{
	uint8_t valid_flag;
	uint8_t logic_chan_num;
	rnti_t  rnti; 
	
	uint8_t logicchannel_id[MAX_LOGICCHAN_NUM];
	uint32_t buffer_byte_size[MAX_LOGICCHAN_NUM];

}rlc_buffer_rpt;

typedef struct rlc_mac_buf_status_rpt_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/

	uint32_t  valid_ue_num; 
	rlc_buffer_rpt   rlc_buffer_rpt[D2D_MAX_USER_NUM];
}rlc_mac_buf_status_rpt;

/**MAC_RLC_DATA_REQ */
typedef struct rlc_data_req_s
{
    rnti_t   rnti; 
    uint16_t  logic_chan_num; 
    
	uint8_t  logicchannel_id[MAX_LOGICCHAN_NUM];
	uint32_t mac_pdu_byte_size[MAX_LOGICCHAN_NUM] ; /**PDU size ,unit:byte*/
}rlc_data_req;




typedef struct mac_rlc_data_req_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/

    rlc_data_req  rlc_data_request[D2D_MAX_USER_NUM]; 
}mac_rlc_data_req;



typedef struct sdu_segment_info_s
{
	uint16_t  valid_flag;
	rnti_t    rnti; 
	
	uint32_t  tb_byte_size;
	uint32_t *data_buffer_adder_ptr;  /**mac buffer address*/

}sdu_segment_info;

/**RLC_MAC_DATA_IND**/
typedef struct rlc_mac_data_ind_s
{
	uint16_t sfn;
	uint16_t  sub_sfn;

	sdu_segment_info   sdu_pdu_info[D2D_MAX_USER_NUM];

}rlc_mac_data_ind;


typedef  struct mac_rlc_data_info_s
{

	uint16_t valid_flag;
	rnti_t   rnti; 
	
	uint16_t logicchannel_id[MAX_LOGICCHAN_NUM];
	uint32_t mac_pdu_size[MAX_LOGICCHAN_NUM] ; /**PDU size ,unit:byte*/
	uint32_t *mac_pdu_buffer_ptr[MAX_LOGICCHAN_NUM; /**PDU buffer address */

}mac_rlc_data_info;
/**MAC_RLC_DATA_RPT*/
typedef struct mac_rlc_data_rpt_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/

	mac_rlc_data_info data_ind[D2D_MAX_USER_NUM];
}mac_rlc_data_rpt;



#endif /* INC_INTERFACE_MAC_RLC_H_ */

