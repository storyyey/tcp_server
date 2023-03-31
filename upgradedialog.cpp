#include "upgradedialog.h"
#include "mainwindow.h"

UpgradeDialog::UpgradeDialog()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QString thisStyle\
            = "QPushButton:hover {\
                  background-color: #BDBDBD;\
                  border-radius: 10px;\
                }"\
                "QPushButton:pressed {\
                   background-color: #8F8F8F;\
                   border-radius: 10px;\
                 }";
    thisStyle += "QPushButton {\
                   background-color: transparent;\
                   height:30px;\
                 }";
    thisStyle += "QLineEdit {\
                    border: 2px solid #D4D4D4;\
                    border-radius: 4px;\
                    height: 30px;\
                   }\
                   QLineEdit:focus{\
                    border: 3px solid #63B8FF;\
                   }";
    thisStyle += "QTableView {\
                  border: 3px solid #FFFFFF;\
                  border-radius: 4px;\
                 }";
    thisStyle += "QDialog {\
                  background-color: #E0E0E0;\
                  }";
    this->setStyleSheet(thisStyle);
    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle("设备升级");
    this->setLayout(mainLayout);
    //this->setFixedWidth(480);
    //upgradeDialog->setFixedSize(QSize(480, 240));
    this->resize(1200, 600);
    QHBoxLayout *snLayout = new QHBoxLayout();
    QLabel *snLabel = new QLabel("序列号  :");
    snLayout->addWidget(snLabel);
    upgradeSN = new QLineEdit();
    upgradeSN->setText("123456789");
    upgradeSN->setEnabled(false);
    snLayout->addWidget(upgradeSN);
    mainLayout->addLayout(snLayout);

    QHBoxLayout *verLayout = new QHBoxLayout();
    QLabel *verLabel = new QLabel("当前版本:");
    verLayout->addWidget(verLabel);
    currVersion = new QLineEdit();
    currVersion->setText("000000000000");
    currVersion->setEnabled(false);
    verLayout->addWidget(currVersion);
    mainLayout->addLayout(verLayout);

    QHBoxLayout *modelLayout = new QHBoxLayout();
    QLabel *modelLabel = new QLabel("设备型号:");
    modelLayout->addWidget(modelLabel);
    devModel = new QLineEdit();
    devModel->setText("设备类型");
    devModel->setEnabled(false);
    modelLayout->addWidget(devModel);
    mainLayout->addLayout(modelLayout);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    QLabel *fileName = new QLabel("文件名  :");
    fileLayout->addWidget(fileName);
    fileSelectEdit = new QLineEdit();
    fileSelectEdit->setPlaceholderText("未选择升级文件...");
    fileLayout->addWidget(fileSelectEdit);
    mainLayout->addLayout(fileLayout);
    //connect(fileSelectBtn, SIGNAL(clicked()), this, SLOT(fileSelectPopDialogSlot()));

    QHBoxLayout *md5Laout = new QHBoxLayout();
    QLabel *md5Label = new QLabel("md5     :");
    md5Laout->addWidget(md5Label);
    md5Edit = new QLineEdit();
    md5Edit->setToolTip("md5");
    md5Laout->addWidget(md5Edit);
    mainLayout->addLayout(md5Laout);

    QHBoxLayout *tvLaout = new QHBoxLayout();
    QLabel *tvLabel = new QLabel("目标版本:");
    tvLaout->addWidget(tvLabel);
    targetVer = new QLineEdit();
    targetVer->setToolTip("目标版本");
    tvLaout->addWidget(targetVer);
    mainLayout->addLayout(tvLaout);

    QHBoxLayout *uptimeLaout = new QHBoxLayout();
    QLabel *uptimeLabel = new QLabel("上传时间:");
    uptimeLaout->addWidget(uptimeLabel);
    uptimeEdit = new QLineEdit();
    uptimeEdit->setToolTip("上传时间");
    uptimeLaout->addWidget(uptimeEdit);
    mainLayout->addLayout(uptimeLaout);

    QHBoxLayout *sizeLaout = new QHBoxLayout();
    QLabel *sizeLabel = new QLabel("文件大小:");
    sizeLaout->addWidget(sizeLabel);
    sizeEdit = new QLineEdit();
    sizeEdit->setToolTip("文件大小");
    sizeLaout->addWidget(sizeEdit);
    mainLayout->addLayout(sizeLaout);

    QHBoxLayout *remarkLaout = new QHBoxLayout();
    QLabel *remarkEditLabel = new QLabel("备注信息:");
    remarkLaout->addWidget(remarkEditLabel);
    remarkEdit = new QLineEdit();
    remarkEdit->setToolTip("备注信息");
    remarkLaout->addWidget(remarkEdit);
    mainLayout->addLayout(remarkLaout);
    //mainLayout->addStretch();

    fileodel = new QStandardItemModel();

    fileView = new QTableView();
    fileView->setModel(fileodel);
    fileView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    fileView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fileView->setAlternatingRowColors(true);
    fileView->setSelectionMode(QAbstractItemView::SingleSelection);
    fileView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(fileView, SIGNAL(pressed(QModelIndex)), this, SLOT(tableSelectRow(QModelIndex)));
    //fileView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mainLayout->addWidget(fileView);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    upgradeAgreeBtn = new QPushButton("确认/升级");
    upgradeAgreeBtn->setIcon(QIcon(":/icon/confirm.png"));
    btnLayout->addWidget(upgradeAgreeBtn);
    upgradeAgreeBtn->setFixedWidth(200);
    upgradeCancelBtn = new QPushButton("取消/关闭");
    upgradeCancelBtn->setIcon(QIcon(":/icon/cancel.png"));
    btnLayout->addWidget(upgradeCancelBtn);
    upgradeCancelBtn->setFixedWidth(200);
    mainLayout->addLayout(btnLayout);

    connect(upgradeAgreeBtn, SIGNAL(clicked()), this, SLOT(upgradeAgreeBtnSlot()));
    connect(upgradeCancelBtn, SIGNAL(clicked()), this, SLOT(upgradeCancelBtnSlot()));
}

void UpgradeDialog::fileSelectPopDialogSlot()
{
    QString fileFull = QFileDialog::getOpenFileName(this, tr("升级文件选择"));

    QFileInfo fileInfo(fileFull);
    QString fileName = fileInfo.fileName();
    filePath = fileInfo.absolutePath();
    fileSelectEdit->setText(fileName);
}

void UpgradeDialog::upgradeAgreeBtnSlot()
{
    QString info = QString("序列号:<font color = blue><u>%1</u></font><br>"\
                           "设备型号:<font color = blue><u>%2</u></font><br>"\
                           "当前版本:<font color = blue><u>%3</u></font><br>"\
                           "目标版本:<font color = blue><u>%4</u></font><br>"\
                           "升级文件:<font color = blue><u>%5</u></font>")\
                            .arg(upgradeSN->text())\
                            .arg(devModel->text())\
                            .arg(currVersion->text())\
                            .arg(targetVer->text())\
                            .arg(fileSelectEdit->text());


    int ret = QMessageBox::information(NULL, "确认", info,\
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (QMessageBox::Yes == ret) {
        /* 保存升级操作记录 */
        updata.file = fileSelectEdit->text();
        updata.remark = QString("升级文件为:" + updata.file + "[" + updata.md5 + "]");
        updata.sn = upgradeSN->text();
        updata.model = devModel->text();
        updata.old_ver = currVersion->text();
        updata.new_ver = targetVer->text();
        updata.state = Devices::statusText(Devices::upgradeWait);
        updata.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        updata.filePath = filePath;
        emit newUpgradeSignal(updata);
        this->close();
    }
}

void UpgradeDialog::upgradeCancelBtnSlot()
{
    this->close();
}

void UpgradeDialog::appendFile(const Devices::fileStore &val)
{
    QList<QStandardItem *> itemList;

    QStandardItem *timeItem = new QStandardItem(val.time);
    timeItem->setToolTip(val.time);
    itemList.append(timeItem);

    QStandardItem *nameItem = new QStandardItem(val.name);
    nameItem->setToolTip(val.name);
    itemList.append(nameItem);

    QStandardItem *md5Item = new QStandardItem(val.md5);
    md5Item->setToolTip(val.md5);
    itemList.append(md5Item);

    itemList.append(new QStandardItem(QString("%1字节").arg(val.size)));
    itemList.append(new QStandardItem(val.version));
    itemList.append(new QStandardItem(val.model));
    QStandardItem *remarkItem = new QStandardItem(val.remark);
    remarkItem->setToolTip(val.remark);
    itemList.append(remarkItem);

    if (devModel->text() == val.model) {
        for (int n = 0; n < itemList.size(); n++) {
            QStandardItem *item = itemList.at(n);
            if (item) {
                item->setBackground(QBrush(QColor("#4EEE94")));
            }
        }
    }

    fileodel->insertRow(0, itemList);
}

void UpgradeDialog::tableSelectRow(const QModelIndex &index)
{
    windowConsole(QString("当前选中行%1").arg(index.row()));

    QStandardItem *item = fileodel->item(index.row(), 1);
    if (item) {
        fileSelectEdit->setText(item->text());
    }

    item = fileodel->item(index.row(), 4);
    if (item) {
        QString tv = item->text();
        if (tv.size()) {
            targetVer->setText(tv);
        }
        else {
            targetVer->setText(currVersion->text());
        }
    }

    item = fileodel->item(index.row(), 2);
    if (item) {
        updata.md5 = item->text();
        md5Edit->setText(item->text());
    }

    item = fileodel->item(index.row(), 3);
    if (item) {
        sizeEdit->setText(item->text());
    }

    item = fileodel->item(index.row(), 6);
    if (item) {
        remarkEdit->setText(item->text());
    }

    item = fileodel->item(index.row(), 0);
    if (item) {
        uptimeEdit->setText(item->text());
    }

    item = fileodel->item(index.row(), 5);
    if (item) {
        if (item->text().size() && \
                devModel->text() != item->text()) {
              switch (QMessageBox::warning(NULL, tr("文件选择"), \
                                         QString("所选升级文件设备型号(<font color = red><u>%1</u></font>)\
                                                  与当前设备型号(<font color = red><u>%2</u></font>)不一致!，是否继续使用？")\
                                            .arg(item->text(), devModel->text()),\
                                         tr("是"), tr("否"), 0, 1)) {
              case 1: lineEditClear(); return ;
              case 0: /* ignore */
              default: break;
            }
        }
    }
}

void UpgradeDialog::popUp()
{
    windowConsole("弹出升级窗口");
    fileodel->clear();
    fileodel->setColumnCount(fileInfo.length());
    fileodel->setHorizontalHeaderLabels(fileInfo);
    QList<Devices::fileStore> &filesStore = DeviceDB::getFileStore();
    for (int index = 0; index < filesStore.count(); index++) {
        appendFile(filesStore.at(index));
    }
    this->exec();
    windowConsole("关闭升级窗口");
}

void UpgradeDialog::setModel(const QString &model)
{
    devModel->setText(model);
}

void UpgradeDialog::setSn(const QString &sn)
{
    upgradeSN->setText(sn);
}

void UpgradeDialog::setCurrVersion(const QString &ver)
{
    currVersion->setText(ver);
    if (!targetVer->text().size()) {
        targetVer->setText(ver);
    }
}

void UpgradeDialog::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->globalPos();
    isPressedDialog = true;
}

void UpgradeDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressedDialog) {
        this->move(this->x() + (event->globalX() - m_lastPos.x()),
                    this->y() + (event->globalY() - m_lastPos.y()));
        m_lastPos = event->globalPos();
    }
}

void UpgradeDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_lastPos = event->globalPos();
    isPressedDialog = false;
}

void UpgradeDialog::lineEditClear()
{
    fileSelectEdit->clear();
    md5Edit->clear();
    uptimeEdit->clear();
    sizeEdit->clear();
    remarkEdit->clear();
}

