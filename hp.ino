#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSOR_PH 34  // Pino analógico do sensor de pH
#define ONE_WIRE_BUS 4 // Pino do DS18B20

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";
const char* serverURL = "http://seu-servidor.com/dados"; // API ou Firebase

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("WiFi Conectado!");

    sensors.begin(); 
}

void loop() {
    // Leitura do sensor de pH
    int leituraPH = analogRead(SENSOR_PH);
    float ph = (leituraPH / 4095.0) * 14.0; // Ajuste conforme calibração

    // Leitura do sensor de temperatura
    sensors.requestTemperatures();
    float temperatura = sensors.getTempCByIndex(0);

    Serial.print("pH: ");
    Serial.println(ph);
    Serial.print("Temperatura: ");
    Serial.println(temperatura);

    // Enviar dados para API
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverURL);
        http.addHeader("Content-Type", "application/json");

        String payload = "{\"ph\": " + String(ph) + ", \"temperatura\": " + String(temperatura) + "}";
        int httpResponseCode = http.POST(payload);
        http.end();
    }

    delay(5000); // Coleta a cada 5 segundos
}