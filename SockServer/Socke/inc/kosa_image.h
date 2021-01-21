#ifndef __KOSA_IMAGE_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __KOSA_IMAGE_H__

#include <kosa.h>

/* =============================================================================
 * Defines
 * =============================================================================
 */
/* MD5У���㷨��Ҫʹ�õĺ� */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define FF 255
#define D8 216
#define FE 254
#define D9 217

/* MD5�㷨���� */
#define MD5STEP(f, w, x, y, z, data, s) \
    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )


/* =============================================================================
 * Enums
 * =============================================================================
 */


/* =============================================================================
 * Structures
 * =============================================================================
 */
#if !defined(_MSC_VER) && !defined(_MINGW_)
#pragma pack(push) //�������״̬
#pragma pack(2)//�趨Ϊ4�ֽڶ���
/*BMP�ļ�ͷ�ṹ*/
typedef struct
{
    Uint16        bfType;
    unsigned long bfSize;
    Uint16        bfReserved1;
    Uint16        bfReserved2;
    unsigned long bfOffBits;
} BITMAPFILEHEADER;

/*BMP�ļ���Ϣͷ�ṹ*/
typedef struct
{
    unsigned long biSize;
    long          biWidth;
    long          biHeight;
    Uint16        biPlanes;
    Uint16        biBitCount;
    unsigned long biCompression;
    unsigned long biSizeImage;
    long          biXPelsPerMeter;
    long          biYPelsPerMeter;
    unsigned long biClrUsed;
    unsigned long biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)//�ָ�����״̬
#endif
/*YUV420Pͼ�������㷨����*/
typedef enum
{
    SCALE_TYEP_NEAREST,
    SCALE_TYPE_BILINEAR,
} KOSA_SCALE_TYPE;

/*YUV420Pͼ�������ڲ�ʹ�õĵ�Ԫ�ṹ*/
typedef struct VSImage
{
    Uint8 *pixels;
    Uint32 width;
    Uint32 height;
    /*����*/
    Uint32 stride;
} KOSA_VSImage;

/*�ⲿ����YUV420P���ź�����Ҫ�Ľṹ*/
typedef struct YUVScaleObj
{
    Uint8 *yuvbuf;
    Uint32 width;
    Uint32 height;
} KOSA_YUVScale;

/*ˮӡ�㷨����ֵ*/
typedef enum
{
    SUCCEED = 0,
    /*�����������Ҳ�����׼JPEG��������ֹλ��FF D8����"FF D9"*/
    ERR_INPUT_STREAM,
    /*���������в���У��ֵ*/
    ERR_NOT_FIND_CHECK_FLAG,
    /*���ն˼���ó���У��ֵ�ͷ��Ͷ˵Ĳ�һ��*/
    ERR_CHECK_VALUE_UNEQUAL
}EReturnType;

/*MD5������*/
typedef struct
{
    Uint32 State[4];
    Uint32 Count[2];
    Uint8  Buffer[64];
}TMD5Context;


/*����һ�������*/
typedef struct
{
    Uint32 dwX;
    Uint32 dwY;
}KOSA_Point;

/*����һ�������*/
typedef struct
{
    Uint32 dwConvexNum;
    KOSA_Point atPoint[16];
}KOSA_Polygon;


/* =============================================================================
 * APIs
 * =============================================================================
 */

/*
 �� �� ��: KOSA_GetBmpWidthAndHeight
 ��    ��: ��ȡBmpͼ��Ŀ���
 ��    ��: pFilePath    ����Bmp�ļ�·��
           pWidth       ���Bmp�ļ���ָ��
           pHeight      ���Bmp�ļ���ָ��
 �� �� ֵ: �ɹ�����0��ʧ�ܷ���-1
 */
Int32 KOSA_GetBmpWidthAndHeight(Char *pFilePath, Uint32 *pWidth, Uint32 *pHeight);

/*
 �� �� ��: KOSA_BmpToYUV420SP
 ��    ��: Bmp��ʽתYUV420SP(ע��:����ȡY����)
 ��    ��: pFilePath    ����Bmp�ļ�·��
           pBufYUV      ���YUV��ʽ��"Y"����ָ��
           pWidth       ���Bmp����YUV��ʽ��ָ��
           pHeight      ���Bmp����YUV��ʽ��ָ��
 �� �� ֵ: �ɹ�����0��ʧ�ܷ���-1
 */
Int32 KOSA_BmpToYUV420SP(Char *pFilePath, Uint8 *pBufYUV, Uint32 *pWidth, Uint32 *pHeight);

/*
 �� �� ��: KOSA_BmpToYUV420_Color
 ��    ��: Bmp��ʽתYUV420SP����YUV420P(��֧��24λRGBͼ��)
 ��    ��: pFilePath    ����Bmp�ļ�·��
           pBufY        ���YUVͼ����Y����ָ��
           pBufUV       ���YUVͼ����UV����ָ��
           IS_SP        ����1ΪYUV420SP,0ΪYUV420P
 �� �� ֵ: �ɹ�����0,
 */
Int32 KOSA_BmpToYUV420_Color(Char *pFilePath, Uint8 *pBufY, Uint8 *pBufUV,Int32 IS_SP);

/*
 �� �� ��: KOSA_Scale_YUV422_Bilinear
 ��    ��: YUV422��ʽͼ�������
 ��    ��: pdest         ���Ŀ��YUV�ṹָ��(���ݣ�����)
           psrc          ����ԴYUV�ṹָ��(���ݣ�����)
 �� �� ֵ: ��
 */
Void KOSA_Scale_YUV422_Bilinear(KOSA_YUVScale *pDest, KOSA_YUVScale *pSrc);

/*
 �� �� ��: KOSA_Scale_YUV422_Magnify
 ��    ��: YUV422��ʽͼ��ķŴ�
 ��    ��: pdest         ���Ŀ��YUV�ṹָ��(���ݣ�����)
           psrc          ����ԴYUV�ṹָ��(���ݣ�����)
           mulriple      �Ŵ���
 �� �� ֵ: ��
 */
Void KOSA_Scale_YUV422_Magnify(KOSA_YUVScale *pDest, KOSA_YUVScale *pSrc, Uint32 mulriple);


/*
 �� �� ��: KOSA_Scale_YUV420P
 ��    ��: YUV420P��ʽͼ�������
 ��    ��: pdest         ���Ŀ��YUV�ṹָ��(���ݣ�����)
           psrc          ����ԴYUV�ṹָ��(���ݣ�����)
           scale_type   ���������㷨����:SCALE_TYEP_NEAREST��SCALE_TYPE_BILINEAR
 �� �� ֵ: ��
 */
Void KOSA_Scale_YUV420P(KOSA_YUVScale *pDest, KOSA_YUVScale *pSrc, KOSA_SCALE_TYPE Scale_Type);

/*
�� �� ��:  KOSA_JpegCheckFlagCreat
��    ��:  ����У��ֵ������д��Jpeg����
�㷨ʵ��:  ��
ȫ�ֱ���:  ��
��    ��:  pInput       ��������ָ��[in]
           InputLen    ���������ĳ���[in]
           pOutput      ���Md5У����ָ��[out]

�� �� ֵ:  ��������������JPEG��ʽ�����سɹ������򣬷���ʧ��
 */
Int32 KOSA_JpegMD5FlagCreat(Uint8 *pInput, Uint32 InputLen, Uint8 *pOutput);

/*
 �� �� ��: KOSA_GetBoundRectangle
 ��    ��: ��ȡ����ε���Ӿ���
 ��    ��: pRectangle    ���ص���Ӿ���
           pPolygon      ����Ķ����

 �� �� ֵ: ������Ķ���α���Ŀ����3�����سɹ������򣬷���ʧ��
 */
Int32 KOSA_GetBoundRectangle(KOSA_Polygon *pRectangle, KOSA_Polygon *pPolygon);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __KOSA_IMAGE_H__ */
