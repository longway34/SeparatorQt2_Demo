#include "sprseparatesummarygistogrammwidget.h"
#include "models/sprmainmodel.h"
#include "models/sprhistorymodel2.h"

#include <QHeaderView>

/**
 * @brief SPRSeparateSummaryGistogrammWidget::TableViewModel::rowCount
 * @param parent
 * @return
 */

SPR_SSGW_TableViewModel::SPR_SSGW_TableViewModel(QWidget *parent) :
    QAbstractTableModel(parent), mainModel(nullptr), extModel(nullptr), separateModel(nullptr)
{
}

int SPR_SSGW_TableViewModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int SPR_SSGW_TableViewModel::columnCount(const QModelIndex &parent) const
{
    if(mainModel){
        return mainModel->getThreads()->getData();
    }
}

QVariant SPR_SSGW_TableViewModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(typeModelData == countInputInSec){
        if(extModel){
            if(role == Qt::DisplayRole){
                if(index.column() < mainModel->getThreads()->getData() && index.row() == 0){
                    QVector<SPRHistoryModel2::histDataOneThread> *d = extModel->getLastSessionRecords();
                    res = QVariant(QString::number((*d)[index.column()].data.i_prd[2],'f', 0));
                }
            }
            if(role == Qt::ToolTipRole){
                res = QVariant(QString(tr("Общее количество кусков с начала сортировки для ручья %1").arg(index.column()+1)));
            }
        }
    } else if(typeModelData == percentConcentrate2Input){
        if(separateModel){
            if(role == Qt::DisplayRole){
                if(index.column() < MAX_SPR_MAIN_THREADS && index.row() == 0){
//                    res = QVariant(QString::number(ext->porogs[index.column()][0]->getData()));
                    res = QVariant(QString::number(separateModel->getSettingsSeparate()->prg[index.column()][0], 'f', 2));
                }
            }
            if(role == Qt::ToolTipRole){
                res = QVariant(QString(tr("Действующий погор для параметра Н1 для ручья %1").arg(index.column()+1)));
            }
        }
    }
    return  res;
}

void SPR_SSGW_TableViewModel::setModelData(SPRMainModel *_mainModel, SPRWorkSeparateTypeData _type)
{
    typeModelData = _type;
    mainModel = _mainModel;
    if(mainModel){
        if(extModel){
            disconnect(extModel, &SPRHistoryModel2::separateDataReady, this, &SPR_SSGW_TableViewModel::onSeparateDataReady);
//            extModel = nullptr;
        }
        extModel = mainModel->getHistoryModel2();
        if(separateModel){
            disconnect(separateModel, &SPRSeparateModel::modelChanget, this, &SPR_SSGW_TableViewModel::onModelChanget);
        }
        separateModel = mainModel->getSeparateModel();
//        if(typeModelData == countInputInSec){
//        } else if(typeModelData == percentConcentrate2Input){
//            extModel = mainModel->getSeparateModel();
//        }
        if(extModel){
            connect(extModel, &SPRHistoryModel2::separateDataReady, this, &SPR_SSGW_TableViewModel::onSeparateDataReady);
        }
        if(separateModel){
            connect(separateModel, &SPRSeparateModel::modelChanget, this, &SPR_SSGW_TableViewModel::onModelChanget);
        }
    }
    beginResetModel();
    endResetModel();
}

void SPR_SSGW_TableViewModel::onModelChanget(IModelVariable *)
{
//    if(separateModel && sender() == separateModel){
//        beginResetModel();
//        endResetModel();
//    }

}

void SPR_SSGW_TableViewModel::onSeparateDataReady()
{
    if(extModel){
        if(sender() == extModel){
            beginResetModel();
            endResetModel();
        }
    }
}


QVariant SPR_SSGW_TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant res = QAbstractTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole || role == Qt::EditRole){
        if(orientation == Qt::Vertical){
            if(typeModelData == countInputInSec){
                res = QVariant(tr("Всего:"));
            } else if(typeModelData == percentConcentrate2Input){
                res = QVariant(tr("Пороги:"));
            }
        }
    }
    return res;
}

/**
 * @brief SPR_SSGW_TableView::SPR_SSGW_TableView
 * @param parent
 */

SPR_SSGW_TableView::SPR_SSGW_TableView(QWidget *parent): QTableView(parent)
{
}


/**
 * @brief SPRSeparateSummaryGistogrammWidget::SPRSeparateSummaryGistogrammWidget
 * @param parent
 */


SPRSeparateSummaryGistogrammWidget::SPRSeparateSummaryGistogrammWidget(QWidget *parent):
    QFrame(parent),
    mainModel(nullptr),
//    historyModel(nullptr),
    historyModel2(nullptr),
    gistogramm(nullptr),
    oneRowTable(nullptr)
{
    QHBoxLayout *hb = new QHBoxLayout(this);
//    setLayout(new QHBoxLayout());
    forVGroup = new QWidget(this);
    hb->addWidget(forVGroup);
    hb->setSpacing(0);
    hb->setContentsMargins(0, 0, 0, 0);

    setFrameStyle(QFrame::Panel);
//    setFrameShadow(QFrame::)
}

void SPRSeparateSummaryGistogrammWidget::setModelData(SPRMainModel* _mainModel, SPRWorkSeparateTypeData _tydeWidget, bool _woDownTable)
{
    if(_mainModel){
        if(mainModel){
            disconnect(mainModel, &SPRMainModel::modelChanget, this, &SPRSeparateSummaryGistogrammWidget::onModelChanget);
        }
        connect(_mainModel, &SPRMainModel::modelChanget, this, &SPRSeparateSummaryGistogrammWidget::onModelChanget);
        mainModel = _mainModel;

        historyModel2 = mainModel->getHistoryModel2();

        typeWidget = _tydeWidget;
        woDownTable = _woDownTable;

        threadsList = mainModel->getThreadsList();

        bool woPorogs = typeWidget == percentConcentrate2Input ? true : false;
        QString hTitle = typeWidget == percentConcentrate2Input ?
                    QString(tr("Выход продуктов сортировки, %")) :
                    QString(tr("Подача руды, (кусков/сек.)")) ;

        foreach(QWidget* w, forVGroup->findChildren<QWidget*>()){
            forVGroup->layout()->removeWidget(w);
        }


        if(gistogramm) delete gistogramm; gistogramm = nullptr;

        gistogramm = new SPRSeparateSummaryGistogramm(this);
        gistogramm->setModelData(_mainModel, historyModel2, hTitle, QColor(Qt::lightGray), mainModel->getThreadsList(), typeWidget, woPorogs);

        QVBoxLayout *vb = new QVBoxLayout(forVGroup);
        vb->setSpacing(0);
        vb->setContentsMargins(0, 0, 0, 0);
        vb->addWidget(gistogramm);


        if(!woDownTable){
            if(oneRowTable) delete oneRowTable; oneRowTable = nullptr;

            oneRowTable = new SPR_SSGW_TableView(this);
            SPR_SSGW_TableViewModel *tvm = new SPR_SSGW_TableViewModel(this);
            tvm->setModelData(mainModel, typeWidget);

            oneRowTable->setModel(tvm);
            oneRowTable->horizontalHeader()->setVisible(false);

            adjustTableToSize(oneRowTable);
            oneRowTable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
            oneRowTable->resize(oneRowTable->sizeHint());

            vb->addWidget(oneRowTable);
        }
        widgetsShow();


//        }
    }
}

void SPRSeparateSummaryGistogrammWidget::widgetsShow()
{
    if(!woDownTable){
//        adjustTableToSize(oneRowTable);
//        updateGeometry();
    }
}

void SPRSeparateSummaryGistogrammWidget::onModelChanget(IModelVariable *send){
    if(send == mainModel->getThreads()){
        setModelData(mainModel, typeWidget, woDownTable);
        widgetsShow();
        this->show();
        return;
    }
//    if(sender() == historyModel2){
//        widgetsShow();
//    }
}

void SPRSeparateSummaryGistogrammWidget::adjustTableToSize(QTableView *tv)
{
    if(tv){
        tv->horizontalHeader()->resizeSections(QHeaderView::Stretch);

        int h = tv->lineWidth();
        if(tv->horizontalHeader()->isVisible()){
            h += tv->horizontalHeader()->size().height();
            h += tv->lineWidth();
        }
        tv->resizeRowsToContents();
        for(int row = 0; row < tv->model()->rowCount(); row++){
            int h1 = tv->verticalHeader()->sectionSize(0);
            int h2 = (h1 * tv->lineWidth());
            h += h2;
        }

        tv->horizontalHeader()->resizeSections(QHeaderView::Stretch);
        tv->resize(tv->size().width(), h);
        tv->setMinimumHeight(h);
        tv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
}



void SPRSeparateSummaryGistogrammWidget::resizeEvent(QResizeEvent *event)
{
    adjustTableToSize(oneRowTable);
    QFrame::resizeEvent(event);
}


QSize SPR_SSGW_TableView::sizeHint() const
{
    QSize newSize = QTableView::sizeHint();

    horizontalHeader()->resizeSections(QHeaderView::Stretch);

    int h = lineWidth();
    if(horizontalHeader()->isVisible()){
        h += horizontalHeader()->size().height();
        h += lineWidth();
    }
//    resizeRowsToContents();
    for(int row = 0; row < model()->rowCount(); row++){
        int h1 = verticalHeader()->sectionSize(0);
        int h2 = (h1 * lineWidth());
        h += h2;
    }
    newSize.setHeight(h);

    return newSize;
}


QSize SPR_SSGW_TableView::minimumSizeHint() const
{
    QSize newSize = sizeHint();
    return newSize;
}
