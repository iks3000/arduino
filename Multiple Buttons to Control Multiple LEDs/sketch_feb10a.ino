#define BUTTON_PIN_1 2
#define BUTTON_PIN_2 3
#define BUTTON_PIN_3 4
#define BUTTON_PIN_4 5

#define LED_PIN_1 8
#define LED_PIN_2 9
#define LED_PIN_3 10
#define LED_PIN_4 11

void setup() {
  //Serial.begin(9600);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);
  
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
}

void loop() {
  byte buttonState1 = digitalRead(BUTTON_PIN_1);
  byte buttonState2 = digitalRead(BUTTON_PIN_2);
  byte buttonState3 = digitalRead(BUTTON_PIN_3);
  byte buttonState4 = digitalRead(BUTTON_PIN_4);
  
  if (buttonState1 == LOW) {
      //Serial.println("Button-1 is pressed");
      digitalWrite(LED_PIN_1, HIGH);
  } else {
      //Serial.println("Button-1 is not pressed");
      digitalWrite(LED_PIN_1, LOW);
  }

  if (buttonState2 == LOW) {
      //Serial.println("Button-2 is pressed");
      digitalWrite(LED_PIN_2, HIGH);
  } else {
      //Serial.println("Button-2 is not pressed");
      digitalWrite(LED_PIN_2, LOW);
  }

  if (buttonState3 == LOW) {
      //Serial.println("Button-3 is pressed");
      digitalWrite(LED_PIN_3, HIGH);
  } else {
      //Serial.println("Button-3 is not pressed");
      digitalWrite(LED_PIN_3, LOW);
  }

  if (buttonState4 == LOW) {
      //Serial.println("Button-4 is pressed");
      digitalWrite(LED_PIN_4, HIGH);
  } else {
      //Serial.println("Button-3 is not pressed");
      digitalWrite(LED_PIN_4, LOW);
  }
  
  delay(100);
}
