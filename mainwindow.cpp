#include "mainwindow.h"

DebugWidget *g_debugWidget;

Devices *MainWindow::devices = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QDesktopWidget dw;
    resize(QSize((dw.width() / 4) * 3, (dw.height() / 4) * 3));

    this->setStyleSheet("QMainWindow{background-color: #4D4D4D;}");
    g_debugWidget = debugWidget = new DebugWidget();

    devices = new Devices();
    DDB = new DeviceDB();
    connect(devices, &Devices::infoUpdate, this, [](Devices::devInfo &data){
        DeviceDB::save(data);
    });

    QDockWidget *debugDock = new QDockWidget("调试窗口", this);
    debugDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    debugDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    debugDock->setWidget(debugWidget);
    addDockWidget(Qt::BottomDockWidgetArea, debugDock);
    debugDock->close();

    actionBar = new ActionBar();
    QDockWidget *actionDock = new QDockWidget("运行", this);
    actionDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    actionDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    actionDock->setWidget(actionBar);
    QWidget* lTitleBar = actionDock->titleBarWidget();
    QLabel *emtryLabel = new QLabel();
    actionDock->setTitleBarWidget(emtryLabel);
    delete lTitleBar;
    addDockWidget(Qt::LeftDockWidgetArea, actionDock);

    deviceTableWidget = new Tables();
    setCentralWidget(deviceTableWidget);

    QToolBar *toolbar = new QToolBar("工具(S)");
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(Qt::TopToolBarArea, toolbar);
    toolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolbar->setFloatable(false);
    toolbar->close();

    QMenuBar *menuBr = menuBar();
    menuBr->setStyleSheet(".QMenuBar{background-color: rgb(220,220,220);}");
    menuBr->addMenu("文件(F)");
    menuBr->addMenu("工具(T)");
    QMenu *helpFile = menuBr->addMenu("帮助(H)");
    QAction *startAction = helpFile->addAction("启动");
    setMenuBar(menuBr);
    helpMenuWidgetInit();
    connect(startAction, SIGNAL(triggered()), this, SLOT(helpMenuConfigAction()));
}
MainWindow::~MainWindow()
{
    delete devices;
    delete DDB;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
   switch (QMessageBox::information(this, tr("退出程序"), tr("确认退出?"), tr("是"), tr("否"), 0, 1 ))
   {
     case 0:
          event->accept();
          break;
     case 1:
     default:
         event->ignore();
         break;
   }
}

void MainWindow::helpMenuWidgetInit()
{
    saDialog = new QDialog(this);
    saDialog->setWindowTitle("配置信息");
    QVBoxLayout *mainLayout = new QVBoxLayout();
    saDialog->setLayout(mainLayout);

    QLabel *exLabel = new QLabel(saDialog);
    exLabel->setText("幼儿园LORA网关配置=> 10.200.67.222:4998\n"\
                     "其他LORA网关配置=> 14.29.249.101:9981");
    mainLayout->addWidget(exLabel);
}

void MainWindow::helpMenuConfigAction()
{
    saDialog->show();
}
