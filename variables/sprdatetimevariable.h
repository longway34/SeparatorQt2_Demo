#ifndef SPRDATETIMEVARIABLE_H
#define SPRDATETIMEVARIABLE_H

#include <QObject>
#include <QDateTime>
#include "models/imodelvariable.h"


class SPRTimeVariable : public IModelVariable
{
    QTime data;
    QString format;
public:
    SPRTimeVariable(QDomDocument *parent, QString xpath, QTime defValue, IModelVariable *_mvparent = nullptr, QString _format="hh:mm:ss");

    virtual QTime getData();
    void setData(const QTime &value, QString _format="");
    QString getFormat() const;
    void setFormat(const QString &value);
    QString toString(QString frm="");
};

class SPRDateTimeVariable : public IModelVariable
{
    QDateTime data;
    QString format;
public:
    SPRDateTimeVariable(QDomDocument *parent, QString xpath, QDateTime defValue, IModelVariable *_mvparent=nullptr, QString _format = "dd.MM.yyyy hh:mm:ss");

    virtual QDateTime getData();
    void setData(const QDateTime &value, QString _format="");
    QString getFormat() const;
    void setFormat(const QString &value);
    QString toString(QString frm="");
};

#endif // SPRDATETIMEVARIABLE_H
