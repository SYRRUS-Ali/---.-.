#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include<QString>

MyTcpServer::~MyTcpServer()
{
    myTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    myTcpServer = new QTcpServer(this);

    connect(myTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!myTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
    QTcpSocket *curr_myTcpSocket;
    curr_myTcpSocket = myTcpServer->nextPendingConnection();
    curr_myTcpSocket->write("Hello, World!!! I am echo server!\r\n");
    connect(curr_myTcpSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(curr_myTcpSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);
    myTcpSocket[curr_myTcpSocket->socketDescriptor()] = curr_myTcpSocket;
}

void MyTcpServer::slotServerRead(){
    QTcpSocket *curr_myTcpSocket = qobject_cast<QTcpSocket*>(sender());
    QString res = "";
    while(curr_myTcpSocket->bytesAvailable() > 0)
    {
        QByteArray array = curr_myTcpSocket->readAll();
        qDebug() << array << "\n";
        if(array == "\x01")
        {
            curr_myTcpSocket->write(res.toUtf8());
            res = "";
        }
        else
            res.append(array);
    }
    curr_myTcpSocket->write(res.toUtf8());
}

void MyTcpServer::slotClientDisconnected() {
    QTcpSocket *curr_myTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if (!curr_myTcpSocket) return;

    myTcpSocket.remove(curr_myTcpSocket->socketDescriptor());
    curr_myTcpSocket->close();
    curr_myTcpSocket->deleteLater();
}
