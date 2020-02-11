/**********************************************************
* @file mac_osp_interface.h
* @brief  define structure for MAC
*
* @author	guicheng.liu
* @date 	2019/11/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#include "typedef.h"

void* _RegTimer4ms();
void* _RegTimer1ms();
void* _timerCreate(uint32_t DstTaskId,  bool isRepeat, uint32_t timeout, uint32_t delay);
int32_t _timerStart(void* pTimer);

char* mem_alloc(uint32_t len);
void mem_free(char* pbuf);

