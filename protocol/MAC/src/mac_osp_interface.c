/**********************************************************
* @file mac_osp_interface.c
* @brief  mac osp interface function
*
* @author	guicheng.liu
* @date 	2019/11/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#include "typedef.h"
#include "osp_ex.h"

void setFrameOffsetTime(int time)
{
	(void )Osp_SetFrameOffsetTime(time);
}

int32_t sfn_sync()
{
	int32_t ret = -1;
	int32_t sfn = 0;

	ret = Osp_FrameSyncIrqRead(&sfn);

	if (ret >= 0)
	{
		return sfn;
	}
	else
	{
		return -1;
	}
}

void* _RegTimer4ms()
{
	return 0;//OSP_RegFrameSync();
}

void* _RegTimer1ms()
{
	return 0;//OSP_RegFrameSyncCal();
}

void* _timerCreate(uint32_t DstTaskId,  bool isRepeat, uint32_t timeout, uint32_t delay)
{
	return OSP_timerCreateSim(DstTaskId, isRepeat, timeout, delay);
}

int32_t _timerStart(void* pTimer)
{
	return OSP_timerStart(pTimer);
}

char* mem_alloc(uint32_t len)
{
	return OSP_Alloc_Mem(len);
}

void mem_free(char* pbuf)
{
	OSP_Free_Mem(pbuf);
}
