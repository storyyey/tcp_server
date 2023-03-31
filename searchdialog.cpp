#include "searchdialog.h"
#include "mainwindow.h"
#include <QDesktopWidget>

SearchDialog::SearchDialog()
{
    QString thisStyle =\
                "QLineEdit {\
                  border: 2px solid #D4D4D4;\
                  border-radius: 4px;\
                  height: 30px;\
                 }\
                 QLineEdit:focus{\
                  border: 3px solid #63B8FF;\
                 }";
    thisStyle += "QProgressBar {\
                 border: 0px solid #63B8FF;\
                 border-radius: 5px;\
                }";
    thisStyle += "QPushButton:hover {\
                 background-color: #7A8B8B;\
                 border: 0px solid #030303;\
                }"\
                "QPushButton:pressed {\
                  background-color: #6E6E6E;\
                  border: 0px solid #030303;\
                 }";
    thisStyle += "QPushButton {\
                  border:0px solid #68838B;\
                  border-radius: 10px;\
                 }";
    thisStyle += "QDialog {\
                  background-color: #E0E0E0;\
                }";
    thisStyle += "QTableView {\
                  border: 3px solid #FFFFFF;\
                  border-radius: 6px;\
                 }";
    thisStyle += "QLabel {\
                    color: #000000;\
                  }";
    this->setStyleSheet(thisStyle);
    QVBoxLayout *mainLayout = new QVBoxLayout();
    this->setWindowTitle("查找");
    this->setLayout(mainLayout);

    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchBtn = new QPushButton();
    searchBtn->setToolTip("搜索");
    searchLayout->addWidget(searchBtn);
    QIcon searchIcon;
    searchIcon.addFile(tr(":/icon/search.png"));
    searchBtn->setIcon(searchIcon);
    searchBtn->setFixedSize(40, 40);
    //searchBtn->setStyleSheet(btnStyle);

    searchLineEdit = new QLineEdit();
    //searchLineEdit->setFixedHeight(40);
    searchLayout->addWidget(searchLineEdit);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout *progressLayout = new QHBoxLayout();
    QLabel *searchProLabel = new QLabel("查找进度");
    progressLayout->addWidget(searchProLabel);
    searchProgress = new QProgressBar();
    progressLayout->addWidget(searchProgress);
    mainLayout->addLayout(progressLayout);

    QHBoxLayout *resultLayout = new QHBoxLayout();
    QLabel *resultPrefix = new QLabel("总数:");
    resultPrefix->setFixedWidth(40);
    resultLayout->addWidget(resultPrefix);
    searchResult = new QLabel();
    resultLayout->addWidget(searchResult);
    mainLayout->addLayout(resultLayout);

    searchModel = new QStandardItemModel();

    searchTabView = new QTableView();
    searchTabView->setModel(searchModel);
    searchTabView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    searchTabView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    searchTabView->setAlternatingRowColors(true);
    mainLayout->addWidget(searchTabView);
    connect(searchBtn, SIGNAL(clicked()), this, SLOT(startSearch()));
}

void SearchDialog::popUp()
{
    QDesktopWidget dw;

    windowConsole("弹出查找窗口");
    initResultModel();
    searchProgress->reset();
    this->resize(QSize(dw.width() / 2, dw.height() / 3 ));
    searchResult->clear();
    searchLineEdit->clear();
    if (targetModel) {
        searchLineEdit->setPlaceholderText(targetModel->horizontalHeaderItem(selectColumn)->text());
    }
    this->exec();

    windowConsole("关闭查找窗口");
}

void SearchDialog::setCondition(QStandardItemModel *model, int column)
{
    targetModel = model;
    selectColumn = column;
}

void SearchDialog::setSearchModel(QStandardItemModel *model)
{
    targetModel = model;
}

void SearchDialog::setSearchColumn(int column)
{
    selectColumn = column;
}

void SearchDialog::initResultModel()
{
    if (!targetModel) {
        return ;
    }

    searchModel->clear();
    int columnCount = targetModel->columnCount();
    searchModel->setColumnCount(columnCount);
    for (int column = 0; column < columnCount; column++) {
        searchModel->setHeaderData(column, Qt::Horizontal, targetModel->horizontalHeaderItem(column)->text());
    }
}

void SearchDialog::startSearch()
{
    int nn = 0;
    int row = 0;

    if (!targetModel) {
        return ;
    }

    QList<QStandardItem *> item = targetModel->findItems(searchLineEdit->text(), Qt::MatchContains, selectColumn);

    searchResult->setText(QString("-- %1 --").arg(item.size()));
    searchProgress->reset();
    if (!item.size()) {
        searchProgress->setMinimum(0);
        searchProgress->setMaximum(1);
        searchProgress->setValue(1);
        return;
    }

    searchProgress->setMinimum(0);
    searchProgress->setMaximum(item.size());
    foreach(QStandardItem * entry, item) {
        for (nn = 0; nn < targetModel->columnCount(); nn++) {
            QStandardItem *item = targetModel->item(entry->row(), nn);
            if (item) {
                searchModel->setItem(row, nn, new QStandardItem(item->text()));
            }
        }
        row++;
        searchProgress->setValue(row);
    }
}
