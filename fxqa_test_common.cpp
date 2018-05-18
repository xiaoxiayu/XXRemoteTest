#include "../../../third_party/clsocket/PassiveSocket.h" 
#include "fxqa_test_common.h"
#include "http_parser.h"
#include "fxqa_statussender.h"
#include <sstream>
#include "fxqa_charset.h"


#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
#include <direct.h>
#include <io.h>
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#define WMKDIR(a) _wmkdir((a))
#else
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h> 
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif


RegistrationCache& __getRegistMap() {
	static RegistrationCache cache;
	return cache;
}

int __errorTestCall(Json::Value& respond, const Json::Value request) {
	return CRunningError::RunningNotFound;
}



int on_message_begin(http_parser* _) {
	(void)_;
	printf("\n***MESSAGE BEGIN***\n\n");
	return 0;
}

int on_headers_complete(http_parser* _) {
	(void)_;
	printf("\n***HEADERS COMPLETE***\n\n");
	return 0;
}

int on_message_complete(http_parser* parser) {
	CTestRunner* runner = (CTestRunner*)parser->data;
	runner->setRequestFinish();
	return 0;
}

int on_url(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf("Url: %.*s\n", (int)length, at);
	return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf("Header field: %.*s\n", (int)length, at);
	return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf("Header value: %.*s\n", (int)length, at);
	return 0;
}

int on_body(http_parser* parser, const char* at, size_t length) {
	CTestRunner* runner = (CTestRunner*)parser->data;
	//printf("Body: %.*s\n", (int)length, at);
	char* tem = new char[(int)length + 1];
	memset(tem, 0, (int)length + 1);
	memcpy(tem, at, (int)length);
	runner->setTestParam(tem);
	delete[] tem;
	return 0;
}

#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
void* FXQA_ServerRun(void* param) {
    std::string* host_p = (std::string*)param;
    std::string host_url = *host_p;
#else
    void FXQA_ServerRun(std::string host_url) {
#endif //_FXQA_PLATFORM_QTIOS_
	std::string  crash_monitor_ip = "127.0.0.1";
	std::string crash_monitor_port = "10018";
	fxqa_status::SOCKET_INFO sockInfo_crash = {};
	sockInfo_crash.ip = crash_monitor_ip;
	sockInfo_crash.port = atoi(crash_monitor_port.c_str());
	sockInfo_crash.production = "FX_COMPARE";

	CPassiveSocket socket;
	CActiveSocket *pClient = NULL;

	//--------------------------------------------------------------------------
	// Initialize our socket object 
	//--------------------------------------------------------------------------
	
	int split_pos = host_url.find(":");
	std::string host_port = host_url.substr(split_pos + 1);
	std::string host_ip = host_url.substr(0, split_pos);

	socket.Initialize();

	socket.Listen(host_ip.c_str(), atoi(host_port.c_str()));

	http_parser parser;// = new http_parser;
	http_parser_init(&parser, HTTP_REQUEST);

	CTestRunner runner;
	parser.data = &runner;

	http_parser_settings settings; /* set up callbacks */
	memset(&settings, 0, sizeof(settings));
	settings.on_message_begin = on_message_begin;
	settings.on_url = on_url;
	settings.on_header_field = on_header_field;
	settings.on_header_value = on_header_value;
	settings.on_headers_complete = on_headers_complete;
	settings.on_body = on_body;
	settings.on_message_complete = on_message_complete;

	int pid = getpid();
	char tmpRes[MAX_PACKET + 1];
	while (true) {
		std::string str = "";
		if ((pClient = socket.Accept()) != NULL) {

			int bytesReceived = 0;
			int nparsed = -1;
			while (!runner.isRequestFinish()) {
				int numBytes = pClient->Receive(MAX_PACKET);
				memset(tmpRes, 0, MAX_PACKET + 1);
				memcpy(tmpRes, pClient->GetData(), numBytes);
				str += tmpRes;

				bytesReceived += numBytes;
				if (numBytes != MAX_PACKET) {
					nparsed = http_parser_execute(&parser, &settings, str.c_str(), bytesReceived);
					if (!runner.isRequestFinish()) {
						continue;
					}

					Json::Reader	jsonReader;
					Json::Value		jsonRoot;
					jsonReader.parse(runner.getParam(), jsonRoot);

					std::string funcName = jsonRoot["func"].asString();
					std::string testfile = jsonRoot["testfile"].asString();

					std::stringstream ss;
					if (funcName.compare("Exit") == 0) {
                        std::string res = "{\"errcode\":0}";
                        pClient->Send( (uint8 *)res.c_str(), res.length() );
                        pClient->Close();
                        socket.Close();

#ifdef _DARWIN
                        break;
#else
                        delete pClient;
#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
                        return NULL;
#else
                        return;
#endif // defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
#endif // _DARWWIN
					}
					Json::Value respondJs;
					int ret = Registration::getFunc(funcName)(respondJs, jsonRoot);
					respondJs["errcode"] = ret;

					std::string res = respondJs.toStyledString();
					pClient->Send( (uint8 *)res.c_str(), res.length() );
					pClient->Close();
					runner.resetRequest();
					break;

				} else {
					printf("Received %d bytes\n", bytesReceived);
				}
			}
			delete pClient;
			pClient = NULL;
		}
#ifdef WIN32
		if (10038 == GetLastError()) {
			break;
		}
#else
		if (9 == errno) {
			break;
		}
#endif
		//delete pClient;

	}

	socket.Close();
}

std::string Respond(const int errcode, Json::Value param) {
	param["errcode"] = 0;
	return param.toStyledString();
}

void CreateDirectoryRecur(std::string sDir, std::string sRootDir) {
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	if(ACCESS(sDir.c_str(), 0) == 0){
		return;
	}
#else
	DIR* dirptr = NULL;
	if(dirptr = opendir(sDir.c_str())){
		closedir(dirptr);
		return;
	}
#endif
	int nStart = sRootDir.length()+1;
	int nFind = sDir.find('/', nStart);
	while(nFind != -1)
	{
		std::string SubDir = sDir.substr(0,nFind);
		//std::string wStrSubDir = "";
		//WstringToUTF8(SubDir, wStrSubDir);
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)		
		if(ACCESS(SubDir.c_str(), 0) != 0){
			MKDIR(SubDir.c_str());
		}
#else
		if(dirptr = opendir(SubDir.c_str())){
			closedir(dirptr);	
		}else{
			MKDIR(SubDir.c_str());
		}
#endif
		nFind = sDir.find('/', nFind+1);
	}
}


