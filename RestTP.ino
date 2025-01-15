#include "src/config.h"
#include "src/sensors.h"
#include "src/display.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);
Display display;
ThresholdConfig threshold;

// Sensors
TemperatureSensor tempSensor(THERMISTOR_PIN, "temp1");
LightSensor lightSensor(LDR_PIN, "light1");
Sensor* sensors[] = {&tempSensor, &lightSensor};
const int sensorCount = 2;

void setup() {
    Serial.begin(115200);
    
    // Initialize display
    display.init();
    display.showStatus("Initializing...");
    
    // Initialize LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize buttons
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    
    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    display.showStatus("WiFi Connected");
    Serial.println(WiFi.localIP());
    
    setupAPI();
    server.begin();
}

void setupAPI() {

    // Add CORS headers to all responses
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

    // Handle OPTIONS requests for CORS preflight
    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404);
        }
    });

    // GET /api/sensors - List all sensors
    server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(1024);
        JsonArray array = doc.createNestedArray();
        
        for(int i = 0; i < sensorCount; i++) {
            sensors[i]->toJson(doc);
        }
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
      
    // GET /api/sensors/{id} - Get specific sensor
    server.on("^\\/api\\/sensors\\/([a-z0-9]+)$", HTTP_GET, [](AsyncWebServerRequest *request) {
        String id = request->pathArg(0);
        Serial.println("Sensor ID requested: " + id); // Ajoutez ceci pour déboguer
        
        for(int i = 0; i < sensorCount; i++) {
            if(sensors[i]->getId() == id) {
                DynamicJsonDocument doc(256);
                sensors[i]->toJson(doc);
                
                String response;
                serializeJson(doc, response);
                Serial.println("Sensor data: " + response); // Ajoutez ceci pour voir les données
                request->send(200, "application/json", response);
                return;
            }
        }
        
        request->send(404, "application/json", "{\"error\": \"Sensor not found\"}");
    });

    // POST /api/led - Control LED
    server.on("/api/led", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, 
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, (const char*)data);
        
        if(doc.containsKey("state")) {
            String state = doc["state"].as<String>();
            if(state == "on") {
                digitalWrite(LED_PIN, HIGH);
                request->send(200, "application/json", "{\"status\": \"LED turned on\"}");
            }
            else if(state == "off") {
                digitalWrite(LED_PIN, LOW);
                request->send(200, "application/json", "{\"status\": \"LED turned off\"}");
            }
            else if(state == "toggle") {
                digitalWrite(LED_PIN, !digitalRead(LED_PIN));
                request->send(200, "application/json", "{\"status\": \"LED toggled\"}");
            }
            else {
                request->send(400, "application/json", "{\"error\": \"Invalid state value\"}");
            }
        }
        else {
            request->send(400, "application/json", "{\"error\": \"Missing state parameter\"}");
        }
    });

    // POST /api/threshold - Set threshold
    server.on("/api/threshold", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, (const char*)data);
        
        if(doc.containsKey("enabled") && doc.containsKey("sensor") && 
           doc.containsKey("value") && doc.containsKey("condition")) {
            threshold.enabled = doc["enabled"].as<bool>();
            threshold.sensor = doc["sensor"].as<String>();
            threshold.value = doc["value"].as<float>();
            threshold.condition = doc["condition"].as<String>();
            
            String response;
            serializeJson(doc, response);
            request->send(200, "application/json", response);
        }
        else {
            request->send(400, "application/json", "{\"error\": \"Missing required parameters\"}");
        }
    });
    // POST /api/display/temp-message - Display a message based on temperature
    server.on("/api/display/temp-message", HTTP_POST, [](AsyncWebServerRequest *request) {
        float temperature = tempSensor.read();
        String message;

        if (temperature < 10) {
            message = "Il fait froid !";
        } else if (temperature >= 10 && temperature <= 25) {
            message = "Il fait beau !";
        } else {
            message = "Il fait chaud !";
        }

        display.clear();
        display.showText(message);

        DynamicJsonDocument response(256);
        response["temperature"] = temperature;
        response["message"] = message;

        String jsonResponse;
        serializeJson(response, jsonResponse);
        request->send(200, "application/json", jsonResponse);
    });


    // POST /api/display - Display text on TTGO
    server.on("/api/display", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
            DynamicJsonDocument doc(256);
            deserializeJson(doc, (const char*)data);
            
            if (doc.containsKey("text")) {
                String text = doc["text"].as<String>();
                
                // Affiche le texte sur le TTGO
                display.clear();
                display.showText(text);
                
                request->send(200, "application/json", "{\"status\": \"Text displayed\"}");
            } else {
                request->send(400, "application/json", "{\"error\": \"Missing text parameter\"}");
            }
        });
}

void checkThreshold() {
    if(!threshold.enabled) return;
    
    float value;
    if(threshold.sensor == "temperature") {
        value = tempSensor.read();
    }
    else if(threshold.sensor == "light") {
        value = lightSensor.read();
    }
    else return;
    
    bool shouldTurnOn = (threshold.condition == "above") ? 
                       (value > threshold.value) : 
                       (value < threshold.value);
    
    digitalWrite(LED_PIN, shouldTurnOn);
}

void loop() {
    static unsigned long lastUpdate = 0;
    const unsigned long UPDATE_INTERVAL = 1000; // Update every second
    
    unsigned long currentTime = millis();
    
    if(currentTime - lastUpdate >= UPDATE_INTERVAL) {
        float temp = tempSensor.read();
        float light = lightSensor.read();
        
        display.showSensorValues(temp, light);
        checkThreshold();
        
        lastUpdate = currentTime;
    }
    
    // Check buttons
    if(digitalRead(BUTTON1_PIN) == LOW) {
        threshold.enabled = !threshold.enabled;
        display.showStatus(threshold.enabled ? "Threshold ON" : "Threshold OFF");
        delay(200); // Debounce
    }
    
    if(digitalRead(BUTTON2_PIN) == LOW) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(200); // Debounce
    }
}
