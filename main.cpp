#include <QCoreApplication>

#include "CServerBLE.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //create New UUID
   /* QBluetoothUuid customUuid((quint32)0x1820);

    QLowEnergyAdvertisingData advertisingData;                                              //serveur
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);  //Mode d'accessibilité
    advertisingData.setLocalName("OS #20 BLE");                                             //Nom du serveur
    advertisingData.setServices(QList<QBluetoothUuid>() << customUuid);

    //Création de la charactéristique

    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid((quint32)0x2aa4));
    charData.setValue(QByteArray(2, 0));
    charData.setProperties(QLowEnergyCharacteristic::Indicate);
    const QLowEnergyDescriptorData clientConfig(QBluetoothUuid::ClientCharacteristicConfiguration, QByteArray(2, 0));
    charData.addDescriptor(clientConfig);

    //Couplage du service avec la caractéristique créée.
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(customUuid);
    serviceData.addCharacteristic(charData);


    const QScopedPointer<QLowEnergyController> leController(QLowEnergyController::createPeripheral());
    const QScopedPointer<QLowEnergyService> serviceCustom(leController->addService(serviceData));
*
    leController->startAdvertising(QLowEnergyAdvertisingParameters(), advertisingData, advertisingData);*/

    CServerBLE *serverBLE = new CServerBLE();

    serverBLE->initServer();

    return a.exec();
}
