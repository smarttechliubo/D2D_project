/**********************************************************
* @file common_channel.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/05
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_COMMON_CHANNEL_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_COMMON_CHANNEL_H_

void schedule_mib(const frame_t frame, mac_info_s *mac);
void schedule_sib(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac);
void schedule_common(const frame_t frame, const sub_frame_t subframe, mac_info_s *mac);

#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_COMMON_CHANNEL_H_ */
