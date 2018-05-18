#include "fxqa_location.h"
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#include <QDebug>

#if 0
void CAudio::play() const
{    
  QString fileName = "demo.mp3";

	NSString *soundFilePath = fileName.toNSString();
	NSURL *soundFileURL = [NSURL fileURLWithPath:soundFilePath];
	static AVAudioPlayer *audioPlayer = [AVAudioPlayer alloc];
	[audioPlayer initWithContentsOfURL:soundFileURL error:nil];
    audioPlayer.numberOfLoops = -1;
    
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
    [[AVAudioSession sharedInstance] setActive: YES error: nil];
    [[UIApplication sharedApplication] beginReceivingRemoteControlEvents];
    
	[audioPlayer play];
}
#endif // 0


void FXQALocation::networkSessionOpened()
{
  src = QGeoPositionInfoSource::createDefaultSource(this);
    
  if (src) {
    src->setUpdateInterval(10000);
    connect(src, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));
    connect(src, SIGNAL(error(QGeoPositionInfoSource::Error)),
            this, SLOT(positionError(QGeoPositionInfoSource::Error)));
    src->startUpdates();
  } else {
  	qWarning() << "Location Start Failed.";
  }
}

void FXQALocation::positionUpdated(QGeoPositionInfo gpsPos)
{
    QGeoCoordinate coord = gpsPos.coordinate();
    qDebug() << coord.toString();
}

void FXQALocation::positionError(QGeoPositionInfoSource::Error e)
{
    Q_UNUSED(e);
    qWarning() << "Position source error. Falling back to simulation mode.";
    src->stopUpdates();
    src->deleteLater();
    src = 0;
}

