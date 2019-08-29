/******************************************************************
 * @file  dictionary.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月28日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月28日        bo.liu       create file

*****************************************************************/

#ifndef     DICTIONARY_H
#define     DICTIONARY_H

#include <typedef.h>
#include <d2d_system_global.h>
#include <stdio.h>
#include <log.h> 
 
 
#define  DICT_MAX_SIZE      D2D_MAX_USER_NUM 

typedef  struct ue_index_info_s
{
	uint16_t   key_ue_rnti; 
	uint16_t   value_ue_index; 
}ue_info; 


typedef struct ue_info_node_s
{
	struct ue_info_node_s *next; 
	struct ue_info_node_s *prev; 
	ue_info   ue_info; 
}ue_info_node; 


typedef struct ue_info_dict_s
{
	ue_info_node *head; 
	ue_info_node *tail; 
	uint32_t      element_number; 
}ue_info_dict; 





extern uint32_t  dict_GetValue(ue_info_dict *dict, uint16_t key_rnti);
extern uint32_t  dict_GetNewUeIndex(ue_info_dict *dict); 
extern void dict_SetValue(ue_info_dict *dict, uint16_t key_rnti,uint16_t value_ue_index); 
extern ue_info_dict  *dict_init(); 


 
 
#endif
