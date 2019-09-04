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


 

#endif
