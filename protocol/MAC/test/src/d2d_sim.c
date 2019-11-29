/**********************************************************
* @file ue_sim.c
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/25
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#include "mac_osp_interface.h"
#include "log.h"
#include "d2d_sim.h"

uint32_t g_ue_mode = 1;

uint32_t init_d2d_sim()
{
	void* pTimer;
	int32_t ret;

	pTimer = _timerCreate(TASK_D2D_RRC, 1, 4,0);
	ret = _timerStart(pTimer);

	LOG_INFO(MAC,"init_d2d_sim pTimer is %p, ret:%u\r\n", pTimer, ret);

	return 0;
}

void d2dMsgHandler(msgDef* msg)
{
	if (g_ue_mode == 1)
	{
		d2dDstMsgHandler(msg);
	}
	else
	{

	}
}

void d2d_sim_thread(msgDef* msg)
{
	if (!is_timer(msg))
	{
		d2dMsgHandler(msg);
	}
	else if (g_ue_mode == 0)
	{
		//rrcSrcStatusHandler();
		//rrcSrcUserStatusHandler();
	}
	else if (g_ue_mode == 1)
	{
		d2dDstMain();
	}
}
