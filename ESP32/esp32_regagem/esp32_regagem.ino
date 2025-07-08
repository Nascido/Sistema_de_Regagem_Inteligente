#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

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
  // Retorna o status atual dos sensores e do relé em JSON
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
  delay(100); // Pequeno delay para garantir envio da resposta
  ESP.restart(); // Comando seguro para reinicialização do ESP32
}
// -----------------------------

void enviarDadosBackend(int umidade, int ldr) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(backend_url);
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"umidade\":" + String(umidade) + ",\"ldr\":" + String(ldr) + "}";
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

void controlarRele(int valorUmidade) {
  if (valorUmidade > umidade_limiar) {
    digitalWrite(PINO_RELE, HIGH);
  } else {
    digitalWrite(PINO_RELE, LOW);
  }
}

void setupWiFi() {
  Serial.print("Conectando ao WiFi: ");
  WiFi.disconnect(true); // Limpa credenciais antigas
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
  server.on("/restart", handleRestart); // <-- Adicionada nova rota
  server.begin();
  Serial.println("Servidor HTTP iniciado na porta 80.");
}

// --- Função setup ---
void setup() {
  Serial.begin(115200);
  pinMode(PINO_RELE, OUTPUT);
  digitalWrite(PINO_RELE, LOW);
  setupWiFi();
  setupHTTPServer();
}

// --- Função loop ---
void loop() {
  server.handleClient();

  unsigned long agora = millis();
  if (agora - ultimaLeitura >= intervaloLeitura) {
    ultimaLeitura = agora;

    int valorUmidade = analogRead(PINO_UMIDADE);
    int valorLDR = analogRead(PINO_LDR);

    controlarRele(valorUmidade);

    Serial.print("Umidade: ");
    Serial.print(valorUmidade);
    Serial.print("\tLDR: ");
    Serial.println(valorLDR);

    // Descomente para enviar ao backend
    // enviarDadosBackend(valorUmidade, valorLDR);
  }
}
