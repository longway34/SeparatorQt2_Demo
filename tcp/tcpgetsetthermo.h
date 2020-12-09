#ifndef TCPGETSETTHERMO_H
#define TCPGETSETTHERMO_H

#include <QObject>

#include "tcp/TCPCommandSet.h"
#include "models/sprmainmodel.h"

class TCPGetSetThermo : public TCPCommandSet
{
    Q_OBJECT
    
//    SPRMainModel *mainModel;
    bool getSetCommand;
    
    
    uint16_t setTermMax;
    uint16_t setTermMin;
    
public:
    struct sterms{
        double termLo;
        double termHi;
        double termCurrent;
    } terms;

    TCPGetSetThermo(bool _get_set=true, uint16_t _min = 0, uint16_t _max=0);
    

    sterms getTerms() const;

public slots:
    virtual void go(TCPCommand *_command);
};

#endif // TCPGETSETTHERMO_H
