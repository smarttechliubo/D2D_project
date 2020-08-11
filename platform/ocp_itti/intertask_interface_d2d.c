/*
  Author: Laurent THOMAS, Open Cells
  copyleft: OpenAirInterface Software Alliance and it's licence 
*/

//#include <sys/eventfd.h>
#include <message_list.h>
#include <pthread.h>

#include <intertask_interface.h>
#include <log.h>
#include <errno.h>
#if    0


typedef struct timer_elm_s {
  timer_type_t      type;     ///< Timer type
  long              instance;
  long duration;
  uint64_t timeout;
  void              *timer_arg; ///< Optional argument that will be passed when timer expires
} timer_elm_t ;





int timer_expired(int fd);
task_list_t tasks[TASK_MAX];




void message_free_mem_block (MessageDef *leP, const char *caller) {
	AssertFatal(leP!=NULL,DRIVER,"free memory error");
	free(leP);
}




void *itti_malloc(task_id_t origin_task_id, task_id_t destination_task_id, int size) {
void *ptr = NULL;
AssertFatal ((ptr=malloc (size)) != NULL, DRIVER,"Memory allocation of %zu bytes failed (%d -> %d)!\n",
             size, origin_task_id, destination_task_id);
return ptr;
}

int itti_free(task_id_t task_id, void *ptr) {
AssertFatal (ptr != NULL,DRIVER, "Trying to free a NULL pointer (%d)!\n", task_id);
free (ptr);
return (EXIT_SUCCESS);
}

MessageDef *itti_alloc_new_message_sized(task_id_t origin_task_id, MessagesIds message_id, MessageHeaderSize size) {
	MessageDef *temp = (MessageDef *)itti_malloc (origin_task_id, TASK_UNKNOWN, sizeof(MessageHeader) + size);
	temp->ittiMsgHeader.messageId = message_id;
	temp->ittiMsgHeader.originTaskId = origin_task_id;
	temp->ittiMsgHeader.ittiMsgSize = size;
	return temp;
}

MessageDef *itti_alloc_new_message(task_id_t origin_task_id, MessagesIds message_id,
                                          char *message_ptr,int message_size) 
{
	int size=sizeof(MessageDef) + message_size;
	MessageDef *temp = (MessageDef *)itti_malloc (origin_task_id, TASK_UNKNOWN, size);
	temp->ittiMsgHeader.messageId = message_id;
	temp->ittiMsgHeader.originTaskId = origin_task_id;
	temp->ittiMsgHeader.ittiMsgSize = size;
	temp->previous = NULL; 
	temp->next = NULL;
	temp->message_ptr = message_ptr;

	return temp;

}

static inline int itti_send_msg_to_task_locked(task_id_t destination_task_id, 
                                                        instance_t instance, 
                                                        MessageDef *message) {
	task_list_t *t=tasks+destination_task_id;
	message->ittiMsgHeader.destinationTaskId = destination_task_id;
	message->ittiMsgHeader.instance = instance;
	message->ittiMsgHeader.lte_time.frame = 0;
	message->ittiMsgHeader.lte_time.slot = 0;
	int message_id = message->ittiMsgHeader.messageId;

	//！往队列中添加消息
	size_t s=t->message_queue.nb_elements;
#if 0
	if ( s > t->admin.queue_size )
	  LOG_ERROR(DRIVER,"Queue for %s task contains %ld messages\n", itti_get_task_name(destination_task_id), s );

	if ( s > 50 )
	  LOG_I(DRIVER,"Queue for %s task size: %ld\n",itti_get_task_name(destination_task_id), s+1);
#endif
	//！往队列中添加消息
	//t->message_queue.insert(t->message_queue.begin(), message);
	message_list_add_tail_eurecom(message, &(t->message_queue));
	
#if 0
	eventfd_t sem_counter = 1;
	AssertFatal ( sizeof(sem_counter) == write(t->sem_fd, &sem_counter, sizeof(sem_counter)), "");
	LOG_D(DRIVER,"sent messages id=%d to %s\n",message_id, t->admin.name);
#endif 
	// LOG_DEBUG(DRIVER,"insert finished,message number = %d \n",t->message_queue.nb_elements);
	return 0;
}

int itti_send_msg_to_task(task_id_t destination_task_id, instance_t instance, MessageDef *message) {
	task_list_t *t=&tasks[destination_task_id];

	pthread_mutex_lock(&t->queue_cond_lock);
	int ret=itti_send_msg_to_task_locked(destination_task_id, instance, message);
#if 0
while ( t->message_queue.size()>0 && t->admin.func != NULL ) {
  if (t->message_queue.size()>1)
LOG_W(DRIVER,"queue in no thread mode is %ld\n", t->message_queue.size());
  pthread_mutex_unlock (&t->queue_cond_lock);
  t->admin.func(NULL); //!？直接调用目的task的主处理函数么？没有看到func的初始化？ 
  pthread_mutex_lock (&t->queue_cond_lock);
} 
#endif 


	LOG_INFO(DRIVER,"task:%d send message:%d to task:%d ,msg_ptr = %ld\n", message->ittiMsgHeader.originTaskId,
	   message->ittiMsgHeader.messageId, message->ittiMsgHeader.destinationTaskId,
	      (message->message_ptr));
	pthread_mutex_unlock (&t->queue_cond_lock);
	return ret;
}



int itti_receive_msg(task_id_t task_id, MessageDef **received_msg) {
// Reception of one message, blocking caller
	task_list_t *t=&tasks[task_id];
	//LOG_DEBUG(DRIVER,"task_id:%d start to fetch receive lock\n",task_id);
	pthread_mutex_lock(&t->queue_cond_lock);

	
#if 0
	// Weird condition to deal with crap legacy itti interface
	if ( t->nb_fd_epoll == 1 ) {
	  while (t->message_queue.empty()) {
	    itti_get_events_locked(task_id, &t->events);
	    pthread_mutex_lock(&t->queue_cond_lock);
	  }
	} else {
	  if (t->message_queue.empty()) {
	    itti_get_events_locked(task_id, &t->events);
	    pthread_mutex_lock(&t->queue_cond_lock);
	  }
	}
#endif 
	// Legacy design: we return even if we have no message
	// in this case, *received_msg is NULL
	if (0 == t->message_queue.nb_elements) {
	  *received_msg=NULL;
	 	//!LOG_ERROR(DRIVER,"task_id:%d:there is no message,error\n",task_id);
	 	pthread_mutex_unlock (&t->queue_cond_lock);//！release the lock 
	  	return -1; 
	 
	} else {
	
	//！从队列中获取消息
	 // *received_msg=t->message_queue.back( );
	 // t->message_queue.pop_back();
	 //!取最新的消息
	// LOG_DEBUG(DRIVER,"task: %d receive new message,message number in queue = %d\n",task_id,t->message_queue.nb_elements);
	 *received_msg = message_list_remove_head(&(t->message_queue));
 
	}

	pthread_mutex_unlock (&t->queue_cond_lock);
	return 0;
}


void itti_free_message(MessageDef *received_msg)
{
 
  LOG_DEBUG(DRIVER,"task_id:%d, message_ptr:%lld, free memory address :%ld\n",received_msg->ittiMsgHeader.destinationTaskId,received_msg,
  					received_msg->message_ptr);
  free(received_msg->message_ptr);
}



int itti_create_task(task_id_t task_id, void *(*start_routine)(void *), int task_priority,void *args_p) {
	 task_list_t *t=&tasks[task_id];
	//!返回thread ip给t->pthread
	 AssertFatal( pthread_create (&t->thread, NULL, start_routine, args_p) ==0,DRIVER,
	              "Thread creation for task %d failed!\n", task_id);
	//!给线程设置名字
	pthread_setname_np(t->thread, itti_get_task_name(task_id) );
	//LOG_I(DRIVER,"Created Posix thread %s\n",  itti_get_task_name(task_id) );
#if 1 // BMC test RT prio
	{
	  int policy;
	  pthread_attr_t attr;
	  struct sched_param getparam;
	  struct sched_param sparam;
	  memset(&sparam, 0, sizeof(sparam));
	  //!设置调度类型为sched_fifo,先入先出，高优先级抢占低优先级
	  #if 0
	  if (task_id == 1)
	  {
		sparam.sched_priority = sched_get_priority_max(SCHED_FIFO)-11;
	  }
	  else if (task_id == 2)
	  {
		sparam.sched_priority = sched_get_priority_max(SCHED_FIFO)-10;
	  }
	  else 
	  {
		sparam.sched_priority = sched_get_priority_max(SCHED_FIFO)-13;
	  }：
	  
	  #else 

	  sparam.sched_priority =  task_priority;
	  if ( (sparam.sched_priority < sched_get_priority_min(SCHED_FIFO))
			  ||  (sparam.sched_priority > sched_get_priority_max(SCHED_FIFO)))
	  {
		  AssertFatal(0,DRIVER,"task  %s : pthread priority:%d exceed the limit in [%d,%d]\n",  itti_get_task_name(task_id) ,
					sparam.sched_priority,sched_get_priority_min(SCHED_FIFO),sched_get_priority_max(SCHED_FIFO)	);

	  }
	  #endif 
	  LOG_DEBUG(DRIVER,"taskid =%d,min_priority = %d,max_priority = %d\n",task_id,sched_get_priority_min(SCHED_FIFO),
			   sched_get_priority_max(SCHED_FIFO) );
	  policy = SCHED_FIFO ; 
	  //!设置了同一个优先级
	  errno = 0; 
	  if (pthread_setschedparam(t->thread, policy, &sparam) != 0) {
		  LOG_ERROR(DRIVER,"task %s : Failed to set pthread priority,errno = %d\n",  itti_get_task_name(task_id),errno );
	    //printf("task %s : Failed to set pthread priority\n",  itti_get_task_name(task_id));
	  }
	  else 
	  {
		pthread_getschedparam(t->thread,&policy, &getparam);
		LOG_ERROR(DRIVER,"create task id = %d, priority=%d\n", task_id,getparam.sched_priority);
	  }

	  
	}
#endif    
	return 0;
}

  void itti_exit_task(void) {
    pthread_exit (NULL);
  }

  void itti_terminate_tasks(task_id_t task_id) {
    // Sends Terminate signals to all tasks.
    itti_send_terminate_message (task_id);
    usleep(100*1000); // Allow the tasks to receive the message before going returning to main thread
  }

   //!进行thread创建，以及互斥锁的初始化
  int itti_init(task_id_t task_max, const task_info_t *tasks_info) {
   // AssertFatal(TASK_MAX<UINT16_MAX, "Max itti tasks");
    //！所有的task的参数初始化，
    //! 对互斥锁进行初始化
    for(int i=0; i<task_max; ++i) {
     LOG_DEBUG(DRIVER,"Starting itti queue: %s as task %d\n", tasks_info[i].name, i);
      pthread_mutex_init(&tasks[i].queue_cond_lock, NULL);
      memcpy(&tasks[i].admin, &tasks_info[i], sizeof(task_info_t));
    //  AssertFatal( ( tasks[i].epoll_fd = epoll_create1(0) ) >=0, "");
   //   AssertFatal( ( tasks[i].sem_fd = eventfd(0, EFD_SEMAPHORE) ) >=0, "");
   //   itti_subscribe_event_fd((task_id_t)i, tasks[i].sem_fd);

      if (tasks[i].admin.threadFunc != NULL)
        itti_create_task((task_id_t)i, tasks[i].admin.threadFunc,tasks[i].admin.priority, NULL);
    }

    return 0;
  }



  const char *itti_get_message_name(MessagesIds message_id) {
   // return messages_info[message_id].name;
  }

  const char *itti_get_task_name(task_id_t task_id) {
    return tasks[task_id].admin.name;
  }

  // void for compatibility
  void itti_send_terminate_message(task_id_t task_id) {
  }

  void itti_wait_tasks_end(void) {
    while(1)
      sleep(24*3600);
  }

#endif 
/****************************************************************************************************************/

MessageDef *itti_alloc_new_message(task_id_t origin_task_id, MessagesIds message_id,
											char *message_backup_ptr,int message_size) 
{
  	      
	  MessageDef  * message; 
	  char * message_content_ptr; 
	  Osp_Msg_Head *temp = OSP_Alloc_Msg(message_size);
	  temp->MsgType = message_id; 
	  temp->MsgSize = message_size; 
	  temp->SrcId = origin_task_id;

	  message = (MessageDef *)temp; 
	  message->ittiMsgHeader = *(Osp_Msg_Head *)temp;
  
	  message_content_ptr = MSG_HEAD_TO_COMM(temp); 

	  memcpy((void *)(message_content_ptr), message_backup_ptr, message_size); 

	  OSP_Free_Mem((char *)message_backup_ptr); //!calloc 对应free, osp_alloc_mem 对应osp_free_mem 

	  return message;
}



int itti_send_msg_to_task(task_id_t destination_task_id, instance_t instance, MessageDef *message) 
{
	
    OSP_STATUS   ret;
	message->ittiMsgHeader.DstId = destination_task_id; 
	ret = OSP_Send_Msg((Osp_Msg_Head *)message);
    if (OSP_OK == ret)
    {

		LOG_INFO(DRIVER,"task:%d send message:%d to task:%d successful!\n", message->ittiMsgHeader.SrcId,
	   				message->ittiMsgHeader.MsgType, message->ittiMsgHeader.DstId);
	   	return ret;
	}
	else 
	{
		AssertFatal(0, DRIVER, "task:%d send message:%d to task:%d fail !\n", message->ittiMsgHeader.SrcId,
	   				message->ittiMsgHeader.MsgType, message->ittiMsgHeader.DstId)
	}
	
}


void itti_free_message(MessageDef *received_msg)
{
 	OSP_Free_Msg((Osp_Msg_Head*)received_msg);
}           
