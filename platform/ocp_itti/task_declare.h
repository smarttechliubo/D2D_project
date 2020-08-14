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
extern int  ip_rlc_sdu_udp_initial(void );

extern void timer_int_task(MessageDef *recv_msg); 
extern int timer_int_init( ); 

extern int32_t init_mac_period();
extern int32_t init_mac_scheduler();
extern void run_period(MessageDef* msg);
extern void run_scheduler();

extern uint32_t init_phy_tx_sim();
extern uint32_t init_phy_rx_sim();
extern void phy_tx_sim_thread(MessageDef* msg);
extern void phy_rx_sim_thread(MessageDef *msg);
#endif
