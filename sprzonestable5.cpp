#include "sprzonestable5.h"
#include "models/sprmainmodel.h"

#include <QHeaderView>
#include <QStringList>
#include <QMessageBox>

#include "sprzonesrange5.h"
#include "firstcollumn2.h"

/**
 * @brief SPRZonesPropertyTable5::SPRZonesPropertyTable5
 * @param parent
 */
void SPRZonesPropertyTable5::setRowHeight(int value)
{
    rowHeight = value;
    verticalHeader()->setDefaultSectionSize(rowHeight);
    for(int r=0; r<rowCount();r++){
        verticalHeader()->resizeSection(r, rowHeight);
    }
    updateGeometry();
}

SPRZonesPropertyTable5::SPRZonesPropertyTable5(QWidget *parent) :QTableWidget(parent), mainModel(nullptr)
{
    setRowCount(0); setColumnCount(1);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(true);
    setHorizontalHeaderLabels(QStringList({"*"}));

    verticalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);
    verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSelectionMode(QAbstractItemView::NoSelection);

    setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
}

void SPRZonesPropertyTable5::setModelData(SPRMainModel *_MainModel)
{
    mainModel = _MainModel;
    if(mainModel){
        connect(mainModel->getElementsModel(), &IModelVariable::modelChanget, this, &SPRZonesPropertyTable5::onModelChanget);
        onModelChanget(nullptr);
    }
}

void SPRZonesPropertyTable5::onModelChanget(IModelVariable *var)
{
    if(mainModel){
        QObject *sndr = sender();
        SPRElementsModel *em = mainModel->getElementsModel();
        if(!sender() || var == mainModel->getElementsModel()){
            QList<EnumElements> elements = mainModel->getElementsModel()->getElements().keys();
            int rows = elements.size();
            while(rowCount() > rows){
                removeRow(rowCount()-1);
            }
            setRowCount(rows);
            mySenders.clear();
            foreach(EnumElements el, elements){
                int r = elements.indexOf(el);
                SPRElementsProperty *elProperty = mainModel->getElementsModel()->getElementProperty(el);
                FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(cellWidget(r, 0));
                if(!fc){
                    fc = new FirstCollumn2();
                    connect(fc, &FirstCollumn2::colorChanged, this, &SPRZonesPropertyTable5::onColorChanget);
                    connect(fc, &FirstCollumn2::deleteRow, this, &SPRZonesPropertyTable5::onDeleteElement);
                    fc->setParams(r, elProperty->sName->getData(), elProperty->color->getData(), false, true, true, false);

                    fc->resize(sizeHint());
                    fc->setMaximumSize(sizeHint());
                    fc->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                    setCellWidget(r,0,fc);
                }
                fc->setParams(r, elProperty->sName->getData(), elProperty->color->getData(), false, true, true, false);
                fc->setModelPrt(elProperty);
                mySenders.insert(elProperty->color);
                mySenders.insert(elProperty->sName);

            }
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
        } else if(mySenders.contains(var)){
            for(int r=0; r<rowCount();r++){
                FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(cellWidget(r,0));
                if(fc){
                    SPRElementsProperty *prop = qobject_cast<SPRElementsProperty*>(fc->getModelPrt());
                    if(prop){
                        fc->setParams(r, prop->sName->getData(), prop->color->getData(), false, true, true, false);
                    }
                }
            }
        }
    }
}

void SPRZonesPropertyTable5::onColorChanget(IModelVariable *var, QColor color)
{
    SPRElementsProperty *em = dynamic_cast<SPRElementsProperty*>(var);
    if(em){
        em->color->setData(color);
    }
}

void SPRZonesPropertyTable5::onDeleteElement(IModelVariable *var)
{
    SPRElementsProperty *em = dynamic_cast<SPRElementsProperty*>(var);
    if(em){
        if(em->key->getData() == Ns){
            QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                    "Это важный параметр в системе...").arg(em->fName->getData()));
            return;

        }
        QString txt = tr("Вы действительно хотите удалить елемент %1 из списка?").
            arg(em->fName->getData());
        int res = QMessageBox::question(nullptr, tr("Внимание"), txt);
        if(res == QMessageBox::Yes){
            if(mainModel){
                SPRSettingsFormulaModel *fm = mainModel->getSettingsFormulaModel();
                int numFormula = -1;
                if(fm->isUsed(em->key->getData(), &numFormula)){
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                            "Он используется в расчете параметра H%2").arg(em->fName->getData()).arg(numFormula+1));
                    return;
                }
                if(mainModel->getSettingsControlModel()->controlArea->getData() == em->key->getData()){
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                            "Он используется для контроля корреляции...").arg(em->fName->getData()));
                    return;

                }
            }
            mainModel->getElementsModel()->deleteElement(em);
        }
    }
}


/**
 * @brief SPRZonesTable5::SPRZonesTable5
 * @param parent
 */
void SPRZonesTable5::setThreadList(const SPRThreadList &value)
{
    if(threadList != value){
        onGotFocus(nullptr, QModelIndex());
    }
    threadList = value;

    for(int c=1; c<columnCount(); c++){
        SPRZonesRange5 *zr = qobject_cast<SPRZonesRange5*>(cellWidget(0,c));
        if(zr){
            uint8_t th = zr->getThreadNumber();
            if(threadList.contains(th)){
                setColumnHidden(c, false);
            } else {
                setColumnHidden(c, true);
            }
        }
    }
}

void SPRZonesTable5::setThread(uint8_t th)
{
    setThreadList(SPRThreadList({th}));
}

SPRZonesTable5::SPRZonesTable5(QWidget *parent): QTableWidget(parent), ztModel(nullptr), mainModel(nullptr), fcWidget(nullptr)
{
    fcWidget = new SPRZonesPropertyTable5();

    verticalHeader()->setVisible(false);
    setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
    setRowCount(1); setColumnCount(1);
    setCellWidget(0,0,fcWidget);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    horizontalHeader()->setSizeAdjustPolicy(QHeaderView::AdjustToContents);

    threadList = getAllThreadsListDefault();

}

void SPRZonesTable5::setModelData(SPRSpectrumZonesTableModel *_zModel)
{
    if(_zModel){
        ztModel = _zModel;
        mainModel = ztModel->getMainModel();
        if(mainModel){
            QStringList hHeaders = QStringList({""});
            connect(ztModel->getElementsProperty(), &SPRElementsModel::modelChanget, this, &SPRZonesTable5::onModelChanget);
            connect(ztModel->getThreads(), &IModelVariable::modelChanget, this, &SPRZonesTable5::onModelChanget);
            int col = ztModel->items.size()+1;
            setColumnCount(col);
            setRowCount(1);
            int cHeight = 0;
            for(int c=0; c<columnCount();c++){
                if(c == 0){
                    fcWidget->setModelData(mainModel);
                } else {
                    SPRZonesRange5 *zr = qobject_cast<SPRZonesRange5*>(cellWidget(0, c));
                    if(!zr){
                        zr = new SPRZonesRange5();
                        connect(zr, &SPRZonesRange5::gotFocus, this, &SPRZonesTable5::onGotFocus);
                    }
                    SPRSpectrumZonesModel *zm = ztModel->getItem(c-1);
                    zr->setModelData(zm);
                    int th = zm->getThreadNumber()+1;
                    hHeaders += tr("Ручей %1").arg(th);
                    setCellWidget(0, c, zr);
                    zr->repaint();
                    resizeColumnToContents(c);
                    if(zr->verticalHeader()->sectionSize(0) > cHeight) cHeight = zr->verticalHeader()->sectionSize(0);
                }
            }
            fcWidget->setRowHeight(cHeight);
            setHorizontalHeaderLabels(hHeaders);
            horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

            resizeRowsToContents();
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

            setThreadList(threadList);
        }
    }
}

void SPRZonesTable5::onModelChanget(IModelVariable *var)
{
    if(var == ztModel->getElementsProperty()){
//        updateGeometry();
        resizeRowsToContents();
    } else if(var == mainModel->getThreads()){
        uint threads = mainModel->getThreads()->getData();
        SPRThreadList toRemote;
        foreach(uint8_t th, threadList){
            if(th >= threads){
                toRemote.append(th);
            }
        }
        foreach(uint8_t th, toRemote){
            threadList.removeAll(th);
        }

        setThreadList(threadList);
    }
}

void SPRZonesTable5::onGotFocus(SPRZonesRange5 *zr, QModelIndex index)
{
    if(!zr){
        emit gotFocus(Ns, 255, noName5);
//        return;
    }
    for(int c=1; c<columnCount();c++){
        SPRZonesRange5 *czr = qobject_cast<SPRZonesRange5 *>(cellWidget(0,c));
        if(czr){
            if(czr != zr){
                QModelIndex nullIndex = czr->model()->index(-1, -1);
                czr->setCurrentIndex(nullIndex);
            }
        }
    }
    uint8_t th = 255;
    if(zr){
        th = zr->getThreadNumber();
        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(fcWidget->cellWidget(index.row(), 0));
        if(fc){
            SPRElementsProperty *pr = qobject_cast<SPRElementsProperty*>(fc->getModelPrt());
            if(pr){
                EnumElements el = pr->key->getData();
                SPRTypeMinMaxField5 type = noName5;
                if(index.column() == 0){
                    type = minField5;
                } else if(index.column() == 1){
                    type = maxField5;
                }
                emit gotFocus(el, th, type);
            }
        }
    }
}
