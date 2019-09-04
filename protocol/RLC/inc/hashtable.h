/******************************************************************
 * @file  hashtable.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年9月3日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年9月3日        bo.liu       create file

*****************************************************************/

#ifndef     HASHTABLE_H
#define     HASHTABLE_H
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef size_t   hash_size_t;
typedef uint64_t hash_key_t;

#define HASHTABLE_NOT_A_KEY_VALUE ((uint64_t)-1)

typedef enum hashtable_return_code_e {
    HASH_TABLE_OK                      = 0,
    HASH_TABLE_INSERT_OVERWRITTEN_DATA = 1,
    HASH_TABLE_KEY_NOT_EXISTS          = 2,
    HASH_TABLE_KEY_ALREADY_EXISTS      = 3,
    HASH_TABLE_BAD_PARAMETER_HASHTABLE = 4,
    HASH_TABLE_SYSTEM_ERROR            = 5,
    HASH_TABLE_CODE_MAX
} hashtable_rc_t;


typedef struct hash_node_s {
    hash_key_t          key;
    void               *data;
    struct hash_node_s *next;
} hash_node_t;

typedef struct hash_table_s {
	hash_size_t         size;
	hash_size_t         num_elements;
	struct hash_node_s **nodes;
	hash_size_t       (*hashfunc)(const hash_key_t); //!hash key generate function 
	void              (*freefunc)(void*);     //! hash node free function 
} hash_table_t;

char*           hashtable_rc_code2string(hashtable_rc_t rcP);
void            hash_free_int_func(void* memoryP);
hash_table_t   *hashtable_create (const hash_size_t   size, hash_size_t (*hashfunc)(const hash_key_t ),
                                        void (*freefunc)(void*));
hashtable_rc_t  hashtable_destroy(hash_table_t * const hashtbl);
hashtable_rc_t  hashtable_is_key_exists (const hash_table_t * const hashtbl, const uint64_t key);
hashtable_rc_t  hashtable_apply_funct_on_elements (hash_table_t * const hashtblP, 
                                                              void funct(hash_key_t keyP, 
                                                              void *dataP, void* parameterP), void* parameterP);
hashtable_rc_t  hashtable_dump_content (const hash_table_t * const hashtblP, char * const buffer_pP,
                                                int * const remaining_bytes_in_buffer_pP );
hashtable_rc_t  hashtable_insert (hash_table_t * const hashtbl, const hash_key_t key, void *data);
hashtable_rc_t  hashtable_remove (hash_table_t * const hashtbl, const hash_key_t key);
hashtable_rc_t  hashtable_get    (const hash_table_t * const hashtbl, const hash_key_t key, void **dataP);
hashtable_rc_t  hashtable_resize (hash_table_t * const hashtbl, const hash_size_t size);




 
 
 
 
#endif
