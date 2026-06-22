#include <DHT.h>             // Biblioteca do sensor DHT22
#include <LiquidCrystal.h>   // Biblioteca do LCD 1602A

// ======================================================
// CONFIGURAÇÃO DO SENSOR DHT22
// ======================================================

// Pino DATA do DHT22 ligado ao pino D8 do Arduino
#define DHTPIN 8

// Define o modelo do sensor
#define DHTTYPE DHT22

// Cria o objeto do sensor
DHT dht(DHTPIN, DHTTYPE);

// ======================================================
// CONFIGURAÇÃO DO LCD 1602A
// ======================================================

// Ligação:
// RS -> D12
// E  -> D11
// D4 -> D5
// D5 -> D4
// D6 -> D3
// D7 -> D2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// ======================================================
// CONFIGURAÇÃO DAS VENTOINHAS
// ======================================================

// Ventoinha principal
#define FAN1_PIN 9

// Ventoinha secundária (opcional)
#define FAN2_PIN 10

// ======================================================
// PARÂMETROS DE CONTROLE TÉRMICO
// ======================================================

// Temperatura para ligar a ventilação
#define TEMP_LIGAR 40.0

// Temperatura para desligar a ventilação
#define TEMP_DESLIGAR 35.0

// Intervalo entre leituras do DHT22
// O DHT22 não deve ser lido muito rápido
#define INTERVALO_LEITURA 2000

// Variável que guarda o estado atual das ventoinhas
bool fansAtivas = false;

// ======================================================
// FUNÇÃO: LIGAR VENTOINHAS
// ======================================================

void ligarFans() {

  // Envia nível lógico alto para os transistores
  digitalWrite(FAN1_PIN, HIGH);
  digitalWrite(FAN2_PIN, HIGH);

  // Atualiza variável de controle
  fansAtivas = true;
}

// ======================================================
// FUNÇÃO: DESLIGAR VENTOINHAS
// ======================================================

void desligarFans() {

  // Envia nível lógico baixo para os transistores
  digitalWrite(FAN1_PIN, LOW);
  digitalWrite(FAN2_PIN, LOW);

  // Atualiza variável de controle
  fansAtivas = false;
}

// ======================================================
// SETUP
// Executa apenas uma vez ao ligar o sistema
// ======================================================

void setup() {

  // Inicializa comunicação serial
  Serial.begin(9600);

  // Configura os pinos das ventoinhas como saída
  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);

  // Garante que o sistema inicia com as ventoinhas desligadas
  desligarFans();

  // Inicializa o sensor DHT22
  dht.begin();

  // Inicializa o LCD 16 colunas x 2 linhas
  lcd.begin(16, 2);

  // Limpa o display
  lcd.clear();

  // Mensagem inicial
  lcd.setCursor(0, 0);
  lcd.print("Sistema iniciado");

  lcd.setCursor(0, 1);
  lcd.print("Aguarde...");

  // Tempo para estabilização do DHT22
  delay(2000);

  lcd.clear();
}

// ======================================================
// LOOP PRINCIPAL
// Executa continuamente
// ======================================================

void loop() {

  // Lê a temperatura atual
  float temp = dht.readTemperature();

  // --------------------------------------------------
  // Verifica erro de leitura
  // --------------------------------------------------

  if (isnan(temp)) {

    Serial.println("ERRO: Falha na leitura do DHT22");

    lcd.setCursor(0, 0);
    lcd.print("Erro DHT22      ");

    lcd.setCursor(0, 1);
    lcd.print("Verifique sensor");

    // Em caso de erro, desliga as ventoinhas
    desligarFans();

    delay(INTERVALO_LEITURA);
    return;
  }

  // --------------------------------------------------
  // Exibe temperatura na serial
  // --------------------------------------------------

  Serial.print("Temperatura: ");
  Serial.print(temp, 1);
  Serial.println(" C");

  // --------------------------------------------------
  // Atualiza primeira linha do LCD
  // --------------------------------------------------

  lcd.setCursor(0, 0);

  lcd.print("Temp: ");
  lcd.print(temp, 1);

  lcd.print((char)223);   // Símbolo °
  lcd.print("C      ");

  // --------------------------------------------------
  // Controle térmico com histerese
  // --------------------------------------------------

  lcd.setCursor(0, 1);

  // Liga as ventoinhas acima da temperatura limite
  if (temp >= TEMP_LIGAR) {

    ligarFans();

    lcd.print("Fan: LIGADA    ");
  }

  // Desliga abaixo da temperatura mínima
  else if (temp <= TEMP_DESLIGAR) {

    desligarFans();

    lcd.print("Fan: DESLIGADA ");
  }

  // Região intermediária:
  // mantém o estado anterior para evitar ficar
  // ligando/desligando rapidamente
  else {

    if (fansAtivas) {

      lcd.print("Fan: LIGADA    ");

    } else {

      lcd.print("Fan: DESLIGADA ");
    }
  }

  // Aguarda antes da próxima leitura
  delay(INTERVALO_LEITURA);
}
