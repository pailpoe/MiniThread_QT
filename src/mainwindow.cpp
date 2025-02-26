/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    console = new Console;
    console->setEnabled(false);
    serial = new QSerialPort(this);
    settings = new SettingsDialog;



    QFont MyFont( "Monospace", 100, QFont::Bold);
    //QFont font("Monospace");
    MyFont.setStyleHint(QFont::TypeWriter);


    labelX = new QLabel;
    labelX->setStyleSheet("QLabel { background-color : black; color : white; }");
    labelX->setFont(MyFont);
    labelX->setFixedWidth(1100);
    DisplayX(0);


    labelY = new QLabel;
    labelY->setStyleSheet("QLabel { background-color : black; color : white; }");
    labelY->setFont(MyFont);
    labelY->setFixedWidth(1100);
    DisplayY(0);

    labelC = new QLabel;
    labelC->setStyleSheet("QLabel { background-color : black; color : white; }");
    labelC->setFont(MyFont);
    labelC->setFixedWidth(1100);
    DisplayC(0);

    labelM = new QLabel;
    labelM->setStyleSheet("QLabel { background-color : black; color : white; }");
    labelM->setFont(MyFont);
    labelM->setFixedWidth(1100);
    DisplayM(0);

    DroLayout = new QVBoxLayout;
    DroLayout->setAlignment(Qt::AlignTop);
    DroLayout->addWidget(labelX);
    DroLayout->addWidget(labelY);
    DroLayout->addWidget(labelC);
    DroLayout->addWidget(labelM);



    mainLayout = new QHBoxLayout;
    mainLayout->addLayout(DroLayout);
    mainLayout->addWidget(console);

    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout(mainLayout);
    setCentralWidget(window);

   // setLayout(mainLayout);
//

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    status = new QLabel;
    ui->statusBar->addWidget(status);

    initActionsConnections();

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);

//! [2]
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
//! [2]
    connect(console, &Console::getData, this, &MainWindow::writeData);
//! [3]
}
//! [3]

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        console->setEnabled(true);
        console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        serial->setDataTerminalReady(true);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
//! [6]

//! [7]
//!
//!
//!
//!

void MainWindow::TestSlot()
{
    qDebug() << "yes";
    qDebug() << serial->bytesAvailable();

}
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    qDebug() << "yes";
    TraitementSerialIn(data);
    console->putData(data);
}
void MainWindow::TraitementSerialIn(QByteArray data)
{
    int Pos;
    bool btEnd = false;
    QByteArray DataExtract;
    TempSerialIn += data;
    while ( btEnd == false )
    {
        Pos = TempSerialIn.indexOf('\n');
        if(Pos != -1)
        {
            DataExtract = TempSerialIn.left(Pos);
            TempSerialIn = TempSerialIn.right(TempSerialIn.length()-Pos-1);
            DataToAnalyse(DataExtract);

        }
        else
        {
            btEnd = true;
        }
    }
    //qDebug() << TempSerialIn;
}
void MainWindow::DataToAnalyse(QByteArray data)
{
    int size;
    int count;
    QList<QByteArray> Parameter;
    Parameter = data.split(':');
    size = Parameter.size();
    for(count=0;count<size;count++)
    {
        if( Parameter[count].data()[0] == 'X' )
        {
            Parameter[count]= Parameter[count].right(Parameter[count].size()-1);
            DisplayX(Parameter[count].toFloat());
            //qDebug()<< Parameter[count].toFloat();
        }
        if( Parameter[count].data()[0] == 'Y' )
        {
            Parameter[count]= Parameter[count].right(Parameter[count].size()-1);
            DisplayY(Parameter[count].toFloat());
            //qDebug()<< Parameter[count].toFloat();
        }
        if( Parameter[count].data()[0] == 'C' )
        {
            Parameter[count]= Parameter[count].right(Parameter[count].size()-1);
            DisplayC(Parameter[count].toFloat());
            //qDebug()<< Parameter[count].toFloat();
        }
        if( Parameter[count].data()[0] == 'M' )
        {
            Parameter[count]= Parameter[count].right(Parameter[count].size()-1);
            DisplayM(Parameter[count].toFloat());
            //qDebug()<< Parameter[count].toFloat();
        }
    }
    qDebug() << data;

}



//! [7]

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionConfigure, &QAction::triggered, settings, &SettingsDialog::show);
    connect(ui->actionClear, &QAction::triggered, console, &Console::clear);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionTest, &QAction::triggered, this, &MainWindow::TestSlot);


}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::DisplayX(float x)
{
    char buffer[16];
    sprintf(buffer,"X %+09.3f",x);
    labelX->setText(buffer);

}
void MainWindow::DisplayY(float y)
{
    char buffer[16];
    sprintf(buffer,"Y %+09.3f",y);
    labelY->setText(buffer);

}
void MainWindow::DisplayC(float c)
{
    char buffer[16];
    sprintf(buffer,"C %+09.3f",c);
    labelC->setText(buffer);

}
void MainWindow::DisplayM(float m)
{
    char buffer[16];
    sprintf(buffer,"M %+09.3f",m);
    labelM->setText(buffer);

}
