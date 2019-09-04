/******************************************************************
 * @file  message_list.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     MESSAGE_LIST_H
#define     MESSAGE_LIST_H
 
 



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//#include<types.h>
#include<stdlib.h>
#include<sys/queue.h>
#include <string.h>


#include <intertask_interface.h>





//-----------------------------------------------------------------------------
void         message_list_init (message_list_t* , char *);
void         message_list_free (message_list_t* listP);
MessageDef * message_list_get_head (message_list_t*);
MessageDef * message_list_remove_head (message_list_t* );
MessageDef * message_list_remove_element (MessageDef*, message_list_t*);
void         message_list_add_head (MessageDef* , message_list_t* );
void         message_list_add_tail_eurecom (MessageDef* , message_list_t* );
void         message_list_add_list (message_list_t* , message_list_t* );
void         message_list_display (message_list_t* );



//-----------------------------------------------------------------------------
/* The following lists are used for sorting numbers */
#ifndef LINUX_LIST
/*! \brief the node structure */
struct node {
  struct node* next; /*!< \brief is a node pointer */
  double val; /*!< \brief is a the value of a node pointer*/
};
//-----------------------------------------------------------------------------
/*! \brief the list structure */
struct list {
  struct node* head; /*!< \brief is a node pointer */
  int size; /*!< \brief is the list size*/
};
#else
//-----------------------------------------------------------------------------
struct entry {
  double val;
  LIST_ENTRY(entry) entries;
};
//-----------------------------------------------------------------------------
struct list {
  LIST_HEAD(listhead, entry) head;
  ssize_t size;
};
#endif
//-----------------------------------------------------------------------------
void   push_front  (struct list*, double); 
void   initialize  (struct list*);         
void   del         (struct list*);         
void   totable     (double*, struct list*);
int compare (const void * a, const void * b);
int32_t calculate_median(struct list *loc_list);


typedef struct {
  size_t size;
  size_t mallocedSize;
  size_t atomSize;
  size_t increment;
} varArray_t;
 
static inline varArray_t * initVarArray(size_t increment, size_t atomSize) {
    varArray_t * tmp=malloc(sizeof(varArray_t)+increment*atomSize);
    tmp->size=0;
    tmp->atomSize=atomSize;
    tmp->mallocedSize=increment;
    tmp->increment=increment;
    return(tmp);
}

static inline void * dataArray(varArray_t * input) {
  return input+1;
}

static inline void appendVarArray(varArray_t * input, void* data) {
  if (input->size>=input->mallocedSize) {
     input->mallocedSize+=input->increment;
     input=realloc(input,sizeof(varArray_t)+input->mallocedSize*input->atomSize);
  }
  memcpy((uint8_t*)(input+1)+input->atomSize*input->size++, data, input->atomSize);
}

static inline void freeVarArray(varArray_t * input) {
   free(input);
}

#endif
 
 
 

