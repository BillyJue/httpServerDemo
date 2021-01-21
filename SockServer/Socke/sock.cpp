
#include "sock.h"
#include "kosa_debug.h"
#define JS_START_ATTEST			1
#define JS_STOP_ATTEST			2
#define JS_UPLOAD_VEHDATA		3
#define JS_HEART				4

#define LISTEN_PORT  19001
#pragma pack(push, 1)
typedef char Char;
typedef unsigned char Uint8;

#pragma pack(pop)

static void GetInfo(char *szbuf, int &nMethod, char *url)
{
	char *p = strtok(szbuf, " ");
	int nCount = 0;


	while (p)
	{
		if (nCount == 0)
		{
			if (strcmp(p, "POST") == 0)
			{
				nMethod = 1;
			}
		}
		else if (nCount == 1)
		{
			sprintf(url, "%s", p);
		}
		p = strtok(NULL, " ");
		nCount++;
	}
}

static Void* AcceptEntryFun(Void* prm)
{
	SockPlateTest *pObj = (SockPlateTest*)prm;
	pObj->AcceptEntryProc();
	return NULL;
}

static Void* RecvFun(Void* prm)
{
	KOSA_printf("%s:%d\n", __FUNCTION__, __LINE__);
	SockPlateTest *pObj = (SockPlateTest*)prm;
	pObj->RecvProc();
	return NULL;
}


SockPlateTest::SockPlateTest()
{	
	gbInit = FALSE;
	gtSockFd.Hndl = INVALID_SOCKET;
	m_semHndl = NULL;
	m_pBody = NULL;
	m_dwMaxLen = 0;
	m_dwBodyLen = 0;
	m_dwMsgType = 0;
}

SockPlateTest::~SockPlateTest()
{
	Release();
}

Int32 SockPlateTest::Init()
{
	if (gbInit)
	{
		return KOSA_SOK;
	}

	Int32 nRet;

	gbInit = TRUE;

	m_pBody = (Char *)malloc(1024*1024*50);
	pHttpBody = NULL;
	m_dwMaxLen = 1024 * 1024 * 5;

	gtSockFd.Hndl = INVALID_SOCKET;
	KOSA_socketInit();

	KOSA_TskObj task;
	task.entryName = NULL;
	task.policy = KOSA_TSK_SCHED_POLICY_NOMAL;
	task.pri = KOSA_TSK_NORMAL_PRI_MIN;
	task.stackSize = KOSA_TSK_STACK_SIZE_DEFAULT;
	task.prm = this;
	task.entryFunc = AcceptEntryFun;
	KOSA_tskCreate(&task);

	return 0;
}

Int32 SockPlateTest::Release()
{
	if (gbInit == FALSE)
	{
		return KOSA_SOK;
	}
	gbInit = FALSE;

	if (gtSockFd.Hndl != INVALID_SOCKET)
	{
		KOSA_socketClose(&gtSockFd);
		gtSockFd.Hndl = INVALID_SOCKET;
	}

	KOSA_waitMsecs(1000);
	free(m_pBody);
	m_pBody = NULL;
}








Int32 SockPlateTest::AcceptEntryProc()
{
	Int32 nRet;
	KOSA_Socket m_hSockFd;
	
	nRet = KOSA_socketOpen(&m_hSockFd, AF_INET, SOCK_STREAM, 0);
	if (KOSA_SFAIL == nRet)
	{
		KOSA_printf("%s:%d KOSA_socketOpen fail\n", __FUNCTION__, __LINE__);
		return KOSA_SFAIL;
	}

	struct sockaddr_in tAddr;
	memset(&tAddr, 0, sizeof(struct sockaddr_in));
	tAddr.sin_family = AF_INET;
	tAddr.sin_addr.s_addr = 0;
	tAddr.sin_port = htons(LISTEN_PORT);

	struct linger tLinger;
	tLinger.l_onoff = 1;
	tLinger.l_linger = 0;
	KOSA_socketSetOpt(&m_hSockFd, SOL_SOCKET, SO_LINGER, (void *)&tLinger, sizeof(tLinger));

	nRet = KOSA_socketBind(&m_hSockFd, (SOCKADDR_IN *)&tAddr, sizeof(struct sockaddr));
	if (nRet == KOSA_SFAIL)
	{
		KOSA_printf("%s:%d KOSA_socketBind %d fail\n", __FUNCTION__, __LINE__, LISTEN_PORT);
		KOSA_socketClose(&m_hSockFd);
		m_hSockFd.Hndl = INVALID_SOCKET;
		return KOSA_SFAIL;
	}

	KOSA_socketListen(&m_hSockFd, 4);

	while (gbInit)
	{
		Int32 nRet;
		fd_set fdReadSet;
		struct timeval tm;

		tm.tv_sec = 4;
		tm.tv_usec = 0;
		FD_ZERO(&fdReadSet);
		FD_SET(m_hSockFd.Hndl, &fdReadSet);

		nRet = select(FD_SETSIZE, &fdReadSet, NULL, NULL, &tm);
		if (nRet == 0)
		{
			continue;
		}

		if (nRet == KOSA_SFAIL)
		{
			KOSA_printf("%s:%d select fail\n", __FUNCTION__, __LINE__, LISTEN_PORT);
			KOSA_socketClose(&m_hSockFd);
			m_hSockFd.Hndl = INVALID_SOCKET;
			return KOSA_SFAIL;
		}

		if (m_hSockFd.Hndl == INVALID_SOCKET ||
			!FD_ISSET(m_hSockFd.Hndl, &fdReadSet))
		{
			continue;
		}

		Uint32 dwPeerIp;
		Uint16 wPeerPort;
		KOSA_Socket hsockNewClientFd;

		struct sockaddr_in tAddrClient;
		Int32 iAddrLen = sizeof(struct sockaddr_in);

		KOSA_socketAccept(&m_hSockFd, &hsockNewClientFd, (SOCKADDR_IN *)&tAddrClient, &iAddrLen);
		if (hsockNewClientFd.Hndl == INVALID_SOCKET)
		{
			continue;
		}
		
		dwPeerIp = tAddrClient.sin_addr.s_addr;
		wPeerPort = ntohs(tAddrClient.sin_port);
		Char szIp[32];
		KOSA_ip2string(dwPeerIp, szIp);
		KOSA_printf("accept info: %d:%s:%d\n", hsockNewClientFd.Hndl, szIp, wPeerPort);
		//test---------------
		


        if(gtSockFd.Hndl == INVALID_SOCKET)
        {
		    gtSockFd.Hndl = hsockNewClientFd.Hndl;

			KOSA_TskObj task;
			task.entryName = NULL;
			task.policy = KOSA_TSK_SCHED_POLICY_NOMAL;
			task.pri = KOSA_TSK_NORMAL_PRI_MIN;
			task.stackSize = KOSA_TSK_STACK_SIZE_DEFAULT;
			task.prm = this;
			task.entryFunc = RecvFun;
			KOSA_tskCreate(&task);
        }
        else
        {
            KOSA_socketClose(&hsockNewClientFd);
            KOSA_printf("already have a link\n");
        }
	}

	KOSA_socketClose(&m_hSockFd);
	m_hSockFd.Hndl = INVALID_SOCKET;

	KOSA_printf("%s:%d exit!\n", __FUNCTION__, __LINE__);

	return KOSA_SOK;
}

Char* SockPlateTest::NeedRealloc(Uint32 dwLenth)
{
	void *temp = realloc(m_pBody, dwLenth);
    if(temp == NULL)
    {
        return NULL;
    }

    m_pBody = (Char *)temp;
	
	return &m_pBody[m_dwBodyLen];
}


Bool SockPlateTest::CheckMaxSize(Uint32 dwLenth)
{
	if ((m_dwBodyLen + dwLenth) >= m_dwMaxLen)
	{
		Uint32 maxLen = m_dwBodyLen + dwLenth + 1;

		if (NeedRealloc(maxLen) == NULL)
		{
			return false;
		}
		else
		{
		    m_dwMaxLen = maxLen;
			return true;
		}
	}

	return true;
}

Void SockPlateTest::ClearBuf()
{
	m_dwBodyLen = 0;
	m_dwMsgType = 0;
	memset(m_pBody, 0, 1024*1024*50);
}


void SockPlateTest::UnpackMsg()
{
	GetUrlInfo();
	Char tParam[1024];
	Char *pStr = tParam;
	sprintf(pStr, "HTTP/1.1 200 OK\r\n");
	pStr += strlen(pStr);
	sprintf(pStr, "Content-Type: text/json\r\n");
	pStr += strlen(pStr);
	if (strcmp("/hi", m_szHttpUrl) == 0)
	{
		Char szBody[64] = R"({ "a":"10" })";
		sprintf(pStr, "Content-Length: %d\r\n\r\n", strlen(szBody));
		pStr += strlen(pStr);
		sprintf(pStr, "%s", szBody);
		pStr += strlen(pStr);
		KOSA_socketSend(&gtSockFd, (char*)&tParam, strlen(tParam), 0);
		printf("-------/hi----------\n");
		
		KOSA_socketClose(&gtSockFd);
		gtSockFd.Hndl == INVALID_SOCKET;
		KOSA_waitMsecs(2000);
	}
	else
	{
		Char szBody[64] = R"({ "key":"value" })";
		sprintf(pStr, "Content-Length: %d\r\n\r\n", strlen(szBody));
		pStr += strlen(pStr);
		sprintf(pStr, "%s", szBody);
		pStr += strlen(pStr);
		KOSA_socketSend(&gtSockFd, (char*)&tParam, strlen(tParam), 0);
		printf("-------other----------\n");
		
		KOSA_socketClose(&gtSockFd);
		gtSockFd.Hndl = INVALID_SOCKET;
		KOSA_waitMsecs(2000);
	}
	printf("%s\n", pHttpBody);
}

void SockPlateTest::GetUrlInfo()
{
	/*解析method和url*/
	char *pInfo = strstr(m_pBody, "\r\n");
	if (NULL == pInfo)
	{
		return;
	}
	char szInfo[128] = { 0 };
	int len = 0;
	len = pInfo - m_pBody;
	memcpy(szInfo, m_pBody, len);
	GetInfo(szInfo, m_dwMethod, m_szHttpUrl);

	pInfo = strstr(m_pBody, "\r\n\r\n");
	m_dwHttpHeaderLen = (pInfo - m_pBody + 4);

	pHttpBody = m_pBody + m_dwHttpHeaderLen;
}

Int32 SockPlateTest::GetReadLenth()
{
	int len = 0;
	if (m_dwBodyLen == 0)
	{
		return 2048;
	}
	else if (m_pBody && m_dwHttpHeaderLen != 0 && m_dwHttpHeaderLen + strlen(pHttpBody) == m_dwBodyLen)
	{
		return 0;
	}
	else if (m_dwBodyLen >= 2048)
	{
		//GetUrlInfo();
		Char szInfo[128];
		/*解析消息头长度*/
		Char *pInfo = strstr(m_pBody, "\r\n\r\n");
		if (NULL == pInfo)
		{
			return 0;
		}
		memset(szInfo, 0, 128);
		m_dwHttpHeaderLen = pInfo - m_pBody+4;
		pHttpBody = m_pBody + m_dwHttpHeaderLen;

		char *pTmp = strstr(m_pBody, "Content-Length: ");
		if (NULL == pTmp)
		{
			return 0;
		}
		pTmp += strlen("Content-Length:");
		char *pTmp2 = strstr(pTmp, "\r\n");
		if (NULL == pTmp2)
		{
			return 0;
		}
		len = pTmp2 - pTmp;
		char szLen[10] = { 0 };
		memcpy(szLen, pTmp, len);
		int nMsgLen = atoi(szLen);
		if (nMsgLen + m_dwHttpHeaderLen > 2048)
		{
			len = nMsgLen + m_dwHttpHeaderLen - 2048;
		}

	}
	return len;
}

Int32 SockPlateTest::RecvProc()
{
	while (1)
	{
		Int32 nRet;
		fd_set fdReadSet;
		struct timeval tm;
		tm.tv_sec = 1;
		tm.tv_usec = 0;
		FD_ZERO(&fdReadSet);
		FD_SET(gtSockFd.Hndl, &fdReadSet);
		nRet = select(FD_SETSIZE, &fdReadSet, NULL, NULL, &tm);
		if (nRet == 0)
		{
			KOSA_socketClose(&gtSockFd);
			gtSockFd.Hndl = INVALID_SOCKET;
			KOSA_printf("%s:%d nRet = 0\n", __FUNCTION__, __LINE__);
			continue;
		}
		else if (nRet == -1)
		{
			KOSA_socketClose(&gtSockFd);
			gtSockFd.Hndl = INVALID_SOCKET;
			KOSA_printf("%s:%d nRet = -1\n", __FUNCTION__, __LINE__);
			return KOSA_SFAIL;
		}

		ClearBuf();
		Char *szReadBuf = m_pBody;
		Uint32 dwRemainLen = 0;
		dwRemainLen = GetReadLenth();
		while (dwRemainLen != 0)
		{
			//KOSA_printf("%s:%d dwRemainLen = %d\n", __FUNCTION__, __LINE__, dwRemainLen);
			nRet = KOSA_socketRecv(&gtSockFd, (Char *)szReadBuf, dwRemainLen, 0);
			//KOSA_printf("%s:%d nRet = %d\n", __FUNCTION__, __LINE__, nRet);
			if (nRet == -1 )
			{
				KOSA_socketClose(&gtSockFd);
				gtSockFd.Hndl = INVALID_SOCKET;
				KOSA_printf("%s:%d nRet = %d\n", __FUNCTION__, __LINE__, nRet);
				return KOSA_SFAIL;
			}
			if (nRet == 0)
			{
				KOSA_socketClose(&gtSockFd);
				gtSockFd.Hndl = INVALID_SOCKET;
				KOSA_printf("%s:%d nRet = %d\n", __FUNCTION__, __LINE__, nRet);
				
				break;
			}

			dwRemainLen -= nRet;
			szReadBuf += nRet;
			m_dwBodyLen += nRet;

			dwRemainLen = GetReadLenth();

			if (dwRemainLen == 0)
			{
				UnpackMsg();
			}
		}
	}

	KOSA_printf("%s:%d exit!\n", __FUNCTION__, __LINE__);

	return KOSA_SOK;
}


Int32 SockPlateTest::Start()
{
    KOSA_printf("%s:%d \n", __FUNCTION__, __LINE__);
	Init();
	return KOSA_SOK;
}

Int32 SockPlateTest::Stop()
{
    KOSA_printf("%s:%d \n", __FUNCTION__, __LINE__);
	return KOSA_SOK;
}


