#pragma once
#include <QWidget>
#include <QTableView>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include "FilterBar.h"
#include "TimelineModel.h"

/**
 * @brief TimelineTab represents a single tab with a loaded timeline file.
 */
class TimelineTab : public QWidget {
    Q_OBJECT
public:
    TimelineTab(const QString& filePath, QWidget* parent = nullptr);
    ~TimelineTab();

private slots:
    void onSearchRequested(const QString& column, const QString& term);

private:
    FilterBar* filterBar;
    QTableView* tableView;
    QStatusBar* statusBar;
    TimelineModel* model;
    QSortFilterProxyModel* proxyModel;
    void updateStatus();
}; 