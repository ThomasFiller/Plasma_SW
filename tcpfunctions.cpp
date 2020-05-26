#include "TcpFunctions.h"

#define DEBUGMODUS

QString strReceivedString;
QString strSent;

TcpFunctions::TcpFunctions(QTcpSocket *socket, QObject *parent): QObject(parent)
{
    socketTcp = socket;
    QObject::connect( socketTcp, SIGNAL(readyRead()), SLOT(readTcpData()));
    QObject::connect(socketTcp, SIGNAL(disconnected()),this, SLOT(disconnected()));
}

void TcpFunctions::disconnected()
{
    qDebug() << "Verbindung unterbrochen...";
    emit tcpDisconnected();
}

void TcpFunctions::readTcpData()//slot, der aufgerufen wird, wenn etwas empfangen wurde
{
//QString strTmp;
    ReceivedTcpData = socketTcp->readAll();
    //strTmp = (QString::fromUtf8(ReceivedTcpData));
    //strTmp = strTmp.fromUtf8(ReceivedTcpData);
    if(ReceivedTcpData.length()>5)
    {
    //    ui->lblReceivedString->setText(ReceivedTcpData);
        strReceivedString.clear();
        strReceivedString.append(strReceivedString.fromUtf8(ReceivedTcpData));
        //strReceivedString = strReceivedString.fromUtf8(ReceivedTcpData);;
#ifdef DEBUGMODUS
//    qDebug() << "Empfangen: " << strReceivedString;
#endif

        //emit Sent(strSent);
    }
}

QString TcpFunctions::SendCmd(QString strDevice,unsigned char ucAddress, QString strCmd, QString strPrm)
{
    QByteArray *data = new QByteArray;
    QString strAddress = QString::number(ucAddress);

    data->append(strDevice);
    data->append(':');
    data->append(strAddress);
    data->append(':');
    data->append(strCmd);

    if(strPrm.compare(""))
    {
        data->append(':');
        data->append(strPrm);
    }
    data->append(' ');

//    if( socketTcp->waitForConnected() )
    {
//        strSent = QString::fromUtf8(*data);
//        socketTcp->write( *data );
#ifdef DEBUGMODUS
//    qDebug() << "Sendestring to other device: " << strSent;
#endif
        //Empfange Daten
//        if(socketTcp->waitForReadyRead())
        {
//            return(strReceivedString);
        }
//        else
        {
//            return("");
        }
    }
//    else
    {
        #ifdef DEBUGMODUS
                    qDebug() << "Verbindung unterbrochen!!!!!!!!!!!!!!!!!!!!!!!!";
        #endif
        return("No connection");
    }


}

TcpFunctions::~TcpFunctions()
{

}
