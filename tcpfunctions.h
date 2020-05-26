#ifndef TCPFUNCTIONS_H
#define TCPFUNCTIONS_H

#include <QObject>

class TcpFunctions : public QObject
{
    Q_OBJECT

public:
    TcpFunctions(QTcpSocket *socket, QObject *parent);
    ~TcpFunctions();

    QString SendCmdToMcps(unsigned char ucAddress, QString strCmd, QString strPrm);
    QString SendCmd(QString strDevice,unsigned char ucAddress, QString strCmd, QString strPrm);

private:
    QTcpSocket *socketTcp;
    QByteArray ReceivedTcpData;

private slots:
    void readTcpData();
    void disconnected();

signals:
    void Received(QString strReceived, QString strSent);
    void tcpDisconnected();
   // void Sent(QString strSent);
};
#endif // TCPFUNCTIONS_H
