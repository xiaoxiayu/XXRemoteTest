#ifndef _FXQA_CONFIG_H_
#define _FXQA_CONFIG_H_

#define QARUN(_interface) QA_DLOG(INFO)<<#_interface;_interface;

#ifdef FXQA_USE_NEDMEMORY
#define fxqa_malloc		nedpmalloc
#else
#define fxqa_malloc		malloc
#endif

#define FXQA_TEST fxqa_test

#define _END_FXQA_TEST_NAMESPACE_	}
#define _START_FXQA_TEST_NAMESPACE_	namespace FXQA_TEST { 


#endif

