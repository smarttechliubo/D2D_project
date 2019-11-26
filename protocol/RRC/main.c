/** @file rrc_thread.c
 *
 *  @brief
 *  @author bo.liu
 *  @date   2019/08/08
 *  COPYRIGHT NOTICE: (c) smartlogictech.
 *  All rights reserved.
 */
#include <typedef.h>
#include <intertask_interface.h>

#include <dummy_test_rrc.h>
#include <rrc_global_def.h>
#include <rlc.h>
#include <test_time.h>
#include <log.h>

#include <syslog.h>  



extern void ip_task( ); 
extern void *rlc_tx_task( ); 
extern void *rlc_rx_task( );

void test_syslog( )
{
    setlogmask(LOG_UPTO(LOG_ERR)); 
    syslog(LOG_ERR, "%s",  "------------------------ D2D_project start log ------------- \n");  
    closelog(); 

}


int gdb=1;
#if 0
int main()
{

  
    unsigned long start_time; 
    unsigned long end_time ;
    
    test_syslog();
#if  0
	itti_init(8, &tasks_info);
    rrc_module_Initial(); 
    rlc_module_initial(); 
    
  
	
	itti_create_task(TASK_D2D_RRC, rrc_Sche_Task,30, NULL);
	itti_create_task(TASK_D2D_RLC, rlc_rrc_config_task,30, NULL);
	itti_create_task(TASK_D2D_IP,  ip_task,25,NULL);
	itti_create_task(TASK_D2D_RLC_TX,rlc_tx_task,20,NULL);
	itti_create_task(TASK_D2D_RLC_RX,rlc_rx_task,20,NULL);
	
#ifdef  RRC_SOURCE 
	itti_create_task(TASK_D2D_DUMMY, dummy_rrc_test,29, D2D_MODE_TYPE_SOURCE);
#else 
	itti_create_task(TASK_D2D_DUMMY, dummy_rrc_test,29, D2D_MODE_TYPE_DESTINATION);
#endif 
#endif 

	while(1) {}
	
	return 0;
}
#endif 

