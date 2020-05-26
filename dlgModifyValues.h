#ifndef DLGMODIFYVALUES_H
#define DLGMODIFYVALUES_H

#include <QObject>
#include <QDialog>
#include <qabstractbutton.h>


namespace Ui {
class dlgModifyValues;
}

class dlgModifyValues : public QDialog
{
    Q_OBJECT

public:
    explicit dlgModifyValues(TcpFunctions *TcpConnection, unsigned char ucDeviceAddress, QString strCurrentlyCurrent, QString strCurrentlyVoltage, QString strCurrentlyMode, QString strMaxCurrent, QString strMaxVoltage);
    ~dlgModifyValues();
    bool bModeModified =0;

private slots:

    void on_btnApply_clicked();

    void on_btnOk_clicked();

    void on_btnEsc_clicked();

    void on_comboboxMode_currentIndexChanged(int index);

    void on_editCurrent_valueChanged(const QString &arg1);

    void on_editVoltage_valueChanged(const QString &arg1);

    void on_boxStepI_valueChanged(int arg1);

    void on_boxStepU_valueChanged(int arg1);

private:
    Ui::dlgModifyValues *ui;
    void ChangeValues();
    TcpFunctions *connectionTcp;
    unsigned int ucAddress;
};

#endif // DLGMODIFYVALUES_H
