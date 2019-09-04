/******************************************************************
 * @file  hashtable.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <hashtable.h>




 /*!   
 * @brief:  change hashtable status code to string 
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: rcP :              [param description ]
 */
char* hashtable_rc_code2string(hashtable_rc_t rcP)
{
    switch (rcP) {
    case HASH_TABLE_OK:                      return "HASH_TABLE_OK";break;
    case HASH_TABLE_INSERT_OVERWRITTEN_DATA: return "HASH_TABLE_INSERT_OVERWRITTEN_DATA";break;
    case HASH_TABLE_KEY_NOT_EXISTS:          return "HASH_TABLE_KEY_NOT_EXISTS";break;
    case HASH_TABLE_KEY_ALREADY_EXISTS:      return "HASH_TABLE_KEY_ALREADY_EXISTS";break;
    case HASH_TABLE_BAD_PARAMETER_HASHTABLE: return "HASH_TABLE_BAD_PARAMETER_HASHTABLE";break;
    default:                                 return "UNKNOWN hashtable_rc_t";
    }
}


 /*!   
 * @brief:  hash_free_int_func() is used when this hashtable is used to store int values as data (pointer = value)
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: memoryP :          [param description ]
 */
void hash_free_int_func(void* memoryP)
{

}



 /*!   
 * @brief: def_hashfunc() is the default used by hashtable_create() when the user didn't specify one 
 *  This is a simple/naive hash function which adds the key's ASCII char values. It will probably generate lots of 
 *  collisions on large hash tables.
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: keyP :             [param description ]
 */
static hash_size_t def_hashfunc(const uint64_t keyP)
{
    return (hash_size_t)keyP;
}



 /*!   
 * @brief:  ets up the initial structure of the hash table. The user specified size will be allocated and 
 *          initialized to NULL.
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: sizeP :            [hash table size  ]
 * @param: *hashfuncP :       [hash key functionIf the hashfunc argument is NULL, a default hash function is used.]
 * @param: freefuncP :        [free function]
 */
hash_table_t *hashtable_create(const hash_size_t sizeP, hash_size_t (*hashfuncP)(const hash_key_t ), 
			                         void (*freefuncP)(void*))
{
    hash_table_t *hashtbl = NULL;

    if(!(hashtbl=malloc(sizeof(hash_table_t)))) {
        return NULL;
    }

    if(!(hashtbl->nodes=calloc(sizeP, sizeof(hash_node_t*)))) {
        free(hashtbl);
        return NULL;
    }

    hashtbl->size=sizeP;

    if(hashfuncP) hashtbl->hashfunc=hashfuncP;
    else hashtbl->hashfunc=def_hashfunc;

    if(freefuncP) hashtbl->freefunc=freefuncP;
    else hashtbl->freefunc=free;

    return hashtbl;
}


 
 /*!   
 * @brief:  walks through the linked lists for each possible hash value, and releases the elements. It 
 *          also releases the nodes array and the hash_table_t.
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: hashtblP :         [param description ]
 */
hashtable_rc_t hashtable_destroy(hash_table_t * hashtblP)
{
    hash_size_t n;
    hash_node_t *node, *oldnode;

    if (hashtblP == NULL) {
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }

    for(n=0; n<hashtblP->size; ++n) {
        node=hashtblP->nodes[n];
        while(node) {
            oldnode=node;
            node=node->next;
            if (oldnode->data) {
                hashtblP->freefunc(oldnode->data);
            }
            free(oldnode);
        }
    }
    free(hashtblP->nodes);
    free(hashtblP);
    hashtblP=NULL;
    return HASH_TABLE_OK;
}

#if 0 

hashtable_rc_t hashtable_is_key_exists (const hash_table_t * const hashtblP, const hash_key_t keyP)
{
    hash_node_t *node = NULL;
    hash_size_t  hash = 0;

    if (hashtblP == NULL) {
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }

    hash=hashtblP->hashfunc(keyP)%hashtblP->size;
    node=hashtblP->nodes[hash];
    while(node) {
        if(node->key == keyP) {
            return HASH_TABLE_OK;
        }
        node=node->next;
    }
    return HASH_TABLE_KEY_NOT_EXISTS;
}



hashtable_rc_t hashtable_apply_funct_on_elements (hash_table_t *const hashtblP, void functP(hash_key_t keyP, 
                                                             void* dataP, void* parameterP), void* parameterP)
//-------------------------------------------------------------------------------------------------------------------
------------
{
    hash_node_t  *node         = NULL;
    unsigned int  i            = 0;
    unsigned int  num_elements = 0;
    if (hashtblP == NULL) {
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }
    while ((num_elements < hashtblP->num_elements) && (i < hashtblP->size)) {
        if (hashtblP->nodes[i] != NULL) {
            node=hashtblP->nodes[i];
            while(node) {
                num_elements += 1;
                functP(node->key, node->data, parameterP);
                node=node->next;
            }
        }
        i += 1;
    }
    return HASH_TABLE_OK;
}
//-------------------------------------------------------------------------------------------------------------------
------------
hashtable_rc_t hashtable_dump_content (const hash_table_t * const hashtblP, char * const buffer_pP, int * const 
remaining_bytes_in_buffer_pP )
//-------------------------------------------------------------------------------------------------------------------
------------
{
    hash_node_t  *node         = NULL;
    unsigned int  i            = 0;
    if (hashtblP == NULL) {
        *remaining_bytes_in_buffer_pP = snprintf(
                buffer_pP,
                *remaining_bytes_in_buffer_pP,
                "HASH_TABLE_BAD_PARAMETER_HASHTABLE");
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }
    while ((i < hashtblP->size) && (*remaining_bytes_in_buffer_pP > 0)) {
        if (hashtblP->nodes[i] != NULL) {
            node=hashtblP->nodes[i];
            while(node) {
                *remaining_bytes_in_buffer_pP = snprintf(
                                buffer_pP,
                                *remaining_bytes_in_buffer_pP,
                                "Key 0x%"PRIx64" Element %p\n",
                                node->key,
                                node->data);
                node=node->next;
            }
        }
        i += 1;
    }
    return HASH_TABLE_OK;
}
#endif 


 
 
 /*!   
 * @brief: Adding a new element to hashtable   
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: hashtblP :         [hashtable pointer  ]
 * @param: keyP :             [key ]
 * @param: *dataP :           [insert element ]
 */
hashtable_rc_t hashtable_insert(hash_table_t * const hashtblP, const hash_key_t keyP, void *dataP)
{
    hash_node_t *node = NULL;
    hash_size_t  hash = 0;
    if (hashtblP == NULL) {
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }
	//! hashfunc()返回的是keyp
    hash=hashtblP->hashfunc(keyP)%hashtblP->size;

    node=hashtblP->nodes[hash]; 
    //!如果hash table中key对应的位置上已经有数据了，那么出现了hash冲突，则那么一直找到空余的位置
    while(node) {
		//!如果当前key对应的是有节点的，并且key对应相同，则替代，并上报overwirten 状态
		//！则free 当前节点
        if(node->key == keyP) {
            if (node->data) {
                hashtblP->freefunc(node->data);
            }
            node->data=dataP;
            return HASH_TABLE_INSERT_OVERWRITTEN_DATA;
        }
        node=node->next; //！一直到找到一个next为NULL的节点，也即是找到一个空位置
    }

	//!开始申请一个新节点，将旧节点之后
    if(!(node=malloc(sizeof(hash_node_t)))) return -1;
    node->key=keyP;  //！新申请节点，并更新key 和dataP
    node->data=dataP;
	//!如果在这个key对应的位置上有节点，并且节点的Key跟当前的key不对
	//!（之前如果key匹配的话，已经free并return了）
	//！将当前这个节点移到插入的新节点后面
    if (hashtblP->nodes[hash]) {
        node->next=hashtblP->nodes[hash];
    } else {
        node->next = NULL; //如果没有，则后面是null
    }
    hashtblP->nodes[hash]=node;
    hashtblP->num_elements += 1;
    return HASH_TABLE_OK;
}


 
 /*!   
 * @brief:  To remove an element from the hash table
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: hashtblP :         [hash table ]
 * @param: keyP :             [key  ]
 */
hashtable_rc_t hashtable_remove(hash_table_t * const hashtblP, const hash_key_t keyP)
{
    hash_node_t *node, *prevnode=NULL;
    hash_size_t  hash = 0;

    if (hashtblP == NULL) {
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }
    hash=hashtblP->hashfunc(keyP)%hashtblP->size;
    node=hashtblP->nodes[hash];
    while(node) {
        if(node->key == keyP) {
            if(prevnode) prevnode->next=node->next;
            else hashtblP->nodes[hash]=node->next;
            if (node->data) {
                hashtblP->freefunc(node->data);
            }
            free(node);
            hashtblP->num_elements -= 1;
            return HASH_TABLE_OK;
        }
        prevnode=node;
        node=node->next;
    }
    return HASH_TABLE_KEY_NOT_EXISTS;
}



 
 /*!   
 * @brief:  search through the linked list for the corresponding hash value,NULL is returned if we didn't find it.
 * @author:  bo.liu
 * @Date:  2019年9月3日
 * @param: hashtblP :         [hash table pointer ]
 * @param: keyP :             [key  ]
 * @param: dataP :            [return element,return NULL if don't find it ]
 */
hashtable_rc_t hashtable_get(const hash_table_t * const hashtblP, const hash_key_t keyP, void** dataP)
{
    hash_node_t *node = NULL;
    hash_size_t  hash = 0;

    if (hashtblP == NULL) {
        *dataP = NULL;
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }
	//! hashfunc()返回的就是keyP
    hash=hashtblP->hashfunc(keyP)%hashtblP->size;
/*	fprintf(stderr, "hashtable_get() key=%s, hash=%d\n", key, hash);*/

    node=hashtblP->nodes[hash];

    while(node) {
        if(node->key == keyP) {
            *dataP = node->data;
            return HASH_TABLE_OK;
        }
        node=node->next;
    }
    *dataP = NULL;
    return HASH_TABLE_KEY_NOT_EXISTS;
}


#if 0
/*
 * Resizing
 * The number of elements in a hash table is not always known when creating the table.
 * If the number of elements grows too large, it will seriously reduce the performance of most hash table operations.
 * If the number of elements are reduced, the hash table will waste memory. That is why we provide a function for 
resizing the table.
 * Resizing a hash table is not as easy as a realloc(). All hash values must be recalculated and each element must be 
inserted into its new position.
 * We create a temporary hash_table_t object (newtbl) to be used while building the new hashes.
 * This allows us to reuse hashtable_insert() and hashtable_remove(), when moving the elements to the new table.
 * After that, we can just free the old table and copy the elements from newtbl to hashtbl.
 */

hashtable_rc_t hashtable_resize(hash_table_t * const hashtblP, const hash_size_t sizeP)
{
    hash_table_t       newtbl;
    hash_size_t        n;
    hash_node_t       *node,*next;

    if (hashtblP == NULL) {
        return HASH_TABLE_BAD_PARAMETER_HASHTABLE;
    }

    newtbl.size     = sizeP;
    newtbl.hashfunc = hashtblP->hashfunc;

    if(!(newtbl.nodes=calloc(sizeP, sizeof(hash_node_t*)))) return -1;

    for(n=0; n<hashtblP->size; ++n) {
        for(node=hashtblP->nodes[n]; node; node=next) {
            next = node->next;
            hashtable_insert(&newtbl, node->key, node->data);
            // Lionel GAUTHIER: BAD CODE TO BE REWRITTEN
            hashtable_remove(hashtblP, node->key);

        }
    }

    free(hashtblP->nodes);
    hashtblP->size=newtbl.size;
    hashtblP->nodes=newtbl.nodes;

    return HASH_TABLE_OK;
}


#endif 

 
 
 
/**************************function******************************/
