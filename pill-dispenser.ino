#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProSwitch.h>
#include <ESP32Servo.h>

/* =========================
   WIFI
========================= */

#define WIFI_SSID "POCO X4 Pro"
#define WIFI_PASS "joshua02"

/* =========================
   SINRIC PRO
========================= */

#define APP_KEY "880650b6-9f2b-4dda-a897-9aec18120a23"
#define APP_SECRET "0e58a4d3-ea0c-4f73-8a2a-ec2ed4db7529-75dea8a8-0277-48c8-9cfb-91e4f005929e"

/* MAIN DEVICE */
#define DISPENSER_ID "6a016eacf9b5f15fa7d23e0f"

/* REMINDER DEVICE */
#define REMINDER_ID "6a0185d8f9b5f15fa7d24e0d"

/* =========================
   PINS
========================= */

#define SERVO_PIN 18

#define RED_LED 25
#define GREEN_LED 33
#define BLUE_LED 27

Servo dispenserServo;

/* =========================
   DISPENSE FUNCTION
========================= */

void dispenseMedicine() {

  Serial.println("Dispensing medicine...");

  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  dispenserServo.write(90);

  delay(1500);

  dispenserServo.write(0);

  delay(500);

  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  Serial.println("Medicine dispensed successfully");
}

/* =========================
   EMERGENCY STOP
========================= */

void emergencyStop() {

  Serial.println("Emergency stop activated");

  dispenserServo.write(0);

  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

/* =========================
   REMINDER MODE
========================= */

void reminderMode() {

  Serial.println("Medicine reminder mode activated");

  for (int i = 0; i < 10; i++) {

    digitalWrite(GREEN_LED, HIGH);
    delay(500);

    digitalWrite(GREEN_LED, LOW);
    delay(500);
  }

  digitalWrite(GREEN_LED, HIGH);

  Serial.println("Reminder mode finished");
}

/* =========================
   MAIN DISPENSER COMMANDS
========================= */

bool onDispenserPowerState(const String &deviceId, bool &state) {

  Serial.println("==============");

  if (state == true) {

    Serial.println("DISPENSE COMMAND RECEIVED");

    dispenseMedicine();

  } else {

    Serial.println("EMERGENCY STOP COMMAND RECEIVED");

    emergencyStop();
  }

  return true;
}

/* =========================
   REMINDER COMMANDS
========================= */

bool onReminderPowerState(const String &deviceId, bool &state) {

  Serial.println("==============");

  if (state == true) {

    reminderMode();

  } else {

    digitalWrite(GREEN_LED, LOW);

    Serial.println("Reminder mode OFF");
  }

  return true;
}

/* =========================
   SETUP
========================= */

void setup() {

  Serial.begin(115200);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);

  dispenserServo.attach(SERVO_PIN);

  dispenserServo.write(0);

  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(RED_LED, HIGH);
    delay(300);

    digitalWrite(RED_LED, LOW);
    delay(300);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(GREEN_LED, HIGH);

  /* MAIN DISPENSER DEVICE */

  SinricProSwitch &dispenserSwitch = SinricPro[DISPENSER_ID];

  dispenserSwitch.onPowerState(onDispenserPowerState);

  /* REMINDER DEVICE */

  SinricProSwitch &reminderSwitch = SinricPro[REMINDER_ID];

  reminderSwitch.onPowerState(onReminderPowerState);

  /* START SINRIC */

  SinricPro.begin(APP_KEY, APP_SECRET);

  Serial.println("SinricPro Connected");
}

/* =========================
   LOOP
========================= */

void loop() {

  SinricPro.handle();

  if (WiFi.status() != WL_CONNECTED) {

    digitalWrite(RED_LED, HIGH);
  }
}