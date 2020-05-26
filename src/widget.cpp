#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QNetworkConfigurationManager>
#include <QThread>
#include "settingsComPort.h"
#include <qmessagebox.h>


#define DEBUGMODUS

#define SOLUTION_800X600 0
#define SOLUTION_1920X1080 1
#define SOLUTION_1920X1200 2
#define SOLUTION_MAX 3
#define SOLUTION_80 4

#define VDD_C   ((double)(48.7))
#define VDD_A   ((double)(-34.13))
#define VDD_X0  ((double)(29860))
#define VDD_L   ((double)(0))

#define VGG_C   ((double)(-7.5))
#define VGG_A   ((double)(6.1564))
#define VGG_X0  ((double)(6058.67))
#define VGG_L   ((double)(0.000012598518))


QVector<unsigned int> Frequency(20);//

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    //******Begrüßungsbildschirm
    QPixmap pixmap(":/Plasma_Startpicture.png");
    QSplashScreen splash(pixmap);
    splash.show();
    bWidgetBooting = 1;
    this->setCursor(Qt::WaitCursor);
    //parent->processEvents();
    splash.showMessage("Connect ComPort");

    ui->setupUi(this);

    settings_ComPort = new settingsComPort;
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &Widget::readData);

    splash.showMessage("Load modules");

    QPalette pal;
    pal.setColor(QPalette::Background, Qt::black);
    this->setPalette(pal);
    ui->tabWidget->setStyleSheet("color: black;"
                                 "background-color: black;"
                                 "selection-color: #000000;"
                                 "selection-background-color: #A98332;");

    ui->spnVgg->setStyleSheet(CUSTOMER_SPIN_BOX_STYLE);
    ui->spnVdd->setStyleSheet(CUSTOMER_SPIN_BOX_STYLE);
//    ui->spnVgg->setVisible(false);
//    ui->spnVdd->setVisible(false);

    ui->spnCtrl1->setStyleSheet(CUSTOMER_SPIN_BOX_STYLE);
    ui->spnCtrl2->setStyleSheet(CUSTOMER_SPIN_BOX_STYLE);



    ui->spnSetup->setStyleSheet(CUSTOMER_SPIN_BOX_STYLE);

//    ui->boxFrequency->setStyleSheet(COMBOBOX_STYLE);
    ui->btnConfigComPort->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );
    ui->btnConnect->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );
    ui->btnReadSetup->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );
    ui->btnReadSetup->setVisible(false);
    ui->btnSaveSetup->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );
    ui->btnStartMeasure->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );
    ui->btnLeaveFbhonly->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );

    //ui->groupBoxSetupCust->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);
    ui->groupBoxMeasure->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);
    ui->groupBox->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);
    ui->groupBox_2->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);
    ui->groupBox_3->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);
    ui->groupBoxCtrl->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);

    ui->btnStartPlasma->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );

    ui->groupBoxVdd->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);
    ui->groupBoxVgg->setStyleSheet(GROUPBOX_STYLE GROUPBOX_TITLE);

    ui->btnResetGraf->setStyleSheet(BUTTON_COLOR BUTTON_COLOR_PRESSED );


//ui->tabWidget->tabBar()->show();
//ui->tabWidget->setCurrentIndex(0);
ui->tabWidget->tabBar()->hide();

//ui->lblW1->setVisible(false);
//ui->lblW1_2->setVisible(false);
//ui->lbl_PwrBack->setVisible(false);
//ui->lbl_PwrTo->setVisible(false);
//ui->lblTxtPwrBack->setVisible(false);
//ui->lblTxtPwrTo->setVisible(false);

    SetupPlot();

    //Setup timer
    tmrWatchdog = new QTimer(this);
    tmrWatchdog->setInterval(500);
    QObject::connect(tmrWatchdog, SIGNAL(timeout()), this, SLOT(tmrslotWatchdog()));

    tmrDelay = new QTimer(this);
    QObject::connect(tmrDelay, SIGNAL(timeout()), this, SLOT(tmrslotDelay()));

    tmrMeasure = new QTimer(this);
    QObject::connect(tmrMeasure, SIGNAL(timeout()), this, SLOT(tmrslotMeasure()));
    tmrMeasure->setInterval(GRAPH_TIME_INTERVALL);

    this->setCursor(Qt::ArrowCursor);
    bWidgetBooting = 0;
    splash.finish(this);//Schließe Begrüßungsbildschirm, sobald app gestartet ist

//    switchPwr = new Switch(ui->tabWidget->widget(0), 15,20);
//    connect(switchPwr, SIGNAL(Switched(bool)),this, SLOT (switchPwrClicked(bool)));



QPixmap pixLogo("FbhLogo.png");
ui->lbl_logo->setPixmap(pixLogo);

}

void Widget::on_tabWidget_currentChanged(int index)
{
    ReadAllDataCmd();
}

void Widget::WriteToRdd(char cReadAndAddress, unsigned char ucLen, char cCmd, QByteArray *Data)
{
    while (bComBusy) QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    if (bConnected)
    {
        char Checksum = 0;
        char cData;
        QByteArray *BytesToSend = new QByteArray;
        BytesToSend->append(cReadAndAddress);//Lesen + Adresse
        Checksum = cReadAndAddress;
        BytesToSend->append(ucLen);//Länge
        Checksum = Checksum^ucLen;
        BytesToSend->append(cCmd);//Command
        Checksum = Checksum^cCmd;

        char *pData = new char(ucLen-4);
        memcpy(pData, Data->data(), ucLen-4);

        for (unsigned int uiCnt=0; uiCnt<((unsigned int)ucLen-4); uiCnt++)
        {
            cData = pData[uiCnt];
            Checksum = Checksum ^ cData;
            BytesToSend->append(cData);
        }
        BytesToSend->append(Checksum);
        writeData(*BytesToSend);
        bComBusy = 1;
        tmrWatchdog->start();
        while (bComBusy) QCoreApplication::processEvents(QEventLoop::AllEvents, 1);
    }
    else
    {
        QMessageBox::critical(this, tr("No connection!"), tr("Press Button 'Connect to ComPort'"),  QMessageBox::Ok);
    }
}

void Widget::on_btnReadSetup_clicked()
{
    QByteArray *Data = new QByteArray;
    WriteToRdd(0xAA, 4, UART_SETUP_NO, Data);
}

void Widget::on_btnConfigComPort_clicked()
{
    settings_ComPort->StartDialog();
}

void Widget::openSerialPort()
{
    settingsComPort::Settings p = settings_ComPort->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);

    if (serial->open(QIODevice::ReadWrite)) {
#ifdef DEBUGMODUS
//        qDebug() << "Verbindung hergestellt";
#endif
        QByteArray *Data = new QByteArray;
        QMessageBox::information(this, tr("Connection established!"), tr("Device successful connected to PC"),  QMessageBox::Ok);
        bConnected = 1;
        ReadAllDataCmd();
    } else {
#ifdef DEBUGMODUS
//        qDebug() << "KEINE Verbindung hergestellt!!!!!!!!!!!!!!!!!!!!!!!!";
#endif
        QMessageBox::critical(this, tr("No connection!"), tr("Press Button Connect to ComPort"),  QMessageBox::Ok);
        bConnected = 0;
        closeSerialPort();
    }
}

void Widget::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    bConnected = 0;
    ui->btnConnect->setText("Connect to ComPort");
}

void Widget::ReadAllDataCmd()
{
bool bNoConnection=0;
    QByteArray *Data = new QByteArray;
    if (bConnected) WriteToRdd(0xAA, 4, UART_SETUP_NO, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_DEVICE_STATE, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_5V_GOOD, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_TEMPERATURE, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_STORAGE, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_SPARK_DURATION, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VDD_SPARK, Data); else bNoConnection=1;

    if (bConnected) WriteToRdd(0xAA, 4, UART_EN_VDD, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VDD_GOOD, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VDD, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_I_VDD, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VGG_SPARK, Data); else bNoConnection=1;

    if (bConnected) WriteToRdd(0xAA, 4, UART_EN_VGG, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VGG, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_I_VGG, Data); else bNoConnection=1;

    if (bConnected) WriteToRdd(0xAA, 4, UART_MW_PWR_TO, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_MW_PWR_BACK, Data); else bNoConnection=1;

    if (bConnected) WriteToRdd(0xAA, 4, UART_CTRL1, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_CTRL2, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VCTRL1_SPARK, Data); else bNoConnection=1;
    if (bConnected) WriteToRdd(0xAA, 4, UART_VCTRL2_SPARK, Data); else bNoConnection=1;

    if(bNoConnection)
    {
        QMessageBox::critical(this, tr("No connection!"), tr("Press Button 'Connect to ComPort'"),  QMessageBox::Ok);
    }
}

void Widget::writeData(const QByteArray &data)
{
    strSent.clear();
    for(int iCnt=0; iCnt<data.length(); iCnt++)
    {
        strSent.append("0x");
        strSent.append(strReceivedString.number((unsigned char)data[iCnt],16));
        strSent.append(" ");
    }
    ui->lblSent->setText(strSent);
    ui->lblReceived->setText("-----");
#ifdef DEBUGMODUS
    //qDebug() << "gesendet: " << data;
//    qDebug() << "gesendet: " << strSent;
#endif
    serial->write(data);
}

void Widget::NewValueReceived(unsigned int uiPhysVal, bool &FlagChangedByUart, QSpinBox *SpinBox)
{
#ifdef DEBUGMODUS
            //qDebug() << "NewValueReceived: uiPhysVal= " << uiPhysVal <<"; FlagChangedByUart= " << FlagChangedByUart<<"; SpinBox= " << SpinBox;
#endif
    if(uiPhysVal != ((unsigned int)SpinBox->value()))
    {
        FlagChangedByUart  = 1;
        SpinBox->setValue(uiPhysVal);
    }
}

void Widget::DeviceStateReceived(unsigned char ucDevState)
{
    ui->lblDeviceState->setText( strDeviceState[ucDevState]);
    if (STATE_OVERTEMP==ucDevState)
    {
        QMessageBox::critical(this, tr("Overtemperature!"), tr("Temperature of Device exceeded the critical value of 80°C! Device switched completely off"),  QMessageBox::Ok);
        ui->btnStartMeasure->setText("Start Measure");
        tmrMeasure->stop();
    }
    if (STATE_IVDD_ERROR==ucDevState)
    {
        QMessageBox::critical(this, tr("Error I(Drain)!"), tr("Drain Current exceeded the critical value of 2,5 A! Device switched completely off"),  QMessageBox::Ok);
        ui->btnStartMeasure->setText("Start Measure");
        tmrMeasure->stop();
    }
    if (STATE_IVGG_ERROR==ucDevState)
    {
        QMessageBox::critical(this, tr("Error I(Gate)!"), tr("Gate Current exceeded the critical value of 10 mA! Device switched completely off"),  QMessageBox::Ok);
        ui->btnStartMeasure->setText("Start Measure");
        tmrMeasure->stop();
    }
}

void Widget::readData()
{
#ifdef DEBUGMODUS
//    QString strHighByte, strLowByte;
#endif
unsigned int uiPhysicalVal;
typUnsignedWord uiDigData;
typSignedWord iDigData;
QString strData;
unsigned char ucValue;
double dVgg, dVdd, dVal;

    //serial->bytesAvailable()
    if(uiRecLen == 0)
    {
        arReceivedData = serial->readAll();
        uiRecLen = arReceivedData[1];
    }
    else
    {
        arReceivedData.append(serial->readAll());
    }
    if(((unsigned int)arReceivedData.length()) > (uiRecLen-1))
    {
        tmrWatchdog->stop();
        bComBusy = 0;
        strReceivedString.clear();
        for(unsigned int uiCnt=0; uiCnt<((unsigned int)arReceivedData.length()); uiCnt++)
        {
            strReceivedString.append("0x");
            strReceivedString.append(strReceivedString.number((unsigned char)arReceivedData[uiCnt],16));
            strReceivedString.append(" ");
        }

        ui->lblReceived->setText(strReceivedString);
#ifdef DEBUGMODUS
        //qDebug() << "Empfangen arReceivedData: " << arReceivedData;
//        qDebug() << "Empfangen strReceivedString: " << strReceivedString;
#endif
        //strReceivedString.append(strReceivedString.fromUcs4(arReceivedData));
        char cCmd = arReceivedData[2];
        if (arReceivedData[0] & 0x80) //PC wollte lesen
        {
            switch (cCmd)
            {
                case UART_SETUP_NO:
                    if(arReceivedData[3] != char(ui->spnSetup->value()))
                    {
                        spnSetupChangedByUart = 1;
                        ui->spnSetup->setValue(arReceivedData[3]);
                    }
                    break;
                case UART_SAVE_SETUP:
                        ParameterChanged = 0;
                    break;
                case UART_DEVICE_STATE:
                    DeviceStateReceived(arReceivedData[3]);
                break;
                case UART_5V_GOOD:
                    if(!arReceivedData[3]) QMessageBox::critical(this, tr("Low Voltage!"), tr("5 V supply improper"),  QMessageBox::Ok);;
                    break;
                case UART_TEMPERATURE:
                    iDigData.stBytes.ucLowByte = arReceivedData[4];
                    iDigData.stBytes.ucHighByte = arReceivedData[3];
                    //dTemperature = ((double)(iDigData.iWord)+(OFFSET_READTEMP)) / (FACTOR_READTEMP);
                    dTemperature = (double)(iDigData.iWord);// / (double)(4);

#ifdef DEBUGMODUS
//+                    qDebug() << "dTemperature= " << dTemperature;
#endif

                    dTemperature = ((double)(dTemperature)*FACTOR_READTEMP)+OFFSET_READTEMP;
                    dTemperature = round(dTemperature);
                    strData.sprintf("%.0f", dTemperature);
                    ui->lblTemperature->setText(strData);

                    break;

                case UART_STORAGE:
                        StorageChangedByUart = 1;
                        ui->editVddmin->setText(QString::number((unsigned char)arReceivedData[3+VDD_MIN]));
                        ui->editVddmax->setText(QString::number((unsigned char)arReceivedData[3+VDD_MAX]));
                        ui->editVggmin->setText(QString::number((unsigned char)arReceivedData[3+VGG_MIN]));
                        ui->editVggmax->setText(QString::number((unsigned char)arReceivedData[3+VGG_MAX]));
                        ui->spnVdd->setMinimum((unsigned char)arReceivedData[3+VDD_MIN]);
                        ui->spnVdd->setMaximum((unsigned char)arReceivedData[3+VDD_MAX]);
                        ui->spnVgg->setMinimum((unsigned char)arReceivedData[3+VGG_MIN]);
                        ui->spnVgg->setMaximum((unsigned char)arReceivedData[3+VGG_MAX]);
                        ui->sliderVdd->setMinimum((unsigned char)arReceivedData[3+VDD_MIN]);
                        ui->sliderVdd->setMaximum((unsigned char)arReceivedData[3+VDD_MAX]);
                        ui->sliderVgg->setMinimum((unsigned char)arReceivedData[3+VGG_MIN]);
                        ui->sliderVgg->setMaximum((unsigned char)arReceivedData[3+VGG_MAX]);
                        ui->editSerNo->setText(QString::number((unsigned char)arReceivedData[3+SER_NO]));
                        ui->lblSerNo->setText(QString::number((unsigned char)arReceivedData[3+SER_NO]));
                    break;


                case UART_READ_ALL:
                        DeviceStateReceived(arReceivedData[3]);
                        if(arReceivedData[4] & 0x01) ui->checkBoxVddEnabled->setChecked(true); else ui->checkBoxVddEnabled->setChecked(false);
                        if(arReceivedData[4] & 0x02) ui->checkBoxVggEnabled->setChecked(true); else ui->checkBoxVggEnabled->setChecked(false);
                        //if(arReceivedData[4] & 0x04) ui->lblVddGood->setText("VDD good"); else ui->lblVddGood->setText("VDD no good");
                        if((arReceivedData[4]>>4) != char(ui->spnSetup->value()))
                        {
                            spnSetupChangedByUart = 1;
                            ui->spnSetup->setValue((arReceivedData[4]>>4));
                        }
                            //VDD
                        uiDigData.stBytes.ucHighByte = arReceivedData[5];
                        uiDigData.stBytes.ucLowByte = arReceivedData[6];
                        uiPhysicalVal = (unsigned int)(round(((double)(uiDigData.uiWord)) / FACTOR_VDD));
                        NewValueReceived(uiPhysicalVal, spnVddChangedByUart, ui->spnVdd);
                        ui->sliderVdd->setValue(ui->spnVdd->value());
                        iVdd = uiPhysicalVal;
                            //VGG
                        uiDigData.stBytes.ucHighByte = arReceivedData[7];
                        uiDigData.stBytes.ucLowByte = arReceivedData[8];
                        uiPhysicalVal = (unsigned int)(round(((double)(uiDigData.uiWord)) / (FACTOR_VGG)));
                        NewValueReceived(uiPhysicalVal, spnVggChangedByUart, ui->spnVgg);
                        ui->sliderVgg->setValue(ui->spnVgg->value());
                        iVgg = uiPhysicalVal;
                            //Idd
                        iDigData.stBytes.ucHighByte = arReceivedData[9];
                        iDigData.stBytes.ucLowByte = arReceivedData[10];
                        dIdd = (((double)(iDigData.iWord)-(OFFSET_I_VDD)) / (FACTOR_I_VDD))/((double)(1000));
                        if (dIdd < 0) dIdd = 0;
                        strData.sprintf("%.1f", (double)(dIdd));
                        ui->lbl_Ivdd->setText(strData);
                            //Igg
                        iDigData.stBytes.ucHighByte = arReceivedData[11];
                        iDigData.stBytes.ucLowByte = arReceivedData[12];
                        dIgg=((((double)(iDigData.iWord+OFFSET_I_VGG)) / FACTOR_I_VGG));
                        if(!(ui->checkBoxVggEnabled->checkState())) dIgg = 0;
                        strData.sprintf("%.1f", dIgg);
                        ui->lbl_Ivgg->setText(strData);
                            //Temperatur
                        iDigData.stBytes.ucHighByte = arReceivedData[13];
                        iDigData.stBytes.ucLowByte = arReceivedData[14];
                        dTemperature = (double)(iDigData.iWord);//
                        dTemperature = ((double)(dTemperature)*FACTOR_READTEMP)+OFFSET_READTEMP;
                        dTemperature = round(dTemperature);
                        strData.sprintf("%.0f", dTemperature);
                        ui->lblTemperature->setText(strData);
                        //PwrTo
                        iDigData.stBytes.ucHighByte = arReceivedData[15];
                        iDigData.stBytes.ucLowByte = arReceivedData[16];
                        dVal=(double)(iDigData.iWord);
                        uiPwrTo = (unsigned int)(round(FACTOR_P_TO*dVal*dVal));
                        //strData.sprintf("%.0f", dTecVoltage*1000);
                        strData.sprintf("%d", uiPwrTo);
                        ui->lbl_PwrTo->setText(strData);
#ifdef DEBUGMODUS
 //                   qDebug() << "(unsigned int)iDigData.iWord= " << (unsigned int)iDigData.iWord << "; uiPwrTo= " << uiPwrTo;
 //                   qDebug() << "strData= " << strData;
#endif
                        //PwrBack
                        iDigData.stBytes.ucHighByte = arReceivedData[17];
                        iDigData.stBytes.ucLowByte = arReceivedData[18];
                        //uiPwrBack = (unsigned int)(round(((double)(iDigData.iWord)-(OFFSET_P_BACK)) / (FACTOR_P_BACK)));
                        dVal=(double)(iDigData.iWord);
                        uiPwrBack = (unsigned int)(round(FACTOR_P_BACK*dVal*dVal));
                        strData.sprintf("%d", uiPwrBack);
                        ui->lbl_PwrBack->setText(strData);

                        iDigData.stBytes.ucHighByte = arReceivedData[19];
                        iDigData.stBytes.ucLowByte = arReceivedData[20];
                        VddMeasured = (double)(iDigData.iWord);
#ifdef DEBUGMODUS
 //                   qDebug() << "VddMeasured= " << VddMeasured << "; iDigData.iWord = " << iDigData.iWord;
#endif
                        VddMeasured = ((double)(VddMeasured)*FACTOR_VDD_MESURED)+OFFSET_VDD_MESURED;
                        strData.sprintf("%.1f", VddMeasured);
                        ui->lblVddMeasured->setText(strData);

                        iDigData.stBytes.ucHighByte = arReceivedData[21];
                        iDigData.stBytes.ucLowByte = arReceivedData[22];
                        VggMeasured = (double)(iDigData.iWord);//
#ifdef DEBUGMODUS
 //                   qDebug() << "VggMeasured= " << VggMeasured << "; iDigData.iWord = " << iDigData.iWord;
#endif
                        VggMeasured = ((double)(VggMeasured)*FACTOR_VGG_MESURED)+OFFSET_VGG_MESURED;
                        strData.sprintf("%.1f", VggMeasured);
                        ui->lblVggMeasured->setText(strData);

                    break;

                case UART_SPARK_DURATION:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    ui->editSparkDuration->setText(QString::number((unsigned int)uiDigData.uiWord));
                    break;

                //VDD------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                case UART_EN_VDD:
                    if(arReceivedData[3]) ui->checkBoxVddEnabled->setChecked(true); else ui->checkBoxVddEnabled->setChecked(false);
                    break;
                case UART_VDD_GOOD:
                    //if(arReceivedData[3]) ui->lblVddGood->setText("VDD good"); else ui->lblVddGood->setText("VDD no good");
                    break;
                case UART_VDD:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    uiPhysicalVal = (unsigned int)(round(((double)(uiDigData.uiWord)) / FACTOR_VDD));
                    NewValueReceived(uiPhysicalVal, spnVddChangedByUart, ui->spnVdd);
                    ui->sliderVdd->setValue(ui->spnVdd->value());
                    iVdd = uiPhysicalVal;
                    break;
                case UART_I_VDD:
                    iDigData.stBytes.ucLowByte = arReceivedData[4];
                    iDigData.stBytes.ucHighByte = arReceivedData[3];
                    //iVdd = (unsigned int)(((double)(iDigData.iWord)-(OFFSET_I_VDD)) / (FACTOR_I_VDD));
                    dIdd = (((double)(iDigData.iWord)-(OFFSET_I_VDD)) / (FACTOR_I_VDD))/((double)(1000));
                    if (dIdd < 0) dIdd = 0;
                    strData.sprintf("%.1f", (double)(dIdd));
#ifdef DEBUGMODUS
//                    qDebug() << "dIdd= " << dIdd;
 //                   qDebug() << "strData= " << strData;
#endif
                    //strData.sprintf("%d", iVdd);
                    ui->lbl_Ivdd->setText(strData);
                    break;

                case UART_VDD_SPARK:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    uiPhysicalVal = (unsigned int)(round(((double)(uiDigData.uiWord)) / FACTOR_VDD));
                    ucValue = (unsigned char)uiPhysicalVal;
                    ui->editVddSpark->setText(QString::number(ucValue));
                    //strData = QString::number((unsigned int)round(Vdd3*(double)(ucValue*ucValue*ucValue)+Vdd2*(double)(ucValue*ucValue)+Vdd1*(double)(ucValue)+Vdd0));

                    dVal = ((double)(ucValue)) * FACTOR_VDD;
                    dVdd = VDD_C + (VDD_A * (1-exp(-dVal / VDD_X0))) + VDD_L*dVal;
                    strData = QString::number((unsigned int)round(dVdd));

                    ui->lblVddSpark->setText(strData);
                    break;

                //VGG------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                case UART_EN_VGG:
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    if(arReceivedData[3]) ui->checkBoxVggEnabled->setChecked(true); else ui->checkBoxVggEnabled->setChecked(false);
                    break;
                case UART_VGG:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    uiPhysicalVal = (unsigned int)(round(((double)(uiDigData.uiWord)) / (FACTOR_VGG)));
                    NewValueReceived(uiPhysicalVal, spnVggChangedByUart, ui->spnVgg);
                    ui->sliderVgg->setValue(ui->spnVgg->value());
                    iVgg = uiPhysicalVal;
                    break;
                case UART_I_VGG:
                    iDigData.stBytes.ucLowByte = arReceivedData[4];
                    iDigData.stBytes.ucHighByte = arReceivedData[3];
                    //uiPhysicalVal = (unsigned int)round((((double)(iDigData.iWord)-(OFFSET_I_VGG)) / (FACTOR_I_VGG)));
                    //strData.sprintf("%.0f", dTecVoltage*1000);
                    //strData.sprintf("%d", uiPhysicalVal);
                    //ui->lbl_Ivgg->setText(strData);
                    dIgg=((((double)(iDigData.iWord+OFFSET_I_VGG)) / FACTOR_I_VGG));
                    strData.sprintf("%.1f", dIgg);
                    if(ui->checkBoxVggEnabled->checkState())  ui->lbl_Ivgg->setText(strData); else ui->lbl_Ivgg->setText("0");
                    break;
                case UART_VGG_SPARK:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    uiPhysicalVal =  (unsigned int)(round(((double)(uiDigData.uiWord)) / (FACTOR_VGG)));
                    ucValue = (unsigned char) uiPhysicalVal;
                    ui->editVggSpark->setText(QString::number(ucValue));

                    dVal = ((double)(ucValue)) * FACTOR_VGG;
                    dVgg = VGG_C + (VGG_A * (1-exp(-dVal / VGG_X0))) + VGG_L*dVal;
                    strData = QString::number(dVgg,'f',2);
                    ui->lblVggSpark->setText(strData);
                    break;

                //MW // Mikrowellenparameter------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                case UART_MW_PWR_TO:
                    iDigData.stBytes.ucLowByte = arReceivedData[4];
                    iDigData.stBytes.ucHighByte = arReceivedData[3];
                    dVal=(double)(iDigData.iWord);
                    uiPwrTo = (unsigned int)(round(FACTOR_P_TO*dVal*dVal));
                    //uiPwrTo = (unsigned int)(round(((double)(iDigData.iWord)-(OFFSET_P_TO)) / (FACTOR_P_TO)));
                    strData.sprintf("%ud", uiPwrTo);
                    ui->lbl_PwrTo->setText(strData);
                    break;
                case UART_MW_PWR_BACK:
                    iDigData.stBytes.ucLowByte = arReceivedData[4];
                    iDigData.stBytes.ucHighByte = arReceivedData[3];
                    dVal=(double)(iDigData.iWord);
                    uiPwrBack = (unsigned int)(round(FACTOR_P_BACK*dVal*dVal));
                    //uiPwrBack = (unsigned int)(round(((double)(iDigData.iWord)-(OFFSET_P_BACK)) / (FACTOR_P_BACK)));
                    //strData.sprintf("%.0f", dTecVoltage*1000);
                    strData.sprintf("%d", uiPwrBack);
                    ui->lbl_PwrBack->setText(strData);
                    break;

                //Steuerspannung für Kapazitäten zur Steuerung des Arbeitspunktes----------------------------------------------------------------------------------------------------------------------------------
                case UART_CTRL1:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    iCtrl1 = (int)(round(((double)(uiDigData.uiWord)-OFFSET_CTRL) / (FACTOR_CTRL)));
                    NewValueReceived(iCtrl1, spnCtrl1ChangedByUart, ui->spnCtrl1);
                    break;
                case UART_CTRL2:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    iCtrl2 = (int)(round(((double)(uiDigData.uiWord)-OFFSET_CTRL) / (FACTOR_CTRL)));
                    NewValueReceived(iCtrl2, spnCtrl2ChangedByUart, ui->spnCtrl2);
                    break;

                case UART_VCTRL1_SPARK:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    iCtrl1Spark = (int)(round(((double)(uiDigData.uiWord)-OFFSET_CTRL) / (FACTOR_CTRL)));
                    ui->editVctrl1Spark->setText(QString::number(iCtrl1Spark));
                    break;

                case UART_VCTRL2_SPARK:
                    uiDigData.stBytes.ucLowByte = arReceivedData[4];
                    uiDigData.stBytes.ucHighByte = arReceivedData[3];
                    iCtrl2Spark = (int)(round(((double)(uiDigData.uiWord)-OFFSET_CTRL) / (FACTOR_CTRL)));
                    ui->editVctrl2Spark->setText(QString::number(iCtrl2Spark));

#ifdef DEBUGMODUS
//    qDebug() << "uiDigData.uiWord=" << uiDigData.uiWord << "; iCtrl2Spark=" << iCtrl2Spark ;
#endif

                    break;

                default:
                    break;
            }
        }
        else //PC hat geschrieben
        {
            QByteArray *Data = new QByteArray;
            switch (cCmd)
            {
                //Fehlermeldung
                case UART_ERROR:
                    switch (arReceivedData[3])
                    {
                        case ERROR_VGG_NOT_RIGHT:
                            arReceivedData.clear();
                            QMessageBox::critical(this, tr("VGG>-6V!"), tr("Set value of VGG below -6V before enable VDD!"),  QMessageBox::Ok);
                            if (bConnected) WriteToRdd(0xAA, 4, UART_EN_VDD, Data);//ReadAllDataCmd();
                        break;
                    }
                default:
                    break;
            }
        }
        uiRecLen = 0;
    }
    else
    {
#ifdef DEBUGMODUS
//    qDebug() << "Länge nicht erreicht;  arReceivedData.length()=" << arReceivedData.length() << "; uiRecLen=" <<uiRecLen << "; ucComTask" << ucComTask;
#endif
    }
}

void Widget::tmrslotWatchdog()
{
    tmrWatchdog->stop();
    bComBusy = 0;
    if(bConnected)
    {
        ui->lblReceived->setText("No answer from Raman Dual wavelength Driver");
        QMessageBox::critical(this, tr("No answer from Plasma Device!"), tr("Connect Raman Dual wavelength Driver to PC!"),  QMessageBox::Ok);
        bConnected = false;
        closeSerialPort();
    }
    else
    {
        ui->lblReceived->setText("No connection!!!");
    }
}

void Widget::on_btnConnect_clicked()
{
    bConnected = !bConnected;
    if(bConnected)
    {
        openSerialPort();
        if(bConnected)
        {
            ui->btnConnect->setText("Close ComPort");
        }
    }
    else
    {
        closeSerialPort();
        ui->btnConnect->setText("Connect to ComPort");
    }
}

Widget::~Widget()
{
    delete settings_ComPort;
    delete ui;
}

void Widget::on_lblReceived_textChanged(const QString &arg1)
{
    if(arg1.contains("fbhonly"))
    {
        ui->tabWidget->tabBar()->show();
    }
}

void Widget::ChangeValue(unsigned int uiDigitalValue, int NewValue, int &OldValue, bool &FlagChangedByUart, QSpinBox *SpinBox, bool &FlagChangeRequired, char cUartCmd, unsigned char ucUartLen)
{

    if(FlagChangedByUart)
    {
        FlagChangedByUart = 0;
        OldValue = NewValue;
    }
    else
    {
        if(abs(OldValue-NewValue) > (3*(SpinBox->singleStep())))
        {
#ifdef DEBUGMODUS
            qDebug() << "Änderung zu groß: singleStep= " << SpinBox->singleStep() << "; abs(OldValue-NewValue)= " << abs(OldValue-NewValue);
#endif
            FlagChangeRequired = 1;
        }
        else
        {
#ifdef DEBUGMODUS
//            qDebug() << "Ändere Value: " << NewValue << "; DigitalValue: " << uiDigitalValue;
#endif
            OldValue = NewValue;
            if (bConnected)
            {
                ParameterChanged = 1;
            }
            typUnsignedWord DataToSend;
            DataToSend.uiWord = uiDigitalValue;
            QByteArray *Data = new QByteArray;
            Data->append((char)DataToSend.stBytes.ucHighByte);
            Data->append((char)DataToSend.stBytes.ucLowByte);
            WriteToRdd(0x2A, ucUartLen, cUartCmd, Data);
        }
    }
}

void Widget::on_spnVdd_editingFinished()
{
#ifdef DEBUGMODUS
            qDebug() << "spnVdd_editingFinished";
#endif
//    if (VddChangeRequired)
    {
        bParameterChangeRunning = 1;
        iVdd = ui->spnVdd->value();
        on_spnVdd_valueChanged(iVdd);
        VddChangeRequired=0;
    }
}

void Widget::on_spnVdd_valueChanged(int arg1)
{
    typUnsignedWord uiDigVal ;
    bParameterChangeRunning = 1;
    uiDigVal.uiWord = (unsigned int)(FACTOR_VDD  * arg1);

    ChangeValue(uiDigVal.uiWord, arg1, iVdd, spnVddChangedByUart, ui->spnVdd, VddChangeRequired, UART_VDD, 6);
    //unsigned int uiVdd = round(Vdd3*(double)(arg1*arg1*arg1)+Vdd2*(double)(arg1*arg1)+Vdd1*(double)(arg1)+Vdd0);

    double dVal = ((double)(arg1)) * FACTOR_VDD;
    unsigned int uiVdd = round(VDD_C + (VDD_A * (1-exp(-dVal / VDD_X0))) + VDD_L*dVal);

    QString strVdd = QString::number(uiVdd);
    ui->lbl_Vdd->setText(strVdd);
    bParameterChangeRunning = 0;
}

void Widget::on_sliderVdd_valueChanged(int value)
{
    if(!(value==ui->spnVdd->value() ))
    {
        bParameterChangeRunning = 1;
        ui->spnVdd->setValue(value);
    }
}

void Widget::on_editVddSpark_returnPressed()
{
    QString strText= ui->editVddSpark->text();
    unsigned char ucValue = (unsigned char) (strText.toInt());
    typUnsignedWord uiDigVal ;
    uiDigVal.uiWord = (unsigned int)(FACTOR_VDD  * ucValue);

    QByteArray *Data = new QByteArray;
    Data->append((char)uiDigVal.stBytes.ucHighByte);
    Data->append((char)uiDigVal.stBytes.ucLowByte);
    WriteToRdd(0x2A, 6, UART_VDD_SPARK, Data);

    //unsigned int uiVdd = round(Vdd3*(double)(ucValue*ucValue*ucValue)+Vdd2*(double)(ucValue*ucValue)+Vdd1*(double)(ucValue)+Vdd0);

    double dVal = ((double)(ucValue)) * FACTOR_VDD;
    unsigned int uiVdd = round(VDD_C + (VDD_A * (1-exp(-dVal / VDD_X0))) + VDD_L*dVal);
    QString strVdd = QString::number(uiVdd);
    ui->lblVddSpark->setText(strVdd);
}

void Widget::on_editSparkDuration_returnPressed()
{
    QString strText= ui->editSparkDuration->text();
    typUnsignedWord stValue;
    stValue.uiWord= (unsigned int) (strText.toInt());
    //typUnsignedWord uiDigVal ;
    //uiDigVal.uiWord = (unsigned int)(FACTOR_VDD  * ucValue);

    QByteArray *Data = new QByteArray;
    Data->append((char)stValue.stBytes.ucHighByte);
    Data->append((char)stValue.stBytes.ucLowByte);
    WriteToRdd(0x2A, 6, UART_SPARK_DURATION, Data);
}

void Widget::on_editVggSpark_returnPressed()
{
    QString strText= ui->editVggSpark->text();
    unsigned char ucValue = (unsigned char) (strText.toInt());
    typUnsignedWord uiDigVal ;
    uiDigVal.uiWord = (unsigned int)(FACTOR_VGG  * ucValue);
#ifdef DEBUGMODUS
//    qDebug() << "uiDigVal.uiWord = " << uiDigVal.uiWord << "; ucValue = " << ucValue;
#endif

    QByteArray *Data = new QByteArray;
    Data->append((char)uiDigVal.stBytes.ucHighByte);
    Data->append((char)uiDigVal.stBytes.ucLowByte);
    WriteToRdd(0x2A, 6, UART_VGG_SPARK, Data);

    double dVal = ((double)(ucValue)) * FACTOR_VGG;
    double dVgg = VGG_C + (VGG_A * (1-exp(-dVal / VGG_X0))) + VGG_L*dVal;

    QString strVgg = QString::number(dVgg,'f',1);
    ui->lblVggSpark->setText(strVgg);
}


void Widget::on_spnCtrl1_valueChanged(int arg1)
{
    bParameterChangeRunning = 1;
    unsigned int uiDigVal = (unsigned int)(round((double)(FACTOR_CTRL * ((double)(arg1)) + OFFSET_CTRL)));

#ifdef DEBUGMODUS
    //qDebug() << "arg1 = " << arg1 << "; iCtrl1 = " << iCtrl1 << "; uiCtrl1 = " << uiCtrl1;
#endif
    ChangeValue(uiDigVal, arg1, iCtrl1, spnCtrl1ChangedByUart, ui->spnCtrl1, Ctrl1ChangeRequired, UART_CTRL1, 6);
    bParameterChangeRunning = 0;
}

void Widget::on_spnCtrl1_editingFinished()
{
    bParameterChangeRunning = 1;
    iCtrl1 = ui->spnCtrl1->value();
    on_spnCtrl1_valueChanged(iCtrl1);
    Ctrl1ChangeRequired=0;
}

void Widget::on_spnCtrl2_valueChanged(int arg1)
{
    bParameterChangeRunning = 1;
    unsigned int uiDigVal = (unsigned int)(round((double)(FACTOR_CTRL * ((double)(arg1)) + OFFSET_CTRL)));
    //int iCtrl2 = (int)uiCtrl2;
    ChangeValue(uiDigVal, arg1, iCtrl2, spnCtrl2ChangedByUart, ui->spnCtrl2, Ctrl2ChangeRequired, UART_CTRL2, 6);
    bParameterChangeRunning = 0;
}

void Widget::on_spnCtrl2_editingFinished()
{
    bParameterChangeRunning = 1;
    iCtrl2 = ui->spnCtrl2->value();
    on_spnCtrl2_valueChanged(iCtrl2);
    Ctrl2ChangeRequired=0;
}

void Widget::on_spnVgg_editingFinished()
{
//    if (VggChangeRequired)
    {
        bParameterChangeRunning = 1;
#ifdef DEBUGMODUS
    qDebug() << "1: bParameterChangeRunning = " << bParameterChangeRunning;
#endif
        iVgg = ui->spnVgg->value();
        on_spnVgg_valueChanged(iVgg);
        VggChangeRequired=0;
    }
}

void Widget::on_spnVgg_valueChanged(int arg1)
{
    bParameterChangeRunning = 1;
#ifdef DEBUGMODUS
    qDebug() << "2: bParameterChangeRunning = " << bParameterChangeRunning;
#endif
    unsigned int uiDigVal = FACTOR_VGG * arg1;

    ChangeValue(uiDigVal, arg1, iVgg, spnVggChangedByUart, ui->spnVgg, VggChangeRequired, UART_VGG, 6);

    double dVal = ((double)(arg1)) * FACTOR_VGG;
    double dVgg = VGG_C + (VGG_A * (1-exp(-dVal / VGG_X0))) + VGG_L*dVal;

#ifdef DEBUGMODUS
    qDebug() << "uiVgg = " << dVgg << "; arg1 = " << arg1;
#endif

    QString strVgg = QString::number(dVgg,'f',2);
    ui->lbl_Vgg->setText(strVgg);
    bParameterChangeRunning = 0;
#ifdef DEBUGMODUS
    qDebug() << "3: bParameterChangeRunning = " << bParameterChangeRunning;
#endif
}

void Widget::on_sliderVgg_valueChanged(int value)
{
    if(!(value==ui->spnVgg->value() ))
    {
        bParameterChangeRunning = 1;
#ifdef DEBUGMODUS
    qDebug() << "on_sliderVgg_valueChanged: bParameterChangeRunning = " << bParameterChangeRunning << "; value=" << value << "; ui->spnVgg->value= " << ui->spnVgg->value();
#endif
        ui->spnVgg->setValue(value);
    }
}

void Widget::on_checkBoxVddEnabled_clicked(bool checked)
{
    bParameterChangeRunning = 1;
    QByteArray *Data = new QByteArray;
    if (checked)  Data->append((char)1); else Data->append((char)0);
    WriteToRdd(0x2A, 5, UART_EN_VDD, Data);
    bParameterChangeRunning = 0;
}

void Widget::on_checkBoxVggEnabled_clicked(bool checked)
{
    bParameterChangeRunning = 1;
    QByteArray *Data = new QByteArray;
    if (checked) Data->append((char)1);  else   Data->append((char)0);
    WriteToRdd(0x2A, 5, UART_EN_VGG, Data);
    bParameterChangeRunning = 0;
}

void Widget::on_btnStartPlasma_clicked(bool checked)
{
    QByteArray *Data = new QByteArray;
    if (checked)  Data->append((char)1); else Data->append((char)0);
    WriteToRdd(0x2A, 5, UART_EN_PLASMA, Data);
    if(checked) ui->btnStartPlasma->setText("Douse Plasma"); else ui->btnStartPlasma->setText("Spark Plasma");
}

void Widget::on_spnSetup_valueChanged(int arg1)
{
    if (bConnected)
    {
        bParameterChangeRunning = 1;
        ParameterChanged = 1;
        if(spnSetupChangedByUart)
        {
            spnSetupChangedByUart = 0;
        }
        else
        {
            QByteArray *Data = new QByteArray;
            Data->append((char)arg1);
            WriteToRdd(0x2A, 5, UART_SETUP_NO, Data);
            Delay(200);
            ReadAllDataCmd();
        }
        bParameterChangeRunning = 0;
    }
    else
    {
        QMessageBox::critical(this, tr("No connection!"), tr("Press Button 'Connect to ComPort'"),  QMessageBox::Ok);
    }
}

void Widget::on_btnSaveSetup_clicked()
{
    bParameterChangeRunning = 1;
    QByteArray *Data = new QByteArray;
    WriteToRdd(0x2A, 4, UART_SAVE_SETUP, Data);
    bParameterChangeRunning = 0;
}

void Widget::tmrslotDelay()
{
    tmrDelay->stop();
    bDelayRunning = 0;
}

void Widget::Delay(int mSec)
{
    tmrDelay->setInterval(mSec);
    tmrDelay->start();
    bDelayRunning = 1;
    while (bDelayRunning) QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Widget::tmrslotMeasure()
{
static     double dAvgTemperature = 0;
#define FILTERDEPTH (double) 0//0.9 //0..1 -> je größer, desto tiefer und langsamer das Filter (gleitender Mittelwert)

#ifdef DEBUGMODUS
//    qDebug() << "-----tmrslotMeasure";
#endif

bool bLoesch = 1;
    while (bParameterChangeRunning)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

#ifdef DEBUGMODUS
        if(bLoesch)
        {
            bLoesch = 0;
            qDebug() << "-----bParameterChangeRunning= "<< bParameterChangeRunning <<"; bConnected=" << bConnected;
        }
#endif
    }

    if(bConnected)
    {
        QByteArray *Data = new QByteArray;
        WriteToRdd(0xAA, 4, UART_READ_ALL, Data);

        if (0==dAvgTemperature)
        {
            dAvgTemperature=dTemperature;
        }
        else
        {
            dAvgTemperature = (dAvgTemperature*FILTERDEPTH)+ (dTemperature*(1-FILTERDEPTH));
            QString strData;
            strData.sprintf("%.0f", round(dAvgTemperature));
            ui->lblTemperature->setText(strData);
        }

        if (!(0==dAvgTemperature))
        {
            if(dAvgTemperature<iMinTemperature)
            {
                iMinTemperature = (int)(round(dAvgTemperature)-10);
            }
            if(dAvgTemperature>iMaxTemperature)
            {
                iMaxTemperature = (int)(round(dAvgTemperature)+10);
            }
            //ui->Funktionsgraph->yAxis->setRange(iMinTemperature,iMaxTemperature);
            ui->TemperatureGraph->yAxis->setRange(iMinTemperature,iMaxTemperature);
        }

        if(((int)(round(dIgg)))<(iMinIgg+1)) iMinIgg = (int)(round(dIgg)-1);
        if(((int)(round(dIgg)))>(iMaxIgg-1)) iMaxIgg = (int)(round(dIgg)+1);
        if(((int)(round(dIdd)))<(iMinIdd+1)) iMinIdd = (int)(round(dIdd)-1);
        if(((int)(round(dIdd)))>(iMaxIdd-1)) iMaxIdd = (int)(round(dIdd)+1);
        ui->ElectricalGraph->yAxis->setRange(iMinIgg,iMaxIgg);
        ui->ElectricalGraph->yAxis2->setRange(iMinIdd,iMaxIdd);

        if(uiPwrTo>(iMaxPwrTo-1)) iMaxPwrTo = uiPwrTo+1;
        ui->PwrGraph->yAxis->setRange(iMinPwrTo,iMaxPwrTo);
        if(iMaxPwrTo>15) ui->PwrGraph->yAxis->setTickStep(10); else ui->PwrGraph->yAxis->setTickStep(1);
#ifdef WITH_PWR_BACK
        if(uiPwrBack>(iMaxPwrBack-1)) iMaxPwrBack = uiPwrBack+1;
        ui->PwrGraph->yAxis2->setRange(iMinPwrBack,iMaxPwrBack);
        if(iMaxPwrBack>15) ui->PwrGraph->yAxis2->setTickStep(10); else ui->PwrGraph->yAxis2->setTickStep(1);
#endif

#ifdef DEBUGMODUS
    qDebug() << "iMinPwrTo = " << iMinPwrTo << "; iMaxPwrTo = " << iMaxPwrTo << "; uiPwrTo = " << uiPwrTo ;//<< "; iMinIdd = " << iMinIdd << "; iMaxIgg = " << iMaxIgg << "; iMaxIdd = " << iMaxIdd;
#endif

        if (!(0==dAvgTemperature))
        {
            ui->TemperatureGraph->graph(0)->addData(dTime,((dAvgTemperature)));
        }
        ui->ElectricalGraph->graph(0)->addData(dTime,dIgg);
        ui->ElectricalGraph->graph(1)->addData(dTime,dIdd);

        ui->PwrGraph->graph(0)->addData(dTime,uiPwrTo);
        ui->PwrGraph->graph(1)->addData(dTime,uiPwrBack);

        ui->TemperatureGraph->replot();
        ui->ElectricalGraph->replot();
        ui->PwrGraph->replot();
        dTime = dTime + ((double)(GRAPH_TIME_INTERVALL)/1000);
        if(dTime > (ui->spnMaxX->value()))  on_btnResetGraf_clicked();
    }
    else
    {
        ui->btnStartMeasure->setText("Start Measure");
        ui->btnStartMeasure->setChecked(false);
        tmrMeasure->stop();
    }
}

void Widget::on_btnStartMeasure_clicked(bool checked)
{
    bParameterChangeRunning = 0;
    if (checked)
    {
        if(bConnected)
        {
            ui->btnStartMeasure->setText("Stop Measure");
            tmrMeasure->start();
        }
        else
        {
            QMessageBox::critical(this, tr("No connection!"), tr("Press Button 'Connect to ComPort'"),  QMessageBox::Ok);
            ui->btnStartMeasure->setChecked(false);
        }
    }
    else
    {
        ui->btnStartMeasure->setText("Start Measure");
        tmrMeasure->stop();
    }
}

void Widget::SetupPlot()
{
    QColor colorGraf1 = QColor(255, 0, 0, 255);
    QColor colorGraf2 = QColor(COLOR_GATE, 255);
    QColor colorGraf3 = QColor(COLOR_DRAIN, 255);
    QColor colorGraf4 = QColor(COLOR_PWR_TO, 255);
    QColor colorGraf5 = QColor(COLOR_PWR_FROM, 255);

    ui->TemperatureGraph->xAxis->setAutoTickStep(0);
    ui->TemperatureGraph->xAxis->setTickStep(ui->spnTickStepX->value());
    ui->TemperatureGraph->xAxis->setNumberFormat("f");
    ui->TemperatureGraph->xAxis->setNumberPrecision(0);
    ui->TemperatureGraph->yAxis->setAutoTickStep(0);
    ui->TemperatureGraph->yAxis->setTickStep(5);
    ui->TemperatureGraph->yAxis->setNumberFormat("f");
    ui->TemperatureGraph->yAxis->setNumberPrecision(0);

    ui->ElectricalGraph->xAxis->setAutoTickStep(0);
    ui->ElectricalGraph->xAxis->setTickStep(ui->spnTickStepX->value());
    ui->ElectricalGraph->xAxis->setNumberFormat("f");
    ui->ElectricalGraph->xAxis->setNumberPrecision(0);
    ui->ElectricalGraph->yAxis->setAutoTickStep(0);
    ui->ElectricalGraph->yAxis->setTickStep(1);
    ui->ElectricalGraph->yAxis->setNumberFormat("f");
    ui->ElectricalGraph->yAxis->setNumberPrecision(1);
    ui->ElectricalGraph->yAxis2->setAutoTickStep(0);
    ui->ElectricalGraph->yAxis2->setTickStep(1);
    ui->ElectricalGraph->yAxis2->setNumberFormat("f");
    ui->ElectricalGraph->yAxis2->setNumberPrecision(1);

    ui->PwrGraph->xAxis->setAutoTickStep(0);
    ui->PwrGraph->xAxis->setTickStep(ui->spnTickStepX->value());
    ui->PwrGraph->xAxis->setNumberFormat("f");
    ui->PwrGraph->xAxis->setNumberPrecision(0);
    ui->PwrGraph->yAxis->setAutoTickStep(0);
    ui->PwrGraph->yAxis->setTickStep(1);
    ui->PwrGraph->yAxis->setNumberFormat("f");
    ui->PwrGraph->yAxis->setNumberPrecision(1);
    ui->PwrGraph->yAxis2->setAutoTickStep(0);
    ui->PwrGraph->yAxis2->setTickStep(1);
    ui->PwrGraph->yAxis2->setNumberFormat("f");
    ui->PwrGraph->yAxis2->setNumberPrecision(1);

    QPen Pen;
    QFont Axisfont;
    //Axisfont = ui->Funktionsgraph->xAxis->labelFont();
    Axisfont = ui->TemperatureGraph->xAxis->labelFont();
    Axisfont.setPointSize(10);
    Axisfont.setBold(0);

    Pen.setColor((QColor(255,255,255)));
    Pen.setWidth(1);

    ui->TemperatureGraph->setBackground(QBrush(QColor(0,0,0,255)));
    ui->TemperatureGraph->xAxis->setLabelColor(QColor(255,255,255));
    ui->TemperatureGraph->xAxis->setTickLabelColor(QColor(255,255,255));
    ui->TemperatureGraph->xAxis->setBasePen(Pen);
    ui->TemperatureGraph->xAxis->setTickPen(Pen);
    ui->TemperatureGraph->xAxis->setSubTickPen(Pen);
    ui->TemperatureGraph->yAxis->setLabelColor(colorGraf1);
    ui->TemperatureGraph->yAxis->setTickLabelColor(colorGraf1);
    ui->TemperatureGraph->yAxis->setBasePen(Pen);
    ui->TemperatureGraph->yAxis->setTickPen(Pen);
    ui->TemperatureGraph->yAxis->setSubTickPen(Pen);

    ui->TemperatureGraph->xAxis->setLabelFont(Axisfont);
    ui->TemperatureGraph->yAxis->setLabelFont(Axisfont);

    ui->ElectricalGraph->setBackground(QBrush(QColor(0,0,0,255)));
    ui->ElectricalGraph->xAxis->setLabelColor(QColor(255,255,255));
    ui->ElectricalGraph->xAxis->setTickLabelColor(QColor(255,255,255));
    ui->ElectricalGraph->xAxis->setBasePen(Pen);
    ui->ElectricalGraph->xAxis->setTickPen(Pen);
    ui->ElectricalGraph->xAxis->setSubTickPen(Pen);
    ui->ElectricalGraph->yAxis->setLabelColor(colorGraf2);
    ui->ElectricalGraph->yAxis->setTickLabelColor(colorGraf2);
    ui->ElectricalGraph->yAxis->setBasePen(Pen);
    ui->ElectricalGraph->yAxis->setTickPen(Pen);
    ui->ElectricalGraph->yAxis->setSubTickPen(Pen);
    ui->ElectricalGraph->yAxis2->setLabelColor(colorGraf3);
    ui->ElectricalGraph->yAxis2->setTickLabelColor(colorGraf3);
    ui->ElectricalGraph->yAxis2->setBasePen(Pen);
    ui->ElectricalGraph->yAxis2->setTickPen(Pen);
    ui->ElectricalGraph->yAxis2->setSubTickPen(Pen);

    ui->ElectricalGraph->xAxis->setLabelFont(Axisfont);
    ui->ElectricalGraph->yAxis->setLabelFont(Axisfont);
    ui->ElectricalGraph->yAxis2->setLabelFont(Axisfont);

    ui->PwrGraph->setBackground(QBrush(QColor(0,0,0,255)));
    ui->PwrGraph->xAxis->setLabelColor(QColor(255,255,255));
    ui->PwrGraph->xAxis->setTickLabelColor(QColor(255,255,255));
    ui->PwrGraph->xAxis->setBasePen(Pen);
    ui->PwrGraph->xAxis->setTickPen(Pen);
    ui->PwrGraph->xAxis->setSubTickPen(Pen);
    ui->PwrGraph->yAxis->setLabelColor(colorGraf2);
    ui->PwrGraph->yAxis->setTickLabelColor(colorGraf2);
    ui->PwrGraph->yAxis->setBasePen(Pen);
    ui->PwrGraph->yAxis->setTickPen(Pen);
    ui->PwrGraph->yAxis->setSubTickPen(Pen);
    ui->PwrGraph->yAxis2->setLabelColor(colorGraf3);
    ui->PwrGraph->yAxis2->setTickLabelColor(colorGraf3);
    ui->PwrGraph->yAxis2->setBasePen(Pen);
    ui->PwrGraph->yAxis2->setTickPen(Pen);
    ui->PwrGraph->yAxis2->setSubTickPen(Pen);

    ui->PwrGraph->xAxis->setLabelFont(Axisfont);
    ui->PwrGraph->yAxis->setLabelFont(Axisfont);
    ui->PwrGraph->yAxis2->setLabelFont(Axisfont);

    Axisfont.setBold(0);
    ui->TemperatureGraph->xAxis->setTickLabelFont(Axisfont);
    ui->TemperatureGraph->yAxis->setTickLabelFont(Axisfont);
    ui->TemperatureGraph->xAxis->setLabel("t / s");
    ui->TemperatureGraph->yAxis->setLabel("T / °C");
    ui->TemperatureGraph->xAxis->setRange(0,ui->spnMaxX->value());
    ui->TemperatureGraph->yAxis->setRange(20,30);

    ui->ElectricalGraph->xAxis->setTickLabelFont(Axisfont);
    ui->ElectricalGraph->yAxis->setTickLabelFont(Axisfont);
    ui->ElectricalGraph->yAxis2->setTickLabelFont(Axisfont);
    ui->ElectricalGraph->xAxis->setLabel("t / s");
    ui->ElectricalGraph->yAxis->setLabel("Igate / mA");
    ui->ElectricalGraph->yAxis2->setLabel("Idrain / A");
    ui->ElectricalGraph->xAxis->setRange(0,ui->spnMaxX->value());
    ui->ElectricalGraph->yAxis->setRange(-1,1);
    ui->ElectricalGraph->yAxis2->setRange(0,2);

    ui->PwrGraph->xAxis->setTickLabelFont(Axisfont);
    ui->PwrGraph->yAxis->setTickLabelFont(Axisfont);
    ui->PwrGraph->yAxis2->setTickLabelFont(Axisfont);
    ui->PwrGraph->xAxis->setLabel("t / s");
    ui->PwrGraph->yAxis->setLabel("Pwr to / W");
    ui->PwrGraph->yAxis2->setLabel("Pwr back / W");
    ui->PwrGraph->xAxis->setRange(0,ui->spnMaxX->value());
    ui->PwrGraph->yAxis->setRange(-1,1);
    ui->PwrGraph->yAxis2->setRange(0,2);


    Pen.setWidth(2);

    ui->TemperatureGraph->addGraph(ui->TemperatureGraph->xAxis, ui->TemperatureGraph->yAxis);
    Pen.setColor(QColor(colorGraf1));
    ui->TemperatureGraph->graph(0)->setPen(Pen);
    ui->TemperatureGraph->graph(0)->setName("Temperatur / °C");
    ui->TemperatureGraph->graph(0)->addToLegend();

    ui->ElectricalGraph->addGraph(ui->ElectricalGraph->xAxis, ui->ElectricalGraph->yAxis);
    Pen.setColor(QColor(colorGraf2));
    ui->ElectricalGraph->graph(0)->setPen(Pen);
    ui->ElectricalGraph->graph(0)->setName("Igate");
    ui->ElectricalGraph->graph(0)->addToLegend();

    ui->ElectricalGraph->addGraph(ui->ElectricalGraph->xAxis, ui->ElectricalGraph->yAxis2);
    Pen.setColor(QColor(colorGraf3));
    ui->ElectricalGraph->graph(1)->setPen(Pen);
    ui->ElectricalGraph->graph(1)->setName("Idrain");
    ui->ElectricalGraph->graph(1)->addToLegend();

    ui->PwrGraph->addGraph(ui->PwrGraph->xAxis, ui->PwrGraph->yAxis);
    Pen.setColor(QColor(colorGraf4));
    ui->PwrGraph->graph(0)->setPen(Pen);
    ui->PwrGraph->graph(0)->setName("Pwr to");
    ui->PwrGraph->graph(0)->addToLegend();

    ui->PwrGraph->addGraph(ui->PwrGraph->xAxis, ui->PwrGraph->yAxis2);
    Pen.setColor(QColor(colorGraf5));
    ui->PwrGraph->graph(1)->setPen(Pen);
    ui->PwrGraph->graph(1)->setName("Pwr Back");
    ui->PwrGraph->graph(1)->addToLegend();


    Axisfont.setBold(0);
    ui->TemperatureGraph->legend->setFont(Axisfont);
    ui->TemperatureGraph->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);//Legende nach rechts unten setzen
    ui->TemperatureGraph->xAxis->setVisible(true);
    ui->TemperatureGraph->yAxis->setVisible(true);
    ui->TemperatureGraph->replot();

    ui->ElectricalGraph->legend->setFont(Axisfont);
    ui->ElectricalGraph->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);//Legende nach rechts unten setzen
    ui->ElectricalGraph->xAxis->setVisible(true);
    ui->ElectricalGraph->yAxis->setVisible(true);
    ui->ElectricalGraph->yAxis2->setVisible(true);
    ui->ElectricalGraph->replot();

    ui->PwrGraph->legend->setFont(Axisfont);
    ui->PwrGraph->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);//Legende nach rechts unten setzen
    ui->PwrGraph->xAxis->setVisible(true);
    ui->PwrGraph->yAxis->setVisible(true);
    ui->PwrGraph->yAxis2->setVisible(true);
    ui->PwrGraph->replot();


}

void Widget::on_btnResetGraf_clicked()
{

    dTime = 0;
    QVector<double> xMeas(1),yTemperature(1), yPwrTo(1), yPwrBack(1), yIdd(1), yIgg(1);
    xMeas[0]=dTime;
    yTemperature[0]=dTemperature;
    yPwrTo[0] = uiPwrTo;
    yPwrBack[0] = uiPwrBack;
    yIgg[0] = dIgg;//uiPwrTo;
    yIdd[0] = dIdd;//uiPwrBack;
//    ui->Funktionsgraph->graph(0)->setData(xMeas,yTemperature);
//    ui->Funktionsgraph->graph(1)->setData(xMeas,yPwrTo);
//    ui->Funktionsgraph->graph(2)->setData(xMeas,yPwrBack);
    ui->TemperatureGraph->graph(0)->setData(xMeas,yTemperature);
    ui->ElectricalGraph->graph(0)->setData(xMeas,yIgg);
    ui->ElectricalGraph->graph(1)->setData(xMeas,yIdd);
    ui->PwrGraph->graph(0)->setData(xMeas,yPwrTo);
#ifdef WITH_PWR_BACK
    ui->PwrGraph->graph(1)->setData(xMeas,yPwrBack);
#endif

    iMinTemperature = (int)(round(dTemperature)-10);
    iMaxTemperature = iMinTemperature+20;

    iMinIgg = (int)round(dIgg)-1;
    iMaxIgg = (int)round(dIgg)+1;
    iMinIdd = 0;
    iMaxIdd = (int)round(dIdd)+1;

    iMinPwrTo = 0;
    iMaxPwrTo = uiPwrTo+1;
    iMinPwrBack = 0;
    iMaxPwrBack = uiPwrBack+1;

    ui->TemperatureGraph->yAxis->setRange(iMinTemperature,iMaxTemperature);
    ui->ElectricalGraph->yAxis->setRange(iMinIgg,iMaxIgg);
    ui->ElectricalGraph->yAxis->setTickStep((iMaxIgg - iMinIgg)/10);
    ui->ElectricalGraph->yAxis2->setRange(iMinIdd,iMaxIdd);
    ui->ElectricalGraph->yAxis2->setTickStep((iMaxIdd-iMinIdd) / 10);
    ui->PwrGraph->yAxis->setRange(iMinPwrTo,iMaxPwrTo);


//    ui->PwrGraph->yAxis->setTickStep((iMaxPwrTo-iMinPwrTo)/10);
#ifdef WITH_PWR_BACK
    ui->PwrGraph->yAxis2->setRange(iMinPwrBack,iMaxPwrBack);
//    ui->PwrGraph->yAxis2->setTickStep((iMaxPwrBack-iMinPwrBack)/10);
#endif
    ui->TemperatureGraph->replot();
    ui->ElectricalGraph->replot();
    ui->PwrGraph->replot();
}

void Widget::WriteToStorage(unsigned char ucAddress, unsigned char ucDate)
{
    if (bConnected)
    {
        if(StorageChangedByUart)
        {
            StorageChangedByUart = 0;
        }
        else
        {
            QByteArray *Data = new QByteArray;
            Data->append((char)ucAddress);
            Data->append((char)ucDate);
            WriteToRdd(0x2A, 6, UART_STORAGE, Data);
#ifdef DEBUGMODUS
//qDebug() << "UART_STORAGE changed. Adresse= " << ucAddress << "; Wert= " << ucDate;
#endif
        }
    }
    else
    {
        QMessageBox::critical(this, tr("No connection!"), tr("Press Button 'Connect to ComPort'"),  QMessageBox::Ok);
    }
}

void Widget::on_editVddmax_returnPressed()
{
    QString strText= ui->editVddmax->text();
    unsigned char ucValue = (unsigned char) strText.toInt();
    WriteToStorage(VDD_MAX, ucValue);
//    ui->spnVdd->setMaximum(ucValue);

    double dVal = ((double)(ucValue)) * FACTOR_VDD;
    unsigned int uiVdd = round(VDD_C + (VDD_A * (1-exp(-dVal / VDD_X0))) + VDD_L*dVal);
    QString strVdd = QString::number(uiVdd);
    ui->lblVddMax->setText(strVdd);

}

void Widget::on_editVddmin_returnPressed()
{
    QString strText= ui->editVddmin->text();
    unsigned char ucValue = (unsigned char) strText.toInt();
    WriteToStorage(VDD_MIN, ucValue);
//    ui->spnVdd->setMinimum(ucValue);

    double dVal = ((double)(ucValue)) * FACTOR_VDD;
    unsigned int uiVdd = round(VDD_C + (VDD_A * (1-exp(-dVal / VDD_X0))) + VDD_L*dVal);
    QString strVdd = QString::number(uiVdd);
    ui->lblVddMin->setText(strVdd);
}

void Widget::on_editVggmin_returnPressed()
{
    QString strText= ui->editVggmin->text();
    unsigned char ucValue = (unsigned char) (strText.toInt());
    WriteToStorage(VGG_MIN, ucValue);
//    ui->spnVgg->setMinimum(ucValue);
    double dVal = ((double)(ucValue)) * FACTOR_VGG;
    double dVgg = VGG_C + (VGG_A * (1-exp(-dVal / VGG_X0))) + VGG_L*dVal;

    QString strVgg = QString::number(dVgg,'f',1);
    ui->lblVggMin->setText(strVgg);
}

void Widget::on_editVggmax_returnPressed()
{
    QString strText= ui->editVggmax->text();
    unsigned char ucValue = (unsigned char) (strText.toInt());
    WriteToStorage(VGG_MAX, ucValue);
//    ui->spnVgg->setMaximum(ucValue);
    double dVal = ((double)(ucValue)) * FACTOR_VGG;
    double dVgg = VGG_C + (VGG_A * (1-exp(-dVal / VGG_X0))) + VGG_L*dVal;

    QString strVgg = QString::number(dVgg,'f',1);
    ui->lblVggMax->setText(strVgg);
}

void Widget::StopMeasurement()
{
    if(ui->btnStartMeasure->isChecked())
    {
        ui->btnStartMeasure->setText("Start Measure");
        ui->btnStartMeasure->setChecked(false);
        tmrMeasure->stop();
        {
            QMessageBox::information(this, tr("Measurement"), tr("Enable TEC for Measurements"),  QMessageBox::Ok);
        }
    }
}

void Widget::on_editSerNo_returnPressed()
{
    QString strText= ui->editSerNo->text();
    unsigned char ucValue = (unsigned char) (strText.toInt());
    WriteToStorage(SER_NO, ucValue);
    ui->lblSerNo->setText(strText);
}

void Widget::on_checkBoxVddEnabled_clicked()
{

}

void Widget::on_btnLeaveFbhonly_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget->tabBar()->hide();
}

void Widget::on_spnMaxX_editingFinished()
{
    SetupPlot();
}

void Widget::on_spnTickStepX_editingFinished()
{
    SetupPlot();
}

void Widget::on_editVctrl1Spark_returnPressed()
{
    QString strText= ui->editVctrl1Spark->text();
    unsigned int uiValue = (unsigned int) (strText.toInt());
    typUnsignedWord uiDigVal;
    uiDigVal.uiWord = (unsigned int)(round((double)(FACTOR_CTRL * ((double)(uiValue)) + OFFSET_CTRL)));

    QByteArray *Data = new QByteArray;
    Data->append((char)uiDigVal.stBytes.ucHighByte);
    Data->append((char)uiDigVal.stBytes.ucLowByte);
    WriteToRdd(0x2A, 6, UART_VCTRL1_SPARK, Data);
}

void Widget::on_editVctrl2Spark_returnPressed()
{
    QString strText= ui->editVctrl2Spark->text();
    unsigned int uiValue = (unsigned int) (strText.toInt());
    typUnsignedWord uiDigVal;
    uiDigVal.uiWord = (unsigned int)(round((double)(FACTOR_CTRL * ((double)(uiValue)) + OFFSET_CTRL)));

    QByteArray *Data = new QByteArray;
    Data->append((char)uiDigVal.stBytes.ucHighByte);
    Data->append((char)uiDigVal.stBytes.ucLowByte);
    WriteToRdd(0x2A, 6, UART_VCTRL2_SPARK, Data);
#ifdef DEBUGMODUS
   // qDebug() << "uiDigVal.uiWord=" << uiDigVal.uiWord << "; ucValue=" << uiValue ;
#endif
}
