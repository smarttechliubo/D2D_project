/** @file interface_rrc_mac.h
 *
 *  @brief
 *  @author bo.liu
 *  @date   2019/07/30
 *  COPYRIGHT NOTICE: (c) smartlogictech.
 *  All rights reserved.
 */

#ifndef INC_INTERFACE_RRC_MAC_H_
#define INC_INTERFACE_RRC_MAC_H_


#include <typedef.h>

/**MAC_RRC_INITIAL_CFM **/
typedef struct rlc_rrc_initial_cfm_s
{
	uint16_t  status; /**1:pass; 0:error*/
	uint16_t  error_code;     /**error code, self definition*/
}mac_rrc_initial_cfm;


typedef mac_rrc_initial_cfm  mac_rrc_release_cfm;



/*****MIB & SIB1 message*******/

/**MAC_RRC_BCCH_SIB1_RPT***/
typedef struct mac_rrc_bcch_sib1_rpt_s
{
	uint16_t  sfn;
	uint16_t  subsfn;

	uint32_t  data_size;
	uint32_t  *data_ptr;
}mac_rrc_bcch_sib1_rpt;

  /**MAC_RRC_CCCH_RPT*/
typedef mac_rrc_bcch_sib1_rpt  mac_rrc_ccch_rpt;

/**MAC_RRC_OUTSYNC_RPT */
typedef struct mac_rrc_outsync_rpt_s
{
	uint16_t  sfn;
	uint16_t  subsfn;

    uint32_t  outsync_flag ; /**1:out_sync; 0:in_sync**/
}mac_rrc_outsync_rpt;


#endif /* INC_INTERFACE_RRC_MAC_H_ */
