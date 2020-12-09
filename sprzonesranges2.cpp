#include "sprzonesranges2.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QHeaderView>

#include "models/sprmainmodel.h"

SPRZonesRanges2Delegate::SPRZonesRanges2Delegate(QObject *parent): QStyledItemDelegate(parent)
{
}

QWidget *SPRZonesRanges2Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QIntValidator *validator = new QIntValidator(0, 255, editor);
    editor->setValidator(validator);
    QVariant qRow(index.row());
//    qRow.setValue<int>(index.row());
    editor->setProperty("row", qRow);

    SPRZonesRanges2 *table = dynamic_cast<SPRZonesRanges2*>(parent->parent());
    if(table){
        connect(editor, &QLineEdit::editingFinished, table, &SPRZonesRanges2::onChangeValueWithDelegate);
    }
    return editor;
}

void SPRZonesRanges2Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *ed = dynamic_cast<QLineEdit*>(editor);
    QString txt = index.model()->data(index,Qt::EditRole).toString();
    qDebug() << "setEditorData :" << index.model()->data(index,Qt::EditRole) << " (" <<  index.model()->data(index,Qt::EditRole).toString() << ")";
    ed->setText(txt);
}

void SPRZonesRanges2Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *ed = dynamic_cast<QLineEdit*>(editor);
    QString txt = ed->text();
    qDebug() << "setModelData :" << ed->text() ;
    model->setData(index, QVariant(txt));
}

/**
 * @brief SPRZonesRanges2::getLastCurrent
 * @return
 */

//QObject *SPRZonesRanges2::getLastCurrent() const
//{
//    return lastCurrent;
//}

SPRSpectrumZonesModel *SPRZonesRanges2::getZoneModel() const
{
    return zoneModel;
}

SPRZonesRanges2::SPRZonesRanges2(QWidget *parent):
    QTableWidget (parent),
    ISPRWidget(),
    zoneModel(nullptr),
    firstCollumnVisible(true)
//    tableDelegate(nullptr)
{
    verticalHeader()->setVisible(false);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    installEventFilter(this);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
//    setAlternatingRowColors(true);

    tableDelegate = new SPRZonesRanges2Delegate(this);
}

EnumElements SPRZonesRanges2::getCurrentElement(bool *isOk){
    EnumElements res = Ns;
    if(isOk) *isOk = false;
    QModelIndex cIndex = currentIndex();
    if(cIndex.isValid()){
        if(cIndex.row() < listElements.size()){
            res = listElements.at(cIndex.row())->key->getData();
            if(isOk) *isOk = true;
        }
    }
    return res;
}

SPRTypeMinMaxField SPRZonesRanges2::getCurrentFieldType()
{
    SPRTypeMinMaxField res = noName;
    QModelIndex cIndex = currentIndex();
    if(cIndex.isValid()){
        if(cIndex.column() == 1){
            res = minField;
        } else if(cIndex.column() == 2){
            res = maxField;
        }
    }
    return res;
}

bool SPRZonesRanges2::getTableData(EnumElements el, SPRTypeMinMaxField type, int *res)
{
    if(res) *res = -1;
    bool bRes = false;
    QTableWidgetItem *twi = getTableWidgetItem(el, type);
    if(twi){
        int r = QString(twi->text()).toInt(&bRes);
        if(bRes){
            if(res) *res = r;
        }
    }
    return bRes;
}

int SPRZonesRanges2::getRowByElement(EnumElements el){
    int res = -1;
    foreach(SPRElementsProperty *element, listElements){
        if(el == element->key->getData()){
            res = listElements.indexOf(element);
            break;
        }
    }
    return res;
}

QTableWidgetItem *SPRZonesRanges2::getTableWidgetItem(EnumElements el, SPRTypeMinMaxField type)
{
    QTableWidgetItem *res = nullptr;
    int row = getRowByElement(el);
    if(row >= 0 && row < rowCount()){
        if(type == minField){
            res = item(row, 1);
        } else if(type == maxField){
            res = item(row, 2);
        }
    }
    return res;
}

void SPRZonesRanges2::setModelData(SPRSpectrumZonesModel *_model){
    if(_model){
        if(zoneModel){
            disconnect(zoneModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        zoneModel = _model;
        connect(zoneModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        connect(this, &QTableWidget::currentItemChanged, this, &SPRZonesRanges2::onCurrentItemChanged);
    }
    widgetsShow();
}

SPRZonesRanges2::~SPRZonesRanges2(){
    clear();
}

void SPRZonesRanges2::setRowItemsWithDelegate(int row, SPRElementsProperty *element){
    if(element && zoneModel){
        EnumElements el = element->key->getData();
        QColor color = element->color->getData();
        QWidget *w = cellWidget(row, 0);
        FirstCollumn2 *fc = nullptr;
        int wText = fontMetrics().width("9999999");
        if(w){
           fc = dynamic_cast<FirstCollumn2*>(cellWidget(row, 0));
        }
        if(!fc){
            fc = new FirstCollumn2(element->sName->getData(), true, color, false, true, this);
            fc->setModelPrt(element);

            connect(fc, &FirstCollumn2::colorChanged, element, &SPRElementsProperty::onChangeColor);
            connect(fc, &FirstCollumn2::deleteRow, this, &SPRZonesRanges2::onDeleteElements);
            fc->setProperty("row", QVariant(row));
            fc->setProperty("col", QVariant(0));
            setCellWidget(row, 0, fc);

        }
        fc->setParams(row, element->sName->getData(), color, false, true, true, false);
//        fc->resize(fc->minimumWidth(), fc->minimumHeight());
        horizontalHeader()->resizeSection(0, fc->minimumWidth());

        QTableWidgetItem *wi = item(row, 1);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 1, wi);
        }
        wi->setSizeHint(QSize(wText, wi->sizeHint().height()));
//        wi->setTextAlignment(Qt::AlignHCenter);
        wi->setTextAlignment(Qt::AlignCenter);
        uint value = zoneModel->getElements()[el]->min->getData();
        wi->setText(QString::number(value));
        QString tt = tr("Минимальное значение области %1").arg(element->fName->getData());
        wi->setToolTip(tt);

        wi = item(row, 2);
        if(!wi){
            wi = new QTableWidgetItem();
            setItem(row, 2, wi);
        }
        wi->setSizeHint(QSize(wText, wi->sizeHint().height()));
        value = zoneModel->getElements()[el]->max->getData();
        wi->setText(QString::number(value));
        wi->setTextAlignment(Qt::AlignCenter);
        tt = tr("Максимальное значение области %1").arg(element->fName->getData());
        wi->setToolTip(tt);

        horizontalHeader()->resizeSection(1, wText);
        horizontalHeader()->resizeSection(2, wText);
    }
}

void SPRZonesRanges2::setRowItems(int row, SPRElementsProperty *element){
    //    if(this->firstCollumnVisible){
    if(element && zoneModel){
        EnumElements el = element->key->getData();
        QColor color = element->color->getData();
        QWidget *w = cellWidget(row, 0);
        FirstCollumn2 *fc = nullptr;
        if(w){
           fc = dynamic_cast<FirstCollumn2*>(cellWidget(row, 0));
        }
        if(!fc){
            fc = new FirstCollumn2(element->sName->getData(), true, color, false, true, this);
            fc->setModelPrt(element);

            connect(fc, &FirstCollumn2::colorChanged, element, &SPRElementsProperty::onChangeColor);
            connect(fc, &FirstCollumn2::deleteRow, this, &SPRZonesRanges2::onDeleteElements);
            fc->setProperty("row", QVariant(row));
            fc->setProperty("col", QVariant(0));
            setCellWidget(row, 0, fc);

        }
        fc->setParams(row, element->sName->getData(), color, false, true, true, false);
    //    }

        QWidget *wi = cellWidget(row, 1);
        QLineEdit *le = nullptr;
        if(!wi){
            le = new QLineEdit(this);
            le->setValidator(new QIntValidator(0, 255, le));
            connect(le, &QLineEdit::editingFinished, this, &SPRZonesRanges2::onChangeValue);
            setCellWidget(row, 1, le);
        } else {
            le = dynamic_cast<QLineEdit*>(wi);
        }
        QString tt = tr("Минимальное значение области %1").arg(element->fName->getData());
        setTableWidgetItemProperty(le, row, 1, QString::number(zoneModel->getElements()[el]->min->getData()), tt);

        wi = cellWidget(row, 2);
        if(!wi){
            le = new QLineEdit(this);
            le->setValidator(new QIntValidator(0, 255, le));
            connect(le, &QLineEdit::editingFinished, this, &SPRZonesRanges2::onChangeValue);
            setCellWidget(row, 2, le);
        } else {
            le = dynamic_cast<QLineEdit*>(wi);
        }
        tt = tr("Максимальное значение области %1").arg(element->fName->getData());
        setTableWidgetItemProperty(le, row, 2, QString::number(zoneModel->getElements()[el]->max->getData()), tt);
    }
}

void SPRZonesRanges2::setTableWidgetItemProperty(QLineEdit* le, int row, int col, QString _text, QString _tooltype){
    if(le){
        if(!_text.isEmpty()){
            le->blockSignals(true); le->setText(_text); le->blockSignals(false);
            QVariant vCol; vCol.setValue<int>(col);
            le->setProperty("col", vCol);
            QVariant vRow; vRow.setValue<int>(row);
            le->setProperty("row", vRow);

            le->setFrame(false);
    //        wi->setData(Qt::DisplayRole,QVariant(_text));
            QString sText = _text + "9999";
            QString sHead = horizontalHeaderItem(col)->text() + "9999";
            int wText = fontMetrics().width(sText);
            int wHead = fontMetrics().width(sHead);
            QSize size = le->sizeHint(); size.setWidth(std::max(wText, wHead));
            le->setMaximumSize(size);
            le->setAlignment(Qt::AlignHCenter);
            horizontalHeader()->resizeSection(col, le->minimumSize().width());
        }
        if(!_tooltype.isEmpty()){
            le->setToolTip(_tooltype);
    //        wi->setData(Qt::ToolTipRole, QVariant(_tooltype));
        }
    }

}

void SPRZonesRanges2::widgetsShow()
{

//    qDebug() << "SPRZonesRanges::widgetsShow(): name: "<< this->objectName();

    if(zoneModel){
//        while(rowCount() > 0){
//            removeRow(0);
//        }
//        blockSignals(true);
        clear();
        listElements.clear();

//        setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

        setRowCount(zoneModel->getElementsProperty()->getElements().size());
        setColumnCount(3);
        QStringList hHeaders;

        hHeaders << "*" << "<<" << ">>";
        setHorizontalHeaderLabels(hHeaders);

        verticalHeader()->setVisible(false);

        int row = 0;
        foreach(SPRElementsProperty *element, zoneModel->getElementsProperty()->getElements().values()){
            setRowItemsWithDelegate(row, element);
            listElements.append(element);
            row++;
        }

//        if(!tableDelegate){
//            tableDelegate = new SPRZonesRanges2Delegate(this);
//        }

        setItemDelegateForColumn(1, tableDelegate);
        setItemDelegateForColumn(2, tableDelegate);

        if(!firstCollumnVisible){
            hideColumn(0);
        } else {
            showColumn(0);
        }

//        resizeColumnsToContents();
        resizeRowsToContents();

//        this->show();
        updateGeometry();
        emit updateSize();
//        setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    }
}

void SPRZonesRanges2::onDeleteElements(IModelVariable *element){
    SPRElementsProperty *em = dynamic_cast<SPRElementsProperty*>(element);
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
            SPRMainModel *mm = zoneModel->getMainModel();
            if(mm){
                SPRSettingsFormulaModel *fm = mm->getSettingsFormulaModel();
                int numFormula = -1;
                if(fm->isUsed(em->key->getData(), &numFormula)){
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                            "Он используется в расчете параметра H%2").arg(em->fName->getData()).arg(numFormula+1));
                    return;
                }
                if(mm->getSettingsControlModel()->controlArea->getData() == em->key->getData()){
                    QMessageBox::warning(nullptr, tr("Ошибка"), tr("Элемент %1 удалить нельзя...\n"
                                            "Он используется для контроля корреляции...").arg(em->fName->getData()));
                    return;

                }
            }
        }
        EnumElements el = em->key->getData();
        zoneModel->deleteElement(el);
    }
}
QSize SPRZonesRanges2::sizeHint() const
{

    QSize res = QTableWidget::sizeHint();

//    return res;

    int h = QTableWidget::sizeHint().height();
    int wc = fontMetrics().width("9999999") * 2 + lineWidth()*3;
        if(rowCount() > 0 && columnCount() > 0){
            if(firstCollumnVisible){
                QWidget *wi = cellWidget(0, 0);
                if(wi){
                    FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(wi);
                    if(fc){
//                                wc = fc->sizeHint().width();
                        wc += fc->sizeHint().width() + lineWidth();
                    }
                }
            }
        }
    return QSize(wc, h);

}

uint8_t SPRZonesRanges2::getThread()
{
    if(zoneModel){
        return zoneModel->getThreadNumber();
    }
    return 255;
}

//QSize SPRZonesRanges2::sizeHint() const
//{
////    return QTableWidget::sizeHint();
//    if(zoneModel){
//        int h = QTableWidget::sizeHint().height();
////        for(int row=0; row<model->elements.size();row++){
////            h += rowHeight(row) + 5* lineWidth();
////        }
//        int w_max = 0;
//        for(int row=0; row<rowCount();row++){
//            int w = lineWidth();
//            for(int col=0; col<columnCount(); col++){
//                int wc = 0;
//                if(col == 0){
//                    if(firstCollumnVisible && col == 0){
//                        QWidget *wi = cellWidget(row, col);
//                        if(wi){
//                            FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(wi);
//                            if(fc){
////                                wc = fc->sizeHint().width();
//                                wc = fc->minimumWidth();
//                            }
//                        }
//                    }
//                } else {
//                    wc = fontMetrics().width("9999999");
////                    QTableWidgetItem *twi = item(row, col);
////                    if(twi){
////                      wc = item(row, col)->sizeHint().width();
////                    } else {
////                      wc = fontMetrics().width("9999999");
////                    }
//                }
//                w += wc;
//            }
//            if(w_max < w) w_max = w;
//        }
//        QSize res = QSize(w_max+lineWidth()+1, h);
//        return res;
//    } else {
//        return QTableWidget::sizeHint();
//    }
//}

//void SPRZonesRanges2::resizeEvent(QResizeEvent *event)
//{
//    QTableWidget::resizeEvent(event);

//    int newSizeWidth = event->size().width();
//    int colCount = 0; int colResizeNum = columnCount();

//    if(rowCount() > 0){
//        if(firstCollumnVisible){
//            newSizeWidth -= columnWidth(0);
//            colCount++; colResizeNum--;
//        }
//        for(int col=colCount; col<columnCount(); col++){
//            int w = columnWidth(col);
//            horizontalHeader()->resizeSection(col, newSizeWidth / colResizeNum);
//        }
//    }
//    horizontalHeader()->resizeSections(QHeaderView::Fixed);
//}

void SPRZonesRanges2::onChangeValueWithDelegate(){
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    if(le){
        int row = le->property("row").value<int>();
//        if(index.isValid()){
            EnumElements el;
//            if(index.isValid()){
//                if(index.row() >= 0 && index.row() < listElements.size()){
                if(row >= 0 && row < listElements.size()){
                    el = listElements[row]->key->getData();
                }
//            }
            QString sMin = "-1", sMax = "-1";
//            QModelIndex iMin = this->model()->index(index.row(), 1);
            QModelIndex iMin = this->model()->index(row, 1);
            int min = -1, max = -1;
            bool bMin = false, bMax = false;
            QTableWidgetItem *twiMin = itemFromIndex(iMin);
            if(twiMin){
                sMin = twiMin->data(Qt::DisplayRole).toString();
                min = sMin.toInt(&bMin);
            }
//            QModelIndex iMax = this->model()->index(index.row(), 2);
            QModelIndex iMax = this->model()->index(row, 2);
            QTableWidgetItem *twiMax = itemFromIndex(iMax);
            if(twiMax){
                sMax = twiMax->data(Qt::DisplayRole).toString();
                max = sMax.toInt(&bMax);
            }
            if(bMin && bMax){
                zoneModel->setMinMax(el, min, max);
            }
        }
//    }
}

void SPRZonesRanges2::onChangeValue()
{
    if(sender()){
        QLineEdit *le = qobject_cast<QLineEdit*>(sender());
        if(le){
            QVariant vProp = le->property("row");
            int elNum = le->property("row").value<int>();
            EnumElements el = listElements.at(elNum)->key->getData();
            int row = getRowByElement(el);
            int min, max; bool minOk = false, maxOk = false;
            QString sMin, sMax;

            if(row >= 0 && row < rowCount()){

                QLineEdit *leMin = qobject_cast<QLineEdit*>(cellWidget(row, 1));
                if(leMin){
                    sMin = leMin->text();
                    min = sMin.toInt(&minOk);
                }

                QLineEdit *leMax = qobject_cast<QLineEdit*>(cellWidget(row, 2));
                if(leMax){
                    sMax = leMax->text();
                    max = sMax.toInt(&maxOk);
                }
    //        min = item(index.row(), 1)->data(Qt::EditRole).toInt();
    //        max = item(index.row(), 2)->data(Qt::EditRole).toInt();
    //        QString txt = le->text();
                if(minOk && maxOk){
                    zoneModel->setMinMax(el, min, max);
                }
            }
        }
    }
}



//bool SPRZonesRanges2::eventFilter(QObject *watched, QEvent *event)
//{

//    if(event->type() == QEvent::FocusIn){
//        int row = watched->property("row").value<int>(); int col = watched->property("col").value<int>();
//        QModelIndex index = model()->index(row, col);
////        setCurrentCell(row, col, QItemSelectionModel::SelectCurrent);
//        QLineEdit *le = (QLineEdit*)watched;
//        QPalette pal = le->palette();
//        pal.setColor(le->backgroundRole(), QColor(Qt::lightGray));
//        le->setPalette(pal);
//        lastCurrent = watched;

//        if(lastCurrent)
//        qDebug()<<"SPRZonesRanges->lastCurrent: name "<< lastCurrent->objectName() << "event " << event->type() << " (" <<event->registerEventType(event->type())<<")";

//        emit gotFocus(this, (QLineEdit*)watched);
//        return false;
//    }
//    if(event->type() == QEvent::FocusOut){
////        int row = watched->property("row").value<int>(); int col = watched->property("col").value<int>();
////        lastCurrent = nullptr;
//        emit gotFocus(this, nullptr);
//        return false;

//    }
//    return false;
//}

//void SPRZonesRanges::focusInEvent(QFocusEvent *event)
//{
//    qDebug()<<"focusInEvent: name "<< event << "event got: " << event->gotFocus()<< "elent lost: "<< event->lostFocus()<< " (" <<event->registerEventType(event->type())<<")";
//}


void SPRZonesRanges2::onModelChanget(IModelVariable *element)
{
    if(zoneModel){
        if(!sender() || sender() == zoneModel){
//            QModelIndex lastCurrentIndex = currentIndex();
            widgetsShow();
//            if(lastCurrentIndex.isValid()){
////                setCurrentIndex(lastCurrentIndex);
//            }
        }
    }
}


//int SPRZonesRanges2::sizeHintForColumn(int column) const
//{
//    int wc = fontMetrics().width("9999999");

//    if(column == 0){
//        wc = 0;
//        if(firstCollumnVisible){
//            if(rowCount() > 0){
//                QWidget *w = cellWidget(0, column);
//                if(w){
//                    wc = w->minimumHeight();
//                }
//            }
//        }
//    }
//    return wc;
//}

void SPRZonesRanges2::onCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(current){
        emit gotFocus(this, this);
    }
}

bool SPRZonesRanges2::eventFilter(QObject *watched, QEvent *event)
{

    if(event->type() == QEvent::FocusIn){
//        int row = watched->property("row").value<int>(); int col = watched->property("col").value<int>();
//        QModelIndex index = model()->index(row, col);
//        setCurrentCell(row, col, QItemSelectionModel::SelectCurrent);
//        QLineEdit *le = (QLineEdit*)watched;
//        QPalette pal = le->palette();
//        pal.setColor(le->backgroundRole(), QColor(Qt::lightGray));
//        le->setPalette(pal);
//        lastCurrent = watched;

//        if(lastCurrent)
//        qDebug()<<"SPRZonesRanges->lastCurrent: name "<< lastCurrent->objectName() << "event " << event->type() << " (" <<event->registerEventType(event->type())<<")";

        emit gotFocus(this, watched);
        return false;
    }
}
