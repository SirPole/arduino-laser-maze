#include <Servo.h>

// Functions
void setup();
void loop();
void setupServo(int);
void handleLightSense();
void handleReset(boolean);
void handleControl(int);
void loopServos(void (*function)(int));
void setPosition(int, int);
void resetPosition(int);
void addPosition(int);
void subPosition(int);

// Constants
const int SERVO_COUNT = 3;
const int DEFAULT_POSITION = 90;
const int SENSE_PIN = A7;
const int LED_PIN = 13;
const int SERVO_PINS[SERVO_COUNT] = {3, 5, 6};
const int CONTROL_PINS[SERVO_COUNT][2] = {{7, 8}, {9, 10}, {11, 12}};
const int CONTROL_LEFT = 0;
const int CONTROL_RIGHT = 1;
const int POSITION_DEFAULT = 0;
const int POSITION_MIN = 1;
const int POSITION_MAX = 2;
const int POSITION_CURRENT = 3;
const float DELAY = 10;
const float RESET_THRESHOLD = 60000;

// Variables
float resetCounter = 0;
Servo servos[SERVO_COUNT];
int positions[SERVO_COUNT][4] = {
  {90, 30, 150, 90},
  {90, 30, 150, 90},
  {90, 30, 150, 90}
};

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  loopServos(setupServo);
}

void loop() {
  loopServos(handleControl);
  handleLightSense();
  delay(DELAY);
}

void setupServo(int id) {
  servos[id].attach(SERVO_PINS[id]);
  servos[id].write(positions[id][POSITION_DEFAULT]);
  pinMode(CONTROL_PINS[id][CONTROL_LEFT], INPUT_PULLUP);
  pinMode(CONTROL_PINS[id][CONTROL_RIGHT], INPUT_PULLUP);
}

void handleLightSense() {
  int lightValue = analogRead(SENSE_PIN);
  Serial.println(lightValue);

  if (lightValue >= 100) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void handleReset(boolean isInput) {
  if (isInput) {
    resetCounter = 0;
  } else {
    resetCounter += DELAY / SERVO_COUNT;

    if (resetCounter >= RESET_THRESHOLD) {
      loopServos(resetPosition);
      resetCounter = 0;
    }
  }
}

void handleControl(int id) {
  bool isRight = digitalRead(CONTROL_PINS[id][CONTROL_LEFT]) == LOW;
  bool isLeft = digitalRead(CONTROL_PINS[id][CONTROL_RIGHT]) == LOW;

  handleReset(isRight || isLeft);

  if (isRight && positions[id][POSITION_CURRENT] > positions[id][POSITION_MIN]) {
    subPosition(id);
  }

  if (isLeft && positions[id][POSITION_CURRENT] < positions[id][POSITION_MAX]) {
    addPosition(id);
  }
}

void loopServos(void (*function)(int id)) {
  for (int id = 0; id < SERVO_COUNT; id++) {
    (*function)(id);
  }
}

void setPosition(int id, int newPos) {
  if (positions[id][POSITION_CURRENT] != newPos) {
    positions[id][POSITION_CURRENT] = newPos;
    servos[id].write(positions[id][POSITION_CURRENT]);
  }
}

void resetPosition(int id) {
  setPosition(id, positions[id][POSITION_DEFAULT]);
}

void addPosition(int id) {
  setPosition(id, positions[id][POSITION_CURRENT] + 1);
}

void subPosition(int id) {
  setPosition(id, positions[id][POSITION_CURRENT] - 1);
}
