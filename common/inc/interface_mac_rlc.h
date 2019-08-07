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

#define  MAX_LOGICCHAN_NUM    4
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
	uint8_t logicchannel_id;
	uint8_t rlc_entity_type;
	uint8_t pad;

	uint32_t buffer_byte_size;

}rlc_buffer_rpt;

typedef struct rlc_mac_buf_status_rpt_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/
	rlc_buffer_rpt   rlc_buffer_rpt[MAX_LOGICCHAN_NUM]
}rlc_mac_buf_status_rpt;

/**MAC_RLC_DATA_REQ */
typedef struct mac_rlc_data_req_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/

	uint8_t  logicchannel_id[MAX_LOGICCHAN_NUM];
	uint32_t mac_pdu_byte_size ; /**PDU size ,unit:byte*/
}mac_rlc_data_req;


typedef struct sdu_segment_info_s
{
	uint16_t  valid_flag;
	uint16_t  logicchannel_id;
	uint32_t  segment_size;
}sdu_segment_info;

/**RLC_MAC_DATA_IND**/
typedef struct rlc_mac_data_ind_s
{
	uint16_t sfn;
	uint16_t  sub_sfn;

	sdu_segment_info   sdu_pdu_info[MAX_LOGICCHAN_NUM];

	uint32_t tb_byte_size; /**unit:byte*/
	uint32_t *data_buffer_adder_ptr;  /**mac buffer address*/
}rlc_mac_data_ind;


typedef  struct mac_rlc_data_info_s
{

	uint16_t valid_flag;
	uint16_t logicchannel_id;
	uint32_t mac_pdu_size ; /**PDU size ,unit:byte*/
	uint32_t *mac_pdu_buffer_ptr; /**PDU buffer address */

}mac_rlc_data_info;
/**MAC_RLC_DATA_RPT*/
typedef struct mac_rlc_data_rpt_s
{

	uint16_t sub_sfn;  /**subsfn number*/
	uint16_t sfn;     /**sfn number*/

	mac_rlc_data_info data_ind[MAX_LOGICCHAN_NUM];
}mac_rlc_data_rpt;



#endif /* INC_INTERFACE_MAC_RLC_H_ */
