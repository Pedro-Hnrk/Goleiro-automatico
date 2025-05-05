// goleiro_robo_arduino_6_sensores.ino
#include <Servo.h>

// Definição dos sensores por quadrante
#define TRIG_Q1 2
#define ECHO_Q1 3
#define TRIG_Q2 4
#define ECHO_Q2 5
#define TRIG_Q3 6
#define ECHO_Q3 7
#define TRIG_Q4 8
#define ECHO_Q4 9
#define TRIG_Q5 10
#define ECHO_Q5 11
#define TRIG_Q6 12
#define ECHO_Q6 13

#define SERVO_PIN A0
#define LED_PIN A1
#define BUZZER_PIN A2

Servo goleiro;
const int distanciaDefesa = 25; // cm

void setup() {
  // Configura os pinos dos sensores como entrada e saída
  int trigPins[] = {TRIG_Q1, TRIG_Q2, TRIG_Q3, TRIG_Q4, TRIG_Q5, TRIG_Q6};
  int echoPins[] = {ECHO_Q1, ECHO_Q2, ECHO_Q3, ECHO_Q4, ECHO_Q5, ECHO_Q6};

  for (int i = 0; i < 6; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  goleiro.attach(SERVO_PIN);
  goleiro.write(90);

  Serial.begin(9600);
}

long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duracao = pulseIn(echoPin, HIGH);
  return duracao * 0.034 / 2;
}

void tocarSomDefesaLamento() {
  // Contato da bola (grave e rápido)
  tone(BUZZER_PIN, 200, 100); // Boom!
  delay(300);

  // Torcida lamentando (tons descendentes)
  int lamento[] = { 784, 659, 523, 392 }; // G5, E5, C5, G4
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, lamento[i], 180);
    delay(200);
  }

  noTone(BUZZER_PIN);
}

void tocarAplausos8bit() {
  for (int i = 0; i < 20; i++) {
    int freq = random(300, 1200);   // Frequência aleatória entre 300 e 1200 Hz
    int duracao = random(20, 60);   // Pulso curto
    tone(BUZZER_PIN, freq, duracao);
    delay(duracao + 5);             // Pequeno intervalo
  }
  noTone(BUZZER_PIN);
}

void tocarImpactoEAplausos() {
  // Beep de impacto (bola tocando o goleiro)
  tone(BUZZER_PIN, 200, 120); // Grave
  delay(150);

  // Aplausos simulados (estilo 8-bits)
  for (int i = 0; i < 20; i++) {
    int freq = random(300, 1200);   // Frequência entre 300 e 1200 Hz
    int duracao = random(20, 60);   // Pulso curto
    tone(BUZZER_PIN, freq, duracao);
    delay(duracao + 10);            // Pequena pausa
  }

  noTone(BUZZER_PIN);
}

void defenderQuadrante(int quadrante) {
  digitalWrite(LED_PIN, HIGH);
  tocarImpactoEAplausos();;

  int angulo = 90;
  switch (quadrante) {
    case 1: angulo = 150; break;   // Alta esquerda
    case 2: angulo = 30; break;  // Alta direita
    case 3: angulo = 120; break;   // Média esquerda
    case 4: angulo = 60; break;  // Média direita
    case 5: angulo = 180; break;    // Baixa esquerda
    case 6: angulo = 0; break;  // Baixa direita
  }

  goleiro.write(angulo);
  delay(300);
  goleiro.write(90);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}


void loop() {
  int trigPins[] = {TRIG_Q1, TRIG_Q2, TRIG_Q3, TRIG_Q4, TRIG_Q5, TRIG_Q6};
  int echoPins[] = {ECHO_Q1, ECHO_Q2, ECHO_Q3, ECHO_Q4, ECHO_Q5, ECHO_Q6};

  for (int i = 0; i < 6; i++) {
    long distancia = medirDistancia(trigPins[i], echoPins[i]);
    Serial.print("Q"); Serial.print(i + 1); Serial.print(": "); Serial.println(distancia);

    if (distancia > 0 && distancia <= distanciaDefesa) {
      defenderQuadrante(i + 1);
    }
  }

  delay(100);
}
