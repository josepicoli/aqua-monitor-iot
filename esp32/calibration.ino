// Código de calibração para sensor de pH com ESP32

const int phPin = 34; // Pino analógico conectado ao sensor de pH
float phValue = 0;
float calibrationValue = 0; // Valor de calibração ajustado
int bufferSolutions[] = {4, 7, 10}; // Valores de pH das soluções de calibração
float voltageValues[3]; // Armazena as leituras para cada solução

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Configura resolução de 12 bits (0-4095)
  Serial.println("Iniciando calibração do sensor de pH");
  Serial.println("Siga as instruções:");
}

void loop() {
  Serial.println("\n=== Menu de Calibração ===");
  Serial.println("1. Calibrar com solução pH 4.0");
  Serial.println("2. Calibrar com solução pH 7.0");
  Serial.println("3. Calibrar com solução pH 10.0");
  Serial.println("4. Mostrar valores calibrados");
  Serial.println("5. Salvar calibração e sair");
  Serial.print("Escolha uma opção: ");
  
  while (!Serial.available()); // Aguarda entrada do usuário
  int option = Serial.read() - '0'; // Converte char para int
  
  switch(option) {
    case 1:
    case 2:
    case 3:
      calibrateWithSolution(option - 1); // Índice do array (0, 1 ou 2)
      break;
    case 4:
      showCalibrationValues();
      break;
    case 5:
      saveCalibration();
      Serial.println("Calibração concluída. Reinicie com o código de leitura.");
      while(1); // Loop infinito para encerrar
    default:
      Serial.println("Opção inválida!");
  }
}

void calibrateWithSolution(int solutionIndex) {
  Serial.print("\nColoque o sensor na solução pH ");
  Serial.print(bufferSolutions[solutionIndex]);
  Serial.println(" e aguarde...");
  delay(3000); // Tempo para estabilizar
  
  float sum = 0;
  for(int i = 0; i < 10; i++) { // Faz 10 leituras para média
    sum += analogRead(phPin);
    delay(100);
  }
  voltageValues[solutionIndex] = sum / 10;
  
  Serial.print("Valor médio lido para pH ");
  Serial.print(bufferSolutions[solutionIndex]);
  Serial.print(": ");
  Serial.println(voltageValues[solutionIndex]);
}

void showCalibrationValues() {
  Serial.println("\nValores de calibração atuais:");
  for(int i = 0; i < 3; i++) {
    Serial.print("pH ");
    Serial.print(bufferSolutions[i]);
    Serial.print(": ");
    Serial.println(voltageValues[i]);
  }
}

void saveCalibration() {
  // Aqui você pode salvar os valores na EEPROM ou apenas usá-los na memória
  Serial.println("\nValores de calibração salvos:");
  showCalibrationValues();
  // Nota: Para salvar permanentemente, use EEPROM.write()
}