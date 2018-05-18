#include "test_demo.h"

FXQA_MODULE_CALLER_INIT(CTestDemo);

REMOTE_INTERFACE_IMPLEMENT(ExportName_Demo0, CTestDemo::Demo0, jsRequest, jsRespond) {
	jsRespond["respond"] = "ok";
	return CRunningError::RunningSuccess;
}

REMOTE_INTERFACE_IMPLEMENT(ExportName_Demo1, CTestDemo::Demo1, jsRequest, jsRespond) {
	jsRespond["respond"] = 0;
	return CRunningError::RunningSuccess;
}
