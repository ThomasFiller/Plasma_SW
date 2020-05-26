#include "DeviceTec.h"

DeviceTEC::DeviceTEC(QWidget *parent): widgetDevice(parent)
{

}

void DeviceTEC::TecInit()
{
    DeviceInit();
    connect(switchOnOff, SIGNAL(Switched(bool)),this, SLOT (BtnOnOffClicked(bool)));
}

void DeviceTEC::BtnOnOffClicked(bool bOn)
{
    if(bOn)
    {
//        connectionTcp->SendCmdToMcps(ucAddress, "","");
    }
    else
    {
//        connectionTcp->SendCmdToMcps(ucAddress, "","");
    }
    ReadDeviceValues();
}

void DeviceTEC::ReadDeviceValues()
{

}
