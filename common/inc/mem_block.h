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

/***************************************************************************
                          mem_block.h  -  description
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#ifndef __MEM_BLOCK_H__
#define __MEM_BLOCK_H__

#include <stdint.h>
#include <stddef.h>
#include <d2d_system_global.h>
#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------

typedef struct mem_block_t {
	  struct mem_block_t *next;
	  struct mem_block_t *previous;
	  size_t size;
	  unsigned char pool_id; //!<从哪个pool中获取的
	  long   data;  //用long 保存数据地址
} mem_block_t;




//-----------------------------------------------------------------------------

void        *pool_buffer_init (void);
void        *pool_buffer_clean (void *arg);
void         free_mem_block (mem_block_t * leP, const char* caller,uint32_t line);
mem_block_t* get_free_mem_block (uint32_t sizeP, const char* caller,uint32_t line);
mem_block_t *get_free_copy_mem_block (void);
mem_block_t *get_free_copy_mem_block_up (void);
mem_block_t *copy_mem_block (mem_block_t * leP, mem_block_t * destP);
void         display_mem_load (void);

void         check_mem_area (void);
void        check_free_mem_block (mem_block_t * leP);
#define MEM_SCALE D2D_MAX_USER_NUM
// definition of the size of the allocated memory area
#define MEM_MNGT_MB0_BLOCK_SIZE     64
// 64
#define MEM_MNGT_MB0_NB_BLOCKS      4096 * MEM_SCALE
#define MEM_MNGT_POOL_ID0           0

#define MEM_MNGT_MB1_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*2
// 128
#define MEM_MNGT_MB1_NB_BLOCKS      4096 * MEM_SCALE
#define MEM_MNGT_POOL_ID1           1

#define MEM_MNGT_MB2_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*4
// 256
#define MEM_MNGT_MB2_NB_BLOCKS      2048 * MEM_SCALE
#define MEM_MNGT_POOL_ID2           2

#define MEM_MNGT_MB3_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*8
// 512
#define MEM_MNGT_MB3_NB_BLOCKS      2048 * MEM_SCALE
#define MEM_MNGT_POOL_ID3           3

#define MEM_MNGT_MB4_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*16
// 1024
#define MEM_MNGT_MB4_NB_BLOCKS      1024 * MEM_SCALE
#define MEM_MNGT_POOL_ID4           4

#define MEM_MNGT_MB5_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*32
// 2048
#define MEM_MNGT_MB5_NB_BLOCKS      1024 * MEM_SCALE // LG WAS 1024
#define MEM_MNGT_POOL_ID5           5

#define MEM_MNGT_MB6_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*64
// 4096
#define MEM_MNGT_MB6_NB_BLOCKS      1024 * MEM_SCALE  // LG WAS 256
#define MEM_MNGT_POOL_ID6           6

#define MEM_MNGT_MB7_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*128
// 8192
#define MEM_MNGT_MB7_NB_BLOCKS      64* MEM_SCALE   // LG WAS 32
#define MEM_MNGT_POOL_ID7           7

#define MEM_MNGT_MB8_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*256

#ifdef JUMBO_FRAMES
#define MEM_MNGT_MB8_NB_BLOCKS      256 * MEM_SCALE
#else
#define MEM_MNGT_MB8_NB_BLOCKS      16 * MEM_SCALE
// 16384
#endif
#define MEM_MNGT_POOL_ID8           8

#define MEM_MNGT_MB9_BLOCK_SIZE     MEM_MNGT_MB0_BLOCK_SIZE*512
// 32768
#define MEM_MNGT_MB9_NB_BLOCKS      8 * MEM_SCALE
#define MEM_MNGT_POOL_ID9           9

#define MEM_MNGT_MB10_BLOCK_SIZE    MEM_MNGT_MB0_BLOCK_SIZE*1024
// 65536
#define MEM_MNGT_MB10_NB_BLOCKS     0 * MEM_SCALE
#define MEM_MNGT_POOL_ID10          10

#define MEM_MNGT_MB11_BLOCK_SIZE    MEM_MNGT_MB0_BLOCK_SIZE*2048
// 131072
#define MEM_MNGT_MB11_NB_BLOCKS     0 * MEM_SCALE
#define MEM_MNGT_POOL_ID11          11

#define MEM_MNGT_MB12_BLOCK_SIZE    MEM_MNGT_MB0_BLOCK_SIZE*4096
// 262144
#define MEM_MNGT_MB12_NB_BLOCKS     32 * MEM_SCALE
#define MEM_MNGT_POOL_ID12          12


#define MEM_MNGT_MBCOPY_NB_BLOCKS   1024
#define MEM_MNGT_NB_ELEMENTS        MEM_MNGT_MB0_NB_BLOCKS + MEM_MNGT_MB1_NB_BLOCKS + MEM_MNGT_MB2_NB_BLOCKS + MEM_MNGT_MB3_NB_BLOCKS + MEM_MNGT_MB4_NB_BLOCKS + MEM_MNGT_MB5_NB_BLOCKS + MEM_MNGT_MB6_NB_BLOCKS + MEM_MNGT_MB7_NB_BLOCKS + MEM_MNGT_MB8_NB_BLOCKS + MEM_MNGT_MB9_NB_BLOCKS + MEM_MNGT_MB10_NB_BLOCKS + MEM_MNGT_MB11_NB_BLOCKS + MEM_MNGT_MB12_NB_BLOCKS + MEM_MNGT_MBCOPY_NB_BLOCKS
#define MEM_MNGT_POOL_ID_COPY        13

#define LIST_NAME_MAX_CHAR 32


typedef struct {
  struct mem_block_t *head;
  struct mem_block_t *tail;
  int                nb_elements;
  char               name[LIST_NAME_MAX_CHAR];
} list2_t;
//-----------------------------------------------------------------------------
typedef struct {
  struct mem_block_t *head;
  struct mem_block_t *tail;
  int                nb_elements;
  char               name[LIST_NAME_MAX_CHAR];
} list_t;



typedef struct {
  //-----------------------------------------------------------
  // basic memory management
  //-----------------------------------------------------------
  //！按照不同的block 大小，进行设置
  //！[4096*UE个数][64]
  char              mem_pool0[MEM_MNGT_MB0_NB_BLOCKS][MEM_MNGT_MB0_BLOCK_SIZE];
  //！[4096 *UE个数][64*2^1]
  char              mem_pool1[MEM_MNGT_MB1_NB_BLOCKS][MEM_MNGT_MB1_BLOCK_SIZE];
  //![2048 * UE个数][64 * 2^2]
  char              mem_pool2[MEM_MNGT_MB2_NB_BLOCKS][MEM_MNGT_MB2_BLOCK_SIZE];
  char              mem_pool3[MEM_MNGT_MB3_NB_BLOCKS][MEM_MNGT_MB3_BLOCK_SIZE];
  char              mem_pool4[MEM_MNGT_MB4_NB_BLOCKS][MEM_MNGT_MB4_BLOCK_SIZE];
  char              mem_pool5[MEM_MNGT_MB5_NB_BLOCKS][MEM_MNGT_MB5_BLOCK_SIZE];
  char              mem_pool6[MEM_MNGT_MB6_NB_BLOCKS][MEM_MNGT_MB6_BLOCK_SIZE];
  char              mem_pool7[MEM_MNGT_MB7_NB_BLOCKS][MEM_MNGT_MB7_BLOCK_SIZE];
  char              mem_pool8[MEM_MNGT_MB8_NB_BLOCKS][MEM_MNGT_MB8_BLOCK_SIZE];
  char              mem_pool9[MEM_MNGT_MB9_NB_BLOCKS][MEM_MNGT_MB9_BLOCK_SIZE];
  char              mem_pool10[MEM_MNGT_MB10_NB_BLOCKS][MEM_MNGT_MB10_BLOCK_SIZE];
  char              mem_pool11[MEM_MNGT_MB11_NB_BLOCKS][MEM_MNGT_MB11_BLOCK_SIZE];
  char              mem_pool12[MEM_MNGT_MB12_NB_BLOCKS][MEM_MNGT_MB12_BLOCK_SIZE];
  mem_block_t     mem_blocks[MEM_MNGT_NB_ELEMENTS];  //！memory 管理数组
  list_t          mem_lists[14];  //！链表

} mem_pool;

mem_pool  *memBlockVar ;
#define mem_block_var (*memBlockVar)

#ifdef __cplusplus
}
#endif
#endif
