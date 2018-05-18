#ifndef _FXQA_TESTLOG_H_
#define _FXQA_TESTLOG_H_

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
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
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#include "fxqa_config.h"
#include "fxqa_testmemory.h"
#include "fxqa_logwriter.h"

#if defined(_FXQA_TOOL_GLOG_)
#   if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
#       define GLOG_NO_ABBREVIATED_SEVERITIES
#       include "windows/glog/logging.h"
#       include "windows/glog/raw_logging.h"
#   else
#       define GLOG_NO_ABBREVIATED_SEVERITIES
#       include "glog/logging_gunc.h"
#       include "glog/raw_logging.h"
#   endif
#endif // _FXQA_TOOL_GLOG_

#ifdef QT_VERSION
#   include <QDebug>
#   define FXQA_printf qDebug
#else
#   define FXQA_printf printf
#endif

#define QA_LOG fxqa_test::CPrinter
#define QA_LOG_path fxqa_test::CLOGPlus::rptPath

#define QALOG_RUNNEDFILE_WRITE			0
#define QALOG_RUNNEDFILE_APPEND			1
#define QALOG_RUNNEDFILE_NOLOG			2

#define QARUN(_interface) QA_DLOG(INFO)<<#_interface;_interface;

_START_FXQA_TEST_NAMESPACE_

class CPrinter {
public:
	CPrinter() {};
	~CPrinter() {};

	const CPrinter& operator << (char *msg) const;
	const CPrinter& operator << (std::string msg) const;
	const CPrinter& operator << (double iMsg) const;
};


typedef struct COMPARE_XML_LOG_INFO_TIME {
	double dNewPDFParseTime;
	double dOldPDFParseTime;
	double dPDFParseTimeRatio;

	double dNewPageParseTime;
	double dOldPageParseTime;
	double dPageParserTimeRatio;

	double dNewRenderTime;
	double dOldRenderTime;
	double dRenderTimeRatio;

	double dNewDeleteDocTime;
	double dOldDeleteDocTime;
	double dDeleteDocTimeRatio;

	double ParsePdfContent_new;
	double ParsePdfContent_old;
	double PdfLoaderSaverNew;
	double PdfLoaderSaverOld;
	double dNewLoadImageTime;
	double dOldLoadImageTime;
	double dNewInsertImageToPDFTime;
	double dOldInsertImageToPDFTime;
	double dNewtimeBig_Allpage_new;
	double dNewtimeBig_Allpage_old;
	double dOldtimeBig_Allpage_new;
	double dOldtimeBig_Allpage_old;
	double dALLNewPagetimeSlowCount;
	double dALLOldPagetimeSlowCount;
	double dNewParserReflowPage_01;
	double dNewParserReflowPage_02;
	double dOldParserReflowPage_01;
	double dOldParserReflowPage_02;
	double dALLPageCount;
	double  m_newDoConvertTime;
	double	m_oldDoConvertTime;
	double	nDynamicFileCount;
	double  m_newStartConvertTime;
	double  m_oldStartConvertTime;
	double dNewCopyPasteTime;
	double dOldCopyPasteTime;
	double dNewConvertTime;
	double dOldConvertTime;
	double dConvertTimeRatio;
	
	int m_AllFileCount;
	double dNewDeleteBookmarkTime;
	double dOldDeleteBookmarkTime;

	std::map<std::wstring, double> NewPDFParsePerFileTime;
	std::map<std::wstring, double> OldPDFParsePerFileTime;

	COMPARE_XML_LOG_INFO_TIME() {
		dNewPDFParseTime = 0;
		dOldPDFParseTime = 0;
		dNewPageParseTime = 0;
		dOldPageParseTime = 0;
		dNewRenderTime = 0;
		dOldRenderTime = 0;
		dNewDeleteDocTime = 0;
		dOldDeleteDocTime = 0;
		ParsePdfContent_new = 0;
		ParsePdfContent_old = 0;
		PdfLoaderSaverNew = 0;
		PdfLoaderSaverOld = 0;
		dNewInsertImageToPDFTime = 0;
		dOldInsertImageToPDFTime = 0;
		dNewtimeBig_Allpage_new = 0;
		dNewtimeBig_Allpage_old = 0;
		dOldtimeBig_Allpage_new = 0;
		dOldtimeBig_Allpage_old = 0;
		dALLNewPagetimeSlowCount = 0;
		dALLOldPagetimeSlowCount = 0;
		dNewParserReflowPage_01 = 0;
		dNewParserReflowPage_02 = 0;
		dOldParserReflowPage_01 = 0;
		dOldParserReflowPage_02 = 0;
		dALLPageCount = 0;
		dPDFParseTimeRatio = 0;
		dRenderTimeRatio = 0;
		dDeleteDocTimeRatio = 0;
		dPageParserTimeRatio = 0;
		m_newDoConvertTime =0;
		m_oldDoConvertTime =0;
		nDynamicFileCount =0;
		m_newStartConvertTime =0;
		m_oldStartConvertTime =0;
		dNewCopyPasteTime = 0;
		dOldCopyPasteTime = 0;
		dNewConvertTime = 0;
	    dOldConvertTime = 0;
	    dConvertTimeRatio = 0;

		m_AllFileCount = 0;
		dNewDeleteBookmarkTime = 0;
	    dOldDeleteBookmarkTime = 0;
	}
} LOG_TIMEINFO;
typedef struct COMPARE_XML_LOG_INFO_PAGE {
	int iOldWidth;
	int iOldHeight;
	int iNewWidth;
	int iNewHeight;

	COMPARE_XML_LOG_INFO_PAGE() {
		iOldWidth = 0;
		iOldHeight = 0;
		iNewWidth = 0;
		iNewHeight = 0;
	}

} LOG_PAGEINFO;

class CLOGPlus {
	static FILE* fp_runned_log;
	static std::string m_runedLogFileName;
	static FILE* fp_question_log;
	static std::string m_QuestionLogFileName;
	static CWriter* m_pLogWriter;
	static std::string m_rptPath;
	static double m_timestamp;

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
	struct timeval {
		long tv_sec, tv_usec;
	};
	static int gettimeofday(struct timeval *tv, void* tz);
#endif
	static void AdjustFileLenInfoStr(int fileLen, char* utf8_adjustBuf);

public:
	static std::string rptPath;

	CLOGPlus() {};
	~CLOGPlus() {};
	static bool InitLogRunnedFile(std::string testcaseName, std::string testName, int runnedfileFlag);
	static bool InitLogQuestionFile(std::string testcaseName, std::string testName, int runnedfileFlag);
	static void LogRunnedFile(std::wstring filePath);
	static void CloseLogRunnedFile();
	static void LogQuestionFile(std::wstring filePath, std::string tstcaseName, std::string testName, std::string type);
	static void CloseLogQuestionFile();

	static bool InitReportFile(std::string reportPath, std::string testsuites);
	static void CloseReportFile();
	static void Flush() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pLogWriter) return;
		m_pLogWriter->Flush();
	};

	static bool CreateTestProject(std::string testPrjName);
	static bool CreateTestCase(std::string testCaseName);
	static bool CreateTest(std::string testName);

	static void FuncName(const char* funcName) {
		m_pLogWriter->PairWrite("func", funcName);
	};

	static void TestFile(std::string testFile) {
		m_pLogWriter->TestFile(testFile.c_str());
	};

	static void FinishTestProject(double used_time) {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pLogWriter) return;
		m_pLogWriter->TestFinish(used_time);
	};
	static void FinishTestCase(double used_time) {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pLogWriter) return;
		m_pLogWriter->TestCaseEnd(used_time);
	};

	static void FinishTest(double used_time) {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pLogWriter) return;
		m_pLogWriter->TestEnd(used_time);
	};

	static void FinishAll() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pLogWriter) return;
		m_pLogWriter->TestCaseEnd(0);
	};

	static void Crash(int errCode) {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif // _FXQA_MICROSERVICE_
		if (!m_pLogWriter) return;
		m_pLogWriter->CreateCrash(errCode);
	};

	static void Bug(const char* typeValue);
	static void Bug(const char* typeValue, std::string bugValue);
	static void Bug(const char* typeValue, std::wstring bugValue);
	static void Bug(const char* typeValue, std::map<std::string, std::string>& infoMap);
	static void Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string bugValue);
	static void Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring bugValue);
	static void Bug(const char* typeValue, int pageIndex);
	static void Bug(const char* typeValue, int pageIndex, std::string bugValue);
	static void Bug(const char* typeValue, int pageIndex, std::wstring bugValue);
	static void Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap);
	static void Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string bugValue);
	static void Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring bugValue);

	static void Warn(const char* warnValue);

	static void Info(const char* infoValue);
	static void Info(const char* typeValue, std::string infoValue);
	static void Info(const char* typeValue, std::wstring infoValue);
	static void Info(const char* typeValue, std::map<std::string, std::string>& infoMap);
	static void Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string infoValue);
	static void Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring infoValue);
	static void Info(const char* infoValue, int pageIndex);
	static void Info(const char* typeValue, int pageIndex, std::string infoValue);
	static void Info(const char* typeValue, int pageIndex, std::wstring infoValue);
	static void Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap);
	static void Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string infoValue);
	static void Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring infoValue);

	static void TimeAll(LOG_TIMEINFO timeInfo, std::string testFile);
	static void TimeAll(LOG_TIMEINFO timeInfo, std::wstring testFile);
	static void TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::string testFile);
	static void TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::wstring testFile);
	static void TimeAll(std::string testName, std::string timeInfo, int writeFlag=QALOG_RUNNEDFILE_WRITE);
	static void RecoveryTime(std::string sPath, std::string testName, LOG_TIMEINFO &m_timeInfo);
	static void InitTotalTime(std::string sPath, std::string testName, std::map<std::string,double> &timeInfo);

	static void PrintOutputFile(std::wstring filePath);

	static void CreateTestInfo(const char* testName, const char* testStatus, std::map<std::string, std::string> infoMap) {
		if (!m_pLogWriter) return;
		m_pLogWriter->CreateTestInfo(testName, testStatus, infoMap);
	};

	static void CreateTestCaseInfo(const char* testCaseName, const char* testStatus, std::map<std::string, std::string> infoMap) {
		if (!m_pLogWriter) return;
		m_pLogWriter->CreateTestCaseInfo(testCaseName, testStatus, infoMap);
	};

	static void InfoMapSprintf(std::map<std::string, std::string>& infoMap, std::string format, ...);

	char* GetSuccessLog(const char* info) const;

	static double	GetCurrentTime();
	static void		TimeStart();
	static double	TimeEnd();
	static FXQA_MEMINFO GetMemoryInfo() {return CMemoryState::GetMemoryInfo();};

	static bool Report(std::string key, std::wstring wReportMs);
	static bool Report(std::string key, std::string reportMsg);
	static bool Report(std::string key, std::map<std::string, std::string> infoMap, std::string reportMsg="");

	static bool ReportTimeAll(std::string key, std::map<std::string, std::string> infoMap, std::string reportMsg="");

	static void InfoMapToString(std::map<std::string, std::string> infoMap, std::string& infoStr);
	//static std::string POSTLog();

	static void Write(std::string buf);
};

_END_FXQA_TEST_NAMESPACE_

#endif // _FXQA_TESTLOG_H_
