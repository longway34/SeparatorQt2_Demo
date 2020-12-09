#ifndef SPRSMILECOMBOBOX_H
#define SPRSMILECOMBOBOX_H

#include <QObject>
#include <QWidget>

#include <QComboBox>
#include <QStyledItemDelegate>

struct SPRSmileComboBoxDelegate: public QStyledItemDelegate
{
    SPRSmileComboBoxDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class SPRSmileComboBox : public QComboBox
{
public:
    explicit SPRSmileComboBox(QWidget *parent=nullptr);
};

#endif // SPRSMILECOMBOBOX_H
