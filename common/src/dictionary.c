/******************************************************************
 * @file  dictionary.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月28日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月28日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <dictionary.h>





/**************************functions******************************/

 /*!   
 * @brief:  dictionary initial 
 * @author:  bo.liu
 * @Date:  2019年8月28日
 */
ue_info_dict  *dict_init()
{
	uint32_t i; 
    ue_info_node *temp; 
    ue_info_dict *p = (ue_info_dict *)OSP_Alloc_Mem(sizeof(ue_info_dict)); 
    p->head = NULL; 
    p->tail = NULL;
	p->element_number = 0; 

	temp = (ue_info_node *)OSP_Alloc_Mem(sizeof(ue_info_node));
	temp->ue_info.value_ue_index = 0; 
	temp->ue_info.key_ue_rnti = 0xffff;
    temp->next = NULL; 
	p->head = temp; //! head point to the first node 
	p->tail = temp; //! tail point to the first node 
	p->element_number ++; 

	return p; 

}



 /*!   
 * @brief:  set new value to the node which have the key = key_rnti 
 * @author:  bo.liu
 * @Date:  2019年8月28日
 * @param: *dict :            [cictionary]
 * @param: key_rnti :         [key:rnti ]
 * @param: value_ue_index :   [value:value ]
 */
void dict_SetValue(ue_info_dict *dict, uint16_t key_rnti,uint16_t value_ue_index)
{
    ue_info_node  *new_node; 
    ue_info_node *temp = dict->head; 
	while(temp != dict->tail)
	{
		if (temp->ue_info.key_ue_rnti == key_rnti)
		{
			temp->ue_info.value_ue_index = value_ue_index; 
			LOG_INFO(RRC,"find right node in dictionary,change the value of element\n"); 
			break; 
		}
		temp = temp->next;
	}

	if (temp == dict->tail)  //!not find the key
    {
		new_node = (ue_info_node  *)OSP_Alloc_Mem(sizeof(ue_info_node)); 
		new_node->next = NULL;
		new_node->ue_info.key_ue_rnti = key_rnti; 
		new_node->ue_info.value_ue_index = value_ue_index;
		dict->tail->next = new_node;
		 
		dict->tail = new_node; 
		dict->element_number++; 
		LOG_INFO(RRC, "don't find the node, so insert new node to the dictionary, key_rnti = %d,value_ue_index = %d,elment_number = %d\n",
		key_rnti,value_ue_index,dict->element_number); 
		
    }

}


 /*!   
 * @brief:  get new ue index from dictionary 
 * @author:  bo.liu
 * @Date:  2019年8月28日
 * @param: *dict :            [dictionary ]
 */
uint32_t  dict_GetNewUeIndex(ue_info_dict *dict)
{
	 ue_info_node *temp = dict->head; 
	 AssertFatal(dict->element_number > 0, RRC, "dictionary element number = 0\n"); 
	 if (dict->element_number < DICT_MAX_SIZE)
	 {
		return dict->element_number - 1; 
	 }
	 else 
	 {
		AssertFatal(dict->element_number < DICT_MAX_SIZE, RRC, "dictionary element overflow\n"); 
	 }
}


 /*!   
 * @brief:  get value from dictionary ,according to the key_rnti
 * @author:  bo.liu
 * @Date:  2019年8月28日
 * @param: *dict :            [dictionayr ]
 * @param: key_rnti :         [rnti  ]
 */
uint32_t  dict_GetValue(ue_info_dict *dict, uint16_t key_rnti)
{
    ue_info_node  *new_node; 
    ue_info_node *temp = dict->head; 
   
	while(temp != dict->tail->next)
	{
		//LOG_DEBUG(RRC, "func:%s, dictionary's node's key = %d \n",__func__,temp->ue_info.key_ue_rnti);
		if (temp->ue_info.key_ue_rnti == key_rnti)
		{
		   // LOG_INFO(RRC, "find right node in dictionary,return the value of element\n"); 
			return temp->ue_info.value_ue_index ;
		}
		temp = temp->next;
	}

	AssertFatal((temp != dict->tail->next), RRC, "can't find the node which's key == key_rnti,pls check it\n"); 

} 
 
/**************************function******************************/
