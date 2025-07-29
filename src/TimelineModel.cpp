#include "TimelineModel.h"
#include "utils/JsonXmlFormatter.h"
#include "utils/FileUtils.h"
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

TimelineModel::TimelineModel(const QString& filePath, QObject* parent)
    : QAbstractTableModel(parent), filePath(filePath), timelineType(Unknown), file(filePath), unsavedChanges(false)
{
    detectFormat();
    buildLineIndex();
    loadTaggedRows();
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
    headers = FileUtils::parseCsvLine(headerLine);
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
    if (!index.isValid())
        return QVariant();
    
    // Handle tag column for Super timelines
    if (timelineType == Super && index.column() == 7) { // tag column
        if (role == Qt::CheckStateRole) {
            return taggedRows.contains(index.row()) ? Qt::Checked : Qt::Unchecked;
        }
        if (role == Qt::DisplayRole) {
            return QVariant(); // Don't show text for checkbox column
        }
    }
    
    // Handle background color for tagged rows
    if (role == Qt::BackgroundRole && taggedRows.contains(index.row())) {
        return QColor(240, 240, 240); // Light gray background for tagged rows
    }
    
    if (role != Qt::DisplayRole)
        return QVariant();
        
    if (!file.isOpen()) file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (index.row() < 0 || index.row() >= lineOffsets.size())
        return QVariant();
    file.seek(lineOffsets[index.row()]);
    QString line = QString::fromUtf8(file.readLine().trimmed());
    QStringList fields = FileUtils::parseCsvLine(line);
    if (index.column() < 0 || index.column() >= fields.size())
        return QVariant();
    
    QString cellData = fields[index.column()];
    
    // Apply JSON/XML formatting for message field in Super timelines
    if (timelineType == Super && index.column() == 4) { // message field
        return JsonXmlFormatter::formatIfApplicable(cellData);
    }
    
    return cellData;
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

bool TimelineModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || timelineType != Super || index.column() != 7)
        return false;
        
    if (role == Qt::CheckStateRole) {
        bool checked = value.toInt() == Qt::Checked;
        setRowTagged(index.row(), checked);
        return true;
    }
    return false;
}

Qt::ItemFlags TimelineModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
        
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    // Make tag column checkable for Super timelines
    if (timelineType == Super && index.column() == 7) {
        flags |= Qt::ItemIsUserCheckable;
    }
    
    return flags;
}

TimelineModel::TimelineType TimelineModel::type() const
{
    return timelineType;
}

bool TimelineModel::isRowTagged(int row) const
{
    return taggedRows.contains(row);
}

void TimelineModel::setRowTagged(int row, bool tagged)
{
    if (tagged) {
        if (!taggedRows.contains(row)) {
            taggedRows.insert(row);
            unsavedChanges = true;
            emit dataChanged(createIndex(row, 0), createIndex(row, columnCount() - 1));
            emit dataChanged(unsavedChanges);
        }
    } else {
        if (taggedRows.contains(row)) {
            taggedRows.remove(row);
            unsavedChanges = true;
            emit dataChanged(createIndex(row, 0), createIndex(row, columnCount() - 1));
            emit dataChanged(unsavedChanges);
        }
    }
}

bool TimelineModel::hasUnsavedChanges() const
{
    return unsavedChanges;
}

bool TimelineModel::saveTaggedRows()
{
    QString tagFilePath = getTagFilePath();
    QFile tagFile(tagFilePath);
    
    if (!tagFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open tag file for writing:" << tagFilePath;
        return false;
    }
    
    QTextStream out(&tagFile);
    for (int row : taggedRows) {
        out << row << "\n";
    }
    
    unsavedChanges = false;
    emit dataChanged(unsavedChanges);
    return true;
}

QString TimelineModel::getFilePath() const
{
    return filePath;
}

void TimelineModel::loadTaggedRows()
{
    QString tagFilePath = getTagFilePath();
    QFile tagFile(tagFilePath);
    
    if (!tagFile.exists() || !tagFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return; // No tag file exists, start with empty set
    }
    
    QTextStream in(&tagFile);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        bool ok;
        int row = line.toInt(&ok);
        if (ok && row >= 0 && row < lineOffsets.size()) {
            taggedRows.insert(row);
        }
    }
}

QString TimelineModel::getTagFilePath() const
{
    QFileInfo fileInfo(filePath);
    QString tagFileName = fileInfo.completeBaseName() + ".tags";
    return fileInfo.absolutePath() + QDir::separator() + tagFileName;
} 