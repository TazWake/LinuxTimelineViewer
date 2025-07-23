#include "TimelineTab.h"
#include <QHeaderView>

TimelineTab::TimelineTab(const QString& filePath, QWidget* parent)
    : QWidget(parent)
{
    filterBar = new FilterBar(this);
    model = new TimelineModel(filePath, this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    tableView = new QTableView(this);
    tableView->setModel(proxyModel);
    tableView->setSortingEnabled(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    statusBar = new QStatusBar(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(filterBar);
    layout->addWidget(tableView);
    layout->addWidget(statusBar);
    setLayout(layout);
    connect(filterBar, &FilterBar::searchRequested, this, &TimelineTab::onSearchRequested);
    updateStatus();
}

TimelineTab::~TimelineTab() {}

void TimelineTab::onSearchRequested(const QString& column, const QString& term)
{
    int col = model->columnIndex(column);
    if (col >= 0) {
        proxyModel->setFilterKeyColumn(col);
        proxyModel->setFilterFixedString(term);
    }
    updateStatus();
}

void TimelineTab::updateStatus()
{
    statusBar->showMessage(QString("Rows: %1").arg(proxyModel->rowCount()));
} 