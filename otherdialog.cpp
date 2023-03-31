#include "otherdialog.h"
#include <QtDebug>

OtherDialog::OtherDialog()
{
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
                  width: 600px;\
                  height: 30px;\
                  }\
                  QLineEdit:focus{\
                     border: 3px solid #63B8FF;\
                  }";
    thisStyle += "QListWidget {\
                 background-color: transparent; \
                 border: 1px solid #D4D4D4;\
                 border-radius: 2px;\
                 outline:0px;\
                }";
    thisStyle += "QListWidget::item {\
                 border-radius: 8px;\
                 height: 50px;\
                 width: 80px;\
                }";
    thisStyle += "QComboBox {\
                border: 2px solid #D4D4D4;\
                border-radius: 4px;\
                width: 600px;\
                height: 30px;\
               }\
               QComboBox:focus{\
                   border: 3px solid #63B8FF;\
                }";

    this->setStyleSheet(thisStyle);
    this->setWindowTitle("更多功能");
    mainLayout = new QHBoxLayout(this);
    this->setLayout(mainLayout);

    listWidget = new QListWidget();
    listWidget->setFixedWidth(120);
    mainLayout->addWidget(listWidget);
    connect(listWidget, &QListWidget::currentRowChanged, this, [this] (int index) {
        mainStackWidget->setCurrentIndex(index);
    });
    mainStackWidget = new QStackedWidget();
    mainLayout->addWidget(mainStackWidget);

    uploadFileWdiget();
    presetIpWidget();
}

void OtherDialog::uploadFileWdiget()
{
    QGridLayout *mLayout = new QGridLayout();
    //mainLayout->addLayout(mLayout);
    QWidget *mWidget = new QWidget();
    mainStackWidget->addWidget(mWidget);
    listWidget->addItem("文件上传");
    listWidget->item(listWidget->count() - 1)->setIcon(QIcon(":/icon/diricon.png"));
    mWidget->setLayout(mLayout);
    fileSelectBtn = new QPushButton();
    fileSelectBtn->setToolTip("选择升级文件");
    fileSelectBtn->setIcon(QIcon(":/icon/diricon.png"));
    //fileSelectBtn->setIconSize(QSize(70, 40));
    mLayout->addWidget(fileSelectBtn, 0, 0);
    fileSelectEdit = new QLineEdit();
    fileSelectEdit->setPlaceholderText("未选择上传文件...");
    mLayout->addWidget(fileSelectEdit, 0, 1);

    QLabel *fileMd5Label = new QLabel("MD5:");
    mLayout->addWidget(fileMd5Label, 1, 0);
    fileMd5Edit = new QLineEdit();
    fileMd5Edit->setPlaceholderText("自动计算勿修改");
    mLayout->addWidget(fileMd5Edit, 1, 1);

    QLabel *fileVersionLabel = new QLabel("版本:");
    mLayout->addWidget(fileVersionLabel, 2, 0);
    fileVersionEdit = new QLineEdit();
    fileVersionEdit->setPlaceholderText("升级文件版本(可选添加)");
    mLayout->addWidget(fileVersionEdit, 2, 1);

    QLabel *fileModelLabel = new QLabel("型号:");
    mLayout->addWidget(fileModelLabel, 3, 0);
    fileModelEdit = new QLineEdit();
    fileModelEdit->setPlaceholderText("适用设备型号(可选添加)");
    mLayout->addWidget(fileModelEdit, 3, 1);

    QLabel *fileRemarkLabel = new QLabel("备注:");
    mLayout->addWidget(fileRemarkLabel, 4, 0);
    fileRemarkEdit = new QLineEdit();
    fileRemarkEdit->setPlaceholderText("文件备注信息(可选添加)");
    mLayout->addWidget(fileRemarkEdit, 4, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    confirmBtn = new QPushButton("确认/上传");
    confirmBtn->setIcon(QIcon(":/icon/confirm.png"));
    //confirmBtn->setFixedSize(100, 40);
    btnLayout->addWidget(confirmBtn);
    cancelBtn = new QPushButton("取消/关闭");
    cancelBtn->setIcon(QIcon(":/icon/cancel.png"));
    //cancelBtn->setFixedSize(100, 40);
    btnLayout->addWidget(cancelBtn);
    mLayout->addLayout(btnLayout, 5, 0, 1, 2);

    connect(confirmBtn, SIGNAL(clicked()), this, SLOT(confirmUpload()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(cancelUpload()));
    connect(fileSelectBtn, SIGNAL(clicked()), this, SLOT(fileSelectPopDialogSlot()));
}

void OtherDialog::presetIpWidget()
{
    QWidget *mWidget = new QWidget();
    QGridLayout *mLayout = new QGridLayout();
    mWidget->setLayout(mLayout);
    mainStackWidget->addWidget(mWidget);
    listWidget->addItem("IP预设置");
    listWidget->item(listWidget->count() - 1)->setIcon(QIcon(":/icon/networkset.png"));

    QRegExp regExp("((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})(\\.((2(5[0-5]|[0-4]\\d))|[0-1]?\\d{1,2})){3}");
    QValidator *validator = new QRegExpValidator(regExp, this);

    QLabel *poslabel = new QLabel("位置:");
    mLayout->addWidget(poslabel, 0, 0);
    presetPosBox = new QComboBox();
    presetPosBox->setEditable(true);
    presetPos = presetPosBox->lineEdit();
    presetPos->setPlaceholderText("点击下拉选择历史记录位置");
    //presetPos->setValidator(validator);
    mLayout->addWidget(presetPosBox, 0, 1);

    QLabel *iplabel = new QLabel("IP:");
    mLayout->addWidget(iplabel, 1, 0);
    presetIp = new QLineEdit();
    presetIp->setPlaceholderText("例:192.168.1.2");
    presetIp->setValidator(validator);
    mLayout->addWidget(presetIp, 1, 1);

    QLabel *masklabel = new QLabel("掩码:");
    mLayout->addWidget(masklabel, 2, 0);
    presetMask = new QLineEdit();
    presetMask->setPlaceholderText("例:255.255.255.0");
    presetMask->setValidator(validator);
    mLayout->addWidget(presetMask, 2, 1);

    QLabel *gatewaylabel = new QLabel("网关:");
    mLayout->addWidget(gatewaylabel, 3, 0);
    presetGateway = new QLineEdit();
    presetGateway->setPlaceholderText("例:192.168.1.1");
    presetGateway->setValidator(validator);
    mLayout->addWidget(presetGateway, 3, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    preseTconfirmBtn = new QPushButton("确认/配置");
    preseTconfirmBtn->setIcon(QIcon(":/icon/confirm.png"));
    //preseTconfirmBtn->setFixedSize(100, 40);
    btnLayout->addWidget(preseTconfirmBtn);
    preseTcancelBtn = new QPushButton("取消/关闭");
    preseTcancelBtn->setIcon(QIcon(":/icon/cancel.png"));
    //preseTcancelBtn->setFixedSize(100, 40);
    btnLayout->addWidget(preseTcancelBtn);
    mLayout->addLayout(btnLayout, 4, 0, 1, 2);

    connect(preseTconfirmBtn, SIGNAL(clicked()), this, SLOT(preseTconfirm()));
    connect(preseTcancelBtn, SIGNAL(clicked()), this, SLOT(preseTcancel()));

    connect(mainStackWidget, &QStackedWidget::currentChanged, this, [this](int index){
        QList<Devices::presetIp> presetIps;

        DeviceDB::select(presetIps);
        for (int n = 0; n < presetIps.size(); n++) {
            addPresetPosBoxItem(presetIps.at(n).pos);
        }
    });
}

void OtherDialog::addPresetPosBoxItem(const QString &val)
{
    if (!boxItemList.contains(val)) {
        presetPosBox->addItem(val);
        boxItemList << val;
    }
}

void OtherDialog::fileSelectPopDialogSlot()
{
    fileFull = QFileDialog::getOpenFileName(this, tr("升级文件选择"));
    if (!fileFull.size()) {
        return ;
    }

    QFileInfo fileInfo(fileFull);
    QFile file(fileFull);
    if (file.open(QIODevice::ReadOnly)) {
        qDebug() << "文件打开成功";
    }
    else {
        QMessageBox::warning(this, tr("上传文件"), tr("文件无法打开!"), tr("是"));
        fileFull.clear();
        return ;
    }

    fileStore.name = fileInfo.fileName();
    fileStore.data = file.readAll();
    fileStore.size = fileStore.data.size();
    fileStore.md5 = QCryptographicHash::hash(fileStore.data, QCryptographicHash::Md5).toHex();

    fileSelectEdit->setText(fileStore.name);
    fileMd5Edit->setText(fileStore.md5);

    fileStore.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    file.close();
}

void OtherDialog::confirmUpload()
{
    if (!fileFull.size() || !fileStore.name.size()||\
        !fileStore.data.size()) {
        QMessageBox::warning(this, tr("上传文件"), tr("未选择上传文件!"), tr("是"));
        return ;
    }

    fileStore.remark = fileRemarkEdit->text();
    fileStore.model = fileModelEdit->text();
    fileStore.version = fileVersionEdit->text();

    DeviceDB::save(fileStore);
    QMessageBox::information(this, tr("上传文件"), tr("文件上传成功"), tr("是"));
    fileFull.clear();
    fileStore.data.clear();
    fileSelectEdit->clear();
    fileRemarkEdit->clear();
    fileModelEdit->clear();
    fileVersionEdit->clear();
    fileMd5Edit->clear();
}

void OtherDialog::cancelUpload()
{
    this->close();
}

void OtherDialog::preseTconfirm()
{
    Devices::presetIp pi;

    if (!presetIp->text().size()) {
        QMessageBox::warning(NULL, tr("IP预设置"), tr("IP不能为空!"), tr("是"));
        return ;
    }

    if (!presetMask->text().size()) {
        QMessageBox::warning(NULL, tr("IP预设置"), tr("掩码不能为空!"), tr("是"));
        return ;
    }

    if (!presetGateway->text().size()) {
        QMessageBox::warning(NULL, tr("IP预设置"), tr("网关不能为空!"), tr("是"));
        return ;
    }

    if (!presetPos->text().size()) {
        QMessageBox::warning(NULL, tr("IP预设置"), tr("位置信息不能为空!"), tr("是"));
        return ;
    }

    pi.ip = QHostAddress(presetIp->text()).toIPv4Address();
    pi.mask = QHostAddress(presetMask->text()).toIPv4Address();
    pi.gateway = QHostAddress(presetGateway->text()).toIPv4Address();
    pi.pos = presetPos->text();
    pi.usen = 0;

    QPair<QHostAddress, int> pair = \
            QHostAddress::parseSubnet(presetIp->text() + "/" + presetMask->text());
    if (pair.first.toIPv4Address() + 1 != pi.gateway) {
        switch (QMessageBox::warning(NULL, tr("确认"), \
                                     tr("网关地址,非子网内最小地址!，是否继续使用？"),\
                                     tr("是"), tr("否"), 0, 1)) {
          case 1: ; return ;
          case 0: /* ignore */
          default: break;
        }
    }

    if (DeviceDB::save(pi)) {
        addPresetPosBoxItem(pi.pos);
        QMessageBox::information(NULL, tr("IP预设置"), tr("添加成功!"), tr("是"));
    }
    else {
        QMessageBox::warning(NULL, tr("IP预设置"), tr("添加失败!\n请勿重复添加"), tr("是"));
    }
}

void OtherDialog::preseTcancel()
{
    this->close();
}
