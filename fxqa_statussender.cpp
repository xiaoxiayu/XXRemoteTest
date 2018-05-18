
#include "fxqa_statussender.h"
#include <sstream>
#include <algorithm>
namespace fxqa_status {

	void Sleep(long time_ms) {
#if defined(_MSC_VER)
		::Sleep(time_ms);
#elif defined(__GNUC__)
		sleep(time_ms / 1000);
#else
		sleep(time_ms / 1000);
#endif
	}

	int getHeaderLength(char *content) {
		const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
		char *findPos;
		int ofset = -1;

		if (!content) return -1;

		findPos = strstr(content, srchStr1);
		if (findPos != NULL)
		{
			ofset = findPos - content;
			ofset += strlen(srchStr1);
		}

		else
		{
			findPos = strstr(content, srchStr2);
			if (findPos != NULL)
			{
				ofset = findPos - content;
				ofset += strlen(srchStr2);
			}
		}
		return ofset;
	}

	void GetHostName(char* hostName) {
		char Name[150];

#ifdef _MSC_VER
		TCHAR infoBuf[150];
		DWORD bufCharCount = 150;
		memset(Name, 0, 150);
		if( GetComputerName( infoBuf, &bufCharCount ) ) {
			for(int i=0; i < 150; i++) {
				Name[i] = infoBuf[i];
			}
		} else {
			strcpy(Name, "Unknown_Host_Name");
		}
#else
		memset(Name, 0, 150);
		gethostname(Name, 150);
#endif
		strncpy(hostName, Name, 150);
	}

	CLock g_lock;
	CLock g_lock2;
#if defined (_MSC_VER)
	void CreateSenderSent(void* param) {
#else
	void* CreateSenderSent(void* param) {
#endif
		SENDER_INFO* sender_info = (SENDER_INFO*)param;

		int pid = getpid();
		while (!sender_info->exitFlag) {
			CQALocker locker(&g_lock2);
			if (sender_info->stopInfoFlag) {
				Sleep(1000);
				continue;
			}

			std::stringstream ss1;
			ss1 << "action=heart&production=RDKUT&pid=" << getpid() ;

			std::string msg_rev = "no";
			int ret = fxqa_status::CHttp::POST("127.0.0.1",
				sender_info->port,
				"/heart", ss1.str(), msg_rev);
			//CStatusSender* pParent = (CStatusSender*)sender_info->parent;
			//int ret = pParent->POST(sender_info->flag, sender_info->infoStr, recv_msg);

			if (ret == -1) {
				printf("send error: %s(errno: %d)",strerror(errno),errno);
			}
			Sleep(1000);
		}
		delete sender_info;
	}

	bool CHttp::bCloseLink = false;
	CHttp::CHttp() {

	}

	CHttp::~CHttp() {

	};

	void CHttp::CloseLink() {
		CHttp::bCloseLink = true;
	}


	int CHttp::POST(std::string host, int port, std::string request, std::string msg, std::string& recv_msg) {
		if (CHttp::bCloseLink) return 0;

		char url[33];
		memset(url, 0, 33);

		sprintf(url, "%s:%d", host.c_str(), port);

		int conn = 0;
#ifdef _MSC_VER
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (::WSAStartup(socketVersion, &wsaData) != 0) {
			return false;
		}
#endif
		if( (conn = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
			return false;
		}

#ifdef _MSC_VER
		DWORD timeout = 5 * 1000;
		setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#if defined(__linux__)
#else
		int set = 1;
		setsockopt(conn, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif // __linux__
#endif

		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(port);
		servAddr.sin_addr.s_addr = inet_addr(host.c_str());
		int ret = connect(conn, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if(ret < 0){
			printf("%s, Socket connect error: %s(errno: %d),status monitor is not running.\n", url, strerror(errno), errno);
			return false;
		}


		std::stringstream ss;
		ss << "POST " << request << " HTTP/1.1\r\n" << \
			"HOST: "<< url <<"\r\n" << \
			"Connection: close\r\n" << \
			"Content-Type: application/x-www-form-urlencoded\r\n" << \
			"Content-Length:" << msg.length() << "\r\n\r\n" << msg;

		std::string strHttpPost = ss.str();
		int iResult = 0;
		iResult = ::send(conn, strHttpPost.c_str(), strHttpPost.length(), MSG_NOSIGNAL);
		if (iResult <= 0) {
			return -1;
		}

		if (recv_msg.length() == 0) {
			RecvData(conn, recv_msg);
		}

#ifdef _MSC_VER
		shutdown(conn, 2);
		closesocket(conn);
		::WSACleanup();
#else
		shutdown(conn, 2);
		close(conn);
#endif

		return 0;

	}

	int CHttp::GET(std::string host, int port, std::string request, std::string& recv_msg) {
		if (CHttp::bCloseLink) return 0;

		char url[33];
		memset(url, 0, 33);

		sprintf(url, "%s:%d", host.c_str(), port);

		int conn = 0;
#ifdef _MSC_VER
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (::WSAStartup(socketVersion, &wsaData) != 0) {
			return false;
		}
#endif
		if( (conn = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
			return false;
		}

#ifdef _MSC_VER
		DWORD timeout = 5 * 1000;
		setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		setsockopt(conn, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#if defined(__linux__)
#else
		int set = 1;
		setsockopt(conn, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
#endif

		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(port);
		servAddr.sin_addr.s_addr = inet_addr(host.c_str());
		int ret = connect(conn, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if(ret < 0){
			printf("%s, Socket connect error: %s(errno: %d),status monitor is not running.\n", url, strerror(errno), errno);
			return false;
		}

		std::stringstream ss;
		ss << "GET " << request  \
			<< " HTTP/1.1\r\n" \
			<< "Host:"<< url <<"\r\n" \
			<< "Connection: close\r\n\r\n";

		std::string requestStr = ss.str();
		int iResult = 0;
		iResult = ::send(conn, requestStr.c_str(), requestStr.length(), MSG_NOSIGNAL);
		if (iResult <= 0) {
			printf("%s, Socket connect error: %s(errno: %d).\n", url, strerror(errno),errno);
			return -1;
		}

		RecvData(conn, recv_msg);

#ifdef _MSC_VER
		shutdown(conn, 2);
		closesocket(conn);
		::WSACleanup();
#else
		shutdown(conn, 2);
		close(conn);
#endif

		return 0;
	}


	int CHttp::RecvData(int conn, std::string &data) {
		char recvbuf[1024];
		memset(recvbuf, 0, 1024);

		int recvbuflen = 1024;

		const int bufSize = 512;
		char readBuffer[bufSize];
		char *tmpResult=NULL, *result;
		long totalBytesRead, thisReadSize, headerLen;
		totalBytesRead = 0;
		while(1) {
			memset(readBuffer, 0, bufSize);
			thisReadSize = recv (conn, readBuffer, bufSize, 0);

			if ( thisReadSize <= 0 )
				break;

			tmpResult = (char*)realloc(tmpResult, thisReadSize+totalBytesRead);

			memcpy(tmpResult+totalBytesRead, readBuffer, thisReadSize);
			totalBytesRead += thisReadSize;
		}
		headerLen = getHeaderLength(tmpResult);
		long contenLen = totalBytesRead-headerLen;
		result = new char[contenLen+1];
		memcpy(result, tmpResult+headerLen, contenLen);
		result[contenLen] = 0x0;
		char *myTmp;

		myTmp = new char[headerLen+1];
		strncpy(myTmp, tmpResult, headerLen);
		myTmp[headerLen] = 0;//NULL;
		free(tmpResult);
		delete[] myTmp;

		data = result;
		delete[] result;
		return 1;
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	CStatusSender::CStatusSender(SOCKET_INFO socket_info) :
	m_socketInfo(),
		m_senderInfo(NULL),
		m_bConnected(false),
		m_sTestcaseName(""),
		m_sTestName("") {
			m_socketInfo = socket_info;
			char host[33];
			memset(host, 0, 33);
			sprintf(host, "%s:%d", socket_info.ip.c_str(), socket_info.port);
			m_host = host;
	}

	CStatusSender::~CStatusSender() {
		CloseSocket();
		if (m_senderInfo != NULL) {
			m_senderInfo = NULL;
		}
	}

	bool CStatusSender::CreateSenderThread() {
		if(InitSocket() == false) return false;
		if(ConnectToServer() == false) return false;

#if defined(_MSC_VER)
		InitializeCriticalSection(&m_lock);
#elif defined(__GNUC__)
		pthread_mutexattr_t tma;
		pthread_mutexattr_init(&tma);
		pthread_mutexattr_settype(&tma, PTHREAD_MUTEX_RECURSIVE);
		int nErr = pthread_mutex_init(&m_lock, &tma);
		pthread_mutexattr_destroy(&tma);
#endif
		SENDER_INFO* sender_info = new SENDER_INFO;

		sender_info->infoStr = "testcase=" + this->m_sTestcaseName;
		sender_info->infoStr += "&test=" + this->m_sTestName;
		sender_info->port = m_socketInfo.port;
		sender_info->stopInfoFlag = true;

		sender_info->exitFlag = false;
		sender_info->parent = this;
		m_senderInfo = sender_info;
		m_socketInfo.sender_thread = NULL;


#if defined(_MSC_VER)
		m_socketInfo.sender_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CreateSenderSent, m_senderInfo, 0, NULL);
		if(NULL == m_socketInfo.sender_thread) return false;
#elif defined(__GNUC__)
		pthread_t thread = 0;
		nErr = pthread_create(&thread , NULL, &CreateSenderSent, sender_info);
		if (nErr != 0) return false;
#endif
		return true;
	}

	bool  CStatusSender::IsRunning() {
#if defined(_FXQA_MICROSERVICE_)
		if (!m_bConnected)
			return false;
		return true;
#endif
		if (m_senderInfo == NULL)
			return false;
		return true;
	}

	void CStatusSender::StopSenderThread() {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->exitFlag = true;
#if defined(_MSC_VER)
		DeleteCriticalSection(&m_lock);
#elif defined(__GNUC__)
		pthread_mutex_destroy(&m_lock);
#endif
	}

	void CStatusSender::SetSenderInfo(const char* flag, const char* str) {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->flag = flag;
		m_senderInfo->infoStr = str;
	}

	void CStatusSender::StartSendInfo() {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->stopInfoFlag = false;
	}

	void CStatusSender::StopSendInfo() {
		CQALocker locker(&g_lock);
		if (m_senderInfo == NULL) return;
		m_senderInfo->stopInfoFlag = true;
	}

	bool CStatusSender::InitSocket() {
#ifdef _MSC_VER
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (::WSAStartup(socketVersion, &wsaData) != 0) {
			return false;
		}
		return true;
#else
		return true;
#endif
	}

	bool CStatusSender::ConnectToServer() {
		if( (m_socketInfo.socket = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
			return false;
		}
		/*
		#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
		u_long iMode = 1;
		ioctlsocket(m_socketInfo.socket, FIONBIO, &iMode);
		#else
		fcntl(m_socketInfo.socket, F_SETFL, O_NONBLOCK);
		#endif
		*/
#ifdef _MSC_VER
		DWORD timeout = 5 * 1000;
		setsockopt(m_socketInfo.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		int set = 1;
		setsockopt(m_socketInfo.socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#if defined(__linux__)
#else
		setsockopt(m_socketInfo.socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
#endif

		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(m_socketInfo.port);
		servAddr.sin_addr.s_addr = inet_addr(m_socketInfo.ip.c_str());
		int ret = connect(m_socketInfo.socket, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if(ret < 0){
			printf("%s:%d, Socket connect error: %s(errno: %d),status monitor is not running.\n",m_socketInfo.ip.c_str(), m_socketInfo.port, strerror(errno),errno);
			return false;
		}
		printf("connect========\n");
		m_bConnected = true;
		return true;
	}

	int CStatusSender::SendMsgToServer(const char* msg) {
		return ::send(m_socketInfo.socket, msg, strlen(msg), MSG_NOSIGNAL);
	}

	int CStatusSender::SendMsgToServer() {
		return ::send(m_socketInfo.socket, "Running...", strlen("Running..."), MSG_NOSIGNAL);
	}

	void CStatusSender::SetTestInfo(std::string testcase_name, std::string test_name) {
		m_sTestcaseName = testcase_name;
		m_sTestName = test_name;
	}

	int CStatusSender::SendStartCrash(std::string testCaseName, std::string testName,
		std::string sVersion, std::string testfile, int iHeartTime) {
			this->SetTestInfo(testCaseName, testName);

			std::stringstream ss;
			ss << "production="<< m_socketInfo.production 
				<< "&action=start&pid=" << getpid()
				<< "&testcase=" << testCaseName
				<< "&test=" << testName
				<< "&testfile=" << testfile
				<< "&timeout=" << iHeartTime
				<< "&version=" << sVersion;

			std::string msg_rev = "no";
			int ret = fxqa_status::CHttp::POST("127.0.0.1",
				m_socketInfo.port,
				"/heart", ss.str(), msg_rev);
#if 0
			std::string post_msg = ss.str();
			std::string rec_msg = "";
			int ret = this->POST("/heart", post_msg, rec_msg);

			std::stringstream ss1;
			ss1 << "production="<< production<<"&action=heart&pid=" << getpid()
				<< "&testcase=" << testCaseName
				<< "&test=" << testName;
			post_msg = ss1.str();
			this->SetSenderInfo("/heart", post_msg.c_str());
#endif // 0

			this->StartSendInfo();
			return ret;
	}

	int CStatusSender::SendStartDeath(std::string testCaseName, std::string testName,
		std::string sVersion, std::string testfile, int iTimeout) {
			this->SetTestInfo(testCaseName, testName);

			std::stringstream ss;
			ss << "production=" << m_socketInfo.production 
				<< "&action=start&pid=" << getpid()
				<< "&testcase=" << testCaseName
				<< "&test=" << testName
				<< "&testfile=" << testfile
				<< "&timeout=" << iTimeout
				<< "&version=" << sVersion;

			std::string post_msg = ss.str();
			std::string rec_msg = "";
			int ret = this->POST("/health", post_msg, rec_msg);

			return ret;
	}

	int CStatusSender::SendStopCrash() {
		std::stringstream ss;
		ss << "production=fxcore&action=stop&pid=" << getpid();

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		int ret = this->POST("/heart", post_msg, rec_msg);
		//this->SetSenderInfo("http://localhost:10018/heart", post_msg);
		this->StopSendInfo();
		return ret;
	}

	int CStatusSender::SendStopDeath() {
		std::stringstream ss;
		ss << "production=fxcore&action=stop&pid=" << getpid();

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		int ret = this->POST("/health", post_msg, rec_msg);
		this->StopSendInfo();
		return ret;
	}

	int CStatusSender::SendNoRun(std::string testCaseName, std::string testName, std::string testfile) {
		std::stringstream ss;
		ss << "production=fxcore&pid=" << getpid()
			<< "&testcase=" << testCaseName
			<< "&test=" << testName
			<< "&testfile=" << testfile;

		std::string post_msg = ss.str();
		std::string rec_msg = "";
		return this->POST("/norun", post_msg, rec_msg);
	}

	int CStatusSender::POST(std::string request, std::string msg, std::string& recv_msg) {
		std::stringstream ss;
		ss << "POST " << request << " HTTP/1.1\r\n" << \
			"HOST: "<< this->m_host <<"\r\n" << \
			"Connection: keep-alive\r\n" << \
			"Content-Type: application/x-www-form-urlencoded\r\n" << \
			"Content-Length:" << msg.length() << "\r\n\r\n" << msg;

		std::string strHttpPost = ss.str();
		int iResult = 0;
		iResult = ::send(m_socketInfo.socket, strHttpPost.c_str(), strHttpPost.length(), MSG_NOSIGNAL);
		if (iResult <= 0) {
			return -1;
		}

		this->RecvData(recv_msg);
		return 0;
	}



	int CStatusSender::RecvData(std::string& data) {
		return 0;
		char recvbuf[1024];
		memset(recvbuf, 0, 1024);

		int recvbuflen = 1024;

		const int bufSize = 2048;
		char readBuffer[bufSize];
		char *tmpResult=NULL, *result;
		long totalBytesRead, thisReadSize, headerLen;
		totalBytesRead = 0;
		while(1) {
			memset(readBuffer, 0, bufSize);
			thisReadSize = recv (m_socketInfo.socket, readBuffer, bufSize, 0);

			if ( thisReadSize <= 0 )
				break;

			tmpResult = (char*)realloc(tmpResult, thisReadSize+totalBytesRead);

			memcpy(tmpResult+totalBytesRead, readBuffer, thisReadSize);
			totalBytesRead += thisReadSize;
		}
		headerLen = getHeaderLength(tmpResult);
		if (headerLen == -1) return -1;

		long contenLen = totalBytesRead-headerLen;
		result = new char[contenLen+1];
		memcpy(result, tmpResult+headerLen, contenLen);
		result[contenLen] = 0x0;
		char *myTmp;

		myTmp = new char[headerLen+1];
		strncpy(myTmp, tmpResult, headerLen);
		myTmp[headerLen] = 0;//NULL;
		delete(tmpResult);
		delete[] myTmp;

		data = result;
		return 0;
	}

	int CStatusSender::GET(std::string request, std::string& recv_msg) {
		std::stringstream ss;
		ss << "GET " << request  \
			<< " HTTP/1.1\r\n" \
			<< "Host:"<< m_host <<"\r\n" \
			<< "Connection: keep-alive\r\n\r\n";

		std::string requestStr = ss.str();
		int iResult = 0;
		iResult = ::send(m_socketInfo.socket, requestStr.c_str(), requestStr.length(), MSG_NOSIGNAL);
		if (iResult <= 0) {
			printf("%s:%d, Socket connect error: %s(errno: %d).\n",m_socketInfo.ip.c_str(), m_socketInfo.port, strerror(errno),errno);
			return -1;
		}

		this->RecvData(recv_msg);

		return 0;
	}


	bool CStatusSender::CloseSocket() {
#ifdef _MSC_VER
		shutdown(m_socketInfo.socket, 2);
		closesocket(m_socketInfo.socket);
		::WSACleanup();
#else
		shutdown(m_socketInfo.socket, 2);
		close(m_socketInfo.socket);
#endif
		return true;
	}


	// StatusToucher implement.
	//
	CStatusSender*  CStatusToucher::m_sender = NULL;
	CStatusToucher::CStatusToucher(SOCKET_INFO sockInfo) {
		m_socketInfo = sockInfo;
		m_pid = getpid();
	}

	bool CStatusToucher::m_bInit = false;
	bool CStatusToucher::Init() {
		m_sender = new CStatusSender(m_socketInfo);
		if (m_sender == NULL)
			return false;
		if (!m_sender->InitSocket())
			return false;
		if (!m_sender->ConnectToServer())
			return false;
		m_bInit = true;
		return true;
	}

	SENDER_INFO  CStatusToucher::m_senderInfo;
	void CStatusToucher::SetSenderInfo(const char* flag, const char* str) {
		if (!m_bInit) return;
		m_senderInfo.flag = flag;
		m_senderInfo.infoStr = str;
	}

	void CStatusToucher::SetSender(CStatusSender* sender) {
		m_bInit = true;
		m_sender = sender;
		m_pid = getpid();
	}

	int  CStatusToucher::m_pid = -1;
	void CStatusToucher::Touch() {
		if (!m_bInit) return;
		std::stringstream ss;
#if !defined(_FXQA_MICROSERVICE_)
		ss << "{\"PID\":\"" << m_pid << "\", \"TEST\":\"" << m_senderInfo.flag << "\", \"INFO\":\"" << m_senderInfo.infoStr << "\"}";
		std::string sendStr = ss.str();
		m_sender->SendMsgToServer(sendStr.c_str());
#else
		ss << "action=heart&pid=" << m_pid;
		std::string sendStr = ss.str();
		std::string rec_msg = "";
		m_sender->POST("http://localhost:10018/health", sendStr, rec_msg);
#endif // _FXQA_MICROSERVICE_
	}

	CStatusToucher::~CStatusToucher() {
		if (m_sender != NULL) {
			delete m_sender;
			m_sender = NULL;
		}
	}

	// CLogSender implement.
	//
	CStatusSender*  CLogSender::m_sender = NULL;
	CLogSender::CLogSender(SOCKET_INFO sockInfo) {
		m_socketInfo = sockInfo;
		m_pid = getpid();
	}

	bool CLogSender::m_bInit = false;
	bool CLogSender::Init() {
		m_sender = new CStatusSender(m_socketInfo);
		if (m_sender == NULL)
			return false;
		m_bInit = true;
		return true;
	}

	int  CLogSender::m_pid = -1;
	void CLogSender::SentLog(const char* flag, const char* str) {
		if (!m_bInit) return;

		if (!m_sender->InitSocket())
			return;
		if (!m_sender->ConnectToServer())
			return;

		std::stringstream ss;
		ss << "{\"PID\":\"" << m_pid << "\", \"TEST\":\"" << flag << "\", \"INFO\":\"" << str << "\"}";
		std::string sendStr = ss.str();
		m_sender->SendMsgToServer(sendStr.c_str());
	}

	int CLogSender::Log2ELK(std::string logType, std::string msg) {
		if (msg.length() == 0) {
			return 0;
		}
		std::replace( msg.begin(), msg.end(), '\t', ' ');
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
		std::replace( msg.begin(), msg.end(), '\\', '/');
#endif // _FXQA_PLATFORM_WINDOWS_WIN32_

		std::stringstream ss;
		ss << "n=fxcore&s={" << 
			"\"production\":\"fxcore\"," <<
			"\"project\":\"fx_compare_v2\"," <<
			"\"version\":\"" << m_sVersion << "\"," <<
			"\"server\":\"" << m_sHostName << "\"," <<
			"\"testcase\":\"" << m_sTestCaseName << "\"," <<
			"\"test\":\"" << m_sTestName << "\"," <<
			"\"testfile\":\"" << m_sTestFile << "\"," <<
			"\"type\":\"" << logType << "\"," << 
			"\"msg\":\"" << msg << "\"}"; 
		std::string post_msg = ss.str();

		const char *pHttpPost = "POST %s HTTP/1.1\r\n"  
			"Host: %s:%d\r\n"  
			"Content-Type: application/x-www-form-urlencoded\r\n"  
			"Content-Length: %d\r\n\r\n"  
			"%s";  

		const char* host = "0.0.0.0";  
		int port = 9090;  

		char* strHttpPost = new char[256 + post_msg.length()];
		memset(strHttpPost, 0, sizeof(strHttpPost));
		sprintf(strHttpPost, pHttpPost, m_sELKUrl.c_str(), host, port, post_msg.length(), post_msg.c_str());  

		int iResult = 0;
		iResult = ::send(m_socket, strHttpPost, strlen(strHttpPost), MSG_NOSIGNAL);
		if (iResult <= 0) {
			return -1;
		}

		char recvbuf[1024];
		memset(recvbuf, 0, 1024);

		int recvbuflen = 1024;
		iResult = recv(m_socket, recvbuf, recvbuflen, 0);
		if ( iResult <= 0 ) {
			return -1;
		}

		std::string ssbuf = recvbuf;
		std::string sLen = ssbuf.substr(ssbuf.find("Content-Length:")+15);
		sLen = sLen.substr(0, sLen.find("\n"));
		recvbuflen = atoi(sLen.c_str());

		m_sELKRes = ssbuf.substr(ssbuf.length() - recvbuflen);

		//std::stringstream ss1;
		//ss1 << recvbuf << std::endl;
		//OutputDebugStringA(recv_msg.c_str());

		return 0;
	}

	std::string CLogSender::m_sELKRes = "";
	std::string CLogSender::GetELKReturn() {
		return m_sELKRes;
	}

	int  CLogSender::m_socket = -1;
	std::string CLogSender::m_sELKUrl = "http://";
	std::string CLogSender::m_sHostName = "Unknown";
	bool CLogSender::InitLinkELK(std::string logServerAddr, int logServerPort) {
#ifdef _MSC_VER
		WSADATA wsaData;
		WORD socketVersion = MAKEWORD(2, 2);
		if (::WSAStartup(socketVersion, &wsaData) != 0) {
			return false;
		}
#endif

		std::stringstream ss1;
		ss1 << "http://" << logServerAddr << ":" << logServerPort << "/logs";
		m_sELKUrl = ss1.str();

		if( (m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
			return false;
		}
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(logServerPort);
		servAddr.sin_addr.s_addr = inet_addr(logServerAddr.c_str());
		int ret = connect(m_socket, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if(ret < 0){
			return false;
		}
		char hostname[150];
		memset(hostname, 0, 150);
		GetHostName(hostname);
		m_sHostName = hostname;
		return true;
	}

	void CLogSender::DisconnectELK() {
#ifdef _MSC_VER
		shutdown(m_socket, 2);
		closesocket(m_socket);
#else
		shutdown(m_socket, 2);
		close(m_socket);
#endif
		m_sTestFile = "";
		m_sTestName = "";
		m_sTestCaseName = "";
	}

	std::string CLogSender::m_sTestCaseName = "";
	std::string CLogSender::m_sTestName = "";
	std::string CLogSender::m_sTestFile = "";
	std::string CLogSender::m_sVersion = "";
	void CLogSender::SetTestInfo(std::map<std::string, std::string> versionMap, std::string testCaseName, std::string testName, std::string testFile) {
		m_sTestFile = testFile;
#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
		std::replace( m_sTestFile.begin(), m_sTestFile.end(), '\\', '/');
#endif
		m_sTestName = testName;
		m_sTestCaseName = testCaseName;
		m_sVersion += versionMap["oldsvn"];
		m_sVersion += "*";
		m_sVersion += versionMap["newsvn"];
	}

	CLogSender::~CLogSender() {
		if (m_sender != NULL) {
			delete m_sender;
			m_sender = NULL;
		}
	}

#if defined (_MSC_VER)
	void _healthSent(void* param) {
#else
	void* _healthSent(void* param) {
#endif
		int used_time = *( (int*)param);
		printf("%d", used_time);
		for (int i = 0; i < used_time; i++) {
			std::string msg_rev = "no";
			std::stringstream ssHeart;
			ssHeart << "action=heart&production=RDKUT&pid=" << getpid();
			fxqa_status::CHttp::POST("127.0.0.1",
				10021,
				"/health", ssHeart.str(), msg_rev);
			Sleep(1000);
		}
	}

#if defined (_MSC_VER)
	void _healthSent2(void* param) {
#else
	void* _healthSent2(void* param) {
#endif
		bool* bStart = (bool*)param;
		while(*bStart) {
			std::string msg_rev = "no";
			std::stringstream ssHeart;
			ssHeart << "action=heart&production=RDKUT&pid=" << getpid();
			fxqa_status::CHttp::POST("127.0.0.1",
				10021,
				"/health", ssHeart.str(), msg_rev);
			Sleep(1000);
		}
		delete bStart;
		bStart = NULL;
	}	

	void CLongTime::Regular(int iTime) {
#if defined(_MSC_VER)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_healthSent, (void*)&iTime, 0, NULL);

#elif defined(__GNUC__)
		pthread_t thread = 0;
		pthread_create(&thread , NULL, &_healthSent, (void*)&iTime);
#endif
	};

	void CLongTime::Start() {
		pStartFlag = new bool(true);
#if defined(_MSC_VER)
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_healthSent2, (void*)pStartFlag, 0, NULL);

#elif defined(__GNUC__)
		pthread_t thread = 0;
		pthread_create(&thread , NULL, &_healthSent2, (void*)pStartFlag);
#endif

	};

	void CLongTime::End() {
		*pStartFlag = false;
	}

	bool* CLongTime::pStartFlag = NULL;

}

