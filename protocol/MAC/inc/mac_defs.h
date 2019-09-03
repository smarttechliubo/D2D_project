/**********************************************************
* @file rrc_mac_interface.h
* @brief  define msg ID and msg struct for MAC and RRC
*
* @author	guicheng.liu
* @date 	2019/03/30
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/

#ifndef _SMARTLOGICTECH_DEFS_H
#define _SMARTLOGICTECH_DEFS_H

#include "typedef.h"

typedef uint32_t message_type_t;
typedef uint32_t message_size_t;

#define MAX(a,b)  (((a)>(b)) ? (a) : (b))
#define MIN(a,b)  (((a)<(b)) ? (a) : (b))

#define INVALID_U8  0XFF
#define INVALID_U16 0XFFFF
#define INVALID_U32 0XFFFFFFFF


#define TIMING_ADVANCE 1
#define TIMING_SYNC_PERIOD 20

#define MAX_SFN 1024
#define MAX_SUBSFN 4

#define MIB_PDU_SIZE 3
#define SIB_PDU_SIZE 64 //TODO
#define DCI_LEN 4
#define MAX_DCI 2
#define MAX_MCS 16
#define MAX_RBS 40
#define MAX_TX_NUM 2
#define MAX_UE  16
#define MAX_RA_NUM 2


#define SI_RNTI 0XFFFF
#define RA_RNTI 0XFFEE

#endif //_SMARTLOGICTECH_DEFS_H
