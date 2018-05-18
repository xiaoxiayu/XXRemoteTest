#ifndef _FXQA_TEST_COMMON_H
#define _FXQA_TEST_COMMON_H

#include <iostream>
#include <map>
#include "fxqa_config.h"
#include "json/json.h"

#define MAX_PACKET 4096 

typedef int (*TESTFUN)(Json::Value&, const Json::Value); 
typedef std::map<std::string, TESTFUN> RegistrationCache;

RegistrationCache& __getRegistMap();
int __errorTestCall(Json::Value& respond, const Json::Value request);

#define TEST_REGEDIT(testname, func) Registration _##testname( #testname, func );

#define REMOTE_RESPOND_DEFINE(param) Json::Value param

#define REMOTE_INTERFACE_DECLARE(funcname, request, respond) \
	static int funcname(Json::Value& respond, const Json::Value request);

#define REMOTE_INTERFACE_IMPLEMENT(remotename, funcname, request, respond) \
	TEST_REGEDIT(remotename, funcname);\
	int funcname(Json::Value& respond, const Json::Value request) 

 typedef enum
    {
        RunningError = -1,          ///< Generic socket error translates to error below.
        RunningSuccess = 0,         ///< No socket error.
		RunningScheme,
        RunningParam,		       ///< Invalid socket handle.
		RunningTest,
        RunningModule,      ///< Invalid destination address specified.
		RunningNotFound,
        RunningEunknown             ///< Unknown error please report to mark@carrierlabs.com
    } CRunningError;

class Registration {
public:
	Registration(std::string name, TESTFUN func) {
		__getRegistMap()[name] = func;
	}

	static TESTFUN getFunc(std::string funcName) {
		TESTFUN func = __getRegistMap()[funcName];
		if(func == NULL) {
			return __errorTestCall;
		}
		return func;
	}
};

class CTestRunner {
	bool			m_bFinish;
	std::string		m_sParam;
public:
	CTestRunner() : m_bFinish(false), m_sParam("") {};
	~CTestRunner() {};

	void setRequestFinish() {m_bFinish = true;};
	bool isRequestFinish() {return m_bFinish;};
	void resetRequest() {m_bFinish = false;};

	void setTestParam(std::string param) {m_sParam = param;};
	std::string getParam() {return m_sParam;};
};
#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
void*			FXQA_ServerRun(void* param);
#else
void			FXQA_ServerRun(std::string host_url);
#endif
std::string		Respond(const int errcode, Json::Value param);
void			CreateDirectoryRecur(std::string sDir, std::string sRootDir);

#endif // _FXQA_TEST_COMMON_H
