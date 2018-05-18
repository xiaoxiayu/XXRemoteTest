
#include "fxqa_testlog.h"
#include "fxqa_charset.h"
#include "fxqa_common.h"
#include "fxqa_statussender.h"

#include <algorithm>

_START_FXQA_TEST_NAMESPACE_

	void CLOGPlus::AdjustFileLenInfoStr(int fileLen, char* utf8_adjustBuf) {
#ifdef QT_VERSION
    QString qBuf;
    qBuf.sprintf("%x", fileLen);
    int len = qBuf.length();
    QString qAdjustBuf;
    if (len < 4) {
        int e = 4 - len;
        QString tems = "";
        for (int i = 0; i < e; i++) {
            tems += "0";
        }
        qAdjustBuf.append(tems);
        qAdjustBuf.append(qBuf);
    } else if (len == 4) {
        qAdjustBuf.append(qBuf);
    }
    std::wstring wtem = qAdjustBuf.toStdWString();
    //wcscpy(utf8_adjustBuf, wtem.c_str());
	std::string wstrWtem = "";
	WstringToUTF8(wtem, wstrWtem);
	strcpy(utf8_adjustBuf, wstrWtem.c_str());
#else
		char buf[5];
		memset(buf, 0, 5);
        //itoa(fileLen, buf, 16);
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		sprintf_s(buf, "%x", fileLen);
#else
        sprintf(buf, "%x", fileLen);
#endif
        int len = strlen(buf);
        std::wstring wStr = s2ws(buf);
        memset(utf8_adjustBuf, 0, 5);
        if (len < 4) {
            int e = 4 - len;
            std::wstring tems = L"";
            for (int i = 0; i < e; i++) {
                /*wcscpy(adjustBuf, L"0");
                adjustBuf++;*/
                tems += L"0";
            }
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
            wcscat_s(adjustBuf, 5, tems.c_str());
			wcscat_s(adjustBuf, 5, wStr.c_str());
#else
			tems += wStr;
			sUxU8(utf8_adjustBuf, (wchar_t*)tems.c_str(), 5);
#endif
        } else if (len == 4) {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
			wcscat_s(adjustBuf, 5, wStr.c_str());
#else
            //wcscat(adjustBuf, wStr.c_str());
			sUxU8(utf8_adjustBuf, (wchar_t*)wStr.c_str(), 5);
#endif
        }
#endif

}

std::string CLOGPlus::rptPath;
std::string testcasePath;
FILE* CLOGPlus::fp_runned_log = NULL;
std::string CLOGPlus::m_runedLogFileName = "";
int iRet;
bool CLOGPlus::InitLogRunnedFile(std::string testcaseName, std::string testName, int runnedfileFlag) {
	testcasePath = rptPath + "/" + testcaseName;
	iRet  = ACCESS(testcasePath.c_str(), 0);
	if (iRet != 0) {
		iRet = MKDIR(testcasePath.c_str());
		if (iRet != 0) {
            FXQA_printf("mkdir failed: %s\n", testcasePath.c_str());
		} 
	}

	m_runedLogFileName = testcasePath + "/" +  testName + "_Compared_PDFs.txt";
	std::string strCrashTemp;
    strCrashTemp = "===============================================================\r\n";
	switch (runnedfileFlag) {
	case QALOG_RUNNEDFILE_WRITE: {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fp_runned_log = NULL;
		fopen_s(&fp_runned_log, m_runedLogFileName.c_str(), "wb");
#else
		fp_runned_log = fopen(m_runedLogFileName.c_str(), "wb");
#endif

#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
		fclose(fp_runned_log);
		fp_runned_log = NULL;
#endif
		return false;
								 }
	case QALOG_RUNNEDFILE_APPEND:{
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fp_runned_log = NULL;
		fopen_s(&fp_runned_log, m_runedLogFileName.c_str(), "ab");
#else
		fp_runned_log = fopen(m_runedLogFileName.c_str(), "ab");
#endif

        fwrite(strCrashTemp.c_str(), sizeof(char), strCrashTemp.length(), fp_runned_log);
        fflush(fp_runned_log);

#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
		fclose(fp_runned_log);
		fp_runned_log = NULL;
#endif
		return true;
								 }
	case QALOG_RUNNEDFILE_NOLOG:
	default:
		break;
	}
#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
	fclose(fp_runned_log);
	fp_runned_log = NULL;
#endif
	return false;
}
FILE* CLOGPlus::fp_question_log = NULL;
std::string CLOGPlus::m_QuestionLogFileName = "";
bool CLOGPlus::InitLogQuestionFile(std::string testcaseName, std::string testName, int runnedfileFlag) {
	testcasePath = m_rptPath;
	iRet  = ACCESS(testcasePath.c_str(), 0);
	if (iRet != 0) {
		iRet = MKDIR(testcasePath.c_str());
		if (iRet != 0) {
			FXQA_printf("mkdir failed: %s\n", testcasePath.c_str());
		} 
	}

	m_QuestionLogFileName = testcasePath + "/question.log";
	switch (runnedfileFlag) {
	case QALOG_RUNNEDFILE_WRITE: {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fp_runned_log = NULL;
		fopen_s(&fp_question_log, m_QuestionLogFileName.c_str(), "wb");
#else
		fp_question_log = fopen(m_QuestionLogFileName.c_str(), "wb");
#endif

#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
		fclose(fp_question_log);
		fp_question_log = NULL;
#endif
		return false;
								 }
	case QALOG_RUNNEDFILE_APPEND:{
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fp_question_log = NULL;
		fopen_s(&fp_question_log, m_QuestionLogFileName.c_str(), "ab");
#else
		fp_question_log = fopen(m_QuestionLogFileName.c_str(), "ab");
#endif

#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
		fclose(fp_runned_log);
		fp_runned_log = NULL;
#endif
		return true;
								 }
	case QALOG_RUNNEDFILE_NOLOG:
	default:
		break;
	}
#if defined(_FXQA_PLATFORM_WINDOWS_RT_)
	fclose(fp_question_log);
	fp_question_log = NULL;
#endif
	return false;
}

CWriter* CLOGPlus::m_pLogWriter = NULL;
std::string CLOGPlus::m_rptPath = "";
bool CLOGPlus::InitReportFile(std::string reportPath, std::string rptPath) {
	m_rptPath = reportPath;
#if defined(_FXQA_MICROSERVICE_)
	return true;
#endif // _FXQA_MICROSERVICE_

	std::string absPath = reportPath + "/" + rptPath;
	//m_pLogWriter = new CWriter(absPath);
	m_pLogWriter = new CWriter(absPath);
	return true;
}

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
int CLOGPlus::gettimeofday(struct timeval *tv, void* tz) {
#define EPOCHFILETIME (116444736000000000ULL)
	FILETIME ft;
	LARGE_INTEGER li;
	unsigned __int64 tt;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	tv->tv_sec = tt / 1000000;
	tv->tv_usec = tt % 1000000;

	return 0;
}
#endif

double CLOGPlus::GetCurrentTime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double time_now = static_cast<double>(tv.tv_sec) * 1000000 + tv.tv_usec;

	return time_now * 0.000001;
}

double CLOGPlus::m_timestamp = 0;
void CLOGPlus::TimeStart() {
	m_timestamp = GetCurrentTime();
}

double CLOGPlus::TimeEnd() {
	double used_time = GetCurrentTime() - m_timestamp;
	return used_time;
}

bool CLOGPlus::Report(std::string key, std::wstring wReportMsg){
	m_pLogWriter->Create(key.c_str(), wReportMsg);
	return true;
}

bool CLOGPlus::Report(std::string key, std::string reportMsg){
	m_pLogWriter->Create(key.c_str(), reportMsg);
	return true;
}

void CLOGPlus::LogRunnedFile(std::wstring filePath) {
	if (!fp_runned_log) {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fopen_s(&fp_runned_log, m_runedLogFileName.c_str(), "ab");
		if (!fp_runned_log) return;
#else
		return;
#endif
	}

	std::wstring log_info = filePath + L"\r\n";

	char* utf8_log_info = NULL;
    int utf8_len = sUxU8(utf8_log_info, (wchar_t*)log_info.c_str(), log_info.length());
    utf8_log_info = new char[utf8_len];
    memset(utf8_log_info, 0, utf8_len);
    sUxU8(utf8_log_info, (wchar_t*)log_info.c_str(), log_info.length());

    fwrite(utf8_log_info, sizeof(char), utf8_len, fp_runned_log);
    fflush(fp_runned_log);

    delete[]  utf8_log_info;
}

void CLOGPlus::LogQuestionFile(std::wstring filePath, std::string tstcaseName, std::string testName, std::string type) {
	if (!fp_question_log) {
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
		fopen_s(&fp_runned_log, m_QuestionLogFileName.c_str(), "ab");
		if (!fp_runned_log) return;
#else
		return;
#endif
	}

	std::wstring log_info = filePath;
	char* strlog = new char[512];
	memset(strlog, 0, 512);
	//itoa(fileLen, buf, 16);
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
	sprintf_s(strlog, "{\"testcase\":\"%s\", \"test\":\"%s\", \"testfile\":\"%s\", \"type\":\"%s\"}\r\n", tstcaseName.c_str(), testName.c_str(), utf8_log_info, type.c_str());
#else
	sprintf(strlog, "{\"testcase\":\"%s\", \"test\":\"%s\", \"testfile\":\"%s\", \"type\":\"%s\"}\r\n", tstcaseName.c_str(), testName.c_str(), ws2s(log_info).c_str(), type.c_str());
#endif
	
	fwrite(strlog, sizeof(char), strlen(strlog), fp_question_log);
	fflush(fp_question_log);
	delete[]  strlog;
}

void CLOGPlus::CloseLogRunnedFile() {
#if defined(_FXQA_MICROSERVICE_)
	return;
#endif // _FXQA_MICROSERVICE_
	if (fp_runned_log) {
		fclose(fp_runned_log);
		fp_runned_log = NULL;
	}
}

void CLOGPlus::CloseLogQuestionFile() {
#if defined(_FXQA_MICROSERVICE_)
	if (fp_question_log) {
		fclose(fp_question_log);
		fp_question_log = NULL;
	}
#endif // _FXQA_MICROSERVICE_
}

void CLOGPlus::CloseReportFile() {
#if defined(_FXQA_MICROSERVICE_)
		return;
#endif 

	if (m_pLogWriter) {
		delete m_pLogWriter;
	}
}


void CLOGPlus::InfoMapSprintf(std::map<std::string, std::string>& infoMap, std::string format, ...) {
	size_t len = std::count(format.begin(), format.end(), '%');

	if ((len < 1) || (len % 2 != 0)) {
		return;
	}

	va_list argp;

	char* s_para = NULL;
	int	i_para = -1;
	double	f_para = -1;
	std::string para = "";

	va_start(argp, format);

	//para = va_arg(argp, char*);

	std::string key = "";
	std::string val = "";

	for(int argno = 0; argno < len; ++argno) {
		int pos = format.find("%");
		std::string typeStr = format.substr(pos + 1, 1);
		format = format.substr(pos + 1, format.length());
		const char* type = typeStr.c_str();
		switch(type[0]) {
		case 'd':
			i_para = va_arg(argp, int);
			para = IntToString(i_para).c_str();
			break;
		case 'f':
			f_para = va_arg(argp, double);
			para = IntToString(f_para).c_str();
			break;
		case 's':
			s_para = va_arg(argp, char*);
			para = s_para;
			break;
		}
		if (argno % 2 == 0) {
			key = para;
		} else {
			val = para;
			infoMap.insert(std::make_pair(key, val));
		}
		para = "";
	}
	va_end( argp );
}

bool CLOGPlus::CreateTestProject(std::string testPrjName) {
#if defined(_FXQA_MICROSERVICE_)
	return true;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return false;

	m_pLogWriter->CreateTestProject(testPrjName.c_str());
	return true;
}

bool CLOGPlus::CreateTestCase(std::string testCaseName) {
	if (m_pLogWriter == NULL) return false;

	m_pLogWriter->CreateTestCase(testCaseName.c_str());
	return true;
}

bool CLOGPlus::CreateTest(std::string testName) {
	if (m_pLogWriter == NULL) return false;

	m_pLogWriter->CreateTest(testName.c_str());
	return true;
}

void CLOGPlus::Bug(const char* bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(bugValue);
}

void CLOGPlus::Bug(const char* typeValue, std::string filePath) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue; 
	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, filePath);
}

void CLOGPlus::Bug(const char* typeValue, std::wstring filePath) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, filePath);
}

void CLOGPlus::Bug(const char* typeValue, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, infoMap, "All_TestFiles");
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr << ";" << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);
	std::string urlinfo = UrlEncode(infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << urlinfo; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex){
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, pageIndex);
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::string bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, pageIndex);
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::wstring bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, pageIndex, bugValue);
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, pageIndex, infoMap, "All_TestFiles");
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr << ";" << bugValue; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, pageIndex, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Bug(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring bugValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(bugValue, utf8Str);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr << ";" << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("bug", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateBug(typeValue, pageIndex, infoMap, bugValue);
	infoMap.clear();
}

void CLOGPlus::Warn(const char* infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << infoValue; 

	fxqa_status::CLogSender::Log2ELK("warn", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
}

void CLOGPlus::Info(const char* infoValue, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << infoStr;

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(infoValue, infoMap, "All_TestFiles");
	infoMap.clear();
}

void CLOGPlus::Info(const char* infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << infoValue; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(infoValue);
}

void CLOGPlus::Info(const char* typeValue, std::string filePath) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue;
	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, filePath);
}

void CLOGPlus::Info(const char* typeValue, std::wstring filePath) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue;

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return;
#endif
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, filePath);
}

void CLOGPlus::Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::string testfile) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, infoMap, testfile);
	infoMap.clear();
}

void CLOGPlus::Info(const char* typeValue, std::map<std::string, std::string>& infoMap, std::wstring testfile) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << infoStr; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, infoMap, testfile);
	infoMap.clear();
}

void CLOGPlus::Info(const char* infoValue, int pageIndex) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(infoValue, pageIndex);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::string infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, pageIndex, infoValue);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::wstring infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, pageIndex, infoValue);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex << ";" << infoStr; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, pageIndex, infoMap, "All_TestFiles");
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::string infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex << ";" << infoStr << ";" << infoValue; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, pageIndex, infoMap, infoValue);
}

void CLOGPlus::Info(const char* typeValue, int pageIndex, std::map<std::string, std::string>& infoMap, std::wstring infoValue) {
#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(infoMap, infoStr);

	std::string utf8Str;
	WstringToUTF8(infoValue, utf8Str);

	std::stringstream ss;
	ss << typeValue << ":" << "page=" << pageIndex << ";" << infoStr << ";" << utf8Str; 

	fxqa_status::CLogSender::Log2ELK("info", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_
	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo(typeValue, pageIndex, infoMap, infoValue);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, std::string testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::stringstream ss;
	ss << infoStr; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pLogWriter == NULL) return;

	m_pLogWriter->CreateInfo("Time_All", attriMap, testFile);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, std::wstring testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::stringstream ss;
	ss << infoStr; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pLogWriter == NULL) return;
	m_pLogWriter->CreateInfo("Time_All", attriMap, testFile);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::string testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::stringstream ss;
	ss << "page=" << pageIndex << ";" << infoStr; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pLogWriter == NULL) return;
	m_pLogWriter->CreateInfo("Time_All", pageIndex, attriMap, testFile);
}

void CLOGPlus::TimeAll(LOG_TIMEINFO timeInfo, int pageIndex, std::wstring testFile) {
	std::map<std::string, std::string> attriMap;
	attriMap.insert(std::make_pair("PDFPARSE_NEW", IntToString(timeInfo.dNewPDFParseTime)));
	attriMap.insert(std::make_pair("PDFPARSE_OLD", IntToString(timeInfo.dOldPDFParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_NEW", IntToString(timeInfo.dNewPageParseTime)));
	attriMap.insert(std::make_pair("PAGEPARSE_OLD", IntToString(timeInfo.dOldPageParseTime)));
	attriMap.insert(std::make_pair("RENDER_NEW", IntToString(timeInfo.dNewRenderTime)));
	attriMap.insert(std::make_pair("RENDER_OLD", IntToString(timeInfo.dOldRenderTime)));
	attriMap.insert(std::make_pair("DELETEDOC_NEW", IntToString(timeInfo.dNewDeleteDocTime)));
	attriMap.insert(std::make_pair("DELETEDOC_OLD", IntToString(timeInfo.dOldDeleteDocTime)));

#if defined(_FXQA_MICROSERVICE_)
	std::string infoStr = "";
	InfoMapToString(attriMap, infoStr);

	std::stringstream ss;
	ss << "page=" << pageIndex << "," << infoStr; 

	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#endif // _FXQA_MICROSERVICE_

	if (m_pLogWriter == NULL) return;
	m_pLogWriter->CreateInfo("Time_All", pageIndex, attriMap, testFile);
}

void CLOGPlus::TimeAll(std::string testName, std::string timeInfo, int writeFlag) {
#if defined(_FXQA_MICROSERVICE_)
	std::stringstream ss;
	ss <<  timeInfo; 
	fxqa_status::CLogSender::Log2ELK("time", ss.str());
	return ;
#else
	std::string rptTimePath = m_rptPath + "/" + testName + "_time.txt";
	FILE* fp = NULL;
	if (writeFlag == QALOG_RUNNEDFILE_WRITE) {
		fp = fopen(rptTimePath.c_str(), "wb");
	} else {
		fp = fopen(rptTimePath.c_str(), "ab+");
	}
	fwrite(timeInfo.c_str(), sizeof(char), timeInfo.length(), fp);
	fflush(fp);
	fclose(fp);
#endif // _FXQA_MICROSERVICE_	
}

void CLOGPlus::PrintOutputFile(std::wstring filePath) {
#ifdef _DEBUG
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	char* utf8_log_info = NULL;
	int utf8_len = sUxU8(utf8_log_info, (wchar_t*)filePath.c_str(), filePath.length());
	utf8_log_info = new char[utf8_len+1];
	memset(utf8_log_info, 0, utf8_len+1);
	sUxU8(utf8_log_info, (wchar_t*)filePath.c_str(), filePath.length());

	std::cout << "*FXQA_OUTPUT*:" << utf8_log_info << std::endl;

	delete[]  utf8_log_info;
#else
	std::string sFilePath = "";
    WstringToUTF8(filePath, sFilePath);
	std::cout << "*FXQA_OUTPUT*:" << sFilePath << std::endl;
#endif
#endif // _DEBUG
}

const CPrinter& CPrinter::operator << (char *msg) const {
	FXQA_printf("%s", msg);
	return *this;
}

const CPrinter& CPrinter::operator << (std::string msg) const {
	FXQA_printf("%s", msg.c_str());
	return *this;
}

const CPrinter& CPrinter::operator << (double iMsg) const {
	std::string msg = IntToString(iMsg);
	FXQA_printf("%s", msg.c_str());
	return *this;
}


void CLOGPlus::RecoveryTime(std::string sPath, std::string testName, LOG_TIMEINFO &m_timeInfo) {
	std::string sTimePath = sPath + "/" + testName + "_TimeAll_time.txt";
	std::vector<std::string> lines;
	std::map<std::string, double> m;

	std::string str;
	std::ifstream file(sTimePath.c_str());
	if(file.is_open()){
		while(getline(file,str)){
			std::string sKey = str.substr(0, str.find("="));
			double nValue = std::atof((str.substr(str.find("=")+1, str.length())).c_str());
			m.insert(std::pair<std::string,double>(sKey, nValue));
		}
		m_timeInfo.dNewRenderTime += m["PDFRENDER_NEW"];
		m_timeInfo.dOldRenderTime += m["PDFRENDER_OLD"];
		m_timeInfo.dNewPDFParseTime += m["PDFPARSE_NEW"];
		m_timeInfo.dOldPDFParseTime += m["PDFPARSE_OLD"];
		m_timeInfo.dNewPageParseTime += m["PAGEPARSE_NEW"];
		m_timeInfo.dOldPageParseTime += m["PAGEPARSE_OLD"];
		m_timeInfo.dNewDeleteDocTime += m["CLOSEPDF_NEW"];
		m_timeInfo.dOldDeleteDocTime += m["CLOSEPDF_OLD"];
		m_timeInfo.PdfLoaderSaverNew+=m["PDFLoaderSaverNew"];
		m_timeInfo.PdfLoaderSaverOld+=m["PDFLoaderSaverOld"];
		m_timeInfo.dNewDeleteBookmarkTime+=m["DeleteBookmark_NEW"];
		m_timeInfo.dOldDeleteBookmarkTime+=m["DeleteBookmark_OLD"];
		m_timeInfo.m_AllFileCount +=m["NomalTestFileCount"];
	}

}

void CLOGPlus::InitTotalTime(std::string sPath, std::string testName, std::map<std::string,double> &timeInfo) {
	std::string sTimePath = sPath + "/" + testName + "_TimeAll_time.txt";
	std::vector<std::string> lines;
	std::map<std::string, double> m;

	std::string str;
	std::ifstream file(sTimePath.c_str());
	if(file.is_open()){
		while(getline(file,str)){
			std::string sKey = str.substr(0, str.find("="));
			double nValue = std::atof((str.substr(str.find("=")+1, str.length())).c_str());
			m.insert(std::pair<std::string,double>(sKey, nValue));
		}
		std::map<std::string,double>::iterator iEnd,iteratorTime = timeInfo.begin();
		iEnd = timeInfo.end();
		double a=0;
		for(;iteratorTime!= iEnd; iteratorTime++)
		{
			a=m[iteratorTime->first];
			iteratorTime->second = a;
		//	timeInfo.at(iteratorTime->first) =m[iteratorTime->first];
		}
	}

}

void CLOGPlus::InfoMapToString(std::map<std::string, std::string> infoMap, std::string& infoStr) {
	std::map<std::string, std::string>::iterator infoItor;
	for (infoItor = infoMap.begin(); infoItor != infoMap.end(); infoItor++) {
		infoStr += infoItor->first;
		infoStr += "=";
		infoStr += infoItor->second;
		infoStr += " ";
	}
}

void CLOGPlus::Write(std::string buf) {

}

//std::string CLOGPlus::POSTLog(std::string url, std::string msg) {
//	//char *pHttpPost = "POST %s HTTP/1.1\r\n"  
//	//	"Host: %s:%d\r\n"  
//	//	"Content-Type: application/x-www-form-urlencoded\r\n"  
//	//	"Content-Length: %d\r\n\r\n"  
//	//	"%s";  
//
//	//char* host = "0.0.0.0";  
//	//int port = 9090;  
//
//	////char strHttpPost[1024] = {0};
//	//char* strHttpPost = new char[256 + msg.length()];
//	//memset(strHttpPost, 0, sizeof(strHttpPost));
//	//sprintf(strHttpPost, pHttpPost, url.c_str(), host, port, msg.length(), msg.c_str());  
//	//::read();
//	//return ::send(m_socketInfo.socket, strHttpPost, strlen(strHttpPost), 0);
//	return "";
//}
_END_FXQA_TEST_NAMESPACE_
