#ifndef DEVICETEC_H
#define DEVICETEC_H

#include "widgetdevice.h"

class DeviceTEC : public widgetDevice
{
        Q_OBJECT
public:
    explicit DeviceTEC(QWidget* parent);

    void TecInit();
    void ReadDeviceValues();

public slots:
    void BtnOnOffClicked(bool bOn);
};

#endif // DEVICETEC_H
