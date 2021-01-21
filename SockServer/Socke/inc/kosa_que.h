#ifndef __KOSA_QUE_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_QUE_H__

#include <kosa.h>
#include <kosa_mutex.h>
#ifdef _LINUX_
#include <pthread.h>
#endif

typedef struct {
    Uint32 msgType;
    /*��KOSA_Msg�����ڴ�����ƫ�ƣ���Ҫ���е���malloc*/
    Void* pMsgBody;
    Bool waitAck;
}KOSA_Msg;

typedef struct {
    Uint32 curRd;
    Uint32 curWr;
    Uint32 len;
    Uint32 count;
    Void **queue;
	KOSA_MutexHndl mutex;
	KOSA_SemHndl semRd;
	KOSA_SemHndl semWr;
} KOSA_QueHndl;


KOSA_API Int32 KOSA_queCreate(KOSA_QueHndl* hndl, Uint32 maxLen);

KOSA_API Int32 KOSA_queDelete(KOSA_QueHndl* hndl);

KOSA_API Int32 KOSA_quePut(KOSA_QueHndl* hndl, Void* value, Uint32 timeout);

KOSA_API Int32 KOSA_queGet(KOSA_QueHndl* hndl, Void** value, Uint32 timeout);

KOSA_API Uint32 KOSA_queGetQueuedCount(KOSA_QueHndl* hndl);

/*�����õ�ַ������������*/
KOSA_API Int32 KOSA_queGetById(KOSA_QueHndl* hndl, Void** value, Uint32 id);
/*Σ��...............ע��Ƕ�׵�������*/
KOSA_API Int32 KOSA_queLock(KOSA_QueHndl* hndl);
KOSA_API Int32 KOSA_queUnLock(KOSA_QueHndl* hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_QUE_H__ */