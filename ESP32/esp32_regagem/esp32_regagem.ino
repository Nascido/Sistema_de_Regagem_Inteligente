#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Definições de pinos ---
#define PINO_LDR      33
#define PINO_UMIDADE  34
#define PINO_RELE     23

// --- Parâmetros do sistema ---
int umidade_limiar = 2000;

// --- Instâncias globais ---
WebServer server(80);

// --- Configuração WiFi ---
const char* ssid = "esp";
const char* password = "12345678";

// --- Backend ---
const char* backend_url = "https://webhook.site/db6d13dc-0429-42ec-b8e9-e30164394b87"; // Altere para seu endpoint

// --- Controle de tempo ---
unsigned long intervaloLeitura = 1000; // ms
unsigned long ultimaLeitura = 0;

// --- LCD I2C ---
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço 0x27, 16 colunas, 2 linhas

// --- Controle da bomba ---
bool bombaLigada = false;
unsigned long tempoInicioBomba = 0;
unsigned long tempoUltimoCiclo = 0;
const unsigned long TEMPO_LIGADA = 2000;   // 2 segundos
const unsigned long TEMPO_ESPERA = 15000;  // 15 segundos

// --- Flag para ativação manual imediata ---
volatile bool ativarBombaImediato = false;

// --- Funções auxiliares ---

void handleSetUmidade() {
  if (server.hasArg("valor")) {
    String valorStr = server.arg("valor");
    int novoLimiar = valorStr.toInt();

    // Validação: valor numérico positivo e dentro do range do ADC
    if (novoLimiar > 0 && novoLimiar < 4096) {
      umidade_limiar = novoLimiar;
      String msg = "Novo limiar de umidade definido para: " + String(umidade_limiar);
      server.send(200, "text/plain", msg);
      Serial.print("[HTTP] Limiar de umidade alterado para: ");
      Serial.println(umidade_limiar);
    } else {
      server.send(422, "text/plain", "Valor inválido para limiar. Use um inteiro entre 1 e 4095.");
    }
  } else {
    server.send(400, "text/plain", "Parametro 'valor' ausente.");
  }
}

void handleGetStatus() {
  int valorUmidade = analogRead(PINO_UMIDADE);
  int valorLDR = analogRead(PINO_LDR);
  bool releAtivo = digitalRead(PINO_RELE);

  String json = "{";
  json += "\"umidade\":" + String(valorUmidade) + ",";
  json += "\"ldr\":" + String(valorLDR) + ",";
  json += "\"rele\":" + String(releAtivo) + ",";
  json += "\"umidade_limiar\":" + String(umidade_limiar);
  json += "}";

  server.send(200, "application/json", json);
}

// --- NOVA ROTA: /restart ---
void handleRestart() {
  server.send(200, "text/plain", "Reiniciando ESP32...");
  Serial.println("[HTTP] Comando de reinicialização recebido via /restart.");
  delay(100);
  ESP.restart();
}

// --- NOVA ROTA: /ativar_bomba ---
void handleAtivarBomba() {
  if (!bombaLigada && !ativarBombaImediato) {
    ativarBombaImediato = true;
    server.send(200, "text/plain", "Bomba será ativada imediatamente por 2 segundos.");
    Serial.println("[HTTP] Comando de ativação imediata recebido via /ativar_bomba.");
  } else {
    server.send(409, "text/plain", "Bomba já está ligada ou aguardando ciclo.");
  }
}

void enviarDadosBackend(int umidade, int ldr) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(backend_url);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"umidade\":" + String(umidade) + ",\"ldr\":" + String(ldr) + ",\"evento\":\"bomba_ativada\"}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Resposta backend: ");
      Serial.println(response);
    } else {
      Serial.print("Erro ao enviar dados. Código HTTP: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi não conectado. Dados não enviados.");
  }
}

// --- Lógica de controle da bomba ---
void controlarBomba(int valorUmidade, int valorLDR) {
  unsigned long agora = millis();

  // Prioridade: ativação imediata via requisição
  if (ativarBombaImediato && !bombaLigada) {
    digitalWrite(PINO_RELE, HIGH);
    bombaLigada = true;
    tempoInicioBomba = agora;
    ativarBombaImediato = false; // Limpa flag
    Serial.println("[BOMBA] Ativada IMEDIATAMENTE por 2s via HTTP.");
    enviarDadosBackend(valorUmidade, valorLDR); // Notifica backend
    return;
  }

  if (bombaLigada) {
    // Se a bomba está ligada, verifica se já passou o tempo de 2 segundos
    if (agora - tempoInicioBomba >= TEMPO_LIGADA) {
      digitalWrite(PINO_RELE, LOW);
      bombaLigada = false;
      tempoUltimoCiclo = agora;
      Serial.println("[BOMBA] Desligada após 2s.");
    }
  } else {
    // Se a bomba está desligada, verifica se já passou o tempo de espera
    if (agora - tempoUltimoCiclo >= TEMPO_ESPERA) {
      if (valorUmidade > umidade_limiar) {
        digitalWrite(PINO_RELE, HIGH);
        bombaLigada = true;
        tempoInicioBomba = agora;
        Serial.println("[BOMBA] Ligada por 2s (umidade acima do limiar).");
        enviarDadosBackend(valorUmidade, valorLDR); // Notifica backend
      }
    }
  }
}

void setupWiFi() {
  Serial.print("Conectando ao WiFi: ");
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tentativas++;
    Serial.print(" [Status WiFi: ");
    Serial.print(WiFi.status());
    Serial.print("]");
    if (tentativas % 10 == 0) {
      Serial.println(" [Aguardando conexão...]");
    } else {
      Serial.print(" ");
    }
    if (tentativas > 40) {
      Serial.println("\n\n\nErro: Não foi possível conectar ao WiFi.");
      Serial.print("Motivo da desconexão (WiFi.status):");
      Serial.println(WiFi.status());
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n\n\n[WiFi conectado com sucesso!]");
    Serial.print("Endereço IP adquirido: ");
    Serial.println(WiFi.localIP());
  }
}

void setupHTTPServer() {
  server.on("/set_umidade", handleSetUmidade);
  server.on("/status", handleGetStatus);
  server.on("/restart", handleRestart);
  server.on("/ativar_bomba", handleAtivarBomba); // NOVA ROTA
  server.begin();
  Serial.println("Servidor HTTP iniciado na porta 80.");
}

// --- Função para exibir dados no LCD ---
void atualizarLCD(const String& ip, int umidade, int ldr) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.print(ip);

  lcd.setCursor(0, 1);
  lcd.print("U:");
  lcd.print(umidade);
  lcd.print(" L:");
  lcd.print(ldr);
}

// --- Função setup ---
void setup() {
  Serial.begin(115200);
  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  // Inicializa o LCD
  Wire.begin(21, 22); // SDA = 21, SCL = 22
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  setupWiFi();
  setupHTTPServer();

  // Exibe o IP após conexão WiFi
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("U:---- L:----");
    delay(2000); // Breve exibição inicial
  }
}

// --- Função loop ---
void loop() {
  server.handleClient();

  unsigned long agora = millis();
  if (agora - ultimaLeitura >= intervaloLeitura) {
    ultimaLeitura = agora;

    int valorUmidade = analogRead(PINO_UMIDADE);
    int valorLDR = analogRead(PINO_LDR);

    controlarBomba(valorUmidade, valorLDR);

    Serial.print("Umidade: ");
    Serial.print(valorUmidade);
    Serial.print("\tLDR: ");
    Serial.println(valorLDR);

    // Atualiza o display LCD com IP, Umidade e LDR
    String ipStr = WiFi.localIP().toString();
    atualizarLCD(ipStr, valorUmidade, valorLDR);
  }
}
