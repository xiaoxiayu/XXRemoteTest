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
	REMOTE_INTERFACE_DECLARE(Demo0, jsRquest, jsRespond); //�½�CPDF_BookmarkTree��
	REMOTE_INTERFACE_DECLARE(Demo1, jsRquest, jsRespond); //ɾ��CPDF_BookmarkTree��
	
};

#endif // _TEST_DEMO_H

