#include "FileUtils.h"
#include <QFileInfo>
#include <QFile>
#include <QTextStream>

namespace FileUtils {

QString baseName(const QString& path)
{
    return QFileInfo(path).baseName();
}

QStringList sniffCsvHeader(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringList();
    QTextStream in(&file);
    QString headerLine = in.readLine();
    return headerLine.split(",");
}

} // namespace FileUtils 