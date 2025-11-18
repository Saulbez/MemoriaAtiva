#include <LiquidCrystal.h>

int order[15];
int i = 0;
int timing = 700;

int blue = 13;
int yellow = 12;
int red = 11;
int green = 10;

int bluebtn = 6;
int yellowbtn = 5;
int redbtn = 4;
int greenbtn = 3;

int piezo = 7;

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, A0, A1, A2, A3);

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
  
  lcd.begin(16, 2);
  lcd.print("Memoria Ativa!");
  delay(2000);
  lcd.clear();
}

void generate_order() {
  int next_color = random(1, 5);
  order[i] = next_color;
  i++;
  if (i > 3) timing -= 100;
  else if (i > 6) timing -= 100;
  else if (i > 9) timing -= 100;
  else if (i > 12) timing -= 100;
}

void run_order() {
  lcd.clear();
  lcd.print("Repita o padrao");
  lcd.setCursor(0,1);
  lcd.print("Nivel: ");
  lcd.print(i);
  delay(1000);
  
  for (int x = 0; x < i; x++) {
    light_led(order[x]);
    delay(timing);
  }
}

void light_led(int color) {
  int pin;
  const char* color_name;
  switch (color) {
    case 1: pin = blue; color_name = "Azul"; break;
    case 2: pin = yellow; color_name = "Amarelo"; break;
    case 4: pin = red; color_name = "Verde"; break;
    case 3: pin = green; color_name = "Vermelho"; break;
  }

  lcd.clear();
  lcd.print("Cor: ");
  lcd.print(color_name);
  
  digitalWrite(pin, HIGH);
  tone(piezo, 440 + (color * 100), timing);
  delay(timing);
  digitalWrite(pin, LOW);
}

int read_button() {
  if (digitalRead(bluebtn) == LOW) return 1;
  if (digitalRead(yellowbtn) == LOW) return 2;
  if (digitalRead(redbtn) == LOW) return 3;
  if (digitalRead(greenbtn) == LOW) return 4;
  return 0;
}

bool read_player_sequence() {
  for (int x = 0; x < i; x++) {
    int pressed = 0;
    
    while (pressed == 0) {
      pressed = read_button();
    }

    light_led(pressed);

    if (pressed != order[x]) {
      lcd.clear();
      lcd.print("Errou! Reiniciando");
      tone(piezo, 200, 1000);
      digitalWrite(13, HIGH);
      digitalWrite(12, HIGH);
      digitalWrite(11, HIGH);
      digitalWrite(10, HIGH);
      
      delay(1000);
      
      digitalWrite(13, LOW);
      digitalWrite(12, LOW);
      digitalWrite(11, LOW);
      digitalWrite(10, LOW);
      
      delay(1000);
      return false;
    }

    while (read_button() != 0);
    delay(200);
  }

  lcd.clear();
  lcd.print("Correto!");
  lcd.setCursor(0,1);
  lcd.print("Prox nivel...");
  delay(1000);

  return true;
}

void loop() {
  generate_order();
  run_order();

  if (read_player_sequence()) {
    delay(timing);
  } else {
    i = 0;
    timing = 700; // reinicia a velocidade
    lcd.clear();
    lcd.print("Jogo reiniciado!");
    delay(2000);
  }
}
