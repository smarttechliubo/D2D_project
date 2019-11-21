#include <pthread.h>
#include <intertask_interface.h>
#include <interface_rrc_rlc.h>
#include <stdio.h>
#include<stdlib.h>
#include <log.h>
#if 0
void * dummy_func(void *args)
{
    int index = 0; 
    MessageDef * message; 
   
    rlc_rrc_buffer_rpt *rpt_ptr;

    for (index = 0; index < 10; index ++)
    {

   	  //！消息队列锁
      rpt_ptr = calloc(1,sizeof(rlc_rrc_buffer_rpt)); 
      rpt_ptr->request_id =  index; 
      rpt_ptr->rlc_buffer_data_size = 256; 
      rpt_ptr->rlc_buffer_valid = 1; 

      message = itti_alloc_new_message(TASK_D2D_DUMMY,  index, (char *)rpt_ptr, sizeof(rlc_rrc_buffer_rpt));
      //!alloc msg 
    
	  itti_send_msg_to_task(TASK_D2D_RRC, 0, message);
	}
    
	

}


void *rrc_process_func(void * arg)
{

	
    MessageDef *recv_msg;
    rlc_rrc_buffer_rpt *buffer_rpt; 
    char *msg_content_ptr;
    uint32_t requstid;

    while (1)
    {
        
		if (0 == itti_receive_msg(TASK_D2D_RRC, &recv_msg))
		{
			switch (recv_msg->ittiMsgHeader.originTaskId)
			{
				case TASK_D2D_DUMMY:
					buffer_rpt= (rlc_rrc_buffer_rpt * )recv_msg->message_ptr;
					LOG_DEBUG(DRIVER,"receive message from %d, msg_id =%d,msg_ptr = %ld,request_id = %d\n",
							recv_msg->ittiMsgHeader.originTaskId,
							recv_msg->ittiMsgHeader.messageId,
							(recv_msg->message_ptr),
							buffer_rpt->request_id
							);


					//LOG_DEBUG(DRIVER,("requset_id = %d\n",requstid);
					//!process message 

					itti_free_message(recv_msg);

					LOG_DEBUG(DRIVER,"process received message success! \n\n\n");
					break;


			}


		}
		
    }

}

#if 0
int main()
{

     
	itti_init(3, &tasks_info);
	

	itti_create_task(TASK_D2D_DUMMY, dummy_func, NULL);
	itti_create_task(TASK_D2D_RRC, rrc_process_func, NULL);
	
    sleep(10);
	return 0;
}
#endif 

#endif 

