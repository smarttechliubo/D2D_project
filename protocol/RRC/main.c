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



