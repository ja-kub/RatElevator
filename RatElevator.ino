#define SENSOR_ARM_PIN 3
#define SENSOR_UP_PIN 4
#define SENSOR_DOWN_PIN 5
#define ENABLE_PIN 6
#define IN_1_PIN 7
#define IN_2_PIN 8
#define BRAKE_DOWN_PIN 9
#define BRAKE_UP_PIN 10
#define POT_PIN 0

const int TIME_TO_LEAVE_CAR_IN_SECONDS = 10;
const int MOVING_CAR_TIMEOUT_IN_SECONDS = 30;
const int INERTIA_TIME_IN_MILISECONDS = 50;
enum direction {up, down};
int speed = 0;

void setup() {
  pinMode(SENSOR_ARM_PIN, INPUT); // IR beam sensor signal
  pinMode(SENSOR_UP_PIN, INPUT); // IR beam sensor signal
  pinMode(SENSOR_DOWN_PIN, INPUT); // IR beam sensor signal
  digitalWrite(SENSOR_ARM_PIN, HIGH); // weird, but without this, it's always LOW on power from adapter (when USB is connected, it's OK)
  digitalWrite(SENSOR_UP_PIN, HIGH); // weird, but without this, it's always LOW on power from adapter (when USB is connected, it's OK)
  digitalWrite(SENSOR_DOWN_PIN, HIGH); // weird, but without this, it's always LOW on power from adapter (when USB is connected, it's OK)
  pinMode(ENABLE_PIN, OUTPUT); // PWM signal for motor
  pinMode(IN_1_PIN, OUTPUT); // Motor direction signal
  pinMode(IN_2_PIN, OUTPUT); // Motor direction signal
  pinMode(BRAKE_UP_PIN, INPUT_PULLUP); // Switch to stop car
  pinMode(BRAKE_DOWN_PIN, INPUT_PULLUP); // Switch to stop car
  pinMode(LED_BUILTIN, OUTPUT); // Built in LED
  Serial.begin(9600); // Setup transmission on COM port
  Serial.println("Welcome to Rat Elevator :)");
}

void loop() {
  speed = analogRead(POT_PIN) / 4;
  checkGoDown();
  checkGoUp();
}

void checkGoDown() {
  if (digitalRead(BRAKE_UP_PIN) == LOW // when car touches upper brake
  && (digitalRead(SENSOR_UP_PIN) == LOW || digitalRead(SENSOR_DOWN_PIN) == LOW)) { // and lower or upper IR beam is broken
    go(down);
  }
}

void checkGoUp() {
  if (digitalRead(BRAKE_DOWN_PIN) == LOW // when car touches lower brake
  && (digitalRead(SENSOR_ARM_PIN) == LOW || digitalRead(SENSOR_DOWN_PIN) == LOW)) { // and lower or upper IR beam is broken
    go(up);
  }
}

void go(direction dir) {
    int brake;
    int timeLeftInMs = MOVING_CAR_TIMEOUT_IN_SECONDS * 1000;
    int periodInMs = 100;

    digitalWrite(LED_BUILTIN, HIGH); // swith LED on
    if (dir == down) {
      brake = BRAKE_DOWN_PIN;
      setMotor(speed, true); // start motor to go down
      Serial.print("Elevator is going DOWN with speed: ");
    } else {
      brake = BRAKE_UP_PIN;
      setMotor(speed, false); // start motor to go up
      Serial.print("Elevator is going UP with speed: ");
    }
    Serial.println(speed);

    Serial.print("Seconds left to emergency timeout: ");
    while (digitalRead(brake) != LOW // until brake is not touched 
    && timeLeftInMs > 0) { // and timeout didn't occur
      if (timeLeftInMs%1000==0) { // display time only at full second
        Serial.print(timeLeftInMs/1000);
        Serial.print(", ");
      }
      delay(periodInMs); //  wait
      timeLeftInMs = timeLeftInMs - periodInMs; 
    }
    Serial.println("0.");

    if (digitalRead(brake) == LOW) { // when brake is touched
      delay(INERTIA_TIME_IN_MILISECONDS); // delay after brake touch to finish motor work
    }
    setMotor(0, 0); // stop motor
    
    timeLeftInMs = TIME_TO_LEAVE_CAR_IN_SECONDS * 1000;
    periodInMs = 1000;
    Serial.print("Seconds to leave the elevator: ");
    while (timeLeftInMs > 0) { // while timeout didn't occur
      if (timeLeftInMs%1000==0) { // display time only at full second
        Serial.print(timeLeftInMs/1000);
        Serial.print(", ");
      }
      delay(periodInMs); //  wait
      timeLeftInMs = timeLeftInMs - periodInMs; 
    }
    Serial.println("0.");

    Serial.println("Elevator ready to go.");
    digitalWrite(LED_BUILTIN, LOW); // switch LED off 
}

void setMotor(int speed, boolean reverse) {
  analogWrite(ENABLE_PIN, speed);
  digitalWrite(IN_1_PIN, !reverse);
  digitalWrite(IN_2_PIN, reverse);
}

