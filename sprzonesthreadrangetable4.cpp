#include "sprzonesthreadrangetable4.h"
#include <QHeaderView>


SPRZonesThreadRangeTable4::SPRZonesThreadRangeTable4(QWidget *parent) :
    QTableWidget (parent), zonesTableModel(nullptr)
{
    setRowCount(1); setColumnCount(0);
//    setSizeAdjustPolicy(QAbstractItemView::AdjustToContents);
    setSelectionBehavior(QAbstractItemView::SelectColumns);
    setSelectionMode(QAbstractItemView::SingleSelection);
    verticalHeader()->setVisible(false);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

}

void SPRZonesThreadRangeTable4::setThreadsVisible(QList<uint8_t> _threadsVisible){
    threadsVisible = _threadsVisible;
    for(int c=0; c<columnCount();c++){
        SPRZonesRange4 *zr = qobject_cast<SPRZonesRange4*>(cellWidget(0, c));
        if(zr){
            if(threadsVisible.contains(zr->getThreadNumber())){
                setColumnHidden(c, false);
                if(threadsVisible.first() == c){
                    zr->setFirstColumnVisible(true);
                } else {
                    zr->setFirstColumnVisible(false);
                }
            } else {
                cellWidget(0,c)->setVisible(false);
                setColumnHidden(c, true);
            }
        }
    }
    resizeMe();
}

void SPRZonesThreadRangeTable4::setThreadsVisible(uint8_t th){
    setThreadsVisible(QList<uint8_t>({th}));
}

void SPRZonesThreadRangeTable4::setModelData(SPRSpectrumZonesTableModel *_zoneTableModel){
    if(_zoneTableModel){
        zonesTableModel = _zoneTableModel;
        connect(zonesTableModel, &SPRSpectrumZonesTableModel::modelChanget, this, &SPRZonesThreadRangeTable4::onModelChanget);

        setColumnCount(zonesTableModel->items.size());
        QStringList hHeadersTitle;
        for(int c=0; c<columnCount(); c++){
            SPRZonesRange4 *zr = qobject_cast<SPRZonesRange4*>(cellWidget(0, c));
            if(!zr){
                zr = new SPRZonesRange4();
//                if(c == 0){
//                    zr->setFirstColumnVisible(true);
//                } else {
//                    zr->setFirstColumnVisible(false);
//                }
                setCellWidget(0, c, zr);
                connect(zr, &SPRZonesRange4::gotFocus, this, &SPRZonesThreadRangeTable4::onGotFocus);
                connect(zr, &SPRZonesRange4::rowCountChanget, this, &SPRZonesThreadRangeTable4::resizeMe);
            }
            zr->setModelData(zonesTableModel->getItem(c));
            QString title = tr("Ручей %1").arg(QString::number(zr->getThreadNumber()+1));
            hHeadersTitle.append(title);
        }
        setHorizontalHeaderLabels(hHeadersTitle);

        setThreadsVisible(zonesTableModel->getMainModel()->getThreadsList());
//        resizeMe();
    }
}

void SPRZonesThreadRangeTable4::resizeMe(){
//    resizeRowsToContents();
//    resizeColumnsToContents();
    QSize s = sizeHint();
//    QLayout *la = layout();
    int sw = 0;
    int sh = 0;
    for(int c=0; c<columnCount(); c++){
        if(!isColumnHidden(c)){
            QWidget *ww = cellWidget(0,c);
            if(ww){
                QSize ss = cellWidget(0,c)->sizeHint();
                int w = ss.width();
                sw += w;
                if(ss.height() > sh) sh = ss.height();
                horizontalHeader()->resizeSection(c, w);
                setCellWidget(0,c, ww);
            }
        }
    }
    verticalHeader()->resizeSection(0, sh);
    setMinimumSize(sw,sh);

//    resize(sw, sh);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    updateGeometry();

}

void SPRZonesThreadRangeTable4::onModelChanget(IModelVariable *var)
{
    if(zonesTableModel){
        if(var == zonesTableModel->getElementsProperty() || sender() == zonesTableModel->getElementsProperty()){
//            resizeMe();
        }
    }
}

void SPRZonesThreadRangeTable4::onGotFocus(SPRZonesRange4 *zr, QModelIndex index, EnumElements el){
    for(int c=0; c<columnCount(); c++){
        if(!(zr && zr == cellWidget(0,c))){
            zr->clearCurrent();
        } else {
            setCurrentCell(0, c);
        }
    }
    gotFocus(zr, index, el);
}


