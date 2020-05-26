#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QHostInfo>
#include <QLabel>
#include <qcombobox.h>
#include <qsignalmapper.h>
#include <QFileDialog>
#include <QDate>
#include "globalconsts.h"
#include <QtSerialPort/QtSerialPort>
#include <QVector>
#include <QLCDNumber>
#include <QRadioButton>
#include <QSplashScreen>
#include <QPicture>
#include <QPainter>
#include <QKeyEvent>
#include <qspinbox.h>
#include "src/qcustomplot.h"
//#include "src/switch.h"

#include <QImage>
#include <QLabel>
#include <QPixmap>

#define COLOR_GATE  0, 200, 100
#define COLOR_DRAIN 0, 100, 250
#define COLOR_PWR_TO 0, 200, 100
#define COLOR_PWR_FROM 0, 100, 250

#define GRAPH_TIME_INTERVALL    200
#define X_MAX                   (int)100
#define X_TICK_STEP (int)10
#define WITH_PWR_BACK

//Faktoren und Offsets zur Umrechnung der Digitalwerte in physikalische Größen
#define FACTOR_VDD (double)(256)

#define FACTOR_I_VDD (double)6.264
#define OFFSET_I_VDD (double)251.79

#define FACTOR_VGG (double)(256)

#define FACTOR_I_VGG (double)1609
#define OFFSET_I_VGG (double)176

#define FACTOR_P_TO (double)0.00000071

#define FACTOR_P_BACK (double)0.00000071

#define FACTOR_READTEMP (double)(-0.00443)//(-0.00344)
#define OFFSET_READTEMP (double)162//(127)

#define FACTOR_VDD_MESURED (double)(0.001375)
#define OFFSET_VDD_MESURED (double)(0)

#define FACTOR_VGG_MESURED (double)(0.000531)
#define OFFSET_VGG_MESURED (double)(-16.5)

#define FACTOR_CTRL (double)(13.107)
#define OFFSET_CTRL (double)(0)

//Allgemein; int. Setup
#define	UART_ERROR	0x00	//Errorcode (see below)
    #define	NO_ERROR                        0
    #define	ERROR_TIMEOUT_BEFORE_1st_BYTE   1
    #define	ERROR_TIMEOUT_BEFORE_2nd_BYTE	2
    #define	ERROR_NOT_ENOUGH_DATABYTES		3
    #define	ERROR_WRONG_CMD					4
    #define	ERROR_VGG_NOT_RIGHT				5
    #define	ERROR_CHECKSUM					7
#define	UART_SETUP_NO	0x03	//	1	1	liest oder schreibt eine der vier Setupnummern, die sich parrallel auch mit dem Schalter einstellen und über die LEDs ablesen lassen
#define	UART_SAVE_SETUP	0x04	//	0	/	veranlasst den uC, das aktuelle Setup und die aktuelle Setupnummer zu speichern
#define	UART_DEVICE_STATE	0x05	//	/	1	OFF=0; BOOTING=1; READY=2; ON=3; OVERTEMP=4; STATE_IVDD_ERROR=5; STATE_IVGG_ERROR=6
    #define STATE_OFF 0
    #define STATE_BOOTING 1
    #define STATE_READY 2
    #define STATE_ON 3
    #define STATE_OVERTEMP 4
    #define STATE_IVDD_ERROR 5
    #define STATE_IVGG_ERROR 6

#define	UART_5V_GOOD        0x06	//	/	1	Liest, ob 5-V-Versorgungsspannung ok ist (0-nicht ok; 1-ok)
#define	UART_TEMPERATURE	0x07	//	/	2	Liest Temperatur, die mit dem STM8-internen Sensor ermittelt wurde
#define	UART_STORAGE        0x09	//	2	16	16 Reservedaten zum Abspeichern     write - 1. Byte: Adresse (0..15); zweites Byte: Datum
                                    //                                          read - alle 16 gespeicherten Daten von 0 bis 15
    #define VDD_MIN     0
    #define VDD_MAX     1
    #define VGG_MIN     2
    #define VGG_MAX     3
    #define SER_NO      4
#define	UART_EN_PLASMA	0x0A	//	1	1	Einschalten oder Ausschalten des Plasmas in geordneter Sequenz
#define	UART_READ_ALL	0x0B	//	/	16	Liest alle Daten, die für zyklische Anzeige benötigt werden
#define	UART_SPARK_DURATION	0x0C//	2	2	Dauer, die Vgg,spark gehalten wird

//VDD (20..40V; 0..2,5A)
#define	UART_EN_VDD     0x10	//	1	1	Einschalten des Spannungsreglers IC20 (VDD)
#define	UART_VDD_GOOD	0x11	//	/	1	Liest, ob VDD ok ist (0-nicht ok; 1-ok)
#define	UART_DIGPOTI_SAVE	0x12	//	0	/	Aktuelle Einstellung des Potis für VDD und VGG in das NonVolatile-Register schreiben; beim nächsten PotiStart wird dieser Wert übernommen
#define	UART_VDD        0x13	//	2	2	Spannung VDD einstellen
#define	UART_I_VDD      0x14	//	/	2	Strom durch VDD lesen
#define	UART_VDD_SPARK	0x15	//	2	2	Spannung einstellen, bei der das Plasma zündet. Diese Spannung wird bei der automatischen Zündung kurz nach Einschalten von Vgg kurz an Vdd angelegt
#define	UART_READ_VDD	0x16	//	/	2	lesen der gemessenen Spannung VDD


//VGG (-6..-1V; -1..10mA)
#define	UART_EN_VGG     0x20	//	1	1	Einschalten des SwitchedCap-Spannungsreglers IC40 (VGG)
#define	UART_VGG        0x21	//	2	2	Spannung VGG einstellen
#define	UART_I_VGG      0x22	//	/	2	Strom durch VGG lesen
#define	UART_VGG_SPARK	0x23//	2	2	Spannung einstellen, bei der das Plasma zündet. Diese Spannung wird bei der automatischen Zündung kurz nach Einschalten von Vdd kurz an Vgg angelegt
#define	UART_READ_VGG	0x24//	/	2	lesen der gemessenen Spannung VGG

//MW // Mikrowellenparameter
#define	UART_MW_PWR_TO      0x30	//	/	2	lesen der Leistung, die in die Mikrowelle geht
#define	UART_MW_PWR_BACK	0x31	//	/	2	lesen der Leistung, die von der Mikrowelle kommt

//Steuerspannung für Kapazitäten zur Steuerung des Arbeitspunktes
#define	UART_CTRL1          0x40	//	2	2	Steuerspannung CTRL1
#define	UART_CTRL2          0x41	//	2	2	Steuerspannung CTRL2
#define	UART_VCTRL1_SPARK	0x42	//	2	2	Steuerspannung CTRL1, welche während des Zündvorgangs angelegt wird
#define	UART_VCTRL2_SPARK	0x43	//	2	2	Steuerspannung CTRL2, welche während des Zündvorgangs angelegt wird

typedef struct
{
    unsigned char	ucLowByte;
    unsigned char	ucHighByte;
} typDoubleByte;

typedef union
{
    quint16	uiWord;
    typDoubleByte	stBytes;
} typUnsignedWord;

typedef union
{
    qint16		iWord;
    typDoubleByte	stBytes;
} typSignedWord;

class settingsComPort;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    double dTime=0;
    bool bConnected = 0;

    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();

//    Switch *switchPwr;

public slots:
    void tmrslotWatchdog();
//    void switchPwrClicked(bool);

private slots:
    void on_btnConnect_clicked();
    void on_btnConfigComPort_clicked();
    void on_btnReadSetup_clicked();
    void on_lblReceived_textChanged(const QString &arg1);
    void on_tabWidget_currentChanged(int index);
    void on_spnVgg_valueChanged(int arg1);
    void on_spnVdd_valueChanged(int arg1);
    void on_checkBoxVddEnabled_clicked(bool checked);
    void on_checkBoxVggEnabled_clicked(bool checked);
    void on_spnSetup_valueChanged(int arg1);
    void on_btnSaveSetup_clicked();

    void tmrslotDelay();
    void tmrslotMeasure();

    void on_btnStartMeasure_clicked(bool checked);
    void on_btnResetGraf_clicked();
    void on_editVggmin_returnPressed();
    void on_editVggmax_returnPressed();
    void on_spnVdd_editingFinished();
    void on_spnVgg_editingFinished();

    void on_editVddmax_returnPressed();

    void on_editVddmin_returnPressed();

    void on_editSerNo_returnPressed();

    void on_btnStartPlasma_clicked(bool checked);

    void on_checkBoxVddEnabled_clicked();

    void on_sliderVgg_valueChanged(int value);

    void on_sliderVdd_valueChanged(int value);

    void on_editVddSpark_returnPressed();

    void on_editVggSpark_returnPressed();

    void on_editSparkDuration_returnPressed();

    void on_btnLeaveFbhonly_clicked();

    void on_spnMaxX_editingFinished();

    void on_spnTickStepX_editingFinished();

    void on_spnCtrl1_valueChanged(int arg1);

    void on_spnCtrl1_editingFinished();

    void on_spnCtrl2_valueChanged(int arg1);

    void on_spnCtrl2_editingFinished();

    void on_editVctrl1Spark_returnPressed();

    void on_editVctrl2Spark_returnPressed();

private:
    Ui::Widget *ui;

    QTimer *tmrMeas;
    QTimer *tmrWatchdog;
    QTimer *tmrDelay;
    QTimer *tmrMeasure;

    QSerialPort *serial;
    settingsComPort *settings_ComPort;

    QString strReceivedString;
    QString strSent;
    QString strDeviceState[7] = {"OFF", "BOOTING", "READY", "ON", "Overtemp", "Over-I Drain", "Over-I Gate"};
    QByteArray arReceivedData;

    bool bWidgetBooting;
    bool bComBusy = 0;
    bool bParameterChangeRunning = 0;
    bool bDelayRunning = 0;
    bool bAutoRead = 1;
    bool ParameterChanged = 0;
    bool VggChangeRequired = 0;
    bool VddChangeRequired = 0;
    bool Ctrl1ChangeRequired = 0;
    bool Ctrl2ChangeRequired = 0;

    bool spnVddChangedByUart = 0;
    bool spnVggChangedByUart = 0;
    bool spnCtrl1ChangedByUart = 0;
    bool spnCtrl2ChangedByUart = 0;
    bool spnSetupChangedByUart = 0;
    bool StorageChangedByUart = 0;

    unsigned char ucComTask;

    unsigned int uiRecLen = 0;
    signed int iMinTemperature = 200, iMaxTemperature = 0, iMinIdd = 0, iMaxIdd = 0, iMinIgg = 0, iMaxIgg=0, iMinPwrTo = 0, iMaxPwrTo = 1, iMinPwrBack = 0, iMaxPwrBack = 1;
    int iVgg = 0;
    int iVdd=0;
    unsigned int uiPwrTo=0;
    unsigned int uiPwrBack=0;
    int iCtrl1 = 0;
    int iCtrl2 = 0;
    int iCtrl1Spark = 0;
    int iCtrl2Spark = 0;

    double dIdd;
    double dIgg;

    double VggMeasured;
    double VddMeasured;

    double dTemperature = 0;

    void NewValueReceived(unsigned int uiPhysVal, bool &FlagChangedByUart, QSpinBox *SpinBox);
    void WriteToStorage(unsigned char ucAddress, unsigned char ucDate);
    void StopMeasurement();
    void ChangeValue(unsigned int uiDigitalValue, int NewValue, int &OldValue, bool &FlagChangedByUart, QSpinBox *SpinBox, bool &FlagChangeRequired, char cUartCmd, unsigned char ucUartLen);
    void Delay(int mSec);
    void ReadAllDataCmd();
    void WriteToRdd(char cReadAndAddress, unsigned char cLen, char cCmd, QByteArray *Data);
    void SetupPlot();
    void DeviceStateReceived(unsigned char ucDevState);
};

#endif // WIDGET_H
