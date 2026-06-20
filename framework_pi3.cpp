#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


// Configuração do DHT22
#define DHTPIN 7        // Pino digital conectado ao Data do DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Configuração do Display LCD I2C (Endereço comum 0x27)
// No Arduino Leonardo: SDA é o pino 2, SCL é o pino 3
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Pinos das Ventoinhas (Via transistores BC547)
#define FAN1_PIN 4
#define FAN2_PIN 5

// ==========================================
// PARÂMETROS DE CONTROLE TÉRMICO
// ==========================================

#define TEMP_LIGAR 40.0
#define TEMP_DESLIGAR 35.0


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
  desligarFans(); 

  // Inicializa Sensor
  dht.begin();
  
  // Inicializa Display LCD
  lcd.init();
  lcd.backlight();
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
    float umidade = dht.readHumidity(); // Lendo umidade só por log

    // Tratamento de erro
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

    // Atualiza linha 1 do LCD com a temperatura
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp, 1); // 1 casa decimal
    lcd.print(" C    "); // Espaços para limpar sujeira da tela

    // Lógica de Histerese e atualização da linha 2 do LCD
    lcd.setCursor(0, 1);
    if (temp >= TEMP_LIGAR) {
      if (!fansAtivas) ligarFans();
      lcd.print("Status: LIGADO  ");
      
    } else if (temp <= TEMP_DESLIGAR) {
      if (fansAtivas) desligarFans();
      lcd.print("Status: DESLIG. ");
      
    } else {
      
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