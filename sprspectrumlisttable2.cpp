#include "sprspectrumlisttable2.h"
#include <QLabel>
#include "_types.h"

#include "models/sprmainmodel.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QAction>

SPRSpectrumListTable2Delegate::SPRSpectrumListTable2Delegate(QObject *parent): QStyledItemDelegate(parent)
{
}

QWidget *SPRSpectrumListTable2Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setProperty("index", QVariant(index));
    SPRSpectrumListTable2 *table = dynamic_cast<SPRSpectrumListTable2*>(parent->parent());
    if(table){
        connect(editor, &QLineEdit::editingFinished, table, &SPRSpectrumListTable2::onChangeSpectrumName);
    }
    return editor;
}

void SPRSpectrumListTable2Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *ed = dynamic_cast<QLineEdit*>(editor);
    QString txt = index.model()->data(index,Qt::EditRole).toString();
    ed->setText(txt);
}

void SPRSpectrumListTable2Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *ed = dynamic_cast<QLineEdit*>(editor);
    QString txt = ed->text();
    model->setData(index, QVariant(txt));

}

/**
 * @brief SPRSpectrumListTable2::connectFirstTable
 * @param fc
 */


void SPRSpectrumListTable2::onRowSelect(IModelVariable *var, bool){
//    if(checked){
        FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(sender());
        if(fc){
            SPRSpectrumItemModel *item = dynamic_cast<SPRSpectrumItemModel*>(var);
            if(spectrums->contains(item)){
                storeCurrentItem = item;
            }
            emit rowSelectedChecked3(getSelectedItems(), storeCurrentItem);
        }
//    }
}
void SPRSpectrumListTable2::onDeleteRow(IModelVariable *var){
    QList<SPRSpectrumItemModel*> currentAndSelectItems;
    FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(sender());
    if(fc){ // press in table delete button
        SPRSpectrumItemModel *item = dynamic_cast<SPRSpectrumItemModel*>(var);
        if(item){
            if(spectrums->contains(item)){
                currentAndSelectItems.append(item);
                storeCheckedItemsNum = spectrums->indexOf(item);
            }
        }
    } else { // press button from form
//        QModelIndex curIndex = currentIndex();
//        if(curIndex.isValid()){
//            SPRSpectrumItemModel *spec = getItemModelByRow(curIndex.row());
//            if(spec){
//                currentAndSelectItems.append(spec);
//            }
//        }
        QList<SPRSpectrumItemModel*> selItems = getSelectedItems();
        foreach(SPRSpectrumItemModel* item, selItems){
            if(item && !currentAndSelectItems.contains(item)){
                currentAndSelectItems.append(item);
            }
        }
    }
//    currentAndSelectItems.append(selItems);
//    toDeleteItems.unite(selItems.toSet());
    QMessageBox::QMessageBox::StandardButtons setButton = currentAndSelectItems.size() > 1 ? QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll | QMessageBox::YesToAll) :
                                                                       QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No);
    QMessageBox::StandardButtons res =  QMessageBox::StandardButtons(QMessageBox::No);
    foreach(SPRSpectrumItemModel *item, currentAndSelectItems){
        if(res != QMessageBox::YesToAll){
            res = QMessageBox::question(nullptr, tr("Удаление спектра"), tr("Вы действительно хотите удалить спектр %1 ручья %2").
                                        arg(item->getSpectrumName()).arg(item->getSpectrumThread()+1),
                                        setButton,
                                        QMessageBox::No);
        }

        if(res == QMessageBox::Yes || res == QMessageBox::YesToAll){
            int row = spectrums->indexOf(item);
            removeRow(row);
            model->removeSpectrum(item);
        } else if(res == QMessageBox::NoToAll){
            break;
        }
    }
//    model->removeSpectrum(item);
}

SPRSpectrumItemModel *SPRSpectrumListTable2::getItemModelByRow(int row)
{
    int pos = row;
    SPRSpectrumItemModel *mod = spectrums->at(pos);
    return mod;
}

void SPRSpectrumListTable2::onChangeSpectrumName(){
    QLineEdit *le = dynamic_cast<QLineEdit*>(sender());
    if(le){
        QModelIndex index = le->property("index").value<QModelIndex>();
        SPRSpectrumItemModel *mod = getItemModelByRow(index.row());
        QString txt = le->text();
        if(mod){
            mod->setSpectrumName(txt);
        }
    }
}

void SPRSpectrumListTable2::onSpectrumColorChange(QColor color){
    QObject *obj = property("model").value<QObject*>();
    if(obj){
        SPRSpectrumItemModel *item = dynamic_cast<SPRSpectrumItemModel*>(obj);
        if(item){
            item->setSpectrumColor(item, color);
        }
    }
}

SPRTypeSpectrumSet SPRSpectrumListTable2::getTypeData() const
{
    return typeData;
}

SPRSpectrumListTable2::SPRSpectrumListTable2(QWidget *parent):
    QTableWidget(parent), model(nullptr), spectrums(nullptr), actHideColumn(nullptr), actShowAllColumns(nullptr)
{
    storeCheckedItemsNum = -1;
    commonHTitles << tr("№ п/п") << tr("Ручей") << tr("Имя") << tr("I") << tr("H1") << tr("H2") << tr("H3")
            << tr("Центр") << tr("Пик") << tr("Rs") << tr("Дата/время") << tr("Время (сек.)") ;

    setColumnCount(18);
//    setHorizontalHeaderLabels(hTitles);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setSectionsMovable(true);
    verticalHeader()->setSectionsMovable(true);

    actHideColumn = new QAction(tr("Скрыть эту колонку"), this);
    connect(actHideColumn, SIGNAL(triggered(bool)), this, SLOT(myHideColumn(bool)));
    actShowAllColumns = new QAction(tr("Отобразить все"), this);
    connect(actShowAllColumns, SIGNAL(triggered(bool)), this, SLOT(myShowColumns(bool)));

    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(onCurrentPosChanged(int, int)));

    this->addAction(actHideColumn); this->addAction(actShowAllColumns);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->resizeSections(QHeaderView::Stretch);
//    resizeRowsToContents();
}

void SPRSpectrumListTable2::onCurrentPosChanged(int row, int){
//    storeCurrentItem = model->getSpectrumItem(row, typeData);
    emit rowSelectedChecked3(getSelectedItems(), spectrums->at(row));
}

ISPRModelData *SPRSpectrumListTable2::setModelData(SPRSpectrumListItemsModel *_model, SPRTypeSpectrumSet _type)
{
    if(model != _model){
        if(model){
            disconnect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        model = _model;
        connect(model, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));

        this->typeData = _type;
        spectrums = model->getSpectrumsModel(typeData);

        widgetsShow();
    }
    return model;
}

QList<SPRSpectrumItemModel *> SPRSpectrumListTable2::getSelectedItems(){
    QList<SPRSpectrumItemModel*> res;
    for(int row=0; row<rowCount();row++){
        FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(cellWidget(row, 0));
        if(fc && fc->isSelect()){
            SPRSpectrumItemModel* item = dynamic_cast<SPRSpectrumItemModel*>(fc->getModelPrt());
            if(item && spectrums->contains(item))
                res.push_back(item);
        }

    }
    return res;
}

QList<SPRSpectrumItemModel *> SPRSpectrumListTable2::getAllItems()
{
    return *spectrums;
}
void SPRSpectrumListTable2::resizeEvent(QResizeEvent *event)
{
    QTableWidget::resizeEvent(event);
}

void SPRSpectrumListTable2::setTableWidgetItemProperty(QTableWidgetItem* wi, int col, QString _text, QString _tooltype, bool _editable){
//    QAbstractItemView::
    Qt::ItemFlags flags = wi->flags();
    if(!_editable){
        flags &= ~Qt::ItemIsEditable;
        wi->setFlags(flags);
    }
    if(!_text.isEmpty()){
        wi->setText(_text);
        QString sText = _text + "999";
        QString sHead = horizontalHeaderItem(col)->text() + "999";
        int wText = fontMetrics().width(sText);
        int wHead = fontMetrics().width(sHead);
        QSize size = wi->sizeHint(); size.setWidth(std::max(wText, wHead));
        wi->setSizeHint(size);
        wi->setTextAlignment(Qt::AlignCenter);
        horizontalHeader()->resizeSection(col, wi->sizeHint().width());
    }
    if(!_tooltype.isEmpty()){
        wi->setToolTip(_tooltype);
    }
}

void SPRSpectrumListTable2::setRowItems(SPRSpectrumItemModel *data){
    if(!spectrums->contains(data)){
        return;
    }
    int row = spectrums->indexOf(data);
    if(row < 0 || row >= rowCount()){
        return;
    }
    SpectrumItemData *d = data->getSpectrumData();
    if(d){
        double t = *d->time_in_sec > 1e-6 ? *d->time_in_sec : 1;
        FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(cellWidget(row, 0));
        if(!fc){
            fc = new FirstCollumn2(this);
//            fc->setProperty("model", QVariant(dynamic_cast<const QObject*>(data)));
            connect(fc, &FirstCollumn2::colorChanged, data, &SPRSpectrumItemModel::setSpectrumColor);
            connect(fc, &FirstCollumn2::selectedRow, this, &SPRSpectrumListTable2::onRowSelect);
            connect(fc, &FirstCollumn2::deleteRow, this, &SPRSpectrumListTable2::onDeleteRow);
            setCellWidget(row, 0, fc);
        }
        fc->setParams(row, QString::number(row+1), d->getColor(), storeCheckedItems.contains(data), true, true, true);
        fc->setModelPrt(data);

        QTableWidgetItem *wi = item(row, 1);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 1, wi);
        }
        setTableWidgetItemProperty(wi, 1, QString::number(data->getSpectrumThread()+1), tr("Номер ручья"), false);

        wi = item(row, 2);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 2, wi);
        }
        setTableWidgetItemProperty(wi, 2, QString(d->name), tr("Наименование спектра"), true);

        wi = item(row, 3);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 3, wi);
        }
        setTableWidgetItemProperty(wi, 3, QString::number(*d->summ / t, 'f', 0), tr("Интеграл"), false);

        wi = item(row, 4);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 4, wi);
        }
        setTableWidgetItemProperty(wi, 4, QString::number(*d->H1, 'f', 3), tr("Параметр Н1"), false);

        wi = item(row, 5);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 5, wi);
        }
        setTableWidgetItemProperty(wi, 5, QString::number(*d->H2, 'f', 3), tr("Параметр Н2"), false);

        wi = item(row, 6);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 6, wi);
        }
        setTableWidgetItemProperty(wi, 6, QString::number(*d->H3, 'f', 3), tr("Параметр Н3"), false);

        wi = item(row, 7);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 7, wi);
        }
//        setTableWidgetItemProperty(wi, 7, QString::number(*d->center, 'f', 0), tr("Центр"), false);
        double center = data->getSpectrumCenter(false, Ns);
        setTableWidgetItemProperty(wi, 7, QString::number(center, 'f', 0), tr("Геометрический центр области Ns спектра"), false);

        wi = item(row, 8);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 8, wi);
        }
        setTableWidgetItemProperty(wi, 8, QString::number(*d->peak, 'f', 0), tr("Пик (%1)").arg(*d->peak_value), false);

        wi = item(row, 9);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 9, wi);
        }
//        wi->setText(QString::number(*d->Rs, 'f', 0)); wi->setToolTip(tr("Rs"));
        setTableWidgetItemProperty(wi, 9, QString::number(*d->Rs, 'f', 0), tr("Rs"), false);

        QString date; date.sprintf("%02d.%02d.%4d %02d:%02d",
                                   *d->day, *d->month, *d->year,
                                   *d->hours, *d->min);
        wi = item(row, 10);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 10, wi);
        }
        setTableWidgetItemProperty(wi, 10, date, tr("Дата и время получения спектра"), false);


        wi = item(row, 11);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 11, wi);
        }
        setTableWidgetItemProperty(wi, 11, QString::number(*d->time_in_sec, 'f', 0), tr("Время измерения спектра (сек.)"), false);

        SPRMainModel *mm = model->getMainModel();
        if(!mm){
            return;
        }
        SPRElementsModel *em = mm->getElementsModel();

        int col = 12;
        foreach(EnumElements el, em->getUsedlElementKeys()){
            wi = item(row, col);
            if(!wi){
                wi = new QTableWidgetItem();
                setItem(row, col, wi);
            }
            QString nameZone = data->getZones()->getElementsProperty()->getElementProperty(el)->fName->getData();
            setTableWidgetItemProperty(wi, col, QString::number(*d->elementsSumm[el] / t, 'f', 0), tr("Интеграл в зоне %1").arg(nameZone), false);
            col++;
        }
        while(col < 18){
            wi = item(row, col);
            if(!wi){
                wi = new QTableWidgetItem();
                setItem(row, col, wi);
            }
            QString nameZone = QString(tr("Не используется"));
            setTableWidgetItemProperty(wi, col, "--", QString(tr("Не используется")), false);
            col++;
        }
    }


}

void SPRSpectrumListTable2::widgetsShow()
{

    if(model){

        spectrums = model->getSpectrumsModel(typeData);
        storeCheckedItems = getSelectedItems();
        QRegion currentRegion = this->viewport()->visibleRegion();
//        storeCurrentItem = model->getSpectrumItem(currentRow(), typeData);


//        this->clear();
        this->setRowCount(spectrums->size());
        this->setColumnCount(18);
        this->setItemDelegateForColumn(2, new SPRSpectrumListTable2Delegate(this));

        SPRMainModel *mm = model->getMainModel();
        QStringList hHeaders = commonHTitles;
        if(mm){
            SPRElementsModel *em = mm->getElementsModel();
//            em->getElements()
            foreach(const SPRElementsProperty *el, em->getElements()){
                hHeaders.append(el->sName->getData());
            }
        }
        int col = hHeaders.size();
        while(col++ < 18){
            hHeaders.append("");
        }
        setHorizontalHeaderLabels(hHeaders);

        foreach(SPRSpectrumItemModel* sp, *spectrums){
            setRowItems(sp);
        }
        resizeColumnsToContents();

        if(storeCheckedItemsNum >=0 && storeCheckedItemsNum <= rowCount()){
            scrollToItem(item(storeCheckedItemsNum, 2));
            storeCheckedItemsNum = -1;
        }
        foreach(SPRSpectrumItemModel* sp, storeCheckedItems){
            if(spectrums->contains(sp)){
                int row = spectrums->indexOf(sp);
                FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(cellWidget(row, 0));
                if(fc){
                    fc->setSelect(true);
                }
            }
        }
    }
}

void SPRSpectrumListTable2::myHideColumn(bool)
{
    int col = currentColumn();
    if(col < 0 || col >= columnCount()){
        return;
    }
    hideColumn(currentColumn());
}

void SPRSpectrumListTable2::myShowColumns(bool)
{
    for(int col=0; col < columnCount(); col++){
        showColumn(col);
    }

}

void SPRSpectrumListTable2::onModelChanget(IModelVariable *var)
{
   if(sender() == model){
        if(var == model){
            widgetsShow();
            return;
        } else {
            SPRSpectrumItemModel *spect = dynamic_cast<SPRSpectrumItemModel*>(var);
            if(spect){
                setRowItems(spect);
                resizeColumnsToContents();
                return;
            }
        }
    }

}


void SPRSpectrumListTable2::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
//    QVector<sectionStates*> sectionsStates;
//    for(int col=0; col<columnCount(); col++){
//        sectionStates *s = new sectionStates(horizontalHeader()->sectionSize(col),
//                         horizontalHeader()->sectionsHidden());
//        sectionsStates.append(s);
//    }
//    QVariant var; var.setValue<QVector<sectionStates*>>(sectionsStates);
    mainSettings->beginGroup(groupId);
    mainSettings->setValue(objectName(), horizontalHeader()->saveState());
    mainSettings->endGroup();
}

void SPRSpectrumListTable2::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
//    QVector<sectionStates*> sectionsStates;
    mainSettings->beginGroup(groupId);
    horizontalHeader()->restoreState(mainSettings->value(objectName()).toByteArray());
    mainSettings->endGroup();

//    for(int col=0; col<sectionsStates.size() && col<columnCount(); col++){
//        horizontalHeader()->restoreState()
//    }

}
