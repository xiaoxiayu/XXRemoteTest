#pragma once

#include <QObject>

class FXQALocation : public QObject
{
    Q_OBJECT
    
public:
    QGeoPositionInfoSource*  src ;
    explicit FXQALocation(QObject *parent = 0) {};
    ~FXQALocation(){};
    
    void networkSessionOpened();
    
    private slots:
	    void positionUpdated(QGeoPositionInfo gpsPos);
	    void positionError(QGeoPositionInfoSource::Error e);   
};
