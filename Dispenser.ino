#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SCK (13)
#define PN532_MOSI (11)
#define PN532_SS (10)
#define PN532_MISO (12)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a SPI connection:
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);



//Variabel
String nfcID = "";
String cardUser = "3397572630";

// Pins for Ultrasonic Sensors

//SRF 1-3 untuk check kondisi botol atau gelas
//SRF 4-6 untuk check kondisi isi air

#define TRIGGER_PIN_1 2
#define ECHO_PIN_1 3
#define TRIGGER_PIN_2 4
#define ECHO_PIN_2 5
#define TRIGGER_PIN_3 6
#define ECHO_PIN_3 7
#define TRIGGER_PIN_4 16
#define ECHO_PIN_4 9
#define TRIGGER_PIN_5 10
#define ECHO_PIN_5 11
#define TRIGGER_PIN_6 12
#define ECHO_PIN_6 13
#define pinBuzzer 8

// Pins for Relay
#define RELAY_PIN_1 A0  //Relay pin untuk panas
#define RELAY_PIN_2 A1  //Relay pin untuk normal
#define RELAY_PIN_3 A2  //Relay pin untuk dingin


// Pins for LED
#define LED1 A3  //led pin untuk panas
#define LED2 A4  //led pinr untuk normal
#define LED3 A5  //led pin untuk dingin


// Threshold distance for each sensor
// threshold 1-3 untuk jarak kondisi gelas
// threshold 4-6 untuk jarak kondisi air

#define THRESHOLD1 200
#define THRESHOLD2 200
#define THRESHOLD3 200
#define THRESHOLD4 200
#define THRESHOLD5 200
#define THRESHOLD6 200
#define isiAir 200

int distance1 = 0;
int distance2 = 0;
int distance3 = 0;
int distance4 = 0;
int distance5 = 0;
int distance6 = 0;
int waktuPengisian = 0;

String mode = "";

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  waktuPengisian = isiAir * 35;
  // Set trigger pins as output
  while (!Serial) delay(10);  // for Leonardo/Micro/Zero

  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1)
      ;  // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  Serial.println("Waiting for an ISO14443A Card ...");
  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(TRIGGER_PIN_3, OUTPUT);
  pinMode(TRIGGER_PIN_4, OUTPUT);
  pinMode(TRIGGER_PIN_5, OUTPUT);
  pinMode(TRIGGER_PIN_6, OUTPUT);

  // Set echo pins as input
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(ECHO_PIN_3, INPUT);
  pinMode(ECHO_PIN_4, INPUT);
  pinMode(ECHO_PIN_5, INPUT);
  pinMode(ECHO_PIN_6, INPUT);

  // Set relay pins as output
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(RELAY_PIN_3, OUTPUT);
  digitalWrite(pinBuzzer, HIGH);
  //set warna LED
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  nfc.begin();
  nfc.SAMConfig();

  delay(20);
}
void loop() {
  
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_1, LOW);
  getNfcIdUser();
  Serial.println(nfcID);
  delay(100);
  if (nfcID == cardUser) {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    Serial.println("oke");
    algoritmaPN532();
  }
  else
  {
    for (int x = 0; x < 5; x++)
    {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, HIGH);
      digitalWrite(pinBuzzer, LOW);
      delay(200);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(pinBuzzer, HIGH);
      delay(200);
    }
  }
}
void algoritmaPN532() {
  digitalWrite(RELAY_PIN_1, HIGH);
  delay(waktuPengisian);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_1, LOW);
  delay(100);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_1, LOW);
  Serial.println("SELESAI");
  delay(100);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_1, LOW);
}
void algoritmaDispenserOne() {
  if (Serial.available() > 0) {  //open communication
    String dataIn = Serial.readString();
    if (dataIn == "o" || dataIn == "O" || dataIn == "o\n" || dataIn == "O\n") {  //waiting for treshold
      digitalWrite(RELAY_PIN_1, HIGH);
      delay(waktuPengisian);
      digitalWrite(RELAY_PIN_1, LOW);
      Serial.println("SELESAI");
      delay(20);
      // break;  //end session
    } else if (dataIn == "b" || dataIn == "B" || dataIn == "b\n" || dataIn == "B\n") {
      digitalWrite(RELAY_PIN_1, HIGH);
      delay(waktuPengisian);
      digitalWrite(RELAY_PIN_1, LOW);
      Serial.println("SELESAI");
      delay(20);
      // break;
    }
  }
}
void getNfcIdUser()  //check NFC ID
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    // Serial.println("Found an ISO14443A card");
    // Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    // Serial.print("  UID Value: ");
    // nfc.PrintHex(uid, uidLength);


    if (uidLength == 4) {
      // We probably have a Mifare Classic card ...
      uint32_t cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];
      cardid <<= 8;
      cardid |= uid[3];
      // Serial.print("Seems to be a Mifare Classic card #");
      // Serial.println(cardid);
      nfcID = cardid;
      digitalWrite(pinBuzzer, LOW);
      delay(200);
      digitalWrite(pinBuzzer, HIGH);
      delay(20);
    }
    // Serial.println("");
  }
}
void algoritmaDispenser() {
  //check distance gelas
  distance1 = measureDistance(TRIGGER_PIN_1, ECHO_PIN_1);
  distance2 = measureDistance(TRIGGER_PIN_2, ECHO_PIN_2);
  distance3 = measureDistance(TRIGGER_PIN_3, ECHO_PIN_3);

  //check distance air
  distance4 = measureDistance(TRIGGER_PIN_4, ECHO_PIN_4);
  distance5 = measureDistance(TRIGGER_PIN_5, ECHO_PIN_5);
  distance6 = measureDistance(TRIGGER_PIN_6, ECHO_PIN_6);

  if (distance1 < THRESHOLD1) {  //proses air panas
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    mode = "panas";
    Serial.println("PANAS");
    if (Serial.available() > 0) {  //open communication
      String dataIn = Serial.readString();
      if (dataIn == "o" || dataIn == "O" || dataIn == "o\n" || dataIn == "O\n") {
        while (measureDistance(TRIGGER_PIN_4, ECHO_PIN_4) > THRESHOLD4) {  //waiting for treshold
          digitalWrite(RELAY_PIN_1, HIGH);
          if (measureDistance(TRIGGER_PIN_4, ECHO_PIN_4) < THRESHOLD4) {
            digitalWrite(RELAY_PIN_1, LOW);
            Serial.println("SELESAI");
            delay(20);
            break;  //end session
          }
        }
      }
    }
  } else if (distance2 < THRESHOLD2) {  //proses air normal
    mode = "normal";
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    Serial.println("NORMAL");
    if (Serial.available() > 0) {  //open communication
      String dataIn = Serial.readString();
      if (dataIn == "o" || dataIn == "O" || dataIn == "o\n" || dataIn == "O\n") {  //open communication
        while (measureDistance(TRIGGER_PIN_5, ECHO_PIN_5) > THRESHOLD5) {          //waiting for treshold
          digitalWrite(RELAY_PIN_2, HIGH);
          if (measureDistance(TRIGGER_PIN_5, ECHO_PIN_5) < THRESHOLD5) {
            digitalWrite(RELAY_PIN_2, LOW);
            Serial.println("SELESAI");
            delay(500);
            break;  //end session
          }
        }
      }
    }
  } else if (distance3 < THRESHOLD3) {  // proses air dingib
    mode = "dingin";
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    Serial.println("DINGIN");
    if (Serial.available() > 0) {  //open communication
      String dataIn = Serial.readString();
      if (dataIn == "o" || dataIn == "O" || dataIn == "o\n" || dataIn == "O\n") {
        while (measureDistance(TRIGGER_PIN_6, ECHO_PIN_6) > THRESHOLD6) {  //waiting for treshold
          digitalWrite(RELAY_PIN_3, HIGH);
          if (measureDistance(TRIGGER_PIN_6, ECHO_PIN_6) < THRESHOLD6) {
            digitalWrite(RELAY_PIN_3, LOW);
            Serial.println("SELESAI");
            delay(500);
            break;  //end session
          }
        }
      }
    }
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED1, LOW);
  }
}

// Function to measure distance using ultrasonic sensor
int measureDistance(int triggerPin, int echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(triggerPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration;

  return distance;
}

// Function to control relays based on sensor readings
void controlRelays(int d1, int d2, int d3, int d4, int d5, int d6) {
  if (d1 < THRESHOLD1) {
    digitalWrite(RELAY_PIN_1, HIGH);
  } else {
    digitalWrite(RELAY_PIN_1, LOW);
  }
  if (d2 < THRESHOLD2) {
    digitalWrite(RELAY_PIN_2, HIGH);
  } else {
    digitalWrite(RELAY_PIN_2, LOW);
  }

  if (d3 < THRESHOLD3) {
    digitalWrite(RELAY_PIN_3, HIGH);
  } else {
    digitalWrite(RELAY_PIN_3, LOW);
  }
}
