#include "sprhistorymodel2.h"

#include "models/sprseparatemodel.h"
#include "models/sprmainmodel.h"
#include "models/sprspectrumlistitemsmodel.h"
#include "models/sprseparatorlogsmodel.h"

void SPRKSpectrumSeparateSummary::init(SPRSettingsControlModel *_model)
{
    if(_model && _model != model){
        model = _model;
    }
    correl = 0;
    correlError = false;
    correlCritical = -10000;

    power = 0;
    diffPower = 0;
    powerError = false;
    powerCritical = 10000;

    airLoad = 0;
    airError = false;
    airLoadCriticalMin = 0;
    airLoadCriticalMax = 10000;

    speedStone = 0;
    speedStoneError = 0;
    speedStoneCritical = 10000;

    isErrors = false;
}

void SPRKSpectrumSeparateSummary::setSpectrumData(SPRSpectrumItemModel *kspect, SPRSpectrumItemModel *bspect, SPRSettingsControlModel *_model)
{
    init(_model);
    if(kspect && bspect){

        SpectrumItemData *k = kspect->getSpectrumData();
        SpectrumItemData *b = bspect->getSpectrumData();

        correl = kspect->getCorrel(bspect, true, model->controlArea->getData());

        double center = kspect->getSpectrumCenter(false, Ns);
        double bCenter = bspect->getSpectrumCenter(false, Ns);

        if(*k->time_in_sec > 1e-6){
            airLoad = *k->summ / *k->time_in_sec;
        }

        if(k && b){
            power = center;
            diffPower = fabs(bCenter - center);
        }

        if(model){
            correlCritical = model->correlSpectrumCritical->getData();
            if(correl < correlCritical){
                correlError = true;
                isErrors = true;
            }

            airLoadCriticalMin = model->airLoadingMinCritical->getData();
            airLoadCriticalMax = model->airLoadingMaxCritical->getData();
            if(airLoad < airLoadCriticalMin || airLoad > airLoadCriticalMax){
                airError = true;
                isErrors = true;
            }

            powerCritical = model->diffCenterGravityCritical->getData();
            if(diffPower > powerCritical){
                powerError = true;
                isErrors = true;
            }
        }
    }
}

void SPRKSpectrumSeparateSummary::setSeparateData(int count, qint64 time_in_msec, SPRSettingsControlModel *_model)
{
    init(_model);

    speedStone = static_cast<double>(count) / (static_cast<double>(time_in_msec) / 1000.);
    if(model){
        speedStoneCritical = model->speedStreamCritical->getData();
        if(speedStone > speedStoneCritical){
            speedStoneError = true;
            isErrors = true;
        }
    }
}

/**
 * @brief SPRTimeAvrVariable::SPRTimeAvrVariable
 * @param _maxTime
 * @param parent
 */

SPRTimeAvrVariable::SPRTimeAvrVariable(qint64 _maxTime_in_msec, QObject *parent):
    QObject(parent), maxSizeAvrInMsec(_maxTime_in_msec)
{
    summ = 0; size_in_ms = 0;
}

SPRTimeAvrVariable::~SPRTimeAvrVariable()
{
    while(!vars.isEmpty() > 0){
        sVars *first = vars.dequeue();
        delete first;
    }
}

void SPRTimeAvrVariable::addVar(double var, QDateTime dt)
{
    bool find = false;
    quint64 tDiff = 0;
    while(!find){
        if(vars.isEmpty()){
            find = true;
            break;
        }
        sVars *first = vars.head();
        tDiff = llabs(dt.msecsTo(first->dt));

        if(llabs(tDiff) > maxSizeAvrInMsec){
            summ -= first->value;
            size_in_ms -= (llabs(size_in_ms - tDiff));
            vars.dequeue();
            delete first;
        } else {
            find = true;
            break;
        }
    }
    summ += var;
    size_in_ms = tDiff;

    vars.enqueue(new sVars(var, dt));
}

quint64 SPRTimeAvrVariable::getSize_in_ms() const
{
    return size_in_ms;
}

double SPRTimeAvrVariable::getAvr_in_sec()
{
    double res = getAvr_in_msec() * 1000;
}

double SPRTimeAvrVariable::getAvr_in_msec()
{
    double res = 0;
    if(size_in_ms > 0){
        res = summ / size_in_ms;
    } else {
        res = summ / 1000;
    }
    return res;
}


/**
 * @brief SPRHistoryModel2::hist_data_one_thread::hist_data_one_thread
 */
SPRHistoryModel2::histDataOneThread::histDataOneThread(){
    clear();
}

SPRHistoryModel2::histDataOneThread::histDataOneThread(SPRWorkSeparate *ws, int th){
    clear();
    set(ws, th);
}

SPRHistoryModel2::histDataOneThread::histDataOneThread(const SPRHistoryModel2::histDataOneThread &o)
{
    clear();
    data.thread = o.data.thread;
    for(int i=0; i<3; i++){data.i_prd[i] = o.data.i_prd[i]; data.p_prd[i] = o.data.p_prd[i];}

    data.p_tk = o.data.p_tk; data.p_tkh1 = o.data.p_tkh1; data.p_tkh2 = o.data.p_tkh2; data.p_tkh3 = o.data.p_tkh3;
    for(int i=0; i<5; i++){data.s_rst[i] = o.data.s_rst[i];}
    data.wcount = o.data.wcount;
    data.size = o.data.size;
    data.mdt = o.data.mdt;
    starting = o.starting;
}

SPRHistoryModel2::histDataOneThread &SPRHistoryModel2::histDataOneThread::operator=(SPRHistoryModel2::histDataOneThread &o)
{
    dt = o.dt;
    data.id = o.data.id;
    data.thread = o.data.thread;
    for(int i=0; i<3; i++){data.i_prd[i] = o.data.i_prd[i]; data.p_prd[i] = o.data.p_prd[i];}

    data.p_tk = o.data.p_tk; data.p_tkh1 = o.data.p_tkh1; data.p_tkh2 = o.data.p_tkh2; data.p_tkh3 = o.data.p_tkh3;
    for(int i=0; i<5; i++){data.s_rst[i] = o.data.s_rst[i];}
    data.wcount = o.data.wcount;
    data.size = o.data.size;
    data.mdt = o.data.mdt;
    starting = o.starting;
    return *this;
}

SPRHistoryModel2::histDataOneThread &SPRHistoryModel2::histDataOneThread::operator+=(const SPRHistoryModel2::histDataOneThread &o)
{
    dt = o.dt;
    data.id = o.data.id;
    data.thread = o.data.thread;
    for(int i=0; i<3; i++){data.i_prd[i] = data.i_prd[i] + o.data.i_prd[i]; data.p_prd[i] = o.data.p_prd[i];}

    data.p_tk = o.data.p_tk; data.p_tkh1 = o.data.p_tkh1; data.p_tkh2 = o.data.p_tkh2; data.p_tkh3 = o.data.p_tkh3;
    for(int i=0; i<5; i++){data.s_rst[i] = data.s_rst[i] + o.data.s_rst[i];}
    data.wcount = data.wcount + o.data.wcount;
    data.size = o.data.size;
    data.mdt = o.data.mdt;
    starting = o.starting;
    return *this;
}

void SPRHistoryModel2::histDataOneThread::clear(){
    memset(&data, 0, sizeof(data));
    starting = false;
    dt = QDateTime::currentDateTime();
    data.mdt = 0;
    data.id = rand();
    data.size = 0;
}

void SPRHistoryModel2::histDataOneThread::set(SPRWorkSeparate *ws, int th){
    data.thread = th;
    data.i_prd[0] = ws->source.i_prd[th][0]; data.i_prd[1] = ws->source.i_prd[th][2]; data.i_prd[2] = ws->source.i_prd[th][3];
    data.p_prd[0] = ws->source.p_prd[th][0]; data.p_prd[1] = ws->source.p_prd[th][2]; data.p_prd[2] = ws->source.p_prd[th][3];

    data.p_tk = ws->source.p_tk[th]; data.p_tkh1 = ws->source.p_tkh1[th]; data.p_tkh2 = ws->source.p_tkh2[th]; data.p_tkh3 = ws->source.p_tkh3[th];
    for(int i=0; i<5; i++){
        data.s_rst[i] = ws->source.s_rst[th][i];
    }
    data.wcount = ws->source.wcount[th];
}

void SPRHistoryModel2::histDataOneThread::setDiff(SPRHistoryModel2::histDataOneThread *last, SPRHistoryModel2::histDataOneThread *prev){

    data.thread = last->data.thread;

    data.mdt = llabs(last->dt.msecsTo(prev->dt));

    for(int i=0; i<3; i++){
        data.i_prd[i] = last->data.i_prd[i] - prev->data.i_prd[i];
        data.p_prd[i] = last->data.p_prd[i] - prev->data.p_prd[i];
    }
    data.p_tk = last->data.p_tk - prev->data.p_tk; data.p_tkh1 = last->data.p_tkh1 - prev->data.p_tkh1; data.p_tkh2 = last->data.p_tkh2 - prev->data.p_tkh2; data.p_tkh3 = last->data.p_tkh3 - prev->data.p_tkh3;
    for(int i=0; i<5; i++){
        data.s_rst[i] = last->data.s_rst[i] - prev->data.s_rst[i];
    }
    data.wcount = last->data.wcount - prev->data.wcount;
    dt = last->dt;
    starting = last->starting;
}

void SPRHistoryModel2::histDataOneThread::addAvr(SPRHistoryModel2::histDataOneThread *last){

    for(int i=0; i<3; i++){
        data.p_prd[i] = last->data.p_prd[i];
    }

    data.p_tkh1 = last->data.p_tkh1;
    data.p_tkh2 = last->data.p_tkh2;
    data.p_tkh3 = last->data.p_tkh3;

    for(int i=0; i<5; i++){
        data.s_rst[i] = last->data.s_rst[i];
    }

//    double d_wcount = data.wcount * maxMulPh + diff->data.wcount * minMulPh;
    data.wcount = last->data.wcount;
    dt = last->dt;
}

void SPRHistoryModel2::histDataOneThread::toDebug(QString note)
{
    if(!note.isEmpty()){
        qDebug() << "******** " << note << " ***********";
    }
    qDebug() << "time in msec:" << data.mdt;
    qDebug() << "i_prd_con:" << data.i_prd[0] << "; i_prd_tail:" << data.i_prd[1] << "; i_prd_all:" << data.i_prd[2];
}

bool SPRHistoryModel2::histDataOneThread::isNotStouns(){
    for(int i=0; i<3; i++){
        if(data.i_prd[i] > 1e-6){
            return false;
        }
    }
    return true;
}


QVector<SPRHistoryModel2::histDataOneThread> *SPRHistoryModel2::getLastRecords()
{
    return &last;
}

QVector<SPRHistoryModel2::histDataOneThread> *SPRHistoryModel2::getDiffRecords()
{
    return &diff;
}

QVector<SPRHistoryModel2::histDataOneThread> *SPRHistoryModel2::getAvr10mRecords()
{
    return &avr10m;
}

QVector<QVector<QPointF>> SPRHistoryModel2::getGistOut() const
{
    return gistOut;
}

SPRSeparateModel *SPRHistoryModel2::getSeparateModel() const
{
    return separateModel;
}

SPRMainModel *SPRHistoryModel2::getMainModel() const
{
    return mainModel;
}

QVector<SPRKSpectrumSeparateSummary> *SPRHistoryModel2::getSpectrumSummaryTable()
{
    return &spectrumSummaryTable;
}

bool SPRHistoryModel2::isKSpectrumsError()
{
    for(int i=0; i<spectrumSummaryTable.size(); i++){
        if(spectrumSummaryTable[i].powerError){
            return true;
        }
        if(spectrumSummaryTable[i].correlError){
            return true;
        }
        if(spectrumSummaryTable[i].airError){
            return true;
        }
    }
    return false;
}

void SPRHistoryModel2::clear()
{
    for(uint th=0; th < MAX_SPR_MAIN_THREADS; th++){
        last[th].clear();
        avr10m[th].clear();
        lastSession[th].clear();
        for(int th=0; th<gistInt.size(); th++){
            gistInt[th].clear();
        }
        startingGist = false;
        startingSeparateData = false;
//        gistInt.clear();
    }

    emit separateDataReady();
}

QVector<SPRHistoryModel2::histDataOneThread> *SPRHistoryModel2::getLastSessionRecords()
{
    return &lastSession;
}

SPRHistoryModel2::SPRHistoryModel2(IModelVariable *parent, SPRSpectrumListItemsModel *spectrums):
    ISPRModelData(parent),
    mainModel(nullptr),
    separateModel(nullptr),
    spectrumsModel(nullptr),
    logModel(nullptr),
    startingGist(false),startingSeparateData(false)
{
    SPRMainModel *mm = dynamic_cast<SPRMainModel*>(parent);
    if(mm){
        setModelData(mm, spectrums);
    }
    last.resize(MAX_SPR_MAIN_THREADS);
    diff.resize(MAX_SPR_MAIN_THREADS);
    avr10m.resize(MAX_SPR_MAIN_THREADS);
    lastSession.resize(MAX_SPR_MAIN_THREADS);

    tempAvr.resize(MAX_SPR_MAIN_THREADS);

    gistInt.resize(MAX_SPR_MAIN_THREADS);
    gistInpLast.resize(MAX_SPR_MAIN_THREADS);

    flagsCorrelCriticalMessageSended.resize(MAX_SPR_MAIN_THREADS);
    flagsAirCriticalMessageSended.resize(MAX_SPR_MAIN_THREADS);
    flagsPowerCriticalMessageSended.resize(MAX_SPR_MAIN_THREADS);

    for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
        tempAvr[th] = new avrDataOneThread(DEF_SPR_HISTOTY_AVR_TIME_MSEC);

        flagsCorrelCriticalMessageSended[th] = false;
        flagsAirCriticalMessageSended[th] = false;
        flagsPowerCriticalMessageSended[th] = false;
    }

    gistOut.resize(MAX_SPR_MAIN_THREADS+1);
    for(int th=0; th<MAX_SPR_MAIN_THREADS + 1; th++){
        gistOut[th].resize(DEF_SPR_SEPARATE_GCOL);
    }

    spectrumSummaryTable.resize(MAX_SPR_MAIN_THREADS);
}

SPRHistoryModel2::~SPRHistoryModel2()
{
    for(int th=0; th<MAX_SPR_MAIN_THREADS; th++){
        delete tempAvr[th];
    }
}

void SPRHistoryModel2::setModelData(SPRMainModel *_main, SPRSpectrumListItemsModel *_spectrums)
{
    if(_main){
        mainModel = _main;
        SPRSeparateModel *mod = mainModel->getSeparateModel();
        if(separateModel != mod && mod){
//            connect(mod, &SPRSeparateModel::separateDataReady, this, &SPRHistoryModel2::onSeparateDataReady);
//            connect(mod, &SPRSeparateModel::gistogrammDataReady, this, &SPRHistoryModel2::setWorkGistogrammData);
        }
        separateModel = mod;
        startingGist = false;
        startingSeparateData = false;

        reinitGistMatrixs();
//        if(spectrumsModel != _spectrums){
//            connect(_spectrums, &SPRSpectrumListItemsModel::modelChanget, this, &SPRHistoryModel2::onModelChanget);
//        }
        spectrumsModel = _spectrums;
    }
}

void SPRHistoryModel2::setLogModel(SPRSeparatorLogsModel *_logModel)
{
    logModel = _logModel;
}

void SPRHistoryModel2::reinitGistMatrixs(){
    gStep=0; gMin=0; gCol; double gDiff = 0;
    if(!separateModel->isSeparateEmpty()){
        gMin = separateModel->getSettingsSeparate()->ming1;
        gDiff = separateModel->getSettingsSeparate()->maxg1 - gMin;
        gCol = separateModel->getSettingsSeparate()->gcol;
    } else {
        SPRMainModel *mm = separateModel->getMainModel();
        if(mm){
            gMin = mm->getSettingsFormulaModel()->min->getData();
            gDiff = fabs(mm->getSettingsFormulaModel()->max->getData() - gMin);
            gCol = separateModel->gcol->getData();
        } else {
            gMin = DEF_SPR_FORMULA_MIN;
            gDiff = fabs(DEF_SPR_FORMULA_MAX - gMin);
            gCol = DEF_SPR_SEPARATE_GCOL;
        }

    }
    if(gCol <= 0){
        gCol = DEF_SPR_SEPARATE_GCOL;
    }
    gStep = gDiff / gCol;

    double x = gMin;
    for(int th=0; th<gistOut.size()+1;th++){
        for(int i=0; i<gistOut.size(); i++){
            gistOut[th][i] = QPointF(x, 0);
            x = x + gStep;
        }
    }
}

void SPRHistoryModel2::onModelChanget(IModelVariable *send)
{
}

void SPRHistoryModel2::setWorkSeparateData(QByteArray rawData){
    int s = sizeof (sourseLast.source);
    memcpy(&sourseLast.source, rawData.constData(), sizeof(sourseLast.source));
    onSeparateDataReady();
}

void SPRHistoryModel2::setWorkGistogrammData(QByteArray rawData, int thread)
{
        if(thread >= 0 && thread < gistOut.size()-1){

            if(separateModel->isSeparateEmpty()){
                gMin = DEF_SPR_FORMULA_MIN;
                gMax = DEF_SPR_FORMULA_MAX;
                gCol = DEF_SPR_SEPARATE_GCOL;
            } else {
                gMin = separateModel->getSettingsSeparate()->ming1;
                gMax = separateModel->getSettingsSeparate()->maxg1;
                gCol = separateModel->getSettingsSeparate()->gcol;
            }
            gStep = fabs(gMax - gMin) / gCol;


            SPRWorkGistogrammRow inpData;
            SPRWorkGistogrammRow d;
            double x = gMin;
            if(!startingGist){
               memcpy(&(gistInpLast[thread].gist[0]), rawData.constData(), sizeof(gistInpLast[thread].gist));
               startingGist = true;
               return;
            }
            memcpy(&inpData.gist[0], rawData.constData(), sizeof(inpData.gist));
            d = inpData - gistInpLast[thread];
            gistInt[thread] += d;

            memcpy(&(gistInpLast[thread].gist[0]), &inpData.gist[0], sizeof(gistInpLast[thread].gist));

            for(int i=0; i<gistOut[thread].size();i++){
                gistOut[thread][i].setY(gistInt[thread].gist[i]);
                gistOut[thread][i].setX(x);
                x += gStep;
            }
            x = gMin;
            for(int i=0; i<gistOut[gistOut.size()-1].size(); i++){
                double s = 0;
                for(int th=0; th<gistOut.size()-1; th++){
                    s += gistOut[th][i].y();
                }
                gistOut[gistOut.size()-1][i].setX(x);
                gistOut[gistOut.size()-1][i].setY(s);
                x += gStep;

            }
        }
        emit gistogrammDataReady();
}

QVector<QPointF> *SPRHistoryModel2::getGistogrammContentData2(int thread)
{
    if(thread >=0 && thread < gistOut.size()){
        return &gistOut[thread];
    } else if(thread < 0){
        return &gistOut[gistOut.size()-1];
    }
}
void SPRHistoryModel2::onSeparateDataReady()
{
    if(mainModel){

            for(int th=0; th<MAX_SPR_MAIN_THREADS;th++){
                histDataOneThread l(&sourseLast, th);
                histDataOneThread d;
                if(!startingSeparateData){
                    d = l;
                    last[th] = l;
                } else {

                    d.setDiff(&l, &last[th]);
                    diff[th] = d;
                    lastSession[th] += d;

                    tempAvr[th]->addAvr(&d, &avr10m[th], QDateTime::currentDateTime());

                    avr10m[th].addAvr(&d);
                    
//                    if(th == 0){
//                        l.toDebug("l matrix");
//                        last[th].toDebug("last matrix");
                        diff[th].toDebug("diff matrix");
//                        avr10m[th].toDebug("avr matrix");
//                    }

                    last[th] = l;
                }
            }
            if(startingSeparateData){
                emit separateDataReady();
                if(logModel){
                    logModel->onSeparateDataReady(this);
                }
            }
            startingSeparateData = true;
//        }
//qDebug() << "time: " << QDateTime::currentDateTime().toString("HH:mm:ss(ms)");
    }
}

QVector<SPRKSpectrumSeparateSummary> *SPRHistoryModel2::onKSpectrumsReady()
{
//    bool signalNeed = false;
    if(spectrumsModel && mainModel){
//        double correlCritical = mainModel->getSettingsControlModel()->correlSpectrumCritical->getData();
//        int airCriticalMin = mainModel->getSettingsControlModel()->airLoadingMinCritical->getData();
//        int airCriticalMax = mainModel->getSettingsControlModel()->airLoadingMaxCritical->getData();
//        double diffPowerCritical = mainModel->getSettingsControlModel()->diffCenterGravityCritical->getData();

        for(int8_t th=0; th<MAX_SPR_MAIN_THREADS; th++){
            QList<SPRSpectrumItemModel *> lkspect = spectrumsModel->getSpectrumsItemByThread(th);
            QList<SPRSpectrumItemModel *> lbspect = spectrumsModel->getSpectrumsItemByThread(th, spectrumBase);
            if(lkspect.size() > 0 && lbspect.size() > 0){
                SPRSpectrumItemModel *kspect = lkspect.first();
                SPRSpectrumItemModel *bspect = lbspect.first();

                spectrumSummaryTable[th].setSpectrumData(kspect, bspect, mainModel->getSettingsControlModel());

                emit kspectrumDataReady();

            }
        }
    }
    return &spectrumSummaryTable;
}

SPRThreadList SPRHistoryModel2::getThreadList()
{
    SPRThreadList ret;
    for(uint8_t th=0; th<last.size(); th++){
        ret.append(th);
    }
    return ret;
}


