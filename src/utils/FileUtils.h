#pragma once
#include <QString>
#include <QStringList>

/**
 * @brief FileUtils provides file-related helper functions.
 */
namespace FileUtils {
    QString baseName(const QString& path);
    QStringList sniffCsvHeader(const QString& filePath);
    QStringList parseCsvLine(const QString& line);
} 