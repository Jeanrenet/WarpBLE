#include "CServerBLE.h"

#define SERVICE_UUID                0x1820
#define CHAR_UUID_TEMP              0x2aa4
#define CHAR_UUID_PRESSURE          0x2aa5
#define CHAR_UUID_ACC_X             0x2aa6
#define CHAR_UUID_ACC_Y             0x2aa7
#define CHAR_UUID_ACC_Z             0x2aa8
#define CHAR_UUID_MAG_X             0x2aa9
#define CHAR_UUID_MAG_Y             0x2aaa
#define CHAR_UUID_MAG_Z             0x2aab
#define CHAR_UUID_GYRO_X            0x2aac
#define CHAR_UUID_GYRO_Y            0x2aad
#define CHAR_UUID_GYRO_Z            0x2aae

QByteArray readValueFromFile(QString filePath)
{
    QByteArray data;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        data = file.readAll();
        data.remove(data.length() - 1, 1);
        file.close();
    }

    return data;
}

CServerBLE::CServerBLE()
{
    //initialisation mémoire partagée
    m_sharedMemory.setKey("DataExchangeMemory");
    m_sharedMemory.attach(QSharedMemory::ReadOnly);

    //timer
    m_eventLoopTimer = new QTimer;
    connect(m_eventLoopTimer, SIGNAL(timeout()), this, SLOT(updateValues()));
}

CServerBLE::~CServerBLE()
{
    delete m_eventLoopTimer;
}

void CServerBLE::initServer()
{
    QBluetoothUuid serviceBleUuid((quint32)SERVICE_UUID);

    QBluetoothUuid charTempUuid((quint32)CHAR_UUID_TEMP);
    QBluetoothUuid charPressureUuid((quint32)CHAR_UUID_PRESSURE);
    QBluetoothUuid charAccXUuid((quint32)CHAR_UUID_ACC_X);
    QBluetoothUuid charAccYUuid((quint32)CHAR_UUID_ACC_Y);
    QBluetoothUuid charAccZUuid((quint32)CHAR_UUID_ACC_Z);
    QBluetoothUuid charMagXUuid((quint32)CHAR_UUID_MAG_X);
    QBluetoothUuid charMagYUuid((quint32)CHAR_UUID_MAG_Y);
    QBluetoothUuid charMagZUuid((quint32)CHAR_UUID_MAG_Z);
    QBluetoothUuid charGyroXUuid((quint32)CHAR_UUID_GYRO_X);
    QBluetoothUuid charGyroYUuid((quint32)CHAR_UUID_GYRO_Y);
    QBluetoothUuid charGyroZUuid((quint32)CHAR_UUID_GYRO_Z);

    //serveur
    m_advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);  //Mode d'accessibilité
    m_advertisingData.setLocalName("NXP_BLE");                                                  //Nom du serveur
    m_advertisingData.setServices(QList<QBluetoothUuid>() <<
                                  serviceBleUuid
                                  );                         //Ajout des services prédéfinis

    //Création de la charactéristique
    QLowEnergyCharacteristicData charTemperature = createCharacteristic(charTempUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charPressure = createCharacteristic(charPressureUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charAccX = createCharacteristic(charAccXUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charAccY = createCharacteristic(charAccYUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charAccZ = createCharacteristic(charAccZUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charMagX = createCharacteristic(charMagXUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charMagY = createCharacteristic(charMagYUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charMagZ = createCharacteristic(charMagZUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charGyroX = createCharacteristic(charGyroXUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charGyroY = createCharacteristic(charGyroYUuid,
                                                                        QLowEnergyCharacteristic::Notify);
    QLowEnergyCharacteristicData charGyroZ = createCharacteristic(charGyroZUuid,
                                                                        QLowEnergyCharacteristic::Notify);

    //Couplage du service avec la caractéristique créée.
    QLowEnergyServiceData serviceBleData;
    serviceBleData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceBleData.setUuid(serviceBleUuid);
    serviceBleData.addCharacteristic(charTemperature);
    serviceBleData.addCharacteristic(charPressure);
    serviceBleData.addCharacteristic(charAccX);
    serviceBleData.addCharacteristic(charAccY);
    serviceBleData.addCharacteristic(charAccZ);
    serviceBleData.addCharacteristic(charMagX);
    serviceBleData.addCharacteristic(charMagY);
    serviceBleData.addCharacteristic(charMagZ);
    serviceBleData.addCharacteristic(charGyroX);
    serviceBleData.addCharacteristic(charGyroY);
    serviceBleData.addCharacteristic(charGyroZ);

    //création du controlleur BLE
    m_bleController = QLowEnergyController::createPeripheral();
    connect(m_bleController,
            SIGNAL(stateChanged(QLowEnergyController::ControllerState)),
            this,
            SLOT(controllerStateChanged(QLowEnergyController::ControllerState)));

    //ajout du service
    m_serviceBle = m_bleController->addService(serviceBleData);

    //permettra de récupérer les données reçues
    connect(m_serviceBle,
            SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this,
            SLOT(characteristicChanged(QLowEnergyCharacteristic,QByteArray)));

    //démarrage "advertising"
    m_bleController->startAdvertising(QLowEnergyAdvertisingParameters(), m_advertisingData, m_advertisingData);
}

void CServerBLE::controllerStateChanged(QLowEnergyController::ControllerState state)
{
    if (state == QLowEnergyController::UnconnectedState)
    {
        qDebug() << "Client Disconnected";
        m_bleController->startAdvertising(QLowEnergyAdvertisingParameters(), m_advertisingData, m_advertisingData);

        m_eventLoopTimer->stop();
    }

    if (state == QLowEnergyController::ConnectedState)
    {
        qDebug() << "Client Connected";

        //démarrage lecture
        m_eventLoopTimer->start(100);
    }
}

void CServerBLE::characteristicChanged(QLowEnergyCharacteristic c, QByteArray data)
{
    Q_UNUSED(c)
    Q_UNUSED(data)
    /*switch(c.uuid().toUInt32())
    {
    case CHAR_UUID_RELAY1:
        m_relay1File->open(QIODevice::ReadWrite);
        m_relay1File->write(data);
        m_relay1File->close();
        break;
    case CHAR_UUID_RELAY2:
        m_relay2File->open(QIODevice::ReadWrite);
        m_relay2File->write(data);
        m_relay2File->close();
        break;
    }*/
}

void CServerBLE::updateValues()
{
    qreal   temperature = 0.0;
    qreal   pressure = 0.0;
    qreal   accelerometerX = 0.0;
    qreal   accelerometerY = 0.0;
    qreal   accelerometerZ = 0.0;
    qreal   magnetometerX = 0.0;
    qreal   magnetometerY = 0.0;
    qreal   magnetometerZ = 0.0;
    qreal   gyroscopeX = 0.0;
    qreal   gyroscopeY = 0.0;
    qreal   gyroscopeZ = 0.0;

    if (m_sharedMemory.isAttached())
    {
        QByteArray data;
        m_sharedMemory.lock();
        data.setRawData((char*)m_sharedMemory.constData(), m_sharedMemory.size());
        m_sharedMemory.unlock();

        QDataStream stream(data);
        stream  >> temperature
                >> pressure
                >> accelerometerX
                >> accelerometerY
                >> accelerometerZ
                >> magnetometerX
                >> magnetometerY
                >> magnetometerZ
                >> gyroscopeX
                >> gyroscopeY
                >> gyroscopeZ;
    }
    else
    {
        m_sharedMemory.attach(); //attacher le segment
    }

    //notification BLE
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_TEMP, temperature);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_PRESSURE, pressure);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_ACC_X, accelerometerX);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_ACC_Y, accelerometerY);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_ACC_Z, accelerometerZ);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_MAG_X, magnetometerX);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_MAG_Y, magnetometerY);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_MAG_Z, magnetometerZ);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_GYRO_X, gyroscopeX);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_GYRO_Y, gyroscopeY);
    setValue((QBluetoothUuid)(quint32)CHAR_UUID_GYRO_Z, gyroscopeZ);
 }

void CServerBLE::setValue(QBluetoothUuid uuid, qreal value)
{
    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream << value;

    QLowEnergyCharacteristic characteristic = m_serviceBle->characteristic(uuid);
    Q_ASSERT(characteristic.isValid());
    m_serviceBle->writeCharacteristic(characteristic, data); // Potentially causes notification.
}

QLowEnergyCharacteristicData CServerBLE::createCharacteristic(QBluetoothUuid uuid, QLowEnergyCharacteristic::PropertyTypes type)
{
    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid(uuid)); //uuid définie de la charactéristique
    charData.setValue(QByteArray(2, 0));
    charData.setProperties(type); //précise le type de la propriété
    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration, QByteArray(2, 0));
    charData.addDescriptor(clientConfig);

    return charData;
}
