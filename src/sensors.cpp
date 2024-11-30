#include "sensors.h"
#include "config.h"
#include <math.h>

TemperatureSensor::TemperatureSensor(int pin, String id) : pin(pin), id(id) {
    pinMode(pin, INPUT);
}

float TemperatureSensor::read() {
    int adcValue = analogRead(pin);
    float voltage = adcValue * 3.3 / 4095.0;
    float resistance = R1 * (3.3 - voltage) / voltage;
    
    float steinhart = log(resistance / THERMISTOR_R25);
    steinhart /= THERMISTOR_BETA;
    steinhart += 1.0 / (25.0 + 273.15);
    float tempC = 1.0 / steinhart - 273.15;
    
    return tempC;
}

JsonObject TemperatureSensor::toJson(JsonDocument& doc) {
    JsonObject obj = doc.createNestedObject();
    obj["id"] = id;
    obj["type"] = "temperature";
    obj["value"] = read();
    obj["unit"] = "°C";
    return obj;
}

LightSensor::LightSensor(int pin, String id) : pin(pin), id(id) {
    pinMode(pin, INPUT);
}

float LightSensor::read() {
    int adcValue = analogRead(pin);
    float voltage = adcValue * 3.3 / 4095.0;
    float resistance = R1 * (3.3 - voltage) / voltage;
    float luminosity = 100.0 * (1.0 - resistance / (resistance + R1));
    return luminosity;
}

JsonObject LightSensor::toJson(JsonDocument& doc) {
    JsonObject obj = doc.createNestedObject();
    obj["id"] = id;
    obj["type"] = "light";
    obj["value"] = read();
    obj["unit"] = "%";
    return obj;
}