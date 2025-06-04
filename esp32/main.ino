#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const float ph4Value = 1500; // Valor lido para pH 4.0
const float ph7Value = 1300; // Valor lido para pH 7.0
const float ph10Value = 1100; // Valor lido para pH 10.0

const int LED = 2;
const int led_red = 32;
const int led_green = 25;
const int sensor_ph = 34;
const int PINO_ONEWIRE = 13;
OneWire oneWire(PINO_ONEWIRE);
DallasTemperature sensor_temp(&oneWire);
DeviceAddress endereco_temp;

const char* ssid = ""; // nome da sua rede
const char* password = "";  // senha do Wi-Fi
HTTPClient http; // Criação do objeto HTTPClient

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    pinMode(led_red, OUTPUT);
    pinMode(led_green, OUTPUT);
	oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    sensor_temp.begin();
    pinMode(sensor_ph, INPUT);

    // Conectar ao Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi conectado!");
}

void show(float temp, float ph) {
    oled.clearDisplay();
    
    oled.drawRect(0, 0, 128, 64, WHITE);
    oled.setTextSize(2);
    oled.setTextWrap(false);

    oled.setTextColor(WHITE);
    oled.setCursor(11, 14);
    oled.println("TEMP:");
    
    oled.setTextColor(WHITE);
    oled.setCursor(71, 14);
    oled.println(temp, 1);

    oled.setTextColor(WHITE);
    oled.setCursor(35, 36);
    oled.println("PH:");
    
    oled.setTextColor(WHITE);
    oled.setCursor(71, 36);
    oled.println(ph, 1);
    
    oled.display();
}

float temp() {
    sensor_temp.requestTemperatures();
    if (!sensor_temp.getAddress(endereco_temp, 0)) { 
        Serial.println("SENSOR_temp NAO CONECTADO");
        return -127.0;
    }
    float temperatura = sensor_temp.getTempC(endereco_temp);
    Serial.print("Temperatura = ");
    Serial.println(temperatura, 1);
    return temperatura;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float ph() {
    int leitura = analogRead(sensor_ph);
    float ph = mapfloat(leitura, 1200, 2800, 2.5, 9.0);
    Serial.print("pH = ");
    Serial.println(ph, 1);
    return ph;
}

float _ph() {
    return (float)random(720, 750) / 100.00;
}

void sendToAPI(float temp, float ph, bool alert, String msg, String uri) {
    if (WiFi.status() == WL_CONNECTED) {
        String url = "http://" + uri + "/data";
        http.begin(url);
        http.addHeader("Content-Type", "application/json");

        // Cria JSON para enviar
        String json = "{\"ph\":" + String(ph, 1) + 
              ", \"temperatura\":" + String(temp, 1) + 
              ", \"aviso\":" + String(alert) + 
              ", \"msg\":\"" + msg + "\"}";
        // Envia POST
        int resposta = http.POST(json);
        Serial.print("Resposta da API: ");
        Serial.println(resposta);

        http.end();  // Finaliza a conexão
    } else {
        Serial.println("Wi-Fi desconectado!");
    }
}

void blink() {
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED, LOW);
    delay(300);
}

bool updateLedStatus(float temp, float ph) {
    if(temp >= 25 && temp <= 28 && ph >= 7 && ph <= 8.5) {
        digitalWrite(led_red, LOW);
        digitalWrite(led_green, HIGH);
        return true;
    } else {
        digitalWrite(led_green, LOW);
        digitalWrite(led_red, HIGH);
        return false;
    }
}

String generateMessage(float temp, float ph) {
    String msg1 = "";
    String msg2 = "";

    if (temp < 25) {
        msg1 = "A temperatura da agua esta em " + String(temp, 1) + "C. Aumente para manter entre 25C e 28C. ";
    }

    if (temp > 28) {
        msg1 = "A temperatura da agua esta em " + String(temp, 1) + "C. Diminua para manter entre 25C e 28C. ";
    }

    if (ph < 7) {
        msg2 = "O pH da agua esta em " + String(ph, 1) + ". Aumente para manter entre 7 e 8.5. ";
    }

    if (ph > 8.5) {
        msg2 = "O pH da agua esta em " + String(ph, 1) + ". Diminua para manter entre 7 e 8.5. ";
    }

    return msg1 + msg2;
}

void loop() {
    float phValue = _ph();
    float tempValue = temp();
    bool alertValue = updateLedStatus(tempValue, phValue);
    String msgValue = generateMessage(tempValue, phValue);

    show(tempValue, phValue);
    sendToAPI(tempValue, phValue, alertValue, msgValue, "127.0.0.1:5000");
    blink();
    delay(3000);
}

// OLED PINOS
// GND  ->  GND
// VDD  ->  3V3
// SCK  ->  D22
// SDA  ->  D21

// TEMP PINOS
// GND  ->  GND
// VCC  ->  3V3
// DATA ->  D13 + Resistor 4.7KΩ para VCC

// PH PINOS
// GND  ->  GND
// GND  ->  GND
// PO   ->  A34
// V++  ->  3V3

// LED VERDE
// CÁTODO  ->  GND + Resistor 150Ω
// ÂNODO   ->  D25

// LED VERMELHO
// CÁTODO  ->  GND + Resistor 150Ω
// ÂNODO   ->  D32