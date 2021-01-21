#ifndef __KOSA_SERIAL_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_SERIAL_H__

#include "kosa.h"



#define INVALID_SERIAL_HANDLE   0

/* ���ô��ڵĲ����� */
#define SERIAL_SET_BAUDRATE                   0
/* ��ѯ���ڵĲ����� */
#define SERIAL_GET_BAUDRATE                   1
/* ���ô��ڵ�ֹͣλ */
#define SERIAL_SET_STOPBIT                    2
/* ��ѯ���ڵ�ֹͣλ */
#define SERIAL_GET_STOPBIT                    3
/* ���ô��ڵ�����λ */
#define SERIAL_SET_DATABIT                    4
/* ��ѯ���ڵ�����λ */
#define SERIAL_GET_DATABIT                    5
/* ���ô��ڵ���żУ��λ */
#define SERIAL_SET_PARITY                     6
/* ��ѯ���ڵ���żУ��λ */
#define SERIAL_GET_PARITY                     7
/* ���ô��ڵ���ʼλ */
#define SERIAL_SET_STARTBIT                   8
/* ��ѯ���ڵ���ʼλ */
#define SERIAL_GET_STARTBIT                   9
/* ���������� */
#define SERIAL_SET_FLOW_CONTROL               10
/* ���������� */
#define SERIAL_GET_FLOW_CONTROL               11

/* =============================================================================
 * Enums
 * =============================================================================
 */
/*У������*/
typedef enum
{
    NO_PARITY,                    //��У��
    EVEN_PARITY,                  //żУ��
    ODD_PARITY,                   //��У��
    SPACE_PARITY,                 //�ո�У��
    MARK_PARITY,                  //���У��
}EParityMode;

/*�����豸���*/
typedef struct {
#ifdef _LINUX_
    Int32 fd;
#endif
#if defined(_MSC_VER) || defined(_MINGW_)
    HANDLE fd;
#endif
} KOSA_SerialHndl;

/*���ڲ���*/
typedef struct {
    Uint32 nBaudrate;           //������
    Uint32 nStopBits;           //ֹͣλ
    Uint32 nDataBits;           //����λ
    Uint32 nParity;             //��żУ��λ
}KOSA_SerialParams;

/* =============================================================================
 * APIs
 * =============================================================================
 */
KOSA_API Int32 KOSA_SerialOpen(KOSA_SerialHndl *hndl, Char *device);

KOSA_API Int32 KOSA_SerialClose(KOSA_SerialHndl *hndl);

KOSA_API Int32 KOSA_SerialSetParams(KOSA_SerialHndl *hndl, KOSA_SerialParams *params);

KOSA_API Int32 KOSA_SerialReadData(KOSA_SerialHndl *hndl, Uint8 *buf, Uint32 len);

KOSA_API Int32 KOSA_SerialWriteData(KOSA_SerialHndl *hndl, Uint8 *buf, Uint32 len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_SERIAL_H__ */

