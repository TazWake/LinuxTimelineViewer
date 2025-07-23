#pragma once
#include <QAbstractTableModel>
#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <memory>

/**
 * @brief TimelineModel is a QAbstractTableModel backed by a timeline CSV file.
 */
class TimelineModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum TimelineType {
        Filesystem,
        Super,
        Unknown
    };
    TimelineModel(const QString& filePath, QObject* parent = nullptr);
    ~TimelineModel();
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnIndex(const QString& name) const;
    TimelineType type() const;

private:
    QString filePath;
    TimelineType timelineType;
    QStringList headers;
    QVector<qint64> lineOffsets; // File offsets for each line
    mutable QFile file;
    void detectFormat();
    void buildLineIndex();
}; 