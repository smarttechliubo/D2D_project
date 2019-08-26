/**********************************************************
* @file mac_var.c
* @brief  define variables for mac 
*
* @author	guicheng.liu
* @date 	2019/07/27
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

//context_s *context = NULL;
#include "log.h"
#include "mac_vars.h"

uint32_t g_prbmap[5] = {54, 108, 216, 432, 720};

uint32_t get_rb_num(const uint16_t bandwith)
{
	if (bandwith >= 5)
	{
		LOG_ERROR(MAC, "invalid bandwith:%u", bandwith);
		return g_prbmap[4];
	}
	return g_prbmap[bandwith];
}

// TODO: define start rb for different bandwith config
uint32_t get_rb_start(const uint16_t bandwith)
{
	if (bandwith >= 5)
	{
		LOG_ERROR(MAC, "invalid bandwith:%u", bandwith);
		return 4;
	}
	return 4;// d2d case, first rb is 4.
}

