#include "sprhitorygraphics.h"


/**
 * @brief SPRHitoryGraphics::getGraphics
 * @param thread
 * @return
 */
QList<QwtPlotItem *> SPRHitoryGraphics::getGraphics(int thread){
    QList<QwtPlotItem *> source;

    foreach(HistoryGraphic *gr, lstGraphics){
        if(gr->getThread() == thread || thread < 0)
        source << gr->getGraphic();
    }

    return source;
}
QList<QwtPlotItem *> SPRHitoryGraphics::getGraphics(HistoryGraphic *_current){
    QList<QwtPlotItem *> ret;

    foreach(HistoryGraphic *gr, lstGraphics){
        if(gr == _current || _current == nullptr)
        ret << gr->getGraphic();
    }

    return ret;
}

/**
 * @brief SPRHitoryGraphics::eventFilter
 * @param object
 * @param event
 * @return
 */
bool SPRHitoryGraphics::eventFilter(QObject *object, QEvent *event){
    switch( event->type() )
    {
    case QEvent::MouseButtonDblClick: {
        onDblClick();
    }
    }
    return QObject::eventFilter( object, event );
}
/**
 * @brief SPRHitoryGraphics::SPRHitoryGraphics
 * @param parent
 */
SPRHitoryGraphics::SPRHitoryGraphics(QWidget *parent):
    QwtPlot(parent), model(nullptr), grid(nullptr), legend(nullptr), timeDraw(nullptr)
{
}
/**
 * @brief SPRHitoryGraphics::~SPRHitoryGraphics
 */
SPRHitoryGraphics::~SPRHitoryGraphics(){
    foreach(HistoryGraphic *gr, lstGraphics){
        if(gr){
            QwtPlotCurve *item = gr->getGraphic();
            if(itemList().contains(item)){
                item->detach();
//                delete item;
            }
        }
    }
    qDeleteAll(lstGraphics);

    if(grid) delete grid;
    if(legend) delete legend;
//    if(timeDraw) delete timeDraw;
}

/**
 * @brief SPRHitoryGraphics::init
 * @param _thread
 * @param _model
 * @param _type
 * @param _titleFormat
 * @param _interval_in_sec
 * @param _size_parts_in_sec
 */

void SPRHitoryGraphics::init(SPRThreadList _thread, SPRHistoryModel2 *_model, SPRWorkSeparateTypeData _type, QString _titleFormat, qint64 _interval_axis_scale_in_sec/*, uint64_t _size_parts_in_sec*/)
{
    if((model != _model) && model){
        disconnect(model, &SPRHistoryModel2::separateDataReady, this, &SPRHitoryGraphics::onSeparateDataReady);
    }
    model = _model;
    if(model)
        connect(model, &SPRHistoryModel2::separateDataReady, this, &SPRHitoryGraphics::onSeparateDataReady);

    threads.clear();
    if(_thread.size() == 0){
        threads = model->getThreadList();
    } else {
        threads = _thread;
    }

    type  = _type;

    if(_titleFormat.isEmpty()){
        titleFormat = "( %1 )";
    } else {
        titleFormat = _titleFormat;
    }

    this->setCanvasBackground(QBrush(Qt::lightGray));
    current = nullptr;

    interval_axis_scale_in_sec = _interval_axis_scale_in_sec;
//    size_parts_in_sec = _size_parts_in_sec;

    foreach(uint8_t th, threads){

        HistoryGraphic *graph = new HistoryGraphic(th, QString(tr("Ручей %1")).arg(th+1), defThreadsColor[th]);

//        curvesData << QVector<QPointF>();
        lstGraphics << graph;
    }

    setGraphicTitle();

    grid = new QwtPlotGrid();
    grid->setMajorPen(QColor(Qt::white), 1);

    grid->attach(this);

    legend = new QwtLegend();
    QFont sys = parentWidget()->font();
    sys.setPointSize(8);
    legend->setFont(sys);
    this->insertLegend(legend, QwtPlot::BottomLegend);

    timeDraw = new TimeScaleDraw();
    this->setAxisScaleDraw(QwtPlot::xBottom, timeDraw);
    this->setAxisAutoScale(QwtPlot::yLeft);
    if(type == percentConcentrate2Input){
        this->setAxisScale(QwtPlot::yLeft, 0, 100);
        this->setAxisMaxMajor(QwtPlot::yLeft, 5);
        this->setAxisMaxMinor(QwtPlot::yLeft, 5);
    } else {
        this->setAxisMaxMajor(QwtPlot::yLeft, 5);
        this->setAxisMaxMinor(QwtPlot::yLeft, 2);
    }
    this->setAutoReplot(true);
    qint64 dt = QDateTime::currentSecsSinceEpoch();

    this->setAxisScale(QwtPlot::xBottom,(double)(dt - 600), (double)dt, (double)100);
    this->setAxisMaxMinor(QwtPlot::xBottom, 3);

    current = nullptr;
    setThreadsVisible(current);

    replot();
    onModelChange(nullptr);

}

void SPRHitoryGraphics::setThreadsVisible(HistoryGraphic *_current){
    QList<QwtPlotItem *> all;
    if(!_current){
        all = getGraphics(-1);
    } else {
        all = QList<QwtPlotItem*>({_current->getCurve()});
    }
    foreach(QwtPlotItem *gr, itemList()){
        gr->detach();
    };
    foreach(QwtPlotItem* gr, all){
        gr->attach(this);
    }
}

void SPRHitoryGraphics::setGraphicTitle(){
    QString titleText;
    if(!current){
        titleText = QString(titleFormat).arg(tr("все ручьи"));
    } else {
        titleText = QString(titleFormat).arg(current->getName());
    }
    setTitle(titleText);
    QwtText txt = QwtText(titleText);
    QFont system  = parentWidget()->font();
    system.setPointSize(10);
    txt.setFont(system);
    setTitle(txt);
}

void SPRHitoryGraphics::onDblClick(){
    if(current == lstGraphics.last()){
        current = nullptr;
    } else {
        if(!current){
            current = lstGraphics.first();
            setTitle(QString(titleFormat).arg(current->getName()));
        } else {
            QList<HistoryGraphic*>::iterator it = lstGraphics.begin();
            while(*it != current && *it != lstGraphics.last()) it++;

            current = *(++it);
        }
    }
    setThreadsVisible(current);
    setGraphicTitle();
    onModelChange(nullptr);
}
/**
 * @brief SPRHitoryGraphics::onModelChange
 */
void SPRHitoryGraphics::onModelChange(IModelVariable *){
}

void SPRHitoryGraphics::onSeparateDataReady()
{
    qint64 dt = QDateTime::currentDateTime().toSecsSinceEpoch();
    if(model){
        double y = 0;
        QPointF val = QPointF(dt, 0);
        foreach(HistoryGraphic* gr, lstGraphics){
            uint8_t th = gr->getThread();
            if(type == percentConcentrate2Input){
                QVector<SPRHistoryModel2::histDataOneThread> *avr = model->getAvr10mRecords();
                double zn = (*avr)[th].data.i_prd[2];
                if(zn > 1e-6){
                    y = ((*avr)[th].data.i_prd[0] / zn) * 100;
                }
            } else {
                QVector<SPRHistoryModel2::histDataOneThread> *avr = model->getAvr10mRecords();
                QVector<SPRHistoryModel2::histDataOneThread> *diff = model->getDiffRecords();
                double t = static_cast<double>((*avr)[th].data.size) / 1000;
//                if((*diff)[th].d.dt > 1e-6){
//                    y = (*avr)[gr->getThread()].d.i_prd[2] / (*diff)[th].d.mdt;
                    y = (*avr)[gr->getThread()].data.i_prd[2] * 1000;
//                }
                if(th == 0){
//                    qDebug() << "t: " << (*diff)[th].data.mdt << "; avr:"<< (*avr)[gr->getThread()].data.i_prd[2] << "; y:" << y << "; size:" << (*avr)[th].data.size << "; x/y:" << (*avr)[gr->getThread()].data.i_prd[2] / (*avr)[th].data.size;
                }
            }
            val.setY(y);
            gr->setCurveDataValue(val, interval_axis_scale_in_sec);
            this->setAxisScale(QwtPlot::xBottom,(dt - interval_axis_scale_in_sec), dt, /*static_cast<double>(size_parts_in_sec*15)*/ interval_axis_scale_in_sec / 10);
//            setThreadsVisible(current);
//            replot();
        }
    }
    replot();
}

QwtPlotCurve *SPRHitoryGraphics::HistoryGraphic::getCurve() const
{
    return curve;
}
