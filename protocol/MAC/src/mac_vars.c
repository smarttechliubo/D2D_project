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
//#include "mac_vars.h"
#include "mac_defs.h"
#include "smac_context.h"
#include "smac_schedule_result.h"

context_s g_context;
schedule_result_s g_sch;

uint32_t g_prbmap[5] = {54, 108, 216, 432, 720}; //0: 1.5M, 1: 3M, 2: 6M, 3: 12M, 4:20M

uint32_t g_rbg_size[5] = {1, 1, 1, 1, 1}; //0: 1.5M, 1: 3M, 2: 6M, 3: 12M, 4:20M

uint32_t cqi_2_mcs[16]= {0, 0, 1, 2, 4, 6, 8, 11, 13, 16, 18, 20, 23, 25, 27, 28};

uint16_t tbs_table[MAX_MCS][MAX_RBS] = {// TODO: MAX_RBS need to be clarify.
	{16,24,32,40,72,88,104,120,136,144,176,176,208,208,256,256,280,280,296,328,328,344,376,376,408,424,440,456,472,488,504,520,536,552,568,584,600,616,632,648},
	{24,40,56,72,104,136,176,176,208,256,280,296,328,344,376,408,424,456,488,504,536,568,584,616,648,680,696,712,744,776,808,840,872,872,904,936,968,1000,1032,1032},
	{32,56,88,136,176,208,256,296,336,376,408,456,504,536,584,616,648,696,744,776,808,872,904,936,968,1032,1064,1096,1128,1192,1224,1256,1288,1352,1384,1416,1480,1480,1544,1608},
	{40,72,136,176,256,296,344,408,456,504,568,616,680,712,776,840,872,936,1000,1032,1096,1160,1224,1256,1320,1384,1416,1480,1544,1608,1608,1672,1736,1800,1864,1928,1992,2024,2088,2152},
	{56,104,176,256,328,376,456,520,584,648,712,776,872,936,1000,1064,1128,1192,1256,1352,1416,1480,1544,1608,1672,1736,1800,1864,1928,2024,2088,2152,2216,2280,2344,2408,2472,2600,2664,2728},
	{56,136,208,296,376,456,536,616,696,776,872,936,1032,1096,1192,1256,1352,1416,1480,1608,1672,1736,1800,1928,1992,2088,2152,2216,2280,2408,2472,2536,2664,2728,2792,2856,2984,2984,3112,3240},
	{72,176,280,376,472,568,648,776,872,968,1064,1160,1256,1352,1480,1544,1672,1736,1864,1928,2024,2152,2216,2344,2408,2536,2664,2728,2856,2984,2984,3112,3240,3368,3368,3496,3624,3752,3752,3880},
	{104,208,336,456,584,696,808,936,1064,1192,1288,1416,1544,1672,1800,1928,2024,2152,2280,2408,2536,2664,2728,2856,2984,3112,3240,3368,3496,3624,3752,3880,4008,4136,4264,4264,4392,4584,4776,4776},
	{120,256,392,520,680,808,936,1096,1224,1352,1480,1608,1800,1928,2024,2216,2344,2472,2600,2728,2856,2984,3112,3368,3496,3624,3752,3880,4008,4136,4264,4392,4584,4776,4776,4968,5160,5160,5352,5544},
	{136,288,440,584,744,904,1064,1224,1352,1544,1672,1800,1992,2152,2280,2472,2600,2728,2856,3112,3240,3368,3496,3624,3880,4008,4136,4264,4392,4584,4776,4968,5160,5160,5352,5544,5736,5736,5992,6200},
	{152,336,504,680,872,1032,1224,1384,1544,1736,1928,2088,2280,2472,2664,2792,2984,3240,3368,3496,3752,3880,4008,4264,4392,4584,4776,4968,5160,5352,5544,5736,5736,5992,6200,6456,6456,6712,6968,6968},
	{176,376,552,744,936,1128,1352,1544,1736,1928,2088,2280,2536,2728,2856,3112,3240,3496,3624,3880,4008,4264,4392,4584,4776,4968,5160,5352,5544,5736,5992,6200,6456,6712,6712,6968,7224,7480,7480,7736},
	{208,408,632,872,1096,1288,1544,1736,1992,2216,2408,2664,2856,3112,3240,3496,3752,4008,4136,4392,4584,4776,5160,5352,5544,5736,5992,6200,6456,6712,6968,6968,7224,7480,7736,7992,8248,8504,8504,8760},
	{208,440,680,904,1160,1384,1608,1864,2088,2344,2536,2792,2984,3240,3496,3752,4008,4264,4392,4584,4968,5160,5352,5544,5736,5992,6200,6456,6712,6968,7224,7480,7736,7992,8248,8504,8760,8760,9144,9528},
	{224,472,712,968,1224,1480,1736,1992,2216,2472,2728,2984,3240,3496,3752,4008,4136,4392,4776,4968,5160,5352,5736,5992,6200,6456,6712,6968,7224,7480,7736,7992,8248,8504,8760,8760,9144,9528,9528,9912},
	{256,504,776,1032,1320,1608,1864,2152,2408,2664,2984,3240,3496,3752,4008,4264,4584,4776,5160,5352,5544,5992,6200,6456,6712,6968,7224,7480,7736,7992,8248,8504,8760,9144,9528,9528,9912,10296,10296,10680}
};

void init_mac_vars()
{
	
}

uint8_t get_rv(uint8_t tx_num)
{
	uint8_t rv[5] = {0, 0, 2, 1, 3};

	if (tx_num >= 5)
	{
		LOG_ERROR(MAC, "get rv, invalid tx_num:%u", tx_num);
		return 0;
	}
	
	return rv[tx_num];
}


uint32_t get_rbg_size(const uint16_t bandwith)
{
	if (bandwith >= 5)
	{
		LOG_ERROR(MAC, "invalid bandwith:%u", bandwith);
		return g_rbg_size[1];
	}
	return g_rbg_size[bandwith];
}

uint32_t get_tbs(const uint8_t mcs, const uint8_t num_rb)
{
	uint32_t tbs = 0;
	
	if(mcs < MAX_MCS && num_rb > 0 && num_rb <= MAX_RBS)
	{
		tbs = tbs_table[mcs][num_rb-1];
		tbs = tbs>>3;
	}
	else
	{
	    LOG_ERROR(MAC, "incorrect parameters mcs:%u, rb:%u", mcs, num_rb);
	}

	return tbs;
}

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

uint32_t get_first_rb(uint16_t bandwith)
{
	mac_info_s *mac = g_context.mac;
	uint32_t rb_start_index = get_rb_start(bandwith);
	uint32_t i = 0;
	
	for (i = rb_start_index; i < MAX_RBS; i++)
	{
		if (mac->rb_available[i] == 1)
			break;
	}

	if (i >= MAX_RBS)
	{
		LOG_ERROR(MAC, "No availabe rb!");
	}

	return i;
}

uint32_t get_available_rbs(const uint16_t bandwith)
{
	uint32_t available_rbs = 0;
	uint32_t rb_num = get_rb_num(bandwith);

	available_rbs = rb_num - get_rb_start(bandwith);

	return available_rbs;
}

uint8_t get_harqId(const sub_frame_t subframe)
{
	uint8_t harqId = INVALID_U8;
	mac_info_s *mac = g_context.mac;
	mode_e mode = mac->mode;
	if (mode == EMAC_SRC)
	{
		harqId = subframe % MAX_SUBSFN;
	}
	else
	{
		harqId = (subframe % MAX_SUBSFN) - 2;
	}
	return harqId;
}

uint16_t cqi_to_mcs(const uint16_t cqi)
{
	uint16_t i = cqi;

	if (i >= 16)
		i = 15;

	return cqi_2_mcs[i];
}

bool pre_check(const sub_frame_t subframe)
{
	mac_info_s *mac = g_context.mac;

	if (mac->status != ESTATUS_ACTIVE)
	{
		return false;
	}

	if ((mac->mode == EMAC_SRC) && (subframe == 0 || subframe == 1))
	{
		return true;
	}
	else if((mac->mode == EMAC_DEST) && (subframe == 2 || subframe == 3))
	{
		return true;
	}

	return false;
}

