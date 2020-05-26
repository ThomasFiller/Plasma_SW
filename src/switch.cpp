#include "switch.h"
#include "ui_switch.h"

Switch::Switch(QWidget *parent, unsigned int uiPosX, unsigned int uiPosY) :
    QWidget(parent),
    ui(new Ui::Switch)
{
    ui->setupUi(this);

    this->setGeometry(uiPosX, uiPosY,45,45);
    btnSwitchOnOff = new QPushButton(this);
    QColor NoColor;
    NoColor.setAlpha(0);
    QPalette Pal(palette());
    Pal.setColor(QPalette::Button, NoColor.alpha());
    btnSwitchOnOff->setAutoFillBackground(true);
    btnSwitchOnOff->setPalette(Pal);
    btnSwitchOnOff->setFlat(true);

    QPixmap Schalter(":/Schalter_rot.png");
    QIcon ButtonIcon(Schalter);
    btnSwitchOnOff->setIcon(ButtonIcon);
    btnSwitchOnOff->setIconSize(QSize(45,45));
    btnSwitchOnOff->setEnabled(true);
    btnSwitchOnOff->show();
    btnSwitchOnOff->update();
    btnSwitchOnOff->setGeometry(0, 0,45,45);
    btnSwitchOnOff->setStyleSheet("QPushButton { background-color : rgba(255,255,255,0); color : rgba(255,255,255,0); }");

    connect(btnSwitchOnOff, SIGNAL(pressed()), SLOT (BtnPressed()));
    connect(btnSwitchOnOff, SIGNAL(released()), SLOT (BtnRelesed()));
    connect(btnSwitchOnOff, SIGNAL(clicked()), SLOT (BtnClicked()));
}

void Switch::BtnPressed()
{
    if(bBtnOnOffState)
    {
        QPixmap Schalter(":/Schalter_gruen_gedrueckt.png");
        QIcon ButtonIcon(Schalter);
        btnSwitchOnOff->setIcon(ButtonIcon);
        bBtnOnOffState = 1;
    }
    else
    {
        QPixmap Schalter(":/Schalter_rot_gedrueckt.png");
        QIcon ButtonIcon(Schalter);
        btnSwitchOnOff->setIcon(ButtonIcon);
        bBtnOnOffState = 0;
    }
    btnSwitchOnOff->update();
}

void Switch::BtnRelesed()
{
    if(bBtnOnOffState)
    {
        QPixmap Schalter(":/Schalter_gruen.png");
        QIcon ButtonIcon(Schalter);
        btnSwitchOnOff->setIcon(ButtonIcon);
        bBtnOnOffState = 1;
    }
    else
    {
        QPixmap Schalter(":/Schalter_rot.png");
        QIcon ButtonIcon(Schalter);
        btnSwitchOnOff->setIcon(ButtonIcon);
        bBtnOnOffState = 0;
    }
    btnSwitchOnOff->update();
}

void Switch::BtnClicked()
{
    emit Switched(bBtnOnOffState);
}

void Switch::SetSwitchOn(bool bOn)
{
    if(bOn)
    {
        QPixmap Schalter(":/Schalter_gruen.png");
        QIcon ButtonIcon(Schalter);
        btnSwitchOnOff->setIcon(ButtonIcon);
        bBtnOnOffState = 1;
    }
    else
    {
        QPixmap Schalter(":/Schalter_rot.png");
        QIcon ButtonIcon(Schalter);
        btnSwitchOnOff->setIcon(ButtonIcon);
        bBtnOnOffState = 0;
    }
    btnSwitchOnOff->update();
}


Switch::~Switch()
{
    delete ui;
}
