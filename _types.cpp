#include "_types.h"

static const EnumElements elementsValue[MAX_SPR_SPECTOR_ELEMENTS] = {Ns, Fe, Cu, Mo, Zn, Mg};

static const DefaultElementsProperty elementsProperty[MAX_SPR_SPECTOR_ELEMENTS] = {
    {"Ns", "Рассеенка", QColor(255,0,255,100), 0, 80, 256},
    {"Fe", "Железо", QColor(255, 0, 0, 100), 1, 20, 40},
    {"Cu", "Медь", QColor(0,255,0,100), 40, 2, 60},
    {"Mo", "Молибден", QColor(0,0,255,100), 3, 60, 80},
    {"Zn", "Цинк", QColor(255,255,0,100), 4, 80, 100},
    {"Mg", "Магний", QColor(0,255,255,100), 5, 100, 120},

};

static const QColor mainColors[] = {Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow,
                                    Qt::darkRed, Qt::darkBlue, Qt::darkGreen, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
                                    Qt::gray, Qt::lightGray, Qt::white, Qt::darkGray};

const SPRThreadList getAllThreadsListDefault(uint8_t ths){
    SPRThreadList res;
    ths = (ths > MAX_SPR_MAIN_THREADS) ? MAX_SPR_MAIN_THREADS : ths;
    for(uint8_t th=0; th<ths; th++) res.append(th);
    return res;
}

SPRDoubleValidator::SPRDoubleValidator(){
    _decPoints.append(".");
    _decPoints.append(",");
    loc = QLocale(QLocale::C);
}

QValidator::State SPRDoubleValidator::validate(QString &s, int &pos) const{
    foreach(QString _dp, _decPoints){
//        s.replace(_dp, loc.decimalPoint());
//          s.replace(_dp, locale().decimalPoint());
    }
    return QDoubleValidator::validate(s, pos);
}
