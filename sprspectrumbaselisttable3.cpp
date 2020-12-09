#include "sprspectrumbaselisttable3.h"

SPRSpectrumBaseListTable3::SPRSpectrumBaseListTable3(QWidget *parent): SPRSpectrumListTable2()
{
    hideColumn(0); // fc
//    hideColumn(1); // thread
    hideColumn(2); // name
//    hideColumn(3); // summ (I)
    hideColumn(4); // H1
    hideColumn(5); // H2
    hideColumn(6); // H3
//    hideColumn(7); // center
    hideColumn(8); // peak
//    hideColumn(9); // RS
    hideColumn(10); // time
    hideColumn(11); // date
    hideColumn(12); // Ns
    hideColumn(13); // Fe
    hideColumn(14); // Cu
    hideColumn(15); // Mo
    hideColumn(16); // Zn
    hideColumn(17); // Mg


    hHeaders << tr("") << tr("Ручей") << tr("Имя") << tr("I") << tr("H1") << tr("H2") << tr("H3")
             << tr("Центр") << tr("Пик") << tr("X-Ray") << tr("Время") << tr("Дата и время")
             << tr("Ns") << tr("Fe") << tr("Cu") << tr("Mo") << tr("Zn") << tr("Mg");


    setHorizontalHeaderLabels(hHeaders);
}

void SPRSpectrumBaseListTable3::widgetsShow()
{
    SPRSpectrumListTable2::widgetsShow();

    blockSignals(true);
    QList<SPRSpectrumItemModel*> *lst = model->getSpectrumsModel(spectrumBase);
    for(int row=0; row < lst->size(); row++){
//        QLabel *la = (QLabel*)cellWidget(row, 9);
//        if(la){
//            double xray = lst->at(row)->getXRay();
//            QString txt = QString::number(xray, 'f', 2);
//            la->setText(txt);
//        }
        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(cellWidget(row,0));
        if(fc){
            fc->setDeleteVisible(false);
            fc->setColorVisible(false);
            fc->setTextVisible(false);
            fc->widgetsShow();
        }
        setColumnHidden(0, false);

        QTableWidgetItem *wi = item(row, 9);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 9, wi);
        }
//        wi->setText(QString::number(lst->at(row)->getXRay(), 'f', 2));
//        wi->setToolTip(tr("Rs"));
        double xray = lst->at(row)->getXRay();
        setTableWidgetItemProperty(wi, 9, QString::number(xray, 'f', 2), tr("X-Ray завеса"), false);

//        wi = item(row, 7); // center
//        if(!wi){
//            wi = new QTableWidgetItem();
//            setItem(row, 7, wi);
//        }
//        double center = lst->at(row)->getSpectrumCenter(false, Ns);
//        setTableWidgetItemProperty(wi, 7, QString::number(center, 'f', 0), tr("Геометрический центр спектра"), false);
    }
    setHorizontalHeaderLabels(hHeaders);
    blockSignals(false);
}
