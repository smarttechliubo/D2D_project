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


#define true 1
#define false 0

typedef unsigned int	   uint32_t;
typedef unsigned short	   uint16_t;
typedef unsigned char	   uint8_t;
typedef signed int		   int32_t;
typedef signed short	   int16_t;
typedef signed char		   int8_t;
//typedef unsigned long long uint64_t;

typedef uint32_t message_type_t;
typedef uint32_t message_size_t;
typedef uint8_t pdu_t;

#define INVALID_VALUE_U8  0XFF
#define INVALID_VALUE_U16 0XFFFF
#define INVALID_VALUE_U32 0XFFFFFFFF

#define MAX_SFN 1024
#define MAX_SUBSFN 4

#define TIMING_ADVANCE 1

#define MIB_PDU_SIZE 3
#define SIB_PDU_SIZE 12 //TODO


#endif //_SMARTLOGICTECH_DEFS_H
