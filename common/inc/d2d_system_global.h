/******************************************************************
 * @file  d2d_system_global.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月12日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月12日        bo.liu       create file

*****************************************************************/

#ifndef     D2D_SYSTEM_GLOBAL_H
#define     D2D_SYSTEM_GLOBAL_H



#define      D2D_MAX_USER_NUM    8 
 
#define  MAX_LOGICCHAN_NUM    4

#define MAX_SFN 1024
#define MAX_SUBSFN 4

#define MAX_TX_UE 4
#define MAX_RX_UE 4
#define MAX_DCI_NUM 2
#define MAX_DCI_LEN 4

#define SI_RNTI 0XFFFF
#define RA_RNTI 0XFFFE
#define C_RNTI  0XFFEE

#define MAX(a,b)  (((a)>(b)) ? (a) : (b))
#define MIN(a,b)  (((a)<(b)) ? (a) : (b))

typedef enum
{
	BANDWIDTH_1P5M,
	BANDWIDTH_3M, 
	BANDWIDTH_6M,
	BANDWIDTH_12M,
	BANDWIDTH_20M
}bandwidth_e;

typedef enum
{
	SUBFRAME_CONFIG_DDUUDDUU, 
    NOT_DEFINED
}subframeconfig_e; 



typedef struct
{
	uint16_t rb_num;// value: 1..2, default: 2
	uint16_t rb_start_index; // value: 2..3, default: 2
}pdcch_config_s;




extern uint32_t   Get_Subsfn( ); 
extern uint32_t   Get_Frame( );

#endif
