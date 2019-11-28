#ifndef __OSPH__
#define __OSPH__

#ifdef __cplusplus
extern "C" {
#endif




#ifndef WAIT_FOREVER
#define WAIT_FOREVER  (-1)
#endif

#ifndef NO_WAIT
#define NO_WAIT    0
#endif

    
#define OSP_OK              (0)
#define OSP_ERROR           (-1)

typedef int OSP_STATUS;
typedef int STATUS;
typedef unsigned char       U8;
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef signed char         S8;
typedef signed short        S16;
typedef signed int          S32;
/* ILP32 and LP64 compatibility */
typedef unsigned long       ULONG;
typedef signed long         SLONG;
typedef unsigned long       UINTPTR;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned char  bool;
#define true 1
typedef long long              S64;
typedef unsigned long long             U64;
#ifndef BOOL
typedef int   BOOL;
#endif
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif
extern __thread __typeof__(U32) thread_local_TaskId;
#define CURRENT_TASKID thread_local_TaskId
typedef SLONG (*OSP_FUNCPTR)();
/*0 ~31 MsgType is used by osp*/
/*0 ~15 TaskId is userd by osp , user taskid should be 16~ 65 */
#define OSPTASK_MAXID   31
typedef struct tag_Osp_Msg_Head
{
        U32   MsgType;                                                        
        U32   MsgSize;        /*is just msgdata len, not include Osp_Msg_Head*/                                                
        U32      SrcId;                                                     
        U32      DstId;
}Osp_Msg_Head;
#define MSG_HEAD_SIZE  sizeof(Osp_Msg_Head)
#define MSG_HEAD_TO_COMM(x)  (char *)((UINTPTR)x + MSG_HEAD_SIZE)
#define MSG_DADA_LEN(x) (x->MsgSize)
#define IS_TIMER_MSG(pMsg)((pMsg->SrcId)<32)

#define NOMSGTASK    0xaa000000
#define MSGTASK        0x55000000
#define RTTASK        0x00aa0000

#define RT_MSG_PRI(x) (x|MSGTASK|RTTASK)
#define RT_NOMSG_PRI(x) (x|NOMSGTASK|RTTASK)

typedef struct tag_OSP_TASKMSG_REG                                                                                         
{   
    U32      TaskId;
    U8       TaskName[32];
    U32      TaskPri;
    OSP_FUNCPTR Init;
    OSP_FUNCPTR MainLoop;
    U32      Cpu; 
    
}OSP_TASKMSG_REG;
#define CMD_DEBUG_LEVEL 0
#define ERR_DEBUG_LEVEL 1
#define WARNING_DEBUG_LEVEL 2
#define RUN_DEBUG_LEVEL 3
#define DBG_DEBUG_LEVEL 4

/*ms unit*/
OSP_STATUS OSP_delay(U32 delay);
/* osp send timer-msg to DstTaskId*/
void * OSP_timerCreateSim(U32 DstTaskId,  BOOL isRepeat, U32 timeout, U32 delay);
OSP_STATUS OSP_timerStart(void* pTimer);
char* OSP_Alloc_Mem( U32 Len);
void OSP_Free_Mem( char* pbuf);
Osp_Msg_Head* OSP_Alloc_Msg( U32 Len);
OSP_STATUS OSP_Send_Msg(Osp_Msg_Head *p_MsgHead);
Osp_Msg_Head* OSP_Rev_Msg(void);
Osp_Msg_Head* OSP_Rev_MsgTry(void);
Osp_Msg_Head* OSP_Rev_MsgTimeout(U32 timeout);
void OSP_Free_Msg( Osp_Msg_Head* pHead);
OSP_STATUS DebugOut(int level, char *fmt,...);
ULONG getOspTicks(void);
ULONG getOspCycel(void);
void OSP_atomicInc(int* pVar);
void OSP_atomicDec(int* pVar);
int OSP_atomicGet(int* pVar);
void OSP_atomicSet(int* pVar, int val);
void DebugOutSetLevel(int level);
OSP_STATUS DebugOutWithTime(int level, char *fmt,...);





#ifdef __cplusplus
}
#endif

#endif  /* __OSPH__ */

