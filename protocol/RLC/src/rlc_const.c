/******************************************************************
 * @file  rlc_const.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月10日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月10日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rlc.h>

char *g_rlc_mode_str[] = {
"RLC_MODE_TM",
"RLC_MODE_TM_DL",
"RLC_MODE_TM_UL",
"RLC_MODE_UM",
"RLC_MODE_UM_DL",
"RLC_MODE_UM_UL",
"RLC_MODE_AM",
};


const uint32_t t_Reordering_tab[31] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,
100,110,120,130,140,150,160,170,180,190,200};

 
 
 
 
/**************************function******************************/
