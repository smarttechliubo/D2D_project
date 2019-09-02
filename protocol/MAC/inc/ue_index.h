/**********************************************************
* @file ue_index.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/08/31
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_UE_INDEX_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_UE_INDEX_H_

#include "typedef.h"

void init_index();
uint16_t new_index();
uint16_t free_index(const uint16_t index);


#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_UE_INDEX_H_ */
