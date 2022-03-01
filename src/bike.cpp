
#include "bike.h"
#include "qdebugfixup.h"
#include <QSettings>

bike::bike() { elapsed.setType(metric::METRIC_ELAPSED); }

void bike::changeResistanceRange(int8_t lower, int8_t upper) {
    // 0.0 is floor and 1.0 is ceil
    double lastResistanceOffsetPerc = 0.0;
    if (RequestedLowerResistance >= 0) {
        lastResistanceOffsetPerc = (Resistance.value() - RequestedLowerResistance) / (RequestedUpperResistance - RequestedLowerResistance);
    } else {
        lastResistanceOffsetPerc = 0.0;
    }

    RequestedLowerResistance = lower;
    RequestedUpperResistance = upper;

    double calculatedResistance = RequestedLowerResistance + (lastResistanceOffsetPerc * (RequestedUpperResistance - RequestedLowerResistance));
    changeResistance(calculatedResistance);
}

void bike::changeResistance(int8_t resistance) {
    double calculatedResistance = resistance * m_difficult + gears();
    lastRawRequestedResistanceValue = resistance;

    if (autoResistanceEnable) {
        requestResistance = calculatedResistance;
        emit resistanceChanged(requestResistance);
    }
    RequestedResistance = calculatedResistance;
}

void bike::changeInclination(double grade, double percentage) {
    qDebug() << QStringLiteral("bike::changeInclination") << autoResistanceEnable << grade << percentage;
    if (autoResistanceEnable) {
        requestInclination = grade;
    }
    emit inclinationChanged(grade, percentage);
}

// originally made for renphobike, but i guess it could be very generic
uint16_t bike::powerFromResistanceRequest(int8_t requestResistance) {
    // this bike has resistance level to N.m so the formula is Power (kW) = Torque (N.m) x Speed (RPM) / 9.5488
    double cadence = RequestedCadence.value();
    if (cadence <= 0)
        cadence = Cadence.value();
    return (requestResistance * cadence) / 9.5488;
}

void bike::changeRequestedPelotonResistance(int8_t resistance) { RequestedPelotonResistance = resistance; }
void bike::changeCadence(int16_t cadence) { RequestedCadence = cadence; }
void bike::changePower(int32_t power) {

    RequestedPower = power;
    requestPower = power; // used by some bikes that have ERG mode builtin
    QSettings settings;
    bool force_resistance = settings.value(QStringLiteral("virtualbike_forceresistance"), true).toBool();
    // bool erg_mode = settings.value(QStringLiteral("zwift_erg"), false).toBool(); //Not used anywhere in code
    double erg_filter_upper = settings.value(QStringLiteral("zwift_erg_filter"), 0.0).toDouble();
    double erg_filter_lower = settings.value(QStringLiteral("zwift_erg_filter_down"), 0.0).toDouble();
    double zwift_erg_resistance_up = settings.value(QStringLiteral("zwift_erg_resistance_up"), 999.0).toDouble();
    double zwift_erg_resistance_down = settings.value(QStringLiteral("zwift_erg_resistance_down"), 0.0).toDouble();

    double deltaDown = wattsMetric().value() - ((double)power);
    double deltaUp = ((double)power) - wattsMetric().value();
    qDebug() << QStringLiteral("filter  ") + QString::number(deltaUp) + " " + QString::number(deltaDown) + " " +
                    QString::number(erg_filter_upper) + " " + QString::number(erg_filter_lower);
    if (!ergModeSupported && force_resistance /*&& erg_mode*/ &&
        (deltaUp > erg_filter_upper || deltaDown > erg_filter_lower)) {
        int8_t r = (int8_t)resistanceFromPowerRequest(power);
        if ((double)r > zwift_erg_resistance_up) {
            qDebug() << "zwift_erg_resistance_up filter enabled!";
            r = (int8_t)zwift_erg_resistance_up;
        } else if ((double)r < zwift_erg_resistance_down) {
            qDebug() << "zwift_erg_resistance_down filter enabled!";
            r = (int8_t)zwift_erg_resistance_down;
        }
        changeResistance(r); // resistance start from 1
    }
}

int8_t bike::gears() { return m_gears; }
void bike::setGears(int8_t gears) {
    qDebug() << "setGears" << gears;
    m_gears = gears;
    if (lastRawRequestedResistanceValue != -1) {
        changeResistance(lastRawRequestedResistanceValue);
    }
}

double bike::currentCrankRevolutions() { return CrankRevs; }
uint16_t bike::lastCrankEventTime() { return LastCrankEventTime; }
metric bike::lastRequestedResistance() { return RequestedResistance; }
metric bike::lastRequestedPelotonResistance() { return RequestedPelotonResistance; }
metric bike::lastRequestedCadence() { return RequestedCadence; }
metric bike::lastRequestedPower() { return RequestedPower; }
metric bike::currentResistance() { return Resistance; }
uint8_t bike::fanSpeed() { return FanSpeed; }
bool bike::connected() { return false; }
uint16_t bike::watts() { return 0; }
metric bike::pelotonResistance() { return m_pelotonResistance; }
int bike::pelotonToBikeResistance(int pelotonResistance) { return pelotonResistance; }
uint8_t bike::resistanceFromPowerRequest(uint16_t power) { return power / 10; } // in order to have something
void bike::cadenceSensor(uint8_t cadence) { Cadence.setValue(cadence); }
void bike::powerSensor(uint16_t power) { m_watt.setValue(power); }

bluetoothdevice::BLUETOOTH_TYPE bike::deviceType() { return bluetoothdevice::BIKE; }

void bike::clearStats() {

    moving.clear(true);
    elapsed.clear(true);
    Speed.clear(false);
    KCal.clear(true);
    Distance.clear(true);
    Heart.clear(false);
    m_jouls.clear(true);
    elevationAcc = 0;
    m_watt.clear(false);
    WeightLoss.clear(false);

    RequestedPelotonResistance.clear(false);
    RequestedResistance.clear(false);
    RequestedCadence.clear(false);
    RequestedPower.clear(false);
    m_pelotonResistance.clear(false);
    Cadence.clear(false);
    Resistance.clear(false);
    WattKg.clear(false);
}

void bike::setPaused(bool p) {

    paused = p;
    moving.setPaused(p);
    elapsed.setPaused(p);
    Speed.setPaused(p);
    KCal.setPaused(p);
    Distance.setPaused(p);
    Heart.setPaused(p);
    m_jouls.setPaused(p);
    m_watt.setPaused(p);
    WeightLoss.setPaused(p);
    m_pelotonResistance.setPaused(p);
    Cadence.setPaused(p);
    Resistance.setPaused(p);
    RequestedPelotonResistance.setPaused(p);
    RequestedResistance.setPaused(p);
    RequestedCadence.setPaused(p);
    RequestedPower.setPaused(p);
    WattKg.setPaused(p);
}

void bike::setLap() {

    moving.setLap(true);
    elapsed.setLap(true);
    Speed.setLap(false);
    KCal.setLap(true);
    Distance.setLap(true);
    Heart.setLap(false);
    m_jouls.setLap(true);
    m_watt.setLap(false);
    WeightLoss.setLap(false);
    WattKg.setLap(false);

    RequestedPelotonResistance.setLap(false);
    RequestedResistance.setLap(false);
    RequestedCadence.setLap(false);
    RequestedPower.setLap(false);
    m_pelotonResistance.setLap(false);
    Cadence.setLap(false);
    Resistance.setLap(false);
}

uint8_t bike::metrics_override_heartrate() {

    QSettings settings;
    QString setting =
        settings.value(QStringLiteral("peloton_heartrate_metric"), QStringLiteral("Heart Rate")).toString();
    if (!setting.compare(QStringLiteral("Heart Rate"))) {
        return qRound(currentHeart().value());
    } else if (!setting.compare(QStringLiteral("Speed"))) {

        return qRound(currentSpeed().value());
    } else if (!setting.compare(QStringLiteral("Inclination"))) {

        return 0;
    } else if (!setting.compare(QStringLiteral("Cadence"))) {

        return qRound(Cadence.value());
    } else if (!setting.compare(QStringLiteral("Elevation"))) {

        return qRound(elevationGain().value());
    } else if (!setting.compare(QStringLiteral("Calories"))) {

        return qRound(calories().value());
    } else if (!setting.compare(QStringLiteral("Odometer"))) {

        return qRound(odometer());
    } else if (!setting.compare(QStringLiteral("Pace"))) {

        return currentPace().second();
    } else if (!setting.compare(QStringLiteral("Resistance"))) {

        return qRound(Resistance.value());
    } else if (!setting.compare(QStringLiteral("Watt"))) {

        return qRound(wattsMetric().value());
    } else if (!setting.compare(QStringLiteral("Weight Loss"))) {

        return qRound(weightLoss());
    } else if (!setting.compare(QStringLiteral("AVG Watt"))) {

        return qRound(wattsMetric().average());
    } else if (!setting.compare(QStringLiteral("FTP"))) {

        return 0;
    } else if (!setting.compare(QStringLiteral("Fan"))) {

        return FanSpeed;
    } else if (!setting.compare(QStringLiteral("Jouls"))) {

        return qRound(jouls().value());
    } else if (!setting.compare(QStringLiteral("Lap Elapsed"))) {

        return lapElapsedTime().second();
    } else if (!setting.compare(QStringLiteral("Elapsed"))) {

        return qRound(elapsed.value());
    } else if (!setting.compare(QStringLiteral("Moving Time"))) {

        return movingTime().second();
    } else if (!setting.compare(QStringLiteral("Peloton Offset"))) {

        return 0;
    } else if (!setting.compare(QStringLiteral("Peloton Resistance"))) {

        return qRound(pelotonResistance().value());
    } else if (!setting.compare(QStringLiteral("Date Time"))) {

        return 0;
    } else if (!setting.compare(QStringLiteral("Target Resistance"))) {

        return qRound(RequestedResistance.value());
    } else if (!setting.compare(QStringLiteral("Target Peloton Resistance"))) {

        return qRound(RequestedPelotonResistance.value());
    } else if (!setting.compare(QStringLiteral("Target Power"))) {

        return qRound(RequestedPower.value());
    } else if (!setting.compare(QStringLiteral("Watt/Kg"))) {
        return qRound(wattKg().value());
    }
    return qRound(currentHeart().value());
}
