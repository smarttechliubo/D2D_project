/**********************************************************
* @file struct_mac_context.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_CONTEXT_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_CONTEXT_H_

#include "typedef.h"
#include "smac_info.h"

typedef struct
{
    frame_t frame;
    sub_frame_t subframe;
	mac_info_s *mac;
}context_s;


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_STRUCT_MAC_CONTEXT_H_ */
