#include "WhatCablePlugin.h"
#include "DeviceModel.h"
#include <qqml.h>

void WhatCablePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.whatcable"));
    qmlRegisterType<WhatCable::DeviceModel>(uri, 1, 0, "DeviceModel");
}
