#include "TimelineModel.h"
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

TimelineModel::TimelineModel(const QString& filePath, QObject* parent)
    : QAbstractTableModel(parent), filePath(filePath), timelineType(Unknown), file(filePath)
{
    detectFormat();
    buildLineIndex();
}

TimelineModel::~TimelineModel() { file.close(); }

void TimelineModel::detectFormat()
{
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        timelineType = Unknown;
        return;
    }
    QTextStream in(&file);
    QString headerLine = in.readLine();
    headers = headerLine.split(",");
    if (headers == QStringList({"Date","Size","Type","Mode","UID","GID","Meta","File Name"}))
        timelineType = Filesystem;
    else if (headers == QStringList({"datetime","timestamp_desc","source","source_long","message","parser","display_name","tag"}))
        timelineType = Super;
    else
        timelineType = Unknown;
    file.seek(0);
}

void TimelineModel::buildLineIndex()
{
    lineOffsets.clear();
    if (!file.isOpen()) file.open(QIODevice::ReadOnly | QIODevice::Text);
    qint64 offset = 0;
    QTextStream in(&file);
    QString line;
    // Skip header
    offset += in.readLine().toUtf8().size() + 1;
    while (!in.atEnd()) {
        lineOffsets.append(offset);
        line = in.readLine();
        offset += line.toUtf8().size() + 1;
    }
    file.seek(0);
}

int TimelineModel::rowCount(const QModelIndex&) const
{
    return lineOffsets.size();
}

int TimelineModel::columnCount(const QModelIndex&) const
{
    return headers.size();
}

QVariant TimelineModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    if (!file.isOpen()) file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (index.row() < 0 || index.row() >= lineOffsets.size())
        return QVariant();
    file.seek(lineOffsets[index.row()]);
    QByteArray line = file.readLine();
    QList<QByteArray> fields = line.trimmed().split(',');
    if (index.column() < 0 || index.column() >= fields.size())
        return QVariant();
    return QString::fromUtf8(fields[index.column()]);
}

QVariant TimelineModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();
    if (section < 0 || section >= headers.size())
        return QVariant();
    return headers[section];
}

int TimelineModel::columnIndex(const QString& name) const
{
    return headers.indexOf(name);
}

TimelineModel::TimelineType TimelineModel::type() const
{
    return timelineType;
} 