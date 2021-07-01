#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QtCore/qbytearray.h>
#include <QtCore/qloggingcategory.h>
#include <QFile>

#include "treadmill.h"
#include "domyostreadmill.h"
#include "domyosbike.h"
#include "domyoselliptical.h"
#include "trxappgateusbtreadmill.h"
#include "trxappgateusbbike.h"
#include "toorxtreadmill.h"
#include "echelonconnectsport.h"
#include "schwinnic4bike.h"
#include "yesoulbike.h"
#include "flywheelbike.h"
#include "m3ibike.h"
#include "fitshowtreadmill.h"
#include "inspirebike.h"
#include "snodebike.h"
#include "horizontreadmill.h"
#include "eslinkertreadmill.h"
#include "proformbike.h"
#include "proformtreadmill.h"
#include "sportstechbike.h"
#include "skandikawiribike.h"
#include "heartratebelt.h"
#include "cscbike.h"
#include "npecablebike.h"
#include "chronobike.h"
#include "stagesbike.h"
#include "soleelliptical.h"
#include "fitplusbike.h"
#include "spirittreadmill.h"
#include "ftmsbike.h"
#include "ftmsrower.h"
#include "smartspin2k.h"
#include "renphobike.h"
#include "tacxneo2.h"
#include "echelonrower.h"
#include "smartrowrower.h"
#include "echelonstride.h"
#include "bluetoothdevice.h"
#include "signalhandler.h"
#include "templateinfosenderbuilder.h"

class bluetooth : public QObject, public SignalHandler
{
    Q_OBJECT
public:
    explicit bluetooth(bool logs, QString deviceName = "", bool noWriteResistance = false, bool noHeartService = false, uint32_t pollDeviceTime = 200, bool noConsole = false, bool testResistance = false, uint8_t bikeResistanceOffset = 4, double bikeResistanceGain = 1.0);
    ~bluetooth();
    bluetoothdevice* device();
    bluetoothdevice* heartRateDevice() {return heartRateBelt;}
    QList<QBluetoothDeviceInfo> devices;
    bool onlyDiscover = false;

private:
    TemplateInfoSenderBuilder * templateManager = 0;
    QFile* debugCommsLog = 0;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    fitshowtreadmill* fitshowTreadmill = 0;
    domyostreadmill* domyos = 0;
    domyosbike* domyosBike = 0;
    domyoselliptical* domyosElliptical = 0;
    toorxtreadmill* toorx = 0;
    trxappgateusbtreadmill* trxappgateusb = 0;
    spirittreadmill* spiritTreadmill = 0;
    trxappgateusbbike* trxappgateusbBike = 0;
    echelonconnectsport* echelonConnectSport = 0;
    yesoulbike* yesoulBike = 0;
    flywheelbike* flywheelBike = 0;
    proformbike* proformBike = 0;
    proformtreadmill* proformTreadmill = 0;
    horizontreadmill* horizonTreadmill = 0;
    schwinnic4bike* schwinnIC4Bike = 0;
    sportstechbike* sportsTechBike = 0;
    inspirebike* inspireBike = 0;
    snodebike* snodeBike = 0;
    eslinkertreadmill* eslinkerTreadmill = 0;
    m3ibike* m3iBike = 0;
    skandikawiribike* skandikaWiriBike = 0;
    cscbike* cscBike = 0;
    npecablebike* npeCableBike = 0;
    stagesbike* stagesBike = 0;
    soleelliptical* soleElliptical = 0;
    chronobike* chronoBike = 0;
    fitplusbike* fitPlusBike = 0;
    echelonrower* echelonRower = 0;
    ftmsrower* ftmsRower = 0;
    smartrowrower* smartrowRower = 0;
    echelonstride *echelonStride = 0;
    ftmsbike* ftmsBike = 0;
    tacxneo2 *tacxneo2Bike = 0;
    renphobike *renphoBike = 0;
    heartratebelt* heartRateBelt = 0;
    smartspin2k* ftmsAccessory = 0;
    cscbike* cadenceSensor = 0;
    QString filterDevice = "";
    bool testResistance = false;
    bool noWriteResistance = false;
    bool noHeartService = false;
    bool noConsole = false;
    bool logs = true;
    uint32_t pollDeviceTime = 200;
    uint8_t bikeResistanceOffset = 4;
    double bikeResistanceGain = 1.0;
    bool forceHeartBeltOffForTimeout = false;

    bool handleSignal(int signal);
    void stateFileUpdate();
    void stateFileRead();
    bool heartRateBeltAvaiable();
    bool ftmsAccessoryAvaiable();
    bool cscSensorAvaiable();

signals:
    void deviceConnected();
    void deviceFound(QString name);
    void searchingStop();

public slots:
    void restart();
    void debug(QString string);
    void heartRate(uint8_t heart);

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    void deviceUpdated(const QBluetoothDeviceInfo &device, QBluetoothDeviceInfo::Fields updateFields);
#endif
    void canceled();
    void finished();
    void speedChanged(double);
    void inclinationChanged(double);
    void connectedAndDiscovered();

signals:

};

#endif // BLUETOOTH_H
