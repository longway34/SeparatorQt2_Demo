#ifndef SPRCOMPLEXTABWIDGET_H
#define SPRCOMPLEXTABWIDGET_H

#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabWidget>


struct _tabData{
    QString title;
    QColor color;
    QIcon icon;
};

class SPRComplexTabBar: public QTabBar
{
public:
    SPRComplexTabBar(QHash<int, _tabData> *colors, QWidget *parent=nullptr):QTabBar(parent){
        mColors = colors;
        first = QIcon(":/icons/icons/b_Complex.png");
        last = QIcon(":/icons/icons/b_Plus.png");
    }

protected:
    QIcon first;
    QIcon last;
private:
    QHash<int, _tabData> *mColors;

//    void paintEvent(QPaintEvent */*event*/){

//        QStylePainter painter(this);
//        QStyleOptionTab opt;

//        for(int i = 0;i < count();i++)
//        {
////            initStyleOption(&opt,i);
////            opt.shape = QTabBar::RoundedNorth;
////            opt.text = (*mColors)[i].title;
////            QPalette pal = opt.palette;
////            opt.palette.setColor(QPalette::WindowText, (*mColors)[i].color);
////            if(i == 0){
////                opt.icon = first;
////            } else if(i == count()-1){
////                opt.shape = QTabBar::TriangularNorth;
////                opt.icon = last;
////            } else {
////                opt.icon = (*mColors)[i].icon;
////            }

////            opt.rect.setSize(QSize(fontMetrics().width(opt.text)+opt.iconSize.width(), opt.rect.height()));

//            painter.drawControl(QStyle::CE_TabBarTabShape,opt);
//            painter.drawControl(QStyle::CE_TabBarTabLabel,opt);
//        }
//    }


};

class SPRComplexTabWidget : public QTabWidget
{
    Q_OBJECT

    QHash<int, _tabData> tabsNameAndColor;
public:
    SPRComplexTabWidget(QWidget *parent=nullptr):QTabWidget(parent){

        tabsNameAndColor[0] = {"All", QColor("black"), QIcon(":/icons/icons/b_Complex.png")};
        tabsNameAndColor[-1] = {"", QColor("black"), QIcon(":/icons/icons/b_Plus.png")};

        setTabBar(new SPRComplexTabBar(&tabsNameAndColor));
    }
    void setTabsNameAndColor(int index, QString title, const QColor &color, QIcon icon)
    {
        if(tabsNameAndColor.contains(index)){
            tabsNameAndColor[index].title = title;
            tabsNameAndColor[index].color = color;
            tabsNameAndColor[index].icon = icon;
        } else {
            tabsNameAndColor.insert(index, {title, color, icon});
        }
    }
};

#endif // SPRCOMPLEXTABWIDGET_H


