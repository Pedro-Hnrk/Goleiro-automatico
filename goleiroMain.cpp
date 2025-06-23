#include <ESP32Servo.h>
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
int valorRecebido = 0;  // 0 = não definido, 1 = Fácil, 2 = Médio, 3 = Difícil

// Definição dos sensores por quadrante
#define TRIG_Q1 25
#define ECHO_Q1 26
#define TRIG_Q2 5
#define ECHO_Q2 18
#define TRIG_Q3 34
#define ECHO_Q3 35
#define TRIG_Q4 22
#define ECHO_Q4 23
#define TRIG_Q5 32
#define ECHO_Q5 33
#define TRIG_Q6 2
#define ECHO_Q6 4

#define SERVO_PIN 14    // Pino PWM válido para o servo

Servo goleiro;
const int distanciaDefesa = 10; // cm

// Pré-configuração dos ângulos de defesa
const int angulosDefesa[6] = {150, 30, 120, 60, 180, 0};  // Posições pré-mapeadas

// Sistema de dificuldade
unsigned long tempoReacao = 100;  // Padrão: médio (100ms)
const unsigned long temposDificuldade[3] = {200, 50, 0}; // [Fácil, Médio, Difícil]

void setup() {
  // Configura os pinos dos sensores como entrada e saída
  int trigPins[] = {TRIG_Q1, TRIG_Q2, TRIG_Q3, TRIG_Q4, TRIG_Q5, TRIG_Q6};
  int echoPins[] = {ECHO_Q1, ECHO_Q2, ECHO_Q3, ECHO_Q4, ECHO_Q5, ECHO_Q6};

  for (int i = 0; i < 6; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  // Configuração otimizada do servo
  goleiro.setPeriodHertz(50);
  goleiro.attach(SERVO_PIN, 500, 2400);
  goleiro.write(90);  // Posição inicial

  Serial.begin(115200);
  SerialBT.begin("Goleiro"); // Nome do dispositivo Bluetooth
}

long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Timeout reduzido para 15ms (50% de redução)
  long duracao = pulseIn(echoPin, HIGH, 15000);
  if (duracao == 0) return -1;
  return duracao * 0.034 / 2;
}

void defenderQuadrante(int quadrante) {
  // Acesso direto ao array pré-configurado
  
  delay(tempoReacao);
  goleiro.write(angulosDefesa[quadrante - 1]);
  
  // Usa o tempo de reação configurado pela dificuldade
  delay(1000);
  
  goleiro.write(90);
  delay(3000);
}

void atualizarDificuldade() {
  if (SerialBT.available()) {
    char cmd = SerialBT.read();
    Serial.print("Comando BT: ");
    Serial.println(cmd);
    
    // Mapeamento dos comandos:
    // '1' = Fácil
    // '2' = Médio
    // '3' = Difícil
    if (cmd >= '1' && cmd <= '3') {
      int nivel = cmd - '1';  // Converte para índice 0-based
      tempoReacao = temposDificuldade[nivel];
      
      Serial.print("Dificuldade alterada para: ");
      Serial.println(cmd == '1' ? "Fácil" : cmd == '2' ? "Médio" : "Difícil");
    }
  }
}

void loop() {
  // Verifica atualizações de dificuldade via Bluetooth
  atualizarDificuldade();

  int trigPins[] = {TRIG_Q1, TRIG_Q2, TRIG_Q3, TRIG_Q4, TRIG_Q5, TRIG_Q6};
  int echoPins[] = {ECHO_Q1, ECHO_Q2, ECHO_Q3, ECHO_Q4, ECHO_Q5, ECHO_Q6};

  for (int i = 0; i < 6; i++) {
    long distancia = medirDistancia(trigPins[i], echoPins[i]);
    Serial.print("Q"); Serial.print(i + 1); Serial.print(": "); Serial.println(distancia);

    if (distancia > 0 && distancia <= distanciaDefesa) {
      defenderQuadrante(i + 1);
    }
  }
  
  // Delay reduzido para 20ms
  delay(20);
}
