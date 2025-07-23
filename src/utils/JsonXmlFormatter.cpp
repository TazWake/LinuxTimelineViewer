#include "JsonXmlFormatter.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDomDocument>

QString JsonXmlFormatter::formatIfApplicable(const QString& rawText)
{
    // Try JSON
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawText.toUtf8(), &jsonError);
    if (jsonError.error == QJsonParseError::NoError && !jsonDoc.isNull()) {
        return jsonDoc.toJson(QJsonDocument::Indented);
    }
    // Try XML
    QDomDocument xmlDoc;
    QString xmlErrMsg;
    int xmlErrLine, xmlErrCol;
    if (xmlDoc.setContent(rawText, &xmlErrMsg, &xmlErrLine, &xmlErrCol)) {
        return xmlDoc.toString(2);
    }
    // Not JSON or XML
    return rawText;
} 