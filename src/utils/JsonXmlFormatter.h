#pragma once
#include <QString>

/**
 * @brief JsonXmlFormatter provides pretty-printing for embedded JSON or XML.
 */
class JsonXmlFormatter {
public:
    static QString formatIfApplicable(const QString& rawText);
}; 