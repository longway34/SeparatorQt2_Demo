#include "sprzonesranges3.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QHeaderView>

#include "models/sprmainmodel.h"

SPRZonesRanges3Delegate::SPRZonesRanges3Delegate(QObject *parent): QStyledItemDelegate(parent)
{
}

QWidget *SPRZonesRanges3Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QIntValidator *validator = new QIntValidator(0, 255, editor);
    editor->setValidator(validator);
    QVariant qRow(index.row());
//    qRow.setValue<int>(index.row());
    editor->setProperty("row", qRow);

    SPRZonesRanges3 *table = dynamic_cast<SPRZonesRanges3*>(parent->parent());
    if(table){
        connect(editor, &QLineEdit::editingFinished, table, &SPRZonesRanges3::onChangeValueWithDelegate);
    }
    return editor;
}

void SPRZonesRanges3Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *ed = dynamic_cast<QLineEdit*>(editor);
    QString txt = index.model()->data(index,Qt::EditRole).toString();
//    qDebug() << "setEditorData :" << index.model()->data(index,Qt::EditRole) << " (" <<  index.model()->data(index,Qt::EditRole).toString() << ")";
    ed->setText(txt);
}

void SPRZonesRanges3Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *ed = dynamic_cast<QLineEdit*>(editor);
    QString txt = ed->text();
//    qDebug() << "setModelData :" << ed->text() ;
    model->setData(index, QVariant(txt));
}

/**
 * @brief SPRZonesRanges3::getLastCurrent
 * @return
 */

//QObject *SPRZonesRanges2::getLastCurrent() const
//{
//    return lastCurrent;
//}

SPRSpectrumZonesModel *SPRZonesRanges3::getZoneModel() const
{
    return zoneModel;
}

SPRZonesRanges3::SPRZonesRanges3(QWidget *parent):
    QTableWidget (parent),
    ISPRWidget(),
    zoneModel(nullptr),
    firstCollumnVisible(true)
//    tableDelegate(nullptr)
{
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(true);

//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    installEventFilter(this);
    setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
//    setAlternatingRowColors(true);

    tableDelegate = new SPRZonesRanges3Delegate(this);
    setRowCount(MAX_SPR_SPECTOR_ELEMENTS);
    setColumnCount(3);
}

EnumElements SPRZonesRanges3::getCurrentElement(bool *isOk){
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

SPRTypeMinMaxField3 SPRZonesRanges3::getCurrentFieldType()
{
    SPRTypeMinMaxField3 res = noName3;
    QModelIndex cIndex = currentIndex();
    if(cIndex.isValid()){
        if(cIndex.column() == 1){
            res = minField3;
        } else if(cIndex.column() == 2){
            res = maxField3;
        }
    }
    return res;
}

bool SPRZonesRanges3::getTableData(EnumElements el, SPRTypeMinMaxField3 type, int *res)
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

int SPRZonesRanges3::getRowByElement(EnumElements el){
    int res = -1;
    foreach(SPRElementsProperty *element, listElements){
        if(el == element->key->getData()){
            res = listElements.indexOf(element);
            break;
        }
    }
    return res;
}

QTableWidgetItem *SPRZonesRanges3::getTableWidgetItem(EnumElements el, SPRTypeMinMaxField3 type)
{
    QTableWidgetItem *res = nullptr;
    int row = getRowByElement(el);
    if(row >= 0 && row < rowCount()){
        if(type == minField3){
            res = item(row, 1);
        } else if(type == maxField3){
            res = item(row, 2);
        }
    }
    return res;
}

void SPRZonesRanges3::setModelData(SPRSpectrumZonesModel *_model){
    if(_model){
        if(zoneModel){
            disconnect(zoneModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        }
        zoneModel = _model;
        connect(zoneModel, SIGNAL(modelChanget(IModelVariable*)), this, SLOT(onModelChanget(IModelVariable*)));
        connect(this, &QTableWidget::currentItemChanged, this, &SPRZonesRanges3::onCurrentItemChanged);
    }
    widgetsShow();
}

SPRZonesRanges3::~SPRZonesRanges3(){
    clear();
}

void SPRZonesRanges3::setRowItemsWithDelegate(int row, SPRElementsProperty *element){
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
            connect(fc, &FirstCollumn2::deleteRow, this, &SPRZonesRanges3::onDeleteElements);
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

void SPRZonesRanges3::widgetsShow()
{
    if(zoneModel){

        clear();
        listElements.clear();

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

        setItemDelegateForColumn(1, tableDelegate);
        setItemDelegateForColumn(2, tableDelegate);

        if(!firstCollumnVisible){
            hideColumn(0);
        } else {
            showColumn(0);
        }

        resizeRowsToContents();

        QSize sss = sizeHint();
        setMinimumSize(sss);
        resize(sizeHint());
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

//        updateGeometry();
        emit updateSize();
    }
}

void SPRZonesRanges3::onDeleteElements(IModelVariable *element){
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



QSize SPRZonesRanges3::sizeHint() const
{

    QSize res = QTableWidget::sizeHint();

    int h = QTableWidget::sizeHint().height();

    int hc = lineWidth();
//    if(horizontalHeader()->isVisible()){
        hc += horizontalHeader()->height();
//    }
    for(int row=0; row < rowCount(); row++){
        hc += verticalHeader()->sectionSize(row);
        hc += lineWidth();
    }

    int wc = fontMetrics().width("9999999") * 2 + lineWidth()*3;
        if(rowCount() > 0 && columnCount() > 0){
            if(firstCollumnVisible){
                QWidget *wi = cellWidget(0, 0);
                if(wi){
                    FirstCollumn2 *fc = dynamic_cast<FirstCollumn2*>(wi);
                    if(fc){
                        wc += fc->sizeHint().width() + lineWidth() + 5;
                    }
                }
            }
        }
    return QSize(wc, hc);
}

uint8_t SPRZonesRanges3::getThread()
{
    if(zoneModel){
        return zoneModel->getThreadNumber();
    }
    return 255;
}

void SPRZonesRanges3::onChangeValueWithDelegate(){
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    if(le){
        int row = le->property("row").value<int>();
            EnumElements el;
                if(row >= 0 && row < listElements.size()){
                    el = listElements[row]->key->getData();
                }
            QString sMin = "-1", sMax = "-1";
            QModelIndex iMin = this->model()->index(row, 1);

            int min = -1, max = -1;
            bool bMin = false, bMax = false;
            QTableWidgetItem *twiMin = itemFromIndex(iMin);
            if(twiMin){
                sMin = twiMin->data(Qt::DisplayRole).toString();
                min = sMin.toInt(&bMin);
            }
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

void SPRZonesRanges3::onChangeValue()
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
                if(minOk && maxOk){
                    zoneModel->setMinMax(el, min, max);
                }
            }
        }
    }
}

void SPRZonesRanges3::onModelChanget(IModelVariable *element)
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

void SPRZonesRanges3::onCurrentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(current){
        emit gotFocus(this, this);
    }
}

bool SPRZonesRanges3::eventFilter(QObject *watched, QEvent *event)
{

    if(event->type() == QEvent::FocusIn){

        emit gotFocus(this, watched);
        return false;
    }
}


void SPRZonesRanges3::resizeEvent(QResizeEvent *event)
{
    QSize sss = sizeHint();
    setMinimumSize(sss);
//    setMinimumSize(sss);
    resize(sss);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    adjustSize();
    show();
    emit updateSize();
//    QTableWidget::resizeEvent(event);
}
