#include "sprseparateforcomplexwidget.h"

#include <QMenu>
#include <QAction>

SPRSeparateForComplexWidget::SPRSeparateForComplexWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
//    resize()
}

ISPRModelData *SPRSeparateForComplexWidget::setModelData(ISPRModelData *_model)
{
    model = _model->getMainModel();

    if(model){
        ui.gInput->setModelData(model,/* model->getHistoryModel(), */countInputInSec, true);
        ui.gOutput->setModelData(model,/* model->getHistoryModel(), */percentConcentrate2Input, true);

        ui.wControl->setModelData(model);
        ui.wControl->setFullMode(true, true);

        ui.wControl->setVerticalMode(false);

        ui.wControl->widgetsShow();
    }

//    this->
}


bool SPRSeparatorForComplexSubWindow::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type())
    {
        case QEvent::Close:
        {
            SPRSeparatorForComplexSubWindow * subWindow = dynamic_cast<SPRSeparatorForComplexSubWindow*>(watched);
            if(subWindow && subWindow == this){
                emit closeSeparator(this);
                event->ignore();
                return true;
            }

            SPRSeparateForComplexWidget *sep = dynamic_cast<SPRSeparateForComplexWidget*>(watched);
            if(sep && sep == this->widget()){
                emit closeSeparator(this);
                event->ignore();
                return true;
            }
        }
            break;
        default:
            qt_noop();
    }
    return QObject::eventFilter(watched, event);
}


void SPRSeparatorForComplexSubWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
}

void SPRSeparatorForComplexSubWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit selectSeparator(this);
}


void SPRSeparatorForComplexSubWindow::widgetsShow()
{
}

ISPRModelData *SPRSeparatorForComplexSubWindow::getModelData()
{
}

void SPRSeparatorForComplexSubWindow::onModelChanget(IModelVariable *)
{
}

void SPRSeparatorForComplexSubWindow::storeWidgetsStates(QString groupId, QSettings *mainSettings)
{
    mainSettings->beginGroup(groupId);
    mainSettings->setValue("sw", saveGeometry());
    mainSettings->endGroup();
}

void SPRSeparatorForComplexSubWindow::restoreWidgetsStates(QString groupId, QSettings *mainSettings)
{
    QList<QAction*> la = this->systemMenu()->actions();
    qDebug() << "******************** actions mdi menu **************";
    foreach(QAction *act, la){
        qDebug() << "act : " << act;
        if(act->text().endsWith("lose")){
            systemMenu()->removeAction(act);
//            act->setEnabled(false);
        }
    }

    mainSettings->beginGroup(groupId);
    restoreGeometry(mainSettings->value("sw").toByteArray());
    mainSettings->endGroup();
}


