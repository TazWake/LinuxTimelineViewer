#include "TimelineTab.h"
#include <QHeaderView>
#include <QFont>

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
    updateFilterBarColumns();
    updateStatus();
}

TimelineTab::~TimelineTab() {}

void TimelineTab::updateFilterBarColumns()
{
    QStringList cols = model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toStringList();
    if (cols.isEmpty())
        cols = model->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString().split(",");
    if (cols.isEmpty())
        cols = model->columnCount() > 0 ? QStringList() : QStringList();
    for (int i = 0; i < model->columnCount(); ++i)
        cols << model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
    cols.removeDuplicates();
    cols.removeAll("");
    cols.prepend("All Columns");
    filterBar->setColumns(cols);
}

QStringList TimelineTab::columnNames() const
{
    QStringList cols;
    for (int i = 0; i < model->columnCount(); ++i)
        cols << model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
    return cols;
}

void TimelineTab::onSearchRequested(const QString& column, const QString& term)
{
    bool found = search(column, term);
    if (!found) {
        updateStatus("No matches found.");
    } else {
        updateStatus();
    }
}

bool TimelineTab::search(const QString& column, const QString& term)
{
    if (term.isEmpty()) {
        proxyModel->setFilterFixedString("");
        return false;
    }
    if (column == "All Columns") {
        proxyModel->setFilterKeyColumn(-1);
        proxyModel->setFilterRegExp(QRegExp(term, Qt::CaseInsensitive, QRegExp::FixedString));
        return proxyModel->rowCount() > 0;
    } else {
        int col = model->columnIndex(column);
        if (col >= 0) {
            proxyModel->setFilterKeyColumn(col);
            proxyModel->setFilterFixedString(term);
            return proxyModel->rowCount() > 0;
        }
    }
    return false;
}

void TimelineTab::setFontSize(int pointSize)
{
    fontSize = pointSize;
    QFont f = tableView->font();
    f.setPointSize(pointSize);
    tableView->setFont(f);
}

void TimelineTab::setLineHeight(int px)
{
    lineHeight = px;
    tableView->verticalHeader()->setDefaultSectionSize(px);
}

void TimelineTab::updateStatus(const QString& msg)
{
    if (!msg.isEmpty()) {
        statusBar->showMessage(msg);
    } else {
        statusBar->showMessage(QString("Rows: %1").arg(proxyModel->rowCount()));
    }
} 