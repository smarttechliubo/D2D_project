/**********************************************************
* @file mac_defs.h
* @brief  mac macro defs
*
* @author	guicheng.liu
* @date 	2019/03/30
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_DEFS_H
#define _SMARTLOGICTECH_DEFS_H

#include "typedef.h"
#include "d2d_system_global.h"

typedef uint32_t message_type_t;
typedef uint32_t message_size_t;

#define INVALID_U8  0XFF
#define INVALID_U16 0XFFFF
#define INVALID_U32 0XFFFFFFFF


#define MAC_SCH_TIMING_ADVANCE 2
#define TIMING_SYNC_PERIOD 20

#define MIB_PDU_SIZE 3
#define SIB_PDU_SIZE 64 //TODO
#define MAX_MCS 30
#define MAX_RBS 100
#define MAX_UE  D2D_MAX_USER_NUM
#define MAX_RA_NUM 2
#define MAX_RA_TIMER 10
#define MAX_HARQ_NUM 2

#define CCCH_ 0  // CCCH / SRB0 logical channel
//#define DCCH 1  // DCCH / SRB1 logical channel
//#define DCCH1 2	// DCCH1 / SRB2  logical channel
#define DTCH_ 1	// DTCH / DRB1  logical channel

#define MAX_LCID 11
#define SHORT_PADDING 31

#define MAX_NUM_CE 4
#define MAX_CRC_TIMER 10
#define MAX_NACK_TIMER 10
#define MAX_CRC_NG 4
#define MAX_NACK 4

#define BIT3 7
#define BIT2 3
#define BIT1 1

#define MAC_DEBUG
#endif //_SMARTLOGICTECH_DEFS_H
