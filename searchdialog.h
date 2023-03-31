#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QTableView>
#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QHeaderView>

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    SearchDialog();
    void popUp();
    void setCondition(QStandardItemModel *model, int column);
    void setSearchModel(QStandardItemModel *model);
    void setSearchColumn(int column);
    void initResultModel();
private:
    QPushButton *searchBtn;
    QLineEdit *searchLineEdit;
    QStandardItemModel *searchModel;
    QTableView *searchTabView;
    QLabel *searchResult;
    QProgressBar *searchProgress;

    QStandardItemModel *targetModel = NULL;
    int selectColumn = 0;
    const QString btnStyle = ("QPushButton{background-color: transparent; \
                                    color: rgb(255,255,255);\
                                    border:1px solid rgb(255,255,255);}"\
                              "QPushButton:hover{background-color: rgb(28,28,28);\
                                    border:1px solid rgb(28,28,28);}"\
                              "QPushButton:pressed{background-color: rgb(79,79,79);\
                                    border:1px solid rgb(255,255,255);}");
private slots:
    void startSearch();
};

#endif // SEARCHDIALOG_H
