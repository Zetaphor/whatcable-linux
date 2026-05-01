// Derived from WhatCable by Darryl Morley (https://github.com/darrylmorley/whatcable)
#include "SysfsReader.h"
#include <QFile>
#include <QDir>
#include <QTextStream>

namespace WhatCable {

QString SysfsReader::readAttribute(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QTextStream stream(&file);
    return stream.readLine().trimmed();
}

std::optional<int> SysfsReader::readIntAttribute(const QString &path)
{
    const QString val = readAttribute(path);
    if (val.isEmpty())
        return std::nullopt;
    bool ok = false;
    int result = val.toInt(&ok);
    return ok ? std::optional(result) : std::nullopt;
}

std::optional<uint32_t> SysfsReader::readHexAttribute(const QString &path)
{
    QString val = readAttribute(path);
    if (val.isEmpty())
        return std::nullopt;
    if (val.startsWith(QStringLiteral("0x")) || val.startsWith(QStringLiteral("0X")))
        val = val.mid(2);
    bool ok = false;
    uint32_t result = val.toUInt(&ok, 16);
    return ok ? std::optional(result) : std::nullopt;
}

QStringList SysfsReader::listSubdirectories(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
        return {};
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
}

QMap<QString, QString> SysfsReader::readAllAttributes(const QString &dirPath)
{
    QMap<QString, QString> attrs;
    QDir dir(dirPath);
    if (!dir.exists())
        return attrs;
    const auto entries = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &entry : entries) {
        const QString val = readAttribute(dir.filePath(entry));
        if (!val.isEmpty())
            attrs.insert(entry, val);
    }
    return attrs;
}

bool SysfsReader::pathExists(const QString &path)
{
    return QFile::exists(path) || QDir(path).exists();
}

} // namespace WhatCable
