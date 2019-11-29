/******************************************************************
 * @file  task_declare.h
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年11月19日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年11月19日        bo.liu       create file

*****************************************************************/

#ifndef     TASK_DECLARE_H
#define     TASK_DECLARE_H
 
#include <intertask_interface.h>


 
 
/*************************function declare**********************************************************/
extern int rrc_module_Initial();

extern void *rrc_Sche_Task(MessageDef *recv_msg); 


extern int rlc_module_initial( ); 
extern void *rlc_rrc_config_task(MessageDef *recv_msg);

extern void *rlc_tx_task(MessageDef *recv_msg);

extern void *rlc_rx_task( MessageDef *recv_msg); 

extern void dummy_test_task(MessageDef *rece_msg);

extern int dummy_init( );
extern void ip_udp_task( );

extern void *ip_msg_task(MessageDef *rec_msg);
#endif
