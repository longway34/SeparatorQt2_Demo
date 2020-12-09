#include "sprsettingsrentgenautosetdialog.h"
#include "qwt_plot_grid.h"

#include <QMessageBox>

void SPRSettingsRentgenAutoSetDialog::onChangeSelectedItemValue(QwtPlotItem *item, double distance, MovedItemPosition position){
    if(item && item == peakCurve){

        int new_peak = ui.spPeakPosition->value() + distance;
        if(rentgenModel){
            rentgenModel->peakPosition->setData(new_peak);
        } else {
            ui.spPeakPosition->blockSignals(true);
            ui.spPeakPosition->setValue(new_peak);
            ui.spPeakPosition->blockSignals(false);
        }
    }
}

void SPRSettingsRentgenAutoSetDialog::onCusorOverSelectItem(QwtPlotItem *item, MovedItemPosition){
    if(item){
        if(item == peakCurve){
            peakCurve->setPen(selPen);
        }
    } else {
        peakCurve->setPen(defPen);
    }
}

void SPRSettingsRentgenAutoSetDialog::widgetsShow()
{

    ui.tDeuCP->setRowCount(threads.size());
    ui.tPeakRS->setRowCount(threads.size());

    int row = 0;
    QString _forTitle = "";
    foreach (int th, threads) {

        if(row == 0){
            _forTitle = _forTitle + tr("%1").arg(th+1);
        } else {
            _forTitle = _forTitle + tr(", %1").arg(th+1);
        }
        ui.tDeuCP->insertRow(ui.tDeuCP->rowCount());
        ui.tPeakRS->insertRow(ui.tPeakRS->rowCount());

        QColor _color = QColor(Qt::white);
        QString _textTitle = QString(tr("Ручей %1")).arg(QString::number(th + 1));
        bool _deleteVisible = false, _colorVisible = true, _selectVisible = false;
        if(spectrums){
            QList<SPRSpectrumItemModel*> lst = spectrums->getSpectrumsItemByThread(th);
            if(lst.size() > 0){
                SPRSpectrumItemModel *spect = lst.last();
                if(spect){
                    _color = spect->getSpectrumColor();
                    _textTitle = spect->getSpectrumName();
                }
            }

        }

        FirstCollumn2 *fc = qobject_cast<FirstCollumn2*>(ui.tDeuCP->cellWidget(row, 0));
        if(!fc){
            fc = new FirstCollumn2();
            ui.tDeuCP->setCellWidget(row, 0, fc);
        }
        fc->setParams(row, _textTitle, _color, _selectVisible, _colorVisible, _deleteVisible, _selectVisible);
//        fc->widgetsShow();

//        ui.tDeuCP->setItem(ui.tDeuCP->rowCount()-1, 0, new QTableWidgetItem(QString::number(th+1)));
        QTableWidgetItem *wi = ui.tDeuCP->item(row, 1);
        if(!wi){
            wi = new QTableWidgetItem();
            ui.tDeuCP->setItem(row, 1, wi);
        }
        wi->setText(QString::number(tempDEU[th]));

        wi = ui.tDeuCP->item(row, 2);
        if(!wi){
            wi = new QTableWidgetItem();
            ui.tDeuCP->setItem(row, 2, wi);
        }
        wi->setText(QString::number(tempCP[th]));
//        ui.tDeuCP->setItem(ui.tDeuCP->rowCount()-1, 1, new QTableWidgetItem());
//        ui.tDeuCP->setItem(ui.tDeuCP->rowCount()-1, 2, new QTableWidgetItem());

        fc = qobject_cast<FirstCollumn2*>(ui.tPeakRS->cellWidget(row, 0));
        if(!fc){
            fc = new FirstCollumn2(_textTitle, _colorVisible, _color, _selectVisible, _deleteVisible, nullptr);
            ui.tPeakRS->setCellWidget(row, 0, fc);
        }
        fc->setParams(row, _textTitle, _color, _selectVisible, _colorVisible, _deleteVisible, _selectVisible);
//        fc->widgetsShow();

//        ui.tPeakRS->setItem(ui.tPeakRS->rowCount()-1, 0, new QTableWidgetItem(QString::number(th+1)));
        QString rs = "0"; QString peak = "0";
        if(spectrums){
            QList<SPRSpectrumItemModel *> lst = spectrums->getSpectrumsItemByThread(th);
            if(lst.size() > 0){
                SPRSpectrumItemModel* spec = lst.last();
                rs = QString::number(*spec->getSpectrumData()->Rs, 'f', 0);
                peak = QString::number(*spec->getSpectrumData()->peak);
            }
        }
        wi = ui.tPeakRS->item(row, 1);
        if(!wi){
            wi = new QTableWidgetItem();
            ui.tPeakRS->setItem(row, 1, wi);
        }
        wi->setText(peak);

        wi = ui.tPeakRS->item(row, 2);
        if(!wi){
            wi = new QTableWidgetItem();
            ui.tPeakRS->setItem(row, 2, wi);
        }
        wi->setText(rs);

        row++;
//        ui.tPeakRS->setItem(ui.tPeakRS->rowCount()-1, 1, new QTableWidgetItem(peak));
//        ui.tPeakRS->setItem(ui.tPeakRS->rowCount()-1, 2, new QTableWidgetItem(rs));
    }

    QString title = tr("Автоустановка кодов ДЭУ и ЦП для каналов %1").arg(_forTitle);
    this->setWindowTitle(title);

    ui.tDeuCP->resizeColumnsToContents();
    ui.tDeuCP->resizeRowsToContents();
    ui.tPeakRS->resizeColumnsToContents();
    ui.tPeakRS->resizeRowsToContents();

    ui.tmContrilDEU->setValue(controlDEU);
    ui.tmContrilDEU->setToolTip(QString::number(controlDEU,'f',1) + "%");

    ui.tmContrilCP->setValue(controlCP);
    ui.tmContrilCP->setToolTip(QString::number(controlCP,'f',1) + "%");

    ui.tmContrilAll->setValue(controlCP);
    ui.tmContrilAll->setToolTip(QString::number(controlAll,'f',1) + "%");

    ui.graphic->hideGraphics(this->peakCurve);
    ui.graphic->getCanvas()->replot();

    peakData[1].setY(ui.graphic->getCanvas()->axisInterval(QwtPlot::yLeft).maxValue());
    peakCurve->setSamples(peakData);
    peakCurve->attach(ui.graphic->getCanvas());

    ui.graphic->getCanvas()->replot();



    setGraphicTitle();
    onDblClickMouseEvent();
}

void SPRSettingsRentgenAutoSetDialog::setGraphicTitle(){
    QString titlePref = QString(tr("Спектры (%1)"));
    int thr = ui.graphic->getCurrentThread();
    QString titleSuff = thr < 0 ? QString(tr("Все ручьи")) : QString(tr("Ручей %1")).arg(thr+1);
    QString title = titlePref.arg(titleSuff);

    ui.graphic->getCanvas()->setTitle(title);
}

SPRThreadList SPRSettingsRentgenAutoSetDialog::getThreads()
{
    return threads;
}

double SPRSettingsRentgenAutoSetDialog::prDeuCpOKAll(QVector<bool> *_src){
    QVector<bool> cont;
    for(int i=0; i<controlCount.size(); i++){
            cont.append(controlCount[i] == 0);
    }
    QVector<bool> src = _src == nullptr ? deuOK + cpOK + cont: *_src;
    double countOk = 0, size = src.size();
    foreach(bool b, src){
        if(b){
            countOk++;
        }
    }
    return (countOk / size) * 100.;
}

void SPRSettingsRentgenAutoSetDialog::setThreads(SPRThreadList &value)
{
    threads = value;
    ui.graphic->setVisibleThreads(threads);

    deuOK.clear();
    cpOK.clear();
    if(mainModel){
        for(int i=0; i< MAX_SPR_MAIN_THREADS; i++){
            tempDEU[i] = mainModel->getSettingsRentgenModel()->deuCodes[i]->getData();
            tempCP[i] = mainModel->getSettingsRentgenModel()->cpCodes[i]->getData();
        }
        foreach(uint8_t i, threads){
            deuOK << false;
            cpOK << false;
            tempCP[i] = 128;
            tempDEU[i] = mainModel->getSettingsRentgenModel()->codeBegin->getData();
        }
    }
}

TCPTimeOutWidget *SPRSettingsRentgenAutoSetDialog::getToWidget() const
{
    return toWidget;
}

void SPRSettingsRentgenAutoSetDialog::setToWidget(TCPTimeOutWidget *value)
{
    toWidget = value;
}

void SPRSettingsRentgenAutoSetDialog::settingBACodes(){
    tempDEU2Command.clear();
    tempCP2Command.clear();

    for(int i=0; i<MAX_SPR_MAIN_THREADS; i++){
        tempDEU2Command.append((char*)&tempDEU[i], sizeof(uint16_t));
        tempCP2Command.append((char*)&tempCP[i], sizeof(uint16_t));
    }
}

void SPRSettingsRentgenAutoSetDialog::onChangePeakPosition(int){
    if(rentgenModel){
        if(sender() == ui.spPeakPosition){
            rentgenModel->peakPosition->setData(ui.spPeakPosition->value());

            int s = peakCurve->data()->size();
            peakData.clear();

            for(int i=0; i<s; i++){
                QPointF sample = peakCurve->data()->sample(i);
                sample.setX(ui.spPeakPosition->value());
                peakData.append(sample);
            }
            peakCurve->setSamples(peakData);

        }
    }
}

SPRSettingsRentgenAutoSetDialog::SPRSettingsRentgenAutoSetDialog(QWidget *parent) :
    QDialog(parent),
    rentgenModel(nullptr),
    mainModel(nullptr),
    startCommand(nullptr),
    autoSetGo(nullptr),
    toWidget(nullptr),
    spectrums(nullptr),
    peakCurve(nullptr),
    separatorOff(nullptr),
    expositionOff(nullptr),
    clearStates(nullptr)

{
    ui.setupUi(this);

    currentViewThread = -1;

    forBlockSignalsWidget = findChildren<QWidget*>();

    grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setMajorPen(Qt::white, 1);
    grid->setMinorPen(Qt::gray, 1, Qt::DotLine);

    grid->attach(ui.graphic->getCanvas());

    peakCurve = new QwtPlotCurve(QString(tr("Пиковое значение")));
    defPen = QPen(Qt::red, 2);
    selPen = QPen(Qt::red, 4, Qt::DotLine);

    peakCurve->setPen(defPen);
    peakData = {QPointF(DEF_SPR_AUTOSETTINGS_PEAC_POSITION, 0), QPointF(DEF_SPR_AUTOSETTINGS_PEAC_POSITION,100)};

    peakCurve->setSamples(peakData);
    peakCurve->attach(ui.graphic->getCanvas());

    ui.graphic->getPorogsMoved()->addMovedItems(peakCurve);

    ui.graphic->setEnableChangeTypeSet(true);
    ui.graphic->setWithLegend(true);

    ui.graphic->getCanvas()->replot();

    connect(ui.graphic->getPorogsMoved(), SIGNAL(setSelectItem(QwtPlotItem*,MovedItemPosition)), this, SLOT(onCusorOverSelectItem(QwtPlotItem*,MovedItemPosition)));
    connect(ui.graphic->getPorogsMoved(), SIGNAL(changeArgumentValue(QwtPlotItem*,double,MovedItemPosition)), this, SLOT(onChangeSelectedItemValue(QwtPlotItem*,double,MovedItemPosition)));
    connect(ui.graphic->getPorogsMoved(), SIGNAL(dblClickMouseEvent()), this, SLOT(onDblClickMouseEvent()));

    connect(ui.bStart, SIGNAL(clicked(bool)), this, SLOT(onClickButtons(bool)));
    connect(ui.bResume, SIGNAL(clicked(bool)), this, SLOT(onClickButtons(bool)));
    connect(ui.bExit, SIGNAL(clicked(bool)), this, SLOT(onClickButtons(bool)));
    connect(ui.bComplite, SIGNAL(clicked(bool)), this, SLOT(onClickButtons(bool)));
    connect(ui.bRentgenOff, SIGNAL(clicked(bool)), this, SLOT(onClickButtons(bool)));

    connect(ui.spPeakPosition, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SPRSettingsRentgenAutoSetDialog::onChangePeakPosition);

    tempCP.resize(MAX_SPR_MAIN_THREADS);
    tempDEU.resize(MAX_SPR_MAIN_THREADS);

    deuOK.resize(MAX_SPR_MAIN_THREADS);
    cpOK.resize(MAX_SPR_MAIN_THREADS);
    controlCount.resize(MAX_SPR_MAIN_THREADS);

    vectorMoveDeu.resize(MAX_SPR_MAIN_THREADS);
    stepDeu.resize(MAX_SPR_MAIN_THREADS);

    for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
        threads.push_back(th);
        ui.graphic->setVisibleThreads(threads);
    }
    setThreads(threads);

    ui.graphic->setEnableChangeTypeSet(true);
//    ui.graphic->setWithLegend(true);
    ui.splitter->setStretchFactor(0, 1);
    ui.splitter->setStretchFactor(1, 0);

    widgetsShow();
}

ISPRModelData *SPRSettingsRentgenAutoSetDialog::setModelData(ISPRModelData *value)
{
    if(value){
        rentgenModel = (SPRSettingsRentgenModel*)value;
        if(rentgenModel){

            connect(rentgenModel, &SPRSettingsRentgenModel::modelChanget, this, &SPRSettingsRentgenAutoSetDialog::onModelChanget);

            IModelVariable *_mod = rentgenModel->getMainModel();
            if(_mod && _mod->objectName() == "MainModel"){
                mainModel = (SPRMainModel*)_mod;
                if(mainModel){
                    ui.bStart->setEnabled(true);

                    for(int th=0; th<mainModel->getThreads()->getData(); th++){
                        tempDEU[th] = rentgenModel->codeBegin->getData();
                        tempCP[th] = 128;
                    }

                    settingBACodes();

                    if(spectrums) delete spectrums;
                    spectrums = new SPRSpectrumListItemsModel(mainModel, nullptr, true, nullptr);

                    ui.graphic->setModelData(spectrums, spectrumsOnly, false, true);
                    ui.graphic->setAllCurrent(true);

                    ui.spPeakPosition->blockSignals(true);
                    ui.spPeakPosition->setValue(mainModel->getSettingsRentgenModel()->peakPosition->getData());
                    ui.spPeakPosition->blockSignals(false);
//                    tempPeakPosition = ;

                    separatorOff= new TCPTestStopSeparate(toWidget, mainModel);
                    expositionOff = new TCPExpositionOnOff(mainModel->getServer(), false, false, mainModel);

                    connect(mainModel->getServer(), SIGNAL(serverStateChange(uint32_t)), this, SLOT(onServerStateChange(uint32_t)));
                    onServerStateChange(0);
                }
            }

            controlCP = 0;
            controlDEU = 0;
            controlAll = 0;

            qreal xmin = 0, xmax = 100;
            peakData = {QPointF(ui.spPeakPosition->value(), xmin), QPointF(ui.spPeakPosition->value(), xmax)};
            peakCurve->setSamples(peakData);

            widgetsShow();
        }
    }
    return value;
}

void SPRSettingsRentgenAutoSetDialog::onDblClickMouseEvent(){
    setGraphicTitle();
    int thr = ui.graphic->getCurrentThread();
    SPRGraphItem *gr = nullptr;
    if(thr >= 0){
        if(spectrums){
            QList<SPRSpectrumItemModel*> lst = spectrums->getSpectrumsItemByThread(thr, spectrumsOnly);
            if(lst.size() > 0){
                 gr = lst.last()->getGraphic();
            }
        }
    }
    ui.graphic->setCurrentItem(gr);
}

SPRSettingsRentgenAutoSetDialog::~SPRSettingsRentgenAutoSetDialog()
{
    if(startCommand) delete startCommand; startCommand = nullptr;
    if(autoSetGo) delete autoSetGo; autoSetGo = nullptr;
    if(separatorOff) delete separatorOff; separatorOff = nullptr;
    if(toWidget) delete toWidget; toWidget = nullptr;
}

QByteArray SPRSettingsRentgenAutoSetDialog::codesToByteArray(uint16_t *table){
    QByteArray res;
    char ren = 0;
    res.append(&ren, 1);
    for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
        res.append((char*)(table + th), sizeof(uint16_t));
    }
    return res;
}

TCPCommand *restartsep = nullptr;


void SPRSettingsRentgenAutoSetDialog::onCommandNotComplite(TCPCommand *send){
    if(mainModel){
        bool isStopSep = true;
        if(sender() == startCommand){
            qDebug() << "ERRORS WORKING Rentgen.... ********************";

            if(send->getCommand() == getren){
                QString title(MSG_TCPSET_RENTGEN_NOT_READY);
                uint16_t mkv, mka; uint8_t err;
                startCommand->isRentgenReady(send->getReplayData(), &mkv, &mka, &err);
                QString msg(MSG_TCPSET_RENTGEN_NOT_READY_MSG( mkv, mka ));
                QMessageBox::warning(nullptr, title, msg);
            }
        }
        if(isStopSep){
            QMessageBox::warning(nullptr, tr("Ошибка..."),tr("Какая то ошибка работы сепаратора...\n"
                                             "Надо выключить рентген..."));

            if(separatorOff) delete separatorOff;
            separatorOff = new TCPTestStopSeparate(nullptr, mainModel);

            separatorOff->send(mainModel->getServer());

        }
    }
}

void SPRSettingsRentgenAutoSetDialog::onCancelCommand(TCPCommand *)
{
//    QMessageBox::warning(nullptr, MSG_TCPSET_CANCEL_COMMAND, MSG_TCPSET_CANCEL_COMMAND_MSG);
    ui.bComplite->setEnabled(true);
    ui.bResume->setEnabled(true);
    ui.cbSuspendMode->setEnabled(true);
}

void SPRSettingsRentgenAutoSetDialog::onServerStateChange(uint32_t)
{
    QList<QWidget*> allButtoms = {
        ui.bStart, ui.bRentgenOff
    };
    if(mainModel){
        foreach(QWidget *w, allButtoms){
            w->setEnabled(mainModel->getServer()->isState(spr_state_server_connect));
        }
        if(mainModel->getServer()->isState(spr_state_rentgen_on)){
            ui.bRentgenOff->setEnabled(true);
        } else {
            ui.bRentgenOff->setEnabled(true);
        }
    }
}

void SPRSettingsRentgenAutoSetDialog::process(){

    ui.bResume->setEnabled(false);
    ui.bComplite->setEnabled(false);

    finish = false;

    QByteArray codeDeu, codeCP;
    for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
        uint16_t deu, cp;
        if(threads.contains(th)){
                if(ui.cbSuspendMode->isEnabled() && ui.cbSuspendMode->isChecked()){
                    deu = tempDEU[th];
                    cp = tempCP[th];

                } else if(ui.cbStartWithCurrentValues->isChecked()){
                    deu = rentgenModel->deuCodes[th]->getData();
                    cp = rentgenModel->cpCodes[th]->getData();
                    vectorMoveDeu[th] = 0;
                    stepDeu[th] = rentgenModel->codeStep->getData();
                } else {
                    deu = rentgenModel->codeBegin->getData();
                    cp = 128;
                    vectorMoveDeu[th] = 0;
                    stepDeu[th] = rentgenModel->codeStep->getData();
                }

                deuOK[th] = false;
                cpOK[th] = false;
                controlCount[th] = 1;
            } else {
                deu = rentgenModel->deuCodes[th]->getData();
                cp = rentgenModel->cpCodes[th]->getData();
                deuOK[th] = true;
                cpOK[th] = true;
                controlCount[th] = 0;
            }


            codeDeu.append((char*)(&deu), sizeof(deu));
            codeCP.append((char*)(&cp), sizeof(cp));
    }

    if(startCommand) delete startCommand; startCommand = nullptr;

    startCommand = new TCPGetSpectrums2(mainModel, 5, DownRGU, threads, false, nullptr);

    connect(startCommand, &TCPAutoSetRentgen::commandComplite, this, &SPRSettingsRentgenAutoSetDialog::onCommandComplite);
    connect(startCommand, &TCPAutoSetRentgen::commandNotComplite, this, &SPRSettingsRentgenAutoSetDialog::onCommandNotComplite);
    connect(startCommand, &TCPAutoSetRentgen::cancelCommand, this, &SPRSettingsRentgenAutoSetDialog::onCancelCommand);

    startCommand->setCodesDEU(codeDeu);
    startCommand->setCodesCP(codeCP);
    startCommand->setThreads(threads);

    widgetsShow();

    startCommand->send(mainModel->getServer());


}

void SPRSettingsRentgenAutoSetDialog::onClickButtons(bool){
    if(mainModel){

        if(sender() == ui.bRentgenOff){
            if(separatorOff) delete separatorOff; separatorOff = nullptr;

            separatorOff = new TCPTestStopSeparate(nullptr, mainModel);

            connect(separatorOff, &TCPTestStopSeparate::commandComplite, this, &SPRSettingsRentgenAutoSetDialog::onCommandComplite);
            connect(separatorOff, &TCPTestStopSeparate::commandNotComplite, this, &SPRSettingsRentgenAutoSetDialog::onCommandNotComplite);

            separatorOff->send(mainModel->getServer());
            return;
        }

        if(sender() == ui.bStart){

            process();
            ui.bResume->setEnabled(true);
            ui.cbSuspendMode->setEnabled(true);

        }
        if(sender() == ui.bResume){
            ui.bComplite->setEnabled(false);
            ui.cbSuspendMode->setEnabled(true);
            ui.cbSuspendMode->setChecked(true);

            finish = false;

            process();
        }
        if(sender() == ui.bComplite){
            if(mainModel){
                foreach(uint8_t th, threads){
                    mainModel->getSettingsRentgenModel()->deuCodes[th]->setData(tempDEU[th]);
                    mainModel->getSettingsRentgenModel()->cpCodes[th]->setData(tempCP[th]);
                }
//                mainModel->getSettingsRentgenModel()->peakPosition->setData(sp);
            }
        }
    }
    if(sender() == ui.bExit){
        finish = true;

//        if(separatorOff) delete separatorOff; separatorOff = nullptr;
//        separatorOff = new TCPTestStopSeparate(nullptr, mainModel);
//        separatorOff->send(mainModel->getServer());
        if(mainModel){

            if(clearStates) delete clearStates;
            clearStates = new TCPClearStates();
            clearStates->send(mainModel->getServer());

            if(expositionOff) delete expositionOff;
            expositionOff = new TCPExpositionOnOff(mainModel->getServer(), false, true, mainModel);
            expositionOff->send(mainModel->getServer());
        }

        this->accept();
    }

}

void SPRSettingsRentgenAutoSetDialog::onCommandComplite(TCPCommand* _command){
    if(spectrums && mainModel){
        if((_command == startCommand || _command == autoSetGo) && !finish){

            countSteps++;

            QList<SPRSpectrumItemModel*> lspect;

            QList<TCPCommand*> vspc = _command->findCommands(getspk);
            for(int i=0; i<vspc.size(); i++){
                if(i < threads.size()){
                    QByteArray rep = vspc[i]->getReplayData();
                    QByteArray spc = rep.right(DEF_SPECTRUM_DATA_LENGTH_BYTE);

                    SPRSpectrumItemModel *mod = nullptr;
                    mod = spectrums->setSpectrumData(threads[i], spc, spectrumsOnly, 10000, QString(tr("Ручей %1")));
                    mod->setTimeScope(10000);
                    lspect.append(mod);
        //            mod->setSpectrumName(QString(tr("Ручей %1")).arg(threads[i]+1));

                }
            }
            ui.graphic->getCanvas()->replot();
            double h = ui.graphic->getCanvas()->axisScaleDiv(QwtPlot::yLeft).upperBound();
            peakData[0].setY(0); peakData[1].setY(h);
            peakCurve->setSamples(peakData);

            bool falseNextParams = false;

            uint peakFinish = ui.spPeakPosition->value();
            uint deuStep = mainModel->getSettingsRentgenModel()->codeStep->getData();

            foreach(SPRSpectrumItemModel *spect, lspect){
                if(!spect){
                    falseNextParams = true;
                    qDebug() << "*****************Error get spectrum.....";
                    break;

                }

                uint8_t th = spect->getSpectrumThread();
                int peakTh = *spect->getSpectrumData()->peak;
                if(!threads.contains(th)){
                    continue;
                }

                int diff = peakTh - peakFinish;
                if(controlCount[th] > 0){
                    if(qAbs(diff) < 5){ // deu ok
                        deuOK[th] = true;
                        if(diff > 0) { // перелет
                            cpOK[th] = false;
                            controlCount[th] = 1;
                            tempCP[th]--;
                            if(tempCP[th] < 0){
                                falseNextParams = true;
                                qDebug() << "*****************false next params..... th: " << th << " cp: " << tempCP[th];
                                break;
                            }
                        } else if(diff < 0){ // недолет
                            cpOK[th] = false;
                            tempCP[th]++;
                            controlCount[th] = 1;
                            if(tempCP[th] > 255){
                                falseNextParams = true;
                                qDebug() << "*****************false next params..... th: " << th << " cp: " << tempCP[th];
                                break;
                            }
                        } else if( diff == 0){
                            deuOK[th] = true;
                            cpOK[th] = true;
                            controlCount[th]--;
                        }
                    } else { // > 5
                        deuOK[th] = false;
                        cpOK[th] = false;
                        controlCount[th] = 1;
                        if(diff < 0){ // недолет
                            if(vectorMoveDeu[th] < 0){ // до этого отнимали
                                stepDeu[th] = stepDeu[th] / 2;
                            }
                            vectorMoveDeu[th] = 1;
                            tempDEU[th] += stepDeu[th];
                            if(tempDEU[th] > 255 || stepDeu[th] == 0){
                                falseNextParams = true;
                                qDebug() << "*****************false next params..... th: " << th << " cp: " << tempDEU[th] << " step:" << stepDeu[th];
                                break;
                            }
                        } else { // перелет
                            if(vectorMoveDeu[th] > 0){ // до этого прибавляли
                                stepDeu[th] = stepDeu[th] / 2;
                            }
                            tempDEU[th] -= deuStep;
                            if(tempDEU[th] < 0 || stepDeu[th] == 0){
                                falseNextParams = true;
                                qDebug() << "*****************false next params..... th: " << th << " cp: " << tempDEU[th] << " step:" << stepDeu[th];
                                break;
                           }
                        }

                    }
                }
            }

            bool allDeuOk = true; bool allCpOk = true; bool allControlOk = true;
            foreach(uint8_t th, threads){
                if(!deuOK[th]){
                    allDeuOk = false;
                }
                if(!cpOK[th]){
                    allCpOk = false;
                }
                if(controlCount[th] > 0){
                    allControlOk = false;
                }
            }

            if(allControlOk){
                ui.bComplite->setEnabled(true);

                QMessageBox::information(nullptr, tr("Завешение операции..."),
                        tr("Установка кодов ДЭУ и кодов ЦП успешно завершена...\n"
                           "Полученные коды можно применить для текущей настройки\n"
                           "Нажав кнопку <Применить>"));


                return;
            }

            settingBACodes();

            if(falseNextParams){
                QMessageBox::warning(nullptr, tr("Ошибка параметров..."), tr("Ошибка параметров кодов ДЭУ и/или кодов ЦП для следующего шага..."));
                return;
            }

            if(autoSetGo) delete autoSetGo; autoSetGo = nullptr;
            autoSetGo = new TCPAutosetCommandGo(nullptr, {}, {}, threads, nullptr, nullptr);
            connect(autoSetGo, &TCPAutosetCommandGo::commandComplite, this, &SPRSettingsRentgenAutoSetDialog::onCommandComplite);
            connect(autoSetGo, &TCPAutosetCommandGo::commandNotComplite, this, &SPRSettingsRentgenAutoSetDialog::onCommandNotComplite);
            connect(autoSetGo, &TCPAutosetCommandGo::cancelCommand, this, &SPRSettingsRentgenAutoSetDialog::onCancelCommand);


            if(!allDeuOk){
                autoSetGo->setDeuCpCodes(tempDEU2Command, {});
            } else if(!allCpOk){
                autoSetGo->setDeuCpCodes({}, tempCP2Command);
            }


            controlCP = prDeuCpOKAll(&cpOK);
            controlDEU = prDeuCpOKAll(&deuOK);
            controlAll = prDeuCpOKAll();

            widgetsShow();
            autoSetGo->send(mainModel->getServer());

        }

    }

}

ISPRModelData *SPRSettingsRentgenAutoSetDialog::getModelData()
{
    return rentgenModel;
}

void SPRSettingsRentgenAutoSetDialog::onModelChanget(IModelVariable *var)
{
    if(var == spectrums){
        ui.graphic->widgetsShow();
    }
    if(var == rentgenModel->peakPosition || sender() == rentgenModel->peakPosition){
        ui.spPeakPosition->blockSignals(true);
        ui.spPeakPosition->setValue(rentgenModel->peakPosition->getData());
        ui.spPeakPosition->blockSignals(false);
    }
}

