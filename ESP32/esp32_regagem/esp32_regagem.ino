// Definições dos pinos
#define PINO_LDR      33    // LDR no GPIO33 (ADC1_CH5)
#define PINO_UMIDADE  34    // Umidade no GPIO34 (ADC1_CH6)
#define PINO_RELE     23    // Relé no GPIO23

// Limiar para acionar a bomba (ajuste conforme necessário)
#define UMIDADE_LIMIAR 2000

void setup() {
  Serial.begin(115200);

  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW); // Inicialmente desligado

  // Não é necessário configurar pinos analógicos no Arduino
}

void loop() {
  int valorUmidade = analogRead(PINO_UMIDADE);
  int valorLDR = analogRead(PINO_LDR);

  // Lógica de acionamento do relé
  if (valorUmidade > UMIDADE_LIMIAR) {
    digitalWrite(PINO_RELE, HIGH); // Liga bomba
  } else {
    digitalWrite(PINO_RELE, LOW);  // Desliga bomba
  }

  // Envio dos valores para o Serial Monitor
  Serial.print("Umidade: ");
  Serial.print(valorUmidade);
  Serial.print("\tLDR: ");
  Serial.println(valorLDR);

  delay(2000); // Aguarda 2 segundos
}
