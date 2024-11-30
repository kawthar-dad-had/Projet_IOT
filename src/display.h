#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>
#include <SPI.h>

class Display {
private:
    TFT_eSPI tft;
    
public:
    Display();
    void init();
    void showSensorValues(float temp, float light);
    void showStatus(const String& status);
    void showError(const String& error);
};

#endif