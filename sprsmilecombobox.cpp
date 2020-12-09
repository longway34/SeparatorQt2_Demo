#include "sprsmilecombobox.h"
#include <QDir>

#include <QStringList>
#include <QApplication>

SPRSmileComboBoxDelegate::SPRSmileComboBoxDelegate(QObject* parent) : QStyledItemDelegate(parent)
{

}

void SPRSmileComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto o = option;
    initStyleOption(&o, index);
    o.decorationSize.setWidth(o.rect.width());
    auto style =  o.widget ? o.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &o, painter, o.widget);
}


SPRSmileComboBox::SPRSmileComboBox(QWidget *parent) : QComboBox(parent)
{
     QDir dir(":/smiles/icons/Smiles");

     this->setItemDelegate(new SPRSmileComboBoxDelegate(this));
     addItem(QIcon(":/smiles/icons/Smiles/smileys_001_01.png"), QString(""), QVariant(QString(":/icons/Smiles/831758b0039b.png")));


     QFileInfoList lst = dir.entryInfoList();
       foreach(QFileInfo fi, lst){
          QString fpath = fi.absoluteFilePath();
          QStringList parts = fpath.split('/');
          if(parts.last() != QString("smileys_001_01.png")){

              addItem(QIcon(fpath), QString(""), QVariant(QString(fpath)));
          }

       }
}
