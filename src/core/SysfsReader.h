// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
// Linux sysfs equivalent of the IOKit property reading in the original.
#pragma once

#include <QString>
#include <QStringList>
#include <QMap>
#include <optional>
#include <cstdint>

namespace WhatCable {

class SysfsReader {
public:
    static QString readAttribute(const QString &path);
    static std::optional<int> readIntAttribute(const QString &path);
    static std::optional<uint32_t> readHexAttribute(const QString &path);
    static QStringList listSubdirectories(const QString &path);
    static QMap<QString, QString> readAllAttributes(const QString &dirPath);
    static bool pathExists(const QString &path);
};

} // namespace WhatCable
