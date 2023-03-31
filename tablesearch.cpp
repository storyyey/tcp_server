#include "tablesearch.h"

TableSearch::TableSearch()
{

}

void TableSearch::run()
{
    int nn = 0;
    int row = 0;

    if (!srcModel || !dstModel || !resultLabel || !progress)
        return;

    QList<QStandardItem *> item = srcModel->findItems(this->subStr, Qt::MatchFixedString, this->column);

    dstModel->removeRows(0, dstModel->rowCount());
    resultLabel->setText(QString("-- %1 --").arg(item.size()));
    progress->reset();
    if (!item.size()) {
        progress->setMinimum(0);
        progress->setMaximum(1);
        progress->setValue(1);
    }
    else {
        progress->setMinimum(0);
        progress->setMaximum(item.size());
        foreach(QStandardItem * entry, item) {
            for (nn = 0; nn < srcModel->columnCount(); nn++) {
                QStandardItem *item = srcModel->item(entry->row(), nn);
                dstModel->setItem(row, nn, new QStandardItem(item->text()));
            }
            row++;
            progress->setValue(row);
        }
    }
    this->srcModel = NULL;
    this->dstModel = NULL;
    this->resultLabel = NULL;
    this->progress = NULL;
}

void TableSearch::setSearchModel(QStandardItemModel *srcModel, QStandardItemModel *dstModel, QLabel *resultLabel, QProgressBar *progress)
{
   this->srcModel = srcModel;
   this->dstModel = dstModel;
   this->resultLabel = resultLabel;
    this->progress = progress;
}

void TableSearch::serSearchRule(const QString &str, int column)
{
    this->subStr = str;
    this->column = column;
}
