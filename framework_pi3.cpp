#include <DHT.h>
#include <LiquidCrystal.h>

// ==========================================
// DEFINIÇÕES DE HARDWARE
// ==========================================
// Configuração do DHT22
#define DHTPIN 8        // D8 pro medidor DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Configuração do Display LCD 16x2 (Ligação Paralela)
// Pinos: RS, E, D4, D5, D6, D7
// Tabela: RS=D12, E=D11, D4=D5, D5=D4, D6=D3, D7=D2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Pinos das Ventoinhas (Via transistores BC547)
#define FAN1_PIN 9      // D9 controla a ventoinha 1
#define FAN2_PIN 10     // D10 controla a ventoinha 2

// ==========================================
// PARÂMETROS DE CONTROLE TÉRMICO
// ==========================================
#define TEMP_LIGAR 40.0
#define TEMP_DESLIGAR 35.0

// O DHT22 é um sensor lento. Requer pelo menos 2000ms entre leituras.
#define INTERVALO_LEITURA 2000 

// ==========================================
// VARIÁVEIS GLOBAIS
// ==========================================
unsigned long ultimaLeitura = 0;
bool fansAtivas = false;

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  
  // Configuração dos pinos das ventoinhas
  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);
  desligarFans(); // Garante inicialização com tudo desligado

  // Inicializa Sensor
  dht.begin();
  
  // Inicializa Display LCD (16 colunas, 2 linhas)
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Sistema Iniciado");
  lcd.setCursor(0, 1);
  lcd.print("Aguarde...      ");
  
  delay(2000); // Tempo para o DHT22 estabilizar
  lcd.clear();
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = tempoAtual;

    // Leitura do DHT22
    float temp = dht.readTemperature();
    float umidade = dht.readHumidity(); // Lendo umidade por log

    // Tratamento de erro nível 2: Leitura inválida
    if (isnan(temp) || isnan(umidade)) {
      Serial.println("ERRO: Falha na leitura do DHT22.");
      
      lcd.setCursor(0, 0);
      lcd.print("Erro no Sensor! ");
      lcd.setCursor(0, 1);
      lcd.print("Fans ATIVADAS   ");
      
      ligarFans(); // Aciona por segurança se o sensor falhar
      return; 
    }

    // Log na Serial
    Serial.print("Temp do Ar: ");
    Serial.print(temp);
    Serial.println(" C");

    
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp, 1); 
    lcd.print(" C    ");

    // Lógica de Histerese e atualização da linha 2 do LCD
    lcd.setCursor(0, 1);
    if (temp >= TEMP_LIGAR) {
      if (!fansAtivas) ligarFans();
      lcd.print("Status: LIGADO  ");
      
    } else if (temp <= TEMP_DESLIGAR) {
      if (!fansAtivas) {
        lcd.print("Status: DESLIG. ");
      } else {
        desligarFans();
        lcd.print("Status: DESLIG. ");
      }
      
    } else {
      // Zona morta da histerese (entre 35 e 40): mantém o estado anterior
      if (fansAtivas) {
        lcd.print("Status: LIGADO  ");
      } else {
        lcd.print("Status: DESLIG. ");
      }
    }
  }
}

// ==========================================
// FUNÇÕES AUXILIARES
// ==========================================
void ligarFans() {
  digitalWrite(FAN1_PIN, HIGH);
  digitalWrite(FAN2_PIN, HIGH);
  fansAtivas = true;
}

void desligarFans() {
  digitalWrite(FAN1_PIN, LOW);
  digitalWrite(FAN2_PIN, LOW);
  fansAtivas = false;
}