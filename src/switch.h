#ifndef SWITCH_H
#define SWITCH_H

#include <QWidget>
#include <qpushbutton.h>

namespace Ui {
class Switch;
}

class Switch : public QWidget
{
    Q_OBJECT

public:
    explicit Switch(QWidget *parent = 0, unsigned int uiPosX=0, unsigned int uiPosY=0);
    ~Switch();

     bool bBtnOnOffState;
     void SetSwitchOn(bool bOn);

private slots:
    void BtnClicked();
    void BtnPressed();
    void BtnRelesed();

signals:
    void Switched(bool bOn);

private:
    Ui::Switch *ui;
    QPushButton *btnSwitchOnOff;
};

#endif // SWITCH_H
