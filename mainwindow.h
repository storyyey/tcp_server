#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QTableView>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QToolBar>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDateTime>
#include <QtHttpServer/qhttpserver.h>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLocalSocket>
#include <QLocalServer>
#include <QCloseEvent>
#include "actionbar.h"
#include "debugwidget.h"
#include "tables.h"

extern DebugWidget *g_debugWidget;

inline void windowConsole(const QString &str)
{
    if (g_debugWidget) {
        g_debugWidget->print(str);
    }
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent * event);
    void helpMenuWidgetInit();

    static Devices *devices;
private:
    ActionBar *actionBar;
    DebugWidget *debugWidget;
    Tables *deviceTableWidget;
    DeviceDB *DDB;
    QDialog *saDialog;
signals:

private slots:
    void helpMenuConfigAction();
};

#endif // MAINWINDOW_H
