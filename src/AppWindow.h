#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>

class TimelineTab;

/**
 * @brief The AppWindow class is the main application window with tabbed interface and menu.
 */
class AppWindow : public QMainWindow {
    Q_OBJECT
public:
    AppWindow(QWidget* parent = nullptr);
    ~AppWindow();

private slots:
    void openFile();

private:
    QTabWidget* tabs;
    QAction* openAction;
    QAction* exitAction;
    void setupMenu();
}; 