#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define I2C_SDA_PIN 8  
#define I2C_SCL_PIN 9
#define FAN1_PIN 2     
#define FAN2_PIN 3     

const double TEMP_LIGAR = 40.0;
const double TEMP_DESLIGAR = 35.0;
const unsigned long INTERVALO_LEITURA = 1000;

unsigned long ultimoTempoLeitura = 0;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(115200);
  
  pinMode(FAN1_PIN, OUTPUT);
  pinMode(FAN2_PIN, OUTPUT);
  desligarFans();

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  Serial.println("[SISTEMA] Iniciando verificação do MLX90614...");
  
  if (!mlx.begin()) {
    Serial.println("[ERRO CRÍTICO] MLX90614 nao detectado. Verifique a fiação (SDA/SCL) e a alimentação.");
    ligarFans(); 
    while (1); 
  }
  
  Serial.println("[SISTEMA] Sensor inicializado com sucesso.");
}

void loop() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - ultimoTempoLeitura >= INTERVALO_LEITURA) {
    ultimoTempoLeitura = tempoAtual;

    double tempObjeto = mlx.readObjectTempC();
    double tempAmbiente = mlx.readAmbientTempC();

    if (isnan(tempObjeto) || isnan(tempAmbiente)) {
      Serial.println("[ERRO] Falha na leitura térmica!");
      ligarFans(); 
      return;
    }

    Serial.print("Ambiente: "); 
    Serial.print(tempAmbiente); 
    Serial.print(" C | Objeto: "); 
    Serial.print(tempObjeto); 
    Serial.println(" C");

    if (tempObjeto >= TEMP_LIGAR) {
      ligarFans();
    } else if (tempObjeto <= TEMP_DESLIGAR) {
      desligarFans();
    }
  }
}

void ligarFans() {
  digitalWrite(FAN1_PIN, HIGH);
  digitalWrite(FAN2_PIN, HIGH);
}

void desligarFans() {
  digitalWrite(FAN1_PIN, LOW);
  digitalWrite(FAN2_PIN, LOW);
}