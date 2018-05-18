#ifndef _TEST_DEMO_H
#define _TEST_DEMO_H

#include "fxqa_test_common.h"

class CTestDemo {
	// Define fxcore library class.
	FXQA_MODULE_CALLER_DEFINE;

public:

	CTestDemo() {
		
	};

	~CTestDemo() {
		
	};
	REMOTE_INTERFACE_DECLARE(Demo0, jsRquest, jsRespond); //新建CPDF_BookmarkTree类
	REMOTE_INTERFACE_DECLARE(Demo1, jsRquest, jsRespond); //删除CPDF_BookmarkTree类
	
};

#endif // _TEST_DEMO_H

