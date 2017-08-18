#ifndef QMLVIDEOPLUGIN_H
#define QMLVIDEOPLUGIN_H

#include "plugin_interface.h"

class QmlVideoPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.uavos.gcu.PluginInterface/1.0")
    Q_INTERFACES(PluginInterface)
public:
    void init(void);
    QmlVideoPlugin();
    ~QmlVideoPlugin();
};


#endif // QMLVIDEOPLUGIN_H
