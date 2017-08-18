#include "qmlvideoplugin.h"
#include "QMandala.h"

#include "ffmpegplayer.h"
#include "videocontroller.h"
#include "videocoords.h"
#include "osdmenu.h"

void QmlVideoPlugin::init(void)
{
    name = "qmlvideo";

    if(!QSettings().contains("plugins/qmlvideo"))
        QSettings().setValue("plugins/qmlvideo", "dock");

    FfmpegPlayer::registerQmlType();
    VideoController::registerQmlType();
    VideoCoords::registerQmlType();
    OsdItem::registerQmlType();
    OsdMenu::registerQmlType();
}

QmlVideoPlugin::QmlVideoPlugin()
{

}

QmlVideoPlugin::~QmlVideoPlugin()
{

}
