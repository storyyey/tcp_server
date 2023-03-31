#ifndef TABLESEARCH_H
#define TABLESEARCH_H

#include <QThread>
#include <QStandardItemModel>
#include <QProgressBar>
#include <QLabel>
#include <QString>

class TableSearch : public QThread
{
public:
    TableSearch();
    void run();
    void setSearchModel(QStandardItemModel *srcModel, QStandardItemModel *dstModel, QLabel *resultLabel, QProgressBar *progress);
    void serSearchRule(const QString &str, int column);
private:
    QStandardItemModel *srcModel;
    QStandardItemModel *dstModel;
    QProgressBar *progress;
    QLabel *resultLabel;
    QString subStr;
    int column;
};

#endif // TABLESEARCH_H
