#ifndef HORIZONTREADMILL_H
#define HORIZONTREADMILL_H

#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
//#include <QtBluetooth/private/qlowenergycontrollerbase_p.h>
//#include <QtBluetooth/private/qlowenergyserviceprivate_p.h>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QtCore/qbytearray.h>

#ifndef Q_OS_ANDROID
#include <QtCore/qcoreapplication.h>
#else
#include <QtGui/qguiapplication.h>
#endif
#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qtimer.h>

#include <QDateTime>
#include <QObject>
#include <QString>

#include "treadmill.h"
#include "virtualbike.h"
#include "virtualtreadmill.h"

#ifdef Q_OS_IOS
#include "ios/lockscreen.h"
#endif

class horizontreadmill : public treadmill {
    Q_OBJECT
  public:
    horizontreadmill(bool noWriteResistance, bool noHeartService);
    bool connected();
    void forceSpeed(double requestSpeed);
    void forceIncline(double requestIncline);

    void *VirtualTreadmill();
    void *VirtualDevice();

    bool autoPauseWhenSpeedIsZero();
    bool autoStartWhenSpeedIsGreaterThenZero();

  private:
    void writeCharacteristic(QLowEnergyService *service, QLowEnergyCharacteristic characteristic, uint8_t *data,
                             uint8_t data_len, QString info, bool disable_log = false, bool wait_for_response = false);
    void waitForAPacket();
    void startDiscover();
    void btinit();

    QTimer *refresh;
    virtualtreadmill *virtualTreadmill = nullptr;
    virtualbike *virtualBike = nullptr;

    QList<QLowEnergyService *> gattCommunicationChannelService;
    QLowEnergyCharacteristic gattWriteCharControlPointId;
    QLowEnergyService *gattFTMSService = nullptr;
    QLowEnergyCharacteristic gattWriteCharCustomService;
    QLowEnergyService *gattCustomService = nullptr;
    volatile int notificationSubscribed = 0;

    uint8_t sec1Update = 0;
    QByteArray lastPacket;
    QByteArray lastPacketComplete;
    QDateTime lastRefreshCharacteristicChanged = QDateTime::currentDateTime();
    bool firstDistanceCalculated = false;
    uint8_t firstStateChanged = 0;
    double lastSpeed = 0.0;
    double lastInclination = 0;
    int64_t lastStart = 0;
    int64_t lastStop = 0;

    bool initDone = false;
    bool initRequest = false;

    bool noWriteResistance = false;
    bool noHeartService = false;

    int32_t customRecv = 0;
    int32_t messageID = 0;
    int GenerateCRC_CCITT(uint8_t *PUPtr8, int PU16_Count);

#ifdef Q_OS_IOS
    lockscreen *h = 0;
#endif

  signals:
    void disconnected();
    void debug(QString string);
    void packetReceived();

  public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);

  private slots:

    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &newValue);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &newValue);
    void stateChanged(QLowEnergyService::ServiceState state);
    void controllerStateChanged(QLowEnergyController::ControllerState state);

    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone(void);
    void update();
    void error(QLowEnergyController::Error err);
    void errorService(QLowEnergyService::ServiceError);

    void changeInclinationRequested(double grade, double percentage);
};

#endif // HORIZONTREADMILL_H
