/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ITCPCommand.cpp
 * Author: longway
 * 
 * Created on 10 февраля 2018 г., 16:47
 */

#include "ITCPCommand.h"

TCPEchoLogsWidget *ITCPCommand::getLogWidget() const
{
    return logWidget;
}

void ITCPCommand::setLogWidget(TCPEchoLogsWidget *value)
{
    logWidget = value;
}

QString ITCPCommand::getName() const
{
    return name;
}

void ITCPCommand::setName(const QString &value)
{
    name = value;
}

ITCPCommand::ITCPCommand(): logWidget(nullptr) {
}


ITCPCommand::~ITCPCommand() {
    sendData.clear();
    replayData.clear();
}

