#include "dlgModifyValues.h"
#include "ui_dlgModifyValues.h"

#define DEBUGMODUS

dlgModifyValues::dlgModifyValues(TcpFunctions *TcpConnection, unsigned char ucDeviceAddress, QString strCurrentlyCurrent, QString strCurrentlyVoltage, QString strCurrentlyMode, QString strMaxCurrent, QString strMaxVoltage) :
    QDialog(),
    ui(new Ui::dlgModifyValues)
{
    ui->setupUi(this);
    connectionTcp = TcpConnection;//new TcpFunctions(socketTcp, this);
    ucAddress = ucDeviceAddress;
    //ui->editCurrent->setText( strCurrentlyCurrent);
    ui->editCurrent->setValue(strCurrentlyCurrent.toInt());
    //ui->editVoltage->setText( strCurrentlyVoltage);
    ui->editVoltage->setValue( strCurrentlyVoltage.toInt());
    ui->editMaxCurrent->setText(strMaxCurrent);
    ui->editMaxVoltage->setText(strMaxVoltage);

    if(strCurrentlyMode.compare("CC-Mode"))
    {
        ui->comboboxMode->setCurrentIndex(1);
    }
    else
    {
        ui->comboboxMode->setCurrentIndex(0);
    }
    bModeModified = 0;

}

dlgModifyValues::~dlgModifyValues()
{
    delete ui;
}

void dlgModifyValues::ChangeValues()
{
    if(bModeModified)
    {
        if(ui->comboboxMode->currentIndex() == 0)
        {//Constant Current Mode
        }
        else
        {//Constant Voltage Mode
        }
    }
}


void dlgModifyValues::on_btnApply_clicked()
{
    ChangeValues();
}

void dlgModifyValues::on_btnOk_clicked()
{
    ChangeValues();
//    strReceived = connectionTcp->SendCmdToMcps(ucAddress, "STRMD", "0");//Speichern des aktuellen Regelungs-Modes im EEPROM
    this->close();
}

void dlgModifyValues::on_btnEsc_clicked()
{
    this->close();
}

void dlgModifyValues::on_comboboxMode_currentIndexChanged(int index)
{
    bModeModified = 1;
}

void dlgModifyValues::on_editCurrent_valueChanged(const QString &arg1)
{
}

void dlgModifyValues::on_editVoltage_valueChanged(const QString &arg1)
{
}

void dlgModifyValues::on_boxStepI_valueChanged(int arg1)
{
    ui->editCurrent->setSingleStep(arg1);
}

void dlgModifyValues::on_boxStepU_valueChanged(int arg1)
{
    ui->editVoltage->setSingleStep(arg1);
}
