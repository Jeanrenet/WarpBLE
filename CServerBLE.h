#ifndef CSERVERBLE_H
#define CSERVERBLE_H

#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
#include <QtEndian>
#include <QFile>
#include <QTimer>
#include <QSharedMemory>
#include <QDataStream>

class CServerBLE : public QObject
{
    Q_OBJECT
public:
    CServerBLE();
    ~CServerBLE();

public:
    void initServer();

protected slots:
    void controllerStateChanged(QLowEnergyController::ControllerState state);
    void characteristicChanged(QLowEnergyCharacteristic c, QByteArray data);
    void updateValues();

protected:
    QLowEnergyCharacteristicData createCharacteristic(QBluetoothUuid uuid,  QLowEnergyCharacteristic::PropertyTypes type);
    void setValue(QBluetoothUuid uuid, qreal value);

private:
    QLowEnergyAdvertisingData   m_advertisingData;
    QSharedMemory               m_sharedMemory;
    QLowEnergyService           *m_serviceBle;
    QLowEnergyController        *m_bleController;
    QTimer                      *m_eventLoopTimer;
};

#endif // CSERVERBLE_H
