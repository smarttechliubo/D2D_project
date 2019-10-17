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
#include <log.h>
#include <dummy_test_rrc.h>
#include <rrc_global_def.h>
#include <rlc.h>
#include <test_time.h>
int main()
{

  
    unsigned long start_time; 
    unsigned long end_time ;

    
 
   
    
	itti_init(6, &tasks_info);
	
    rrc_module_Initial(); 
    
    
    rlc_module_initial(); 
    
    
	
	itti_create_task(TASK_D2D_RRC, rrc_Sche_Task, NULL);
	itti_create_task(TASK_D2D_RLC, rlc_rrc_config_task, NULL);
#ifdef  RRC_SOURCE 
	itti_create_task(TASK_D2D_DUMMY, dummy_rrc_test, D2D_MODE_TYPE_SOURCE);
#else 
	itti_create_task(TASK_D2D_DUMMY, dummy_rrc_test, D2D_MODE_TYPE_DESTINATION);
#endif 

	while(1) {}
	
	return 0;
}
