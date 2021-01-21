#ifndef __KOSA_TIMER_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_TIMER_H__

#include <kosa.h>
#include <kosa_tsk.h>
#include <kosa_mutex.h>
#include <kosa_list.h>

typedef Int32 (*timerCb)(Void* );

typedef struct
{
    Bool bIdle;
    void* cbMethodArgs;
    timerCb cbMethod;
    Int32 startInterval;
    KOSA_SemHndl waitReady;
    void* (*run)(Void* );
    KOSA_TskObj task;
    struct list_head listNode;
}KOSA_timer;

/*method: ��ʱ��ִ�еķ�����method�ķ���ֵ��ʾ��ʱ�೤ʱ���ٴ�ִ�и÷���������0��timer����
  methodArgs: ���������Ĳ���
  delayMs: ��ʱ�೤ʱ�䣬ִ�и÷���*/
KOSA_API Int32 KOSA_timerStart(timerCb method, Void* methodArgs, Int32 delayMs);

KOSA_API Void KOSA_timerInit(Int32 maxTimer);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_TIMER_H__ */

