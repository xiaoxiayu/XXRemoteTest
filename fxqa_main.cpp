#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
#include "mainwindow.h"
#include <QApplication>
#include <pthread.h>
#include <QNetworkInterface>
#incldue "fxqa_location.h"
#endif //_FXQA_PLATFORM_QTIOS_

#include <stdio.h>
#include <iostream>

#include "fxqa_test_common.h"


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define _PORT_TO_STR ":" STR(_FXQA_PORT_)

std::string GetLocalHostIP() {
#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
    QList<QHostAddress> list = QNetworkInterface::allAddresses();

     for(int nIter=0; nIter<list.count(); nIter++) {
          if(!list[nIter].isLoopback())
              if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
                return list[nIter].toString().toLocal8Bit().constData();
      }
#else
    return "127.0.0.1";
#endif
}




int main(int argc, char** argv) {
#if defined(_FXQA_PLATFORM_QTIOS_)
	  QApplication a(argc, argv);
	  FXQALocation lc;
	  lc.networkSessionOpened();
	  MainWindow w;
	  w.show();
#elif defined(_FXQA_PLATFORM_QTANDROID_)
	  QApplication a(argc, argv);
	  MainWindow w;
	  w.show();
#endif
    std::string host_url = GetLocalHostIP() + _PORT_TO_STR;
    if (argc > 1) {
		  printf("URL=%s\n", argv[1]);
		  host_url = argv[1];
	  }

#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_)
    pthread_t thread = 0;
    int nErr = pthread_create(&thread , NULL, &FXQA_ServerRun, &host_url);
    return a.exec();
#else
    FXQA_ServerRun(host_url);
	return 1;
#endif
}
