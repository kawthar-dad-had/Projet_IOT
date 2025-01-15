#include "display.h"

Display::Display() : tft() {
}

void Display::init() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.println("Sensor Monitor");
}

void Display::clear() {
    tft.fillScreen(TFT_BLACK); // Efface tout l'écran
}

void Display::showSensorValues(float temp, float light) {
    tft.fillRect(0, 30, 240, 60, TFT_BLACK);
    tft.setCursor(0, 30);
    tft.setTextColor(TFT_GREEN);
    tft.printf("Temp: %.1fC\n", temp);
    tft.printf("Light: %.1f%%", light);
}

void Display::showStatus(const String& status) {
    tft.fillRect(0, 100, 240, 20, TFT_BLACK);
    tft.setCursor(0, 100);
    tft.setTextColor(TFT_BLUE);
    tft.println(status);
}

void Display::showError(const String& error) {
    tft.fillRect(0, 100, 240, 20, TFT_BLACK);
    tft.setCursor(0, 100);
    tft.setTextColor(TFT_RED);
    tft.println(error);
}

void Display::showText(const String &text) {
    clear(); // Efface l'écran
    tft.setCursor(0, 0); // Définit la position initiale
    tft.setTextColor(TFT_WHITE); // Définit la couleur du texte
    tft.setTextSize(2); // Définit la taille du texte
    tft.println(text); // Affiche le texte
}

