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
    return parseCsvLine(headerLine);
}

QStringList parseCsvLine(const QString& line)
{
    QStringList fields;
    QString current;
    bool inQuotes = false;
    bool escapeNext = false;
    
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];
        
        if (escapeNext) {
            current += c;
            escapeNext = false;
        } else if (c == '\\') {
            escapeNext = true;
        } else if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.append(current);
            current.clear();
        } else {
            current += c;
        }
    }
    
    fields.append(current);
    return fields;
}

} // namespace FileUtils 