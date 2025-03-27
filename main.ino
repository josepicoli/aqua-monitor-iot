#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//  CORES OLED
//#define BLACK    0x0000
//#define BLUE     0x001F
//#define RED      0xF800
//#define GREEN    0x07E0
//#define CYAN     0x07FF
//#define MAGENTA  0xF81F
//#define YELLOW   0xFFE0 
//#define WHITE    0xFFFF

const int LED = 2;

//const int PINO_ONEWIRE = 12; // Define pino do sensor
//OneWire oneWire(PINO_ONEWIRE); // Cria um objeto OneWire
//DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire
//DeviceAddress endereco_temp; // Cria um endereco temporario da leitura do sensor

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
	oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    //sensor.begin(); ; // Inicia o sensor
}
        
void show(float temp, int ph) {
    oled.clearDisplay();
    
    oled.drawRect(0, 0, 128, 64, WHITE);

    oled.setTextSize(2);
    oled.setTextWrap(false);

    // LABEL TEMP
    oled.setTextColor(WHITE);
    oled.setCursor(11, 14);
    oled.println("TEMP:");
    
    // TEMP
    oled.setTextColor(WHITE);
    oled.setCursor(71, 14);
    oled.println(temp, 1);

    // LABEL PH
    oled.setTextColor(WHITE);
    oled.setCursor(35, 36);
    oled.println("PH:");
    
    // PH
    oled.setTextColor(WHITE);
    oled.setCursor(71, 36);
    oled.println(ph);
    
    oled.display();
}

//float temp() {
//    sensor.requestTemperatures(); // Envia comando para realizar a conversão de temperatura
//    if (!sensor.getAddress(endereco_temp,0)) { // Encontra o endereco do sensor no barramento
//        Serial.println("SENSOR NAO CONECTADO"); // Sensor conectado, imprime mensagem de erro
//    } else {
//        Serial.print("Temperatura = "); // Imprime a temperatura no monitor serial
//        Serial.println(sensor.getTempC(endereco_temp), 1); // Busca temperatura para dispositivo
//        return sensor.getTempC(endereco_temp)
//    }
//}

void blink() {
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED, LOW);
    delay(300);
}

void loop() {
    show(25.5, 14);
    blink();
}

// OLED PINOS
// GND  ->  GND
// VCC  ->  3V3
// SCL  ->  D22
// SDA  ->  D21

// TEMP PINOS
// GND  ->  GND
// VCC  ->  3V3
// DATA ->  D12
