/*
  Author: Laurent THOMAS, Open Cells
  Copyleft: OpenAirInterface software alliance and it's licence 
*/
#ifndef INTERTASK_INTERFACE_H_
#define INTERTASK_INTERFACE_H_
#include <typedef.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

#include <osp_ex.h>



/*TASK: 32--127 */
typedef enum {
	TASK_UNKNOWN = 32, 
    TASK_D2D_RRC, 
    TASK_D2D_RLC, 
    TASK_D2D_RLC_TX,
    TASK_D2D_RLC_RX,
    TASK_D2D_IP_UDP,
    TASK_D2D_IP_MSG,
    TASK_D2D_MAC, 
    TASK_D2D_PHY,
    TASK_D2D_DUMMY,
    TASK_D2D_DUMMY_INT,
    TASK_ID_MAX_VALUE = 127

}task_id_t;



#if  0

#define  TASK_MAX       10




typedef enum timer_type_s {
  TIMER_PERIODIC,
  TIMER_ONE_SHOT,
  TIMER_TYPE_MAX,
} timer_type_t;

typedef struct {
  void *arg;
  long  timer_id;
} timer_has_expired_t;

typedef struct {
  uint32_t      interval_sec;
  uint32_t      interval_us;
  long     task_id;
  int32_t       instance;
  timer_type_t  type;
  void         *timer_arg;
  long          timer_id;
} timer_create_t;

typedef struct {
  long     task_id;
  long          timer_id;
} timer_delete_t;


typedef struct itti_lte_time_s {
  uint32_t frame;
  uint8_t slot;
} itti_lte_time_t;


typedef struct IttiMsgEmpty_s {
} IttiMsgEmpty;

typedef struct IttiMsgText_s {
  uint32_t  size;
  char      text[];
} IttiMsgText;




typedef uint32_t MessageHeaderSize;
typedef uint32_t itti_message_types_t;
typedef unsigned long message_number_t;
#define MESSAGE_NUMBER_SIZE (sizeof(unsigned long))

typedef enum task_priorities_e {
  TASK_PRIORITY_MAX       = 100,
  TASK_PRIORITY_MAX_LEAST = 85,
  TASK_PRIORITY_MED_PLUS  = 70,
  TASK_PRIORITY_MED       = 55,
  TASK_PRIORITY_MED_LEAST = 40,
  TASK_PRIORITY_MIN_PLUS  = 25,
  TASK_PRIORITY_MIN       = 10,
} task_priorities_t;

typedef struct {
  task_priorities_t priority;
  unsigned int queue_size;
  /* Printable name */
  char name[256];
  void *(*func)(void *) ;
  void *(*threadFunc)(void *) ;
} task_info_t;
//
//TASK_DEF(TASK_RRC_ENB,  TASK_PRIORITY_MED,  200, NULL,NULL)
//TASK_DEF(TASK_RRC_ENB,  TASK_PRIORITY_MED,  200, NULL, NULL)
//TASK_DEF(TASK_GTPV1_U,  TASK_PRIORITY_MED,  1000,NULL, NULL)
//TASK_DEF(TASK_UDP,      TASK_PRIORITY_MED,  1000, NULL, NULL)

#define FOREACH_TASK(TASK_DEF)     \
TASK_DEF(TASK_UNKNOWN,	TASK_PRIORITY_MED, 200, NULL, NULL)   \
TASK_DEF(TASK_D2D_RRC,	TASK_PRIORITY_MED, 200, NULL, NULL)   \
TASK_DEF(TASK_D2D_RLC,	TASK_PRIORITY_MED, 200, NULL, NULL)	\
TASK_DEF(TASK_D2D_RLC_TX,	TASK_PRIORITY_MED, 200, NULL, NULL) \
TASK_DEF(TASK_D2D_RLC_RX,	TASK_PRIORITY_MED, 200, NULL, NULL)	\
TASK_DEF(TASK_D2D_MAC,	TASK_PRIORITY_MED, 200, NULL, NULL)   \
TASK_DEF(TASK_D2D_IP,	TASK_PRIORITY_MAX, 200, NULL, NULL)   \
TASK_DEF(TASK_D2D_MAC_RLC,TASK_PRIORITY_MED, 200, NULL, NULL)   \
TASK_DEF(TASK_D2D_PHY,	TASK_PRIORITY_MED, 200, NULL, NULL)	\
TASK_DEF(TASK_D2D_DUMMY,TASK_PRIORITY_MED, 200, NULL, NULL)   



#define TASK_DEF(TaskID, pRIO, qUEUEsIZE, FuNc, ThreadFunc)          { pRIO, qUEUEsIZE, #TaskID, FuNc, ThreadFunc },

/* Map task id to printable name. */
static const task_info_t tasks_info[] = {
  FOREACH_TASK(TASK_DEF)
};

#define TASK_ENUM(TaskID, pRIO, qUEUEsIZE, FuNc,ThreadFunc ) TaskID,
//! Tasks id of each task
typedef enum {
  FOREACH_TASK(TASK_ENUM)
} task_id_t;


typedef task_id_t thread_id_t;

typedef enum message_priorities_e {
  MESSAGE_PRIORITY_MAX       = 100,
  MESSAGE_PRIORITY_MAX_LEAST = 85,
  MESSAGE_PRIORITY_MED_PLUS  = 70,
  MESSAGE_PRIORITY_MED       = 55,
  MESSAGE_PRIORITY_MED_LEAST = 40,
  MESSAGE_PRIORITY_MIN_PLUS  = 25,
  MESSAGE_PRIORITY_MIN       = 10,
} message_priorities_t;

#if 0
#define FOREACH_MSG(INTERNAL_MSG)         \
  INTERNAL_MSG(TIMER_HAS_EXPIRED,  MESSAGE_PRIORITY_MED, timer_has_expired_t, timer_has_expired) \
  INTERNAL_MSG(INITIALIZE_MESSAGE, MESSAGE_PRIORITY_MED, IttiMsgEmpty, initialize_message)  \
  INTERNAL_MSG(ACTIVATE_MESSAGE,   MESSAGE_PRIORITY_MED, IttiMsgEmpty, activate_message) \
  INTERNAL_MSG(DEACTIVATE_MESSAGE, MESSAGE_PRIORITY_MED, IttiMsgEmpty, deactivate_message)  \
  INTERNAL_MSG(TERMINATE_MESSAGE,  MESSAGE_PRIORITY_MAX, IttiMsgEmpty, terminate_message) \
  INTERNAL_MSG(MESSAGE_TEST,       MESSAGE_PRIORITY_MED, IttiMsgEmpty, message_test)

/* This enum defines messages ids. Each one is unique. */
typedef enum {
#define MESSAGE_DEF(iD, pRIO, sTRUCT, fIELDnAME) iD,
  FOREACH_MSG(MESSAGE_DEF)
#include <all_msg.h>
#undef MESSAGE_DEF
  MESSAGES_ID_MAX,
} MessagesIds;

typedef union msg_s {
#define MESSAGE_DEF(iD, pRIO, sTRUCT, fIELDnAME) sTRUCT fIELDnAME;
  FOREACH_MSG(MESSAGE_DEF)
#include <all_msg.h>
#undef MESSAGE_DEF
} msg_t;

typedef struct message_info_s {
  int id;
  message_priorities_t priority;
  /* Message payload size */
  MessageHeaderSize size;
  /* Printable name */
  const char name[256];
} message_info_t;

/* Map message id to message information */
static const message_info_t messages_info[] = {
#define MESSAGE_DEF(iD, pRIO, sTRUCT, fIELDnAME) { iD, pRIO, sizeof(sTRUCT), #iD },
  FOREACH_MSG(MESSAGE_DEF)
#include <all_msg.h>
#undef MESSAGE_DEF
};

#endif 



#define  UINT16_MAX     65536

typedef struct MessageHeader_s {
  MessagesIds messageId;          /**< Unique message id as referenced in enum MessagesIds */
  task_id_t  originTaskId;        /**< ID of the sender task */
  task_id_t  destinationTaskId;   /**< ID of the destination task */
  instance_t instance;            /**< Task instance for virtualization */
  itti_lte_time_t lte_time;
  MessageHeaderSize ittiMsgSize;         /**< Message size (not including header size) */
} MessageHeader;





typedef struct __attribute__ ((__packed__)) MessageDef_s {
  struct MessageDef_s *next;
  struct MessageDef_s *previous;
  MessageHeader ittiMsgHeader; /**< Message header */
 // msg_t         ittiMsg;
  char           *message_ptr;                      //!message_ptr; 


} MessageDef;

#define LIST_NAME_MAX_CHAR   256

typedef struct message_list_t_s{
  MessageDef  *head;
  MessageDef  *tail;
  int          nb_elements;
  char         name[LIST_NAME_MAX_CHAR];
} message_list_t;




typedef struct task_list_s {
  task_info_t admin;
  pthread_t thread;
  pthread_mutex_t queue_cond_lock;
  message_list_t message_queue;
//  std::map<long,timer_elm_t> timer_map;
 // uint64_t next_timer=UINT64_MAX;
//  struct epoll_event  *events =NULL;
//  int nb_fd_epoll=0;
//  int nb_events=0;
 // int epoll_fd=-1;
 // int sem_fd=-1;
} task_list_t;




/* Extract the instance from a message */
#define ITTI_MESSAGE_GET_INSTANCE(mESSAGE)  ((mESSAGE)->ittiMsgHeader.instance)
#define ITTI_MSG_ID(mSGpTR)                 ((mSGpTR)->ittiMsgHeader.messageId)
#define ITTI_MSG_ORIGIN_ID(mSGpTR)          ((mSGpTR)->ittiMsgHeader.originTaskId)
#define ITTI_MSG_DESTINATION_ID(mSGpTR)     ((mSGpTR)->ittiMsgHeader.destinationTaskId)
#define ITTI_MSG_INSTANCE(mSGpTR)           ((mSGpTR)->ittiMsgHeader.instance)
#define ITTI_MSG_NAME(mSGpTR)               itti_get_message_name(ITTI_MSG_ID(mSGpTR))
#define ITTI_MSG_ORIGIN_NAME(mSGpTR)        itti_get_task_name(ITTI_MSG_ORIGIN_ID(mSGpTR))
#define ITTI_MSG_DESTINATION_NAME(mSGpTR)   itti_get_task_name(ITTI_MSG_DESTINATION_ID(mSGpTR))
#define TIMER_HAS_EXPIRED(mSGpTR)           (mSGpTR)->ittiMsg.timer_has_expired
#endif 


/*******************************************************************************************************************************/

typedef  struct 
{
	Osp_Msg_Head ittiMsgHeader; /*message header */

	char *message_ptr;     /** message content */


}MessageDef; 

/** \brief Send a message to a task (could be itself)
  \param task_id Task ID
  \param instance Instance of the task used for virtualization
  \param message Pointer to the message to send
  @returns -1 on failure, 0 otherwise
 **/
int itti_send_msg_to_task(task_id_t task_id, instance_t instance, MessageDef *message);

/** \brief Add a new fd to monitor.
   NOTE: it is up to the user to read data associated with the fd
    \param task_id Task ID of the receiving task
    \param fd The file descriptor to monitor
 **/
void itti_subscribe_event_fd(task_id_t task_id, int fd);

/** \brief Remove a fd from the list of fd to monitor
    \param task_id Task ID of the task
    \param fd The file descriptor to remove
 **/
void itti_unsubscribe_event_fd(task_id_t task_id, int fd);

/** \brief Return the list of events excluding the fd associated with itti
    \param task_id Task ID of the task
    \param events events list
    @returns number of events to handle
 **/
int itti_get_events(task_id_t task_id, struct epoll_event **events);

/** \brief Retrieves a message in the queue associated to task_id.
   If the queue is empty, the thread is blocked till a new message arrives.
  \param task_id Task ID of the receiving task
  \param received_msg Pointer to the allocated message
 **/
int itti_receive_msg(task_id_t task_id, MessageDef **received_msg);

/** \brief Try to retrieves a message in the queue associated to task_id.
  \param task_id Task ID of the receiving task
  \param received_msg Pointer to the allocated message
 **/
void itti_poll_msg(task_id_t task_id, MessageDef **received_msg);

/** \brief Start thread associated to the task
   \param task_id task to start
   \param start_routine entry point for the task
   \param args_p Optional argument to pass to the start routine
   @returns -1 on failure, 0 otherwise
 **/
int itti_create_task(task_id_t task_id, void *(*start_routine)(void *), int task_priority,void *args_p);
void itti_free_message(MessageDef *received_msg);
/** \brief Exit the current task.
 **/ 
void itti_exit_task(void);

/** \brief Initiate termination of all tasks.
   \param task_id task that is completed
 **/
void itti_terminate_tasks(task_id_t task_id);

// Void for legacy compatibility
void itti_wait_ready(int wait_tasks);
void itti_mark_task_ready(task_id_t task_id);

/** \brief Return the printable string associated with the message
   \param message_id Id of the message
 **/
const char *itti_get_message_name(MessagesIds message_id);

/** \brief Return the printable string associated with a task id
   \param thread_id Id of the task
 **/
const char *itti_get_task_name(task_id_t task_id);



/** \brief Alloc and memset(0) a new itti message.
   \param origin_task_id Task ID of the sending task
   \param message_id Message ID
   @returns NULL in case of failure or newly allocated mesage ref
 **/
MessageDef *itti_alloc_new_message(task_id_t origin_task_id, MessagesIds message_id,
                                            char *message_ptr,int message_size) ;



#if 0
/** \brief Alloc and memset(0) a new itti message.
   \param origin_task_id Task ID of the sending task
   \param message_id Message ID
   \param size size of the payload to send
   @returns NULL in case of failure or newly allocated mesage ref
 **/
MessageDef *itti_alloc_new_message_sized(
  task_id_t         origin_task_id,
  MessagesIds       message_id,
  MessageHeaderSize size);

/** \brief handle signals and wait for all threads to join when the process complete.
   This function should be called from the main thread after having created all ITTI tasks.
 **/
void itti_wait_tasks_end(void);
#define  THREAD_MAX 0 //for compatibility
void itti_set_task_real_time(task_id_t task_id);

/** \brief Send a termination message to all tasks.
   \param task_id task that is broadcasting the message.
 **/
void itti_send_terminate_message(task_id_t task_id);

void *itti_malloc(task_id_t origin_task_id, task_id_t destination_task_id, int size);
void *calloc_or_fail(size_t size);
void *malloc_or_fail(size_t size);
int memory_read(const char *datafile, void *data, size_t size);



int itti_free(task_id_t task_id, void *ptr);



int itti_init(task_id_t task_max,const task_info_t *tasks_info);


int timer_setup(
  uint32_t      interval_sec,
  uint32_t      interval_us,
  task_id_t     task_id,
  int32_t       instance,
  timer_type_t  type,
  void         *timer_arg,
  long         *timer_id);


int timer_remove(long timer_id);
#define timer_stop timer_remove
int signal_handle(int *end);

extern  task_list_t tasks[TASK_MAX];
#endif 

#ifdef __cplusplus
}
#endif
#endif /* INTERTASK_INTERFACE_H_ */
