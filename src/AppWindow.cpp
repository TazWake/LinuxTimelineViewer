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
}

void AppWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Timeline File", QString(), "Timeline Files (*.csv *.txt)");
    if (fileName.isEmpty())
        return;
    TimelineTab* tab = new TimelineTab(fileName, this);
    tabs->addTab(tab, QFileInfo(fileName).fileName());
    tabs->setCurrentWidget(tab);
} 