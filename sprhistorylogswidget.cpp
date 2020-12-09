#include "sprhistorylogswidget.h"
#include "ui_sprhistorylogswidget.h"

#include <QLabel>

#include "models/sprseparatorlogsmodel.h"


QWidget *SPRHtmlViewTableDeledate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLabel *editor = new QLabel(parent);
    editor->setAlignment(Qt::AlignCenter);
    return editor;
}


void SPRHtmlViewTableDeledate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    if(editor){
        QLabel *ed = qobject_cast<QLabel*>(editor);
        if(ed){
            delete ed;
            ed = nullptr;
        }
    }
    QStyledItemDelegate::destroyEditor(editor, index);
}

void SPRHtmlViewTableDeledate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).value<QString>();

    QLabel *htmlLe = static_cast<QLabel*>(editor);
    htmlLe->setText(value);
//    spinBox->setWordWrap(true);
}


void SPRHtmlViewTableDeledate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLabel *htmlLe = static_cast<QLabel*>(editor);
    //spinBox->interpretText();
    //int value = spinBox->value();
    //    int value = 10;

    model->setData(index, QString(""), Qt::EditRole);
}

void SPRHtmlViewTableDeledate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
/**
 * @brief SPRHistoryLogsSeparateTableModel::rowCount
 * @param parent
 * @return
 */
SPRHistoryLogsSeparateTableModel::SPRHistoryLogsSeparateTableModel(SPRSeparatorLogsModel *_logModel, QWidget *parent):
    QAbstractTableModel(parent), logModel(nullptr)
{
    if(_logModel && _logModel != logModel){
        logModel = _logModel;
    }
}

int SPRHistoryLogsSeparateTableModel::rowCount(const QModelIndex &parent) const
{
    if(logModel){
        QVector<SPRSeparatorLogsModel::ToTableViewResumeData*> *source = logModel->getTableViewResumeData();
        return source->size();
    } else {
        return 0;
    }
}

int SPRHistoryLogsSeparateTableModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant SPRHistoryLogsSeparateTableModel::data(const QModelIndex &index, int role) const
{
    QVariant res; // = QAbstractTableModel::data(index, role);
    if(logModel){
        if(role == Qt::DisplayRole || role == Qt::EditRole){
            QVector<SPRSeparatorLogsModel::ToTableViewResumeData*> *source = logModel->getTableViewResumeData();
            if(index.row() < source->size() && index.column() < columnCount(index)){
                switch (index.column()) {
                case 0:{
                    QDateTime b = (*source)[index.row()]->dtBegin;
                    QDateTime e = (*source)[index.row()]->dtEnd;
                    qint64 ddiff = llabs(e.secsTo(b));

                    int MIN_SEC = 60;
                    int HOUR_SEC = (MIN_SEC * 60);

                    int h = ddiff / HOUR_SEC;
                    int m = (ddiff % HOUR_SEC) / MIN_SEC;

                    int sec = (ddiff & MIN_SEC);
                    QTime td = QTime(h,  m,  sec);
                    if(role == Qt::EditRole){
//                        findRow->dtEnd = QString("%1 - %2 (%3)").arg(res->getBegin().toString("HH:mm")).arg(res->getEnd().toString("HH:mm")).arg(ddiff.toString("HH:mm:ss"));
                    QString s = QString("%1 - %2 ( <b>%3</b> )").
                            arg(b.toLocalTime().toString("HH:mm")).
                            arg(e.toLocalTime().toString("HH:mm")).
                            arg(td.toString("HH:mm:ss"));
                    res = QVariant(s);
                    } else {
                        res = QVariant(QString(""));
                    }
                }
                    break;
//                    res = QVariant((*source)[index.row()]->dtEnd); break;
                case 1:{
                        if(role == Qt::DisplayRole){
                            res = QVariant(QString(""));
                        } else {
                            res = QVariant((*source)[index.row()]->strResume);
                        }
                    }
                    break;
                case 2:{
                    QString s = QString("%1%").arg(QString::number((*source)[index.row()]->prAllResume, 'f', 2));
                    res = QVariant(s);
                }
                    break;
                case 3:{
                    QString s = tr("%1").arg(QString::number((*source)[index.row()]->countAllResume));
                    res = QVariant(s);
                }
                    break;
                case 4:{
                    QString s = (*source)[index.row()]->smenaName;
                    res = QVariant(s);
                }
                    break;
                case 5:{
                    QString s = (*source)[index.row()]->userNicName;
                    res = QVariant(s);
                }
                    break;
                }
            }
        }
        if(role == Qt::TextAlignmentRole){
//            if(index.column() < 4){
                res = QVariant(Qt::AlignCenter);
//            } else {
//                res = QVariant(Qt::AlignLeft | Qt::AlignHCenter);
//            }
        }
    }
    return res;
}

QVariant SPRHistoryLogsSeparateTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Vertical){
            if(logModel){
                QVector<SPRSeparatorLogsModel::ToTableViewResumeData*> *source = logModel->getTableViewResumeData();
                if(section < source->size()){
                    QString s = (*source)[section]->dtBegin.toString("dd.MM.yyyy");
                    res = QVariant(s);
                }
            }
        } else {
            switch (section) {
            case 0:
                res = QVariant(tr("  Время работы  "));
                break;
            case 1:
                res = QVariant(tr("  Процент выхода  "));
                break;
            case 2:
                res = QVariant(tr("  Общий процент  "));
                break;
            case 3:
                res = QVariant(tr("  Количество руды  "));
                break;
            case 4:
                res = QVariant(tr("  Смена  "));
                break;
            case 5:
                res = QVariant(tr("  Оператор  "));
                break;
            default:
                break;
            }
        }
    }
    if(role == Qt::TextAlignmentRole){
//        if(orientation == Qt::Vertical){
            res = QVariant(Qt::AlignHCenter);
//        } else {
//            res = QVariant(Qt::AlignCenter);
//        }
    }

    return res;
}

void SPRHistoryLogsSeparateTableModel::onResumeTableChanget()
{
    beginResetModel();
    endResetModel();

}


/**
 * @brief SPRHistroryLogsMessagesTableModel::rowCount
 * @param parent
 * @return
 */
SPRHistroryLogsMessagesTableModel::SPRHistroryLogsMessagesTableModel(SPRSeparatorLogsModel *_logModel, QWidget *parent):
    QAbstractTableModel(parent), logModel(nullptr)
{
    if(_logModel && _logModel != logModel){
        logModel = _logModel;

//        connect()
    }
}

int SPRHistroryLogsMessagesTableModel::rowCount(const QModelIndex &parent) const
{
    if(logModel){
        QVector<SPRSeparatorLogsModel::ToTableViewMessagesData*> *source = logModel->getTableViewMessagesData();
        return source->size();
    }
    return 0;
}

int SPRHistroryLogsMessagesTableModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant SPRHistroryLogsMessagesTableModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(logModel){
        QVector<SPRSeparatorLogsModel::ToTableViewMessagesData*> *source = logModel->getTableViewMessagesData();
        if(index.row() < source->size()){
            SPRSeparatorLogsModel::ToTableViewMessagesData* trow = (*source)[index.row()];
            if(role == Qt::DisplayRole || role == Qt::EditRole){
                switch(index.column()){
                case 0: res = QVariant(trow->messageText); break;
                case 1: res = QVariant(trow->smenaName); break;
                case 2: res = QVariant(trow->userNicName); break;
                }
                return res;
            }
            if(role ==Qt::BackgroundColorRole){
                switch(trow->type){
                case tError: res = QColor("#fdd1d3"); break;
                case tWarning: res = QColor("#d6d1fd"); break;
                }
                return res;
            }
            if(role == Qt::TextColorRole){
                switch(trow->type){
                case tError: res = QColor("#0010b8"); break;
                case tWarning: res = QColor("#0010b8"); break;
                }
                return res;
            }
            if(role == Qt::TextAlignmentRole){
                res = QVariant(Qt::AlignCenter);
            }
        }
    }
    return res;
}

QVariant SPRHistroryLogsMessagesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Vertical){
            if(logModel){
                QVector<SPRSeparatorLogsModel::ToTableViewMessagesData*> *source = logModel->getTableViewMessagesData();
                if(section < source->size()){
                    QDateTime rDt = (*source)[section]->dt;
                    res = QVariant(rDt.toString("dd.MM.yyyy HH:mm:ss"));
                }
            }
        } else {
            switch(section){
            case 0: res = QVariant(tr("Сообщение")); break;
            case 1: res = QVariant(tr("Смена")); break;
            case 2: res = QVariant(tr("Оператор")); break;
            }
        }
        return res;
    }
    if(role == Qt::TextAlignmentRole){
        res = QVariant(Qt::AlignCenter);
    }
    return res;
}

void SPRHistroryLogsMessagesTableModel::onMessageTableChanget()
{
    beginResetModel();
    endResetModel();
}


/**
 * @brief SPRHistoryLogsWidget::SPRHistoryLogsWidget
 * @param parent
 */
SPRHistoryLogsWidget::SPRHistoryLogsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SPRHistoryLogsWidget),
    logModel(nullptr)
{
    ui->setupUi(this);
}

void SPRHistoryLogsWidget::setModelsData(SPRSeparatorLogsModel *_logModel)
{
//    mainModel = _mainModel;
    logModel = _logModel;

    if(logModel){
        connect(logModel, &SPRSeparatorLogsModel::resumesTableChanget, this, &SPRHistoryLogsWidget::onResumeTableChanget);
        connect(logModel, &SPRSeparatorLogsModel::echoLogMessage, this, &SPRHistoryLogsWidget::onEchoLogMessage);
    }

    delegate = new SPRHtmlViewTableDeledate();

    ui->tvSeparate->setModel(new SPRHistoryLogsSeparateTableModel(logModel));
    ui->tvSeparate->setItemDelegateForColumn(1, delegate);
    ui->tvSeparate->setItemDelegateForColumn(0, delegate);
    ui->tvSeparate->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui->tvSeparate->setSizeAdjustPolicy(QTableView::AdjustToContents);

    ui->tvMessages->setModel(new SPRHistroryLogsMessagesTableModel(logModel));
    ui->tvSeparate->horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    ui->tvSeparate->setSizeAdjustPolicy(QTableView::AdjustToContents);

}

void SPRHistoryLogsWidget::onEchoLogMessage(SPRSeparateMsgModel*, SPRSeparatorLogsModel*){
    if(logModel){
        SPRHistroryLogsMessagesTableModel *mod = qobject_cast<SPRHistroryLogsMessagesTableModel*>(ui->tvMessages->model());
        if(mod){
            mod->onMessageTableChanget();
        }
    }
}

void SPRHistoryLogsWidget::onResumeTableChanget(){
   SPRHistoryLogsSeparateTableModel *separateTableModel = qobject_cast<SPRHistoryLogsSeparateTableModel*>(ui->tvSeparate->model());
   if(separateTableModel){
       separateTableModel->onResumeTableChanget();
       for(int row = 0; row < ui->tvSeparate->model()->rowCount(); row++){
           QModelIndex index = ui->tvSeparate->model()->index(row, 1);
           ui->tvSeparate->openPersistentEditor(index);
           index = ui->tvSeparate->model()->index(row, 0);
           ui->tvSeparate->openPersistentEditor(index);
       }
   }
//   ui->tvSeparate->resizeColumnsToContents();
}

SPRHistoryLogsWidget::~SPRHistoryLogsWidget()
{
    delete ui;
}

void SPRHistoryLogsWidget::widgetsShow()
{
}

ISPRModelData *SPRHistoryLogsWidget::getModelData()
{
    return nullptr;
}

void SPRHistoryLogsWidget::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName()+"."+ui->splitter->objectName(), ui->splitter->saveState());
    mainSettings->setValue(objectName()+"."+ui->tvSeparate->objectName(), ui->tvSeparate->horizontalHeader()->saveState());
    mainSettings->endGroup();
}

void SPRHistoryLogsWidget::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    ui->splitter->restoreState(mainSettings->value(objectName()+"."+ui->splitter->objectName()).toByteArray());
    ui->tvSeparate->horizontalHeader()->restoreState(mainSettings->value(objectName()+"."+ui->tvSeparate->objectName()).toByteArray());
    mainSettings->endGroup();
}

//void SPRHistoryLogsWidget::selLastSmenaOnly(bool only)
//{

//}


