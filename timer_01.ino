//**********PROGRAMAÇÃO PARA O TIMER**********//

//*****Globais*****

#define buzzer 10
#define botaodir 8  // Trocar modo
#define botaoesq 7  // Iniciar Pomodoro

//*****LCD*****

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//*****MILLIS*****

// Para teste: 10 segundos
// Normal: 15min, 30min, 60min

unsigned long temposEstudo[] = { 10 * 1000, 15 * 60 * 1000, 30 * 60 * 1000, 60 * 60 * 1000 };
unsigned long temposPausa[] = { 5 * 1000, 5 * 60 * 1000, 5 * 60 * 1000, 10 * 60 * 1000 };

byte modoAtual = 0;  // 0:0:10/0:05, 1:15/5, 2:30/5, 60/10

unsigned long millisAnterior = 0;
unsigned long duracaoAtual = 0;

bool pomodoroAtivo = false;

//*****Estados*****

enum State { FOCO,
             PARADA };
State estadoAtual = FOCO;

//*****Debounce Botões*****

unsigned long debounceDelay = 50;
unsigned long ultimoCliqueDir = 0;
unsigned long ultimoCliqueEsq = 0;

//--------------------------------

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(botaoesq, INPUT_PULLUP);
  pinMode(botaodir, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  mostrarModoAtual();
}

//--------------------------------

void loop() {
  
  unsigned long agora = millis();

  // ---------- Botao direito: Trocar modo ----------
  if (digitalRead(botaodir) == LOW && agora - ultimoCliqueDir > debounceDelay) {
    ultimoCliqueDir = agora;
    modoAtual = (modoAtual + 1) % 4;
    mostrarModoAtual();
  }

  // ---------- Botao esquerdo: Foco ----------
  if (digitalRead(botaoesq) == LOW && agora - ultimoCliqueEsq > debounceDelay) {
    ultimoCliqueEsq = agora;
    if (!pomodoroAtivo) {
      pomodoroAtivo = true;
      estadoAtual = FOCO;
      duracaoAtual = temposEstudo[modoAtual];
      millisAnterior = agora;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pomodoro time");
    }
  }

  // ---------- Timer do Pomodoro ----------
  if (pomodoroAtivo && agora - millisAnterior >= duracaoAtual) {
    millisAnterior = agora;

    if (estadoAtual == FOCO) {
      estadoAtual = PARADA;
      duracaoAtual = temposPausa[modoAtual];
      emitirSomPausa();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Break");
    } else {
      estadoAtual = FOCO;
      duracaoAtual = temposEstudo[modoAtual];
      emitirSomFoco();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pomodoro time");
    }
  }
}

//--------------------------------

void mostrarModoAtual() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selected mode:");
  lcd.setCursor(0, 1);
  switch (modoAtual) {
    case 0:
      lcd.print("Test 10s / 5s");
      delay(1500);
      // lcd.scrollDisplayLeft();
      break;
    case 1:
      lcd.print("15m Pomo / 5m");
      delay(1500);
      lcd.scrollDisplayLeft();
      break;
    case 2:
      lcd.print("30m Pomo / 5m");
      delay(1500);
      // lcd.scrollDisplayLeft();
      break;
    case 3:
      lcd.print("60m Pomo / 10m");
      delay(1500);
      // lcd.scrollDisplayLeft();
      break;
  }
}

//--------------------------------

// Sons não tão chatos

void emitirSomPausa() {
  tone(buzzer, 1000, 200);
  delay(250);
  tone(buzzer, 800, 200);
  delay(250);
  noTone(buzzer);
}

void emitirSomFoco() {
  tone(buzzer, 500, 100);
  delay(150);
  tone(buzzer, 600, 100);
  delay(150);
  tone(buzzer, 700, 100);
  delay(150);
  noTone(buzzer);

  }

