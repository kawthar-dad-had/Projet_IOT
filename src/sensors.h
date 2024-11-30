#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor {
public:
    virtual float read() = 0;
    virtual String getType() = 0;
    virtual String getId() = 0;
    virtual JsonObject toJson(JsonDocument& doc) = 0;
};

class TemperatureSensor : public Sensor {
private:
    int pin;
    String id;
public:
    TemperatureSensor(int pin, String id);
    float read() override;
    String getType() override { return "temperature"; }
    String getId() override { return id; }
    JsonObject toJson(JsonDocument& doc) override;
};

class LightSensor : public Sensor {
private:
    int pin;
    String id;
public:
    LightSensor(int pin, String id);
    float read() override;
    String getType() override { return "light"; }
    String getId() override { return id; }
    JsonObject toJson(JsonDocument& doc) override;
};

#endif