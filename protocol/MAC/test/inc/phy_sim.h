/**********************************************************
* @file phy_sim.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/11/07
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_PHY_SIM_H_
#define _SMARTLOGICTECH_PROTOCOL_PHY_SIM_H_

#include "typedef.h"

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	uint16_t    mode;
	uint16_t    cellId;
	uint16_t    bandwith;

	bool flag_pbch;
	bool flag_pdcch;
	bool flag_pusch;
	PHY_PBCHSendReq pbch;
	PHY_PdcchSendReq pdcch;
	PHY_PuschSendReq pusch;
}phy_tx_info;

typedef struct
{
    frame_t     frame;
    sub_frame_t subframe;
	uint16_t    mode;
	uint16_t    cellId;
	uint16_t    bandwith;

	bool flag_pbch;
	PHY_PBCHSendReq pbch;

	bool flag_pdcch;
	bool flag_pusch;
	bool flag_pdcch1;
	bool flag_pusch1;
	bool flag_pdcch2;
	bool flag_pusch2;
	bool flag_pdcch3;
	bool flag_pusch3;
	PHY_PdcchSendReq pdcch;
	PHY_PuschSendReq pusch;
	PHY_PdcchSendReq pdcch1;
	PHY_PuschSendReq pusch1;
	PHY_PdcchSendReq pdcch2;
	PHY_PuschSendReq pusch2;
	PHY_PdcchSendReq pdcch3;
	PHY_PuschSendReq pusch3;

}phy_rx_info;

#endif /* _SMARTLOGICTECH_PROTOCOL_PHY_SIM_H_ */


