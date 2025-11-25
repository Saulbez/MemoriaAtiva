#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int order[200]; // Aumentado para suporte ao modo Livre
int i = 0;

// --- LEDs ---
int blue = 13;
int yellow = 12;
int red = 11;
int green = 10;

// --- Botões ---
int bluebtn = 6;
int yellowbtn = 5;
int redbtn = 4;
int greenbtn = 3;

int piezo = 7;

// Dificuldade selecionada
int difficulty = 1;

// Timing base
int timing = 700;

// Regras de dificuldade
int max_length = 50;  // limitado, exceto modo livre
int reduce_step = 50; // quanto diminui por nível
bool free_mode = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  pinMode(bluebtn, INPUT_PULLUP);
  pinMode(yellowbtn, INPUT_PULLUP);
  pinMode(redbtn, INPUT_PULLUP);
  pinMode(greenbtn, INPUT_PULLUP);

  pinMode(blue, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);

  pinMode(piezo, OUTPUT);

  randomSeed(analogRead(0));


  lcd.init();
  lcd.backlight();
  lcd.clear();

  show_menu();
  select_difficulty();
  lcd.clear();
  lcd.print("Memoria Ativa!");
  delay(2000);
  lcd.clear();
}

void show_menu() {
  lcd.clear();
  lcd.print("Selecione:");
  lcd.setCursor(0, 1);
  lcd.print("1F 2M 3D 4Livre");
}

void select_difficulty() {
  while (true) {
    if (digitalRead(bluebtn) == LOW) { // Fácil
      difficulty = 1;
      reduce_step = 30;
      max_length = 20;
      free_mode = false;
      break;
    }
    if (digitalRead(yellowbtn) == LOW) { // Médio
      difficulty = 2;
      reduce_step = 50;
      max_length = 30;
      free_mode = false;
      break;
    }
    if (digitalRead(redbtn) == LOW) { // Difícil
      difficulty = 3;
      reduce_step = 80;
      max_length = 50;
      free_mode = false;
      break;
    }
    if (digitalRead(greenbtn) == LOW) { // Livre
      difficulty = 4;
      free_mode = true;
      reduce_step = 0; // timing não muda
      max_length = 200; 
      break;
    }
  }

  lcd.clear();
  lcd.print("Dificuldade:");
  lcd.setCursor(0,1);

  if (difficulty == 1) lcd.print("Facil");
  else if (difficulty == 2) lcd.print("Medio");
  else if (difficulty == 3) lcd.print("Dificil");
  else lcd.print("Livre");

  delay(1500);
}

void generate_order() {
  int next_color = random(1, 5);
  order[i] = next_color;
  i++;

  if (!free_mode && timing > 200) {
    timing -= reduce_step;
    if (timing < 200) timing = 200;
  }
}

void run_order() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Nivel: ");
  lcd.print(i);

  delay(600);

  for (int x = 0; x < i; x++) {
    light_led(order[x]);
    delay(timing);
  }
}

void light_led(int color) {
  int pin;
  switch (color) {
    case 1: pin = blue; break;
    case 2: pin = yellow; break;
    case 3: pin = green; break;
    case 4: pin = red; break;
  }

  lcd.clear();
  lcd.print("Repita!");
  
  digitalWrite(pin, HIGH);
  tone(piezo, 440 + (color * 100), timing);
  delay(timing);
  digitalWrite(pin, LOW);
}

int read_button() {
  if (digitalRead(bluebtn) == LOW) return 1;
  if (digitalRead(yellowbtn) == LOW) return 2;
  if (digitalRead(greenbtn) == LOW) return 3;
  if (digitalRead(redbtn) == LOW) return 4;
  return 0;
}

bool read_player_sequence() {
  for (int x = 0; x < i; x++) {
    int pressed = 0;

    while (pressed == 0) pressed = read_button();

    light_led(pressed);

    if (pressed != order[x]) {
      lcd.clear();
      lcd.print("Errou!");
      tone(piezo, 200, 800);

      // Pisca todos LEDs
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      digitalWrite(11, HIGH);
      digitalWrite(10, HIGH);
      delay(800);
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
      digitalWrite(11, LOW);
      digitalWrite(10, LOW);
      delay(800);

      return false;
    }

    while (read_button() != 0);
    delay(200);
  }

  lcd.clear();
  lcd.print("Correto!");
  delay(1000);

  return true;
}

void loop() {

  // Para modos limitados
  if (!free_mode && i >= max_length) {
    lcd.clear();
    lcd.print("Voce Venceu!");
    delay(3000);
    i = 0;
    timing = 700;
    show_menu();
    select_difficulty();
  }

  generate_order();
  run_order();

  if (!read_player_sequence()) {
    i = 0;
    timing = 700;

    lcd.clear();
    lcd.print("Reiniciando...");
    delay(2000);

    show_menu();
    select_difficulty();
  }
}
