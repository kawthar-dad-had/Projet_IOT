#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>  // Ajout de cet en-tête pour le type String

// WiFi Configuration
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// Pin Configuration
const int LED_PIN = 13;
const int THERMISTOR_PIN = 36;
const int LDR_PIN = 39;
const int BUTTON1_PIN = 0;  // TTGO T-Display button 1
const int BUTTON2_PIN = 35; // TTGO T-Display button 2

// Sensor Configuration
const float R1 = 10000.0;           // Fixed resistor for voltage divider (10kΩ)
const float THERMISTOR_R25 = 10000.0; // Thermistor resistance at 25°C (10kΩ)
const float THERMISTOR_BETA = 3950.0; // Beta coefficient of the thermistor

// Threshold Configuration
struct ThresholdConfig {
    bool enabled = false;
    float value = 25.0;
    String sensor = "temperature"; // "temperature" or "light"
    String condition = "above";    // "above" or "below"
};

#endif