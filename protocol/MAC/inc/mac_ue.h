/**********************************************************
* @file mac_ue.h
* 
* @brief  ue info haeder
* @author   guicheng.liu
* @date     2019/08/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_UE_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_UE_H_

#include "mac_defs.h"
#include "interface_rrc_mac.h"

bool is_temp_ue(uint16_t ueIndex);
void mac_user_setup(const rrc_mac_connnection_setup *req);
void mac_user_release(const rrc_mac_release_req *req);
uint16_t find_ue(const rnti_t rnti);
bool remove_ue(const uint16_t ueIndex, const uint32_t mode);
void add_temp_ue(const rnti_t rnti);
void handle_ack_result(const rnti_t rnti, const uint8_t ack_num, const uint8_t ack_bits);
bool handle_crc_result(const sub_frame_t subframe, const rnti_t rnti, const uint16_t crc);
bool update_temp_ue_crc_result(const sub_frame_t subframe, const rnti_t rnti, const uint16_t crc);
bool update_crc_result(const sub_frame_t subframe, const rnti_t rnti, const uint16_t crc);
bool update_ue_cqi(const rnti_t rnti, const uint16_t cqi);
void update_harq_info(const sub_frame_t subframe, const rnti_t rnti, const uint16_t ack);
void update_ue_status(const rnti_t rnti, const uint16_t status);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_MAC_UE_H_ */
