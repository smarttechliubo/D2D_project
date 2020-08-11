/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/*! \file list.c
* \brief list management primimtives. It includes three implementations: (1) with mem_blocks, (2) standard list implementation (mem usage 314MB) (3) Linux Queue-based implementation (mem usage 394 MB)
* \author  Navid Nikaein, Mohamed Said MOSLI BOUKSIAA, Lionel GAUTHIER
* \date 2012 - 2014
* \version 0.5
* @ingroup util
*/

#define LIST_C
#define NULL 0

#include "message_list.h"
#include <string.h>
#include <intertask_interface.h>


#if 0
//-----------------------------------------------------------------------------
/*
 * initialize list
 */
//-----------------------------------------------------------------------------
void message_list_init (message_list_t * listP, char *nameP)
{
  //-----------------------------------------------------------------------------
  if (nameP) {
    strncpy( listP->name, nameP, LIST_NAME_MAX_CHAR );
    listP->name[LIST_NAME_MAX_CHAR-1] = 0; // terminate string
  }

  listP->tail = NULL;
  listP->head = NULL;
  listP->nb_elements = 0;
}



//-----------------------------------------------------------------------------
/*
 *  remove an element from head of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed MessageDef
 */
MessageDef *message_list_remove_head (message_list_t *listP)
{
  //-----------------------------------------------------------------------------

  // access optimisation
  MessageDef      *head;
  head = listP->head;

  // almost one element
  if (head != NULL) {
    listP->head = head->next; //！head的next作为新的head 
    listP->nb_elements = listP->nb_elements - 1; 

    // if only one element, update tail
    if (listP->head == NULL) {
      listP->tail = NULL;
    } else {
      head->next = NULL;
    }
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }

  return head;
}



//-----------------------------------------------------------------------------
void  message_list_free (message_list_t *listP)
{
  //-----------------------------------------------------------------------------
  MessageDef      *le;
   //！将全部节点都释放
  while ((le = message_list_remove_head (listP))) {
    message_free_mem_block (le, __func__);
  }
}
//-----------------------------------------------------------------------------
MessageDef * message_list_get_head (message_list_t *listP)
{
  //-----------------------------------------------------------------------------
  return listP->head;
}

//-----------------------------------------------------------------------------
MessageDef *  message_list_remove_element (MessageDef * elementP, message_list_t *listP)
{
  //-----------------------------------------------------------------------------

  // access optimisation;
  MessageDef      *head;

  if (elementP != NULL) {
    // test head
    head = listP->head;

    if (listP->head == elementP) {
      // almost one element
      if (head != NULL) {
        listP->head = head->next;
        listP->nb_elements = listP->nb_elements - 1;

        // if only one element, update tail
        if (listP->head == NULL) {
          listP->tail = NULL;
        } else {
          head->next = NULL;
        }
      }
    } else {
      while (head) {
        if (head->next == elementP) {
          head->next = elementP->next;
          listP->nb_elements = listP->nb_elements - 1;

          if (listP->tail == elementP) {
            listP->tail = head;
          }

          return elementP;
        } else {
          head = head->next;
        }
      }
    }
  }

  return elementP;
}

//-----------------------------------------------------------------------------
/*
 *  add an element to the beginning of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed MessageDef
 */
void   message_list_add_head (MessageDef * elementP, message_list_t *listP)
{
  //-----------------------------------------------------------------------------

  // access optimisation;
  MessageDef      *head;

  if (elementP != NULL) {
    head = listP->head;
    listP->nb_elements = listP->nb_elements + 1;

    // almost one element
    if (head == NULL) {
      listP->head = elementP;
      listP->tail = elementP;
    } else {
      elementP->next = head;
      listP->head = elementP;
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  add an element to the end of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed MessageDef
 */
void   message_list_add_tail_eurecom (MessageDef * elementP, message_list_t *listP)
{
  MessageDef      *tail;
  //-----------------------------------------------------------------------------

  if (elementP != NULL) {
    // access optimisation
    listP->nb_elements = listP->nb_elements + 1; //！总的个数加1 
    elementP->next = NULL; //!尾节点的next = NULL 
    tail = listP->tail; //!当前list的tail节点

    // almost one element
    if (tail == NULL) {   //！如果当前tail节点为空，则加入进去的就是头节点
      listP->head = elementP;
    } else {
      tail->next = elementP; //!否者在当前tail节点后面加上当前节点，使当前节点成为位节点
    }

    listP->tail = elementP; //！更新list的tail节点指针
  } else {
    //msg("[CNT_LIST][ERROR] add_cnt_tail() element NULL\n");
  }
}

//-----------------------------------------------------------------------------
void  message_list_add_list (message_list_t *sublistP, message_list_t *listP)
{
  //-----------------------------------------------------------------------------

  if (sublistP) {
    if (sublistP->head) {  //！如果子LIST . 
      // access optimisation
      MessageDef      *tail;

      tail = listP->tail;

      // almost one element
      if (tail == NULL) {
        listP->head = sublistP->head;
      } else {
        tail->next = sublistP->head; //!<将子list的head 添加到LIST的tail上去，添加在后面
      }

      listP->tail = sublistP->tail; //!原list的tail 修改为子list的tail 
      // clear sublist
      sublistP->head = NULL;  //!<将子list 清除
      sublistP->tail = NULL;
      listP->nb_elements = listP->nb_elements + sublistP->nb_elements; //list的element 个数增加
      sublistP->nb_elements = 0; //!清除子list的元素个数
    }
  }
}

//-----------------------------------------------------------------------------
void  message_list_display (message_list_t *listP)
{
  //-----------------------------------------------------------------------------

  MessageDef      *cursor;
  //  unsigned short             nb_elements = 0;

  // test lists
  if (listP) {
    cursor = listP->head;

    if (cursor) {
      // almost one element
      //msg ("Display list %s %p", listP->name, listP);
      // while (cursor != NULL) {
      // msg ("%d:", cursor->pool_id);
      // msg ("\n");
      // cursor = cursor->next;
      // nb_elements++;
      //}
      //      msg (" found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
    }
  } else {
    //msg ("[SDU_MNGT][WARNING] display_cnt_list() : list is NULL\n");
  }
}

#endif 


 
