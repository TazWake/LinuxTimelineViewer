#include "AppWindow.h"
#include "TimelineTab.h"
#include <QMessageBox>

AppWindow::AppWindow(QWidget* parent)
    : QMainWindow(parent)
{
    tabs = new QTabWidget(this);
    setCentralWidget(tabs);
    setupMenu();
    setWindowTitle("Linux Timeline Viewer");
    resize(1200, 800);
}

AppWindow::~AppWindow() {}

void AppWindow::setupMenu()
{
    QMenuBar* menuBar = this->menuBar();
    QMenu* fileMenu = menuBar->addMenu("&File");
    openAction = new QAction("&Open", this);
    exitAction = new QAction("E&xit", this);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    connect(openAction, &QAction::triggered, this, &AppWindow::openFile);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    QMenu* viewMenu = menuBar->addMenu("&View");
    fontIncAction = new QAction("Increase Font Size", this);
    fontDecAction = new QAction("Decrease Font Size", this);
    lineIncAction = new QAction("Increase Line Height", this);
    lineDecAction = new QAction("Decrease Line Height", this);
    resetFontAction = new QAction("Reset Font/Line Height", this);
    viewMenu->addAction(fontIncAction);
    viewMenu->addAction(fontDecAction);
    viewMenu->addSeparator();
    viewMenu->addAction(lineIncAction);
    viewMenu->addAction(lineDecAction);
    viewMenu->addSeparator();
    viewMenu->addAction(resetFontAction);
    connect(fontIncAction, &QAction::triggered, this, &AppWindow::increaseFontSize);
    connect(fontDecAction, &QAction::triggered, this, &AppWindow::decreaseFontSize);
    connect(lineIncAction, &QAction::triggered, this, &AppWindow::increaseLineHeight);
    connect(lineDecAction, &QAction::triggered, this, &AppWindow::decreaseLineHeight);
    connect(resetFontAction, &QAction::triggered, this, &AppWindow::resetFontAndLineHeight);

    QMenu* searchMenu = menuBar->addMenu("&Search");
    searchCurrentTabAction = new QAction("Search in Current Tab...", this);
    searchAllTabsAction = new QAction("Search in All Tabs...", this);
    searchMenu->addAction(searchCurrentTabAction);
    searchMenu->addAction(searchAllTabsAction);
    connect(searchCurrentTabAction, &QAction::triggered, this, &AppWindow::searchInCurrentTab);
    connect(searchAllTabsAction, &QAction::triggered, this, &AppWindow::searchInAllTabs);
}

void AppWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Timeline File", QString(), "Timeline Files (*.csv *.txt)");
    if (fileName.isEmpty())
        return;
    TimelineTab* tab = new TimelineTab(fileName, this);
    tab->setFontSize(currentFontSize);
    tab->setLineHeight(currentLineHeight);
    tabs->addTab(tab, QFileInfo(fileName).fileName());
    tabs->setCurrentWidget(tab);
}

void AppWindow::increaseFontSize() { currentFontSize = qMin(currentFontSize + 1, 32); applyFontAndLineHeight(); }
void AppWindow::decreaseFontSize() { currentFontSize = qMax(currentFontSize - 1, 6); applyFontAndLineHeight(); }
void AppWindow::increaseLineHeight() { currentLineHeight = qMin(currentLineHeight + 2, 80); applyFontAndLineHeight(); }
void AppWindow::decreaseLineHeight() { currentLineHeight = qMax(currentLineHeight - 2, 10); applyFontAndLineHeight(); }
void AppWindow::resetFontAndLineHeight() { currentFontSize = 10; currentLineHeight = 20; applyFontAndLineHeight(); }
void AppWindow::applyFontAndLineHeight() {
    for (int i = 0; i < tabs->count(); ++i) {
        TimelineTab* tab = qobject_cast<TimelineTab*>(tabs->widget(i));
        if (tab) {
            tab->setFontSize(currentFontSize);
            tab->setLineHeight(currentLineHeight);
        }
    }
}

void AppWindow::searchInCurrentTab() { showSearchDialog(false); }
void AppWindow::searchInAllTabs() { showSearchDialog(true); }

void AppWindow::showSearchDialog(bool allTabs)
{
    // Gather columns
    QStringList allColumns;
    if (allTabs) {
        QSet<QString> colSet;
        for (int i = 0; i < tabs->count(); ++i) {
            TimelineTab* tab = qobject_cast<TimelineTab*>(tabs->widget(i));
            if (tab) colSet.unite(QSet<QString>(tab->columnNames().begin(), tab->columnNames().end()));
        }
        allColumns = QStringList(colSet.begin(), colSet.end());
    } else {
        TimelineTab* tab = qobject_cast<TimelineTab*>(tabs->currentWidget());
        if (tab) allColumns = tab->columnNames();
    }
    allColumns.removeAll("");
    allColumns.sort();
    allColumns.prepend("All Columns");

    // Dialog
    QDialog dialog(this);
    dialog.setWindowTitle(allTabs ? "Search in All Tabs" : "Search in Current Tab");
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QLabel* label = new QLabel("Search term:", &dialog);
    QLineEdit* input = new QLineEdit(&dialog);
    QLabel* colLabel = new QLabel("Column:", &dialog);
    QComboBox* colPicker = new QComboBox(&dialog);
    colPicker->addItems(allColumns);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(label);
    layout->addWidget(input);
    layout->addWidget(colLabel);
    layout->addWidget(colPicker);
    layout->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString term = input->text();
    QString col = colPicker->currentText();
    if (term.isEmpty()) return;

    int totalMatches = 0;
    int firstTabWithMatch = -1;
    int firstRow = -1;
    for (int i = 0; i < tabs->count(); ++i) {
        TimelineTab* tab = qobject_cast<TimelineTab*>(tabs->widget(i));
        if (tab) {
            bool found = tab->search(col, term);
            if (found) {
                if (firstTabWithMatch == -1) {
                    firstTabWithMatch = i;
                    firstRow = 0; // Could be improved to select first match
                }
                totalMatches++;
            }
        }
    }
    if (totalMatches > 0 && firstTabWithMatch != -1) {
        tabs->setCurrentIndex(firstTabWithMatch);
        // Optionally, scroll/select first match
        statusBar()->showMessage(QString("%1 tab(s) matched for '%2'.").arg(totalMatches).arg(term));
    } else {
        statusBar()->showMessage("No matches found.");
    }
} 