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

void AppWindow::increaseFontSize()
{
    currentFontSize = qMin(currentFontSize + 1, 32);
    applyFontAndLineHeight();
}

void AppWindow::decreaseFontSize()
{
    currentFontSize = qMax(currentFontSize - 1, 6);
    applyFontAndLineHeight();
}

void AppWindow::increaseLineHeight()
{
    currentLineHeight = qMin(currentLineHeight + 2, 80);
    applyFontAndLineHeight();
}

void AppWindow::decreaseLineHeight()
{
    currentLineHeight = qMax(currentLineHeight - 2, 10);
    applyFontAndLineHeight();
}

void AppWindow::resetFontAndLineHeight()
{
    currentFontSize = 10;
    currentLineHeight = 20;
    applyFontAndLineHeight();
}

void AppWindow::applyFontAndLineHeight()
{
    for (int i = 0; i < tabs->count(); ++i) {
        TimelineTab* tab = qobject_cast<TimelineTab*>(tabs->widget(i));
        if (tab) {
            tab->setFontSize(currentFontSize);
            tab->setLineHeight(currentLineHeight);
        }
    }
} 