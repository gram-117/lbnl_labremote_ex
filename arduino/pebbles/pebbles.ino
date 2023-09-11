
#include <string.h>

const uint8_t LED = 13;

// LMK03806 pins
const uint8_t LMK_SDO = 2;
const uint8_t LMK_SCL = 3;
const uint8_t LMK_LATCH = 4;
const uint8_t LMK_SDI = 5;

// pebbles pins
const uint8_t P_SOUT2 = 22;
const uint8_t P_SOUT1 = 24;
const uint8_t P_RST = 26;
const uint8_t P_SCLK = 28;
const uint8_t P_CFGCLK = 30;
const uint8_t P_MODE = 32;
const uint8_t P_PRIME = 34;
const uint8_t P_LOOPNK_EN = 36;
const uint8_t P_CFGIN = 38;


String inputString = "";         // A string to hold incoming data
boolean stringComplete = false;  // Whether the string is complete

const int MAX_PARTS = 5;
const int MAX_STRING_SIZE = 20;
char inputString_split[MAX_PARTS][MAX_STRING_SIZE];


void setup() {
  Serial.begin(9600);           // Starts the serial communication
  inputString.reserve(200);     // Reserve 200 bytes for the inputString

  pinMode(LED, OUTPUT);
  pinMode(LMK_SDO, INPUT);
  pinMode(LMK_SCL, OUTPUT);
  pinMode(LMK_LATCH, OUTPUT);
  pinMode(LMK_SDI, OUTPUT);

  pinMode(P_SOUT2, INPUT);
  pinMode(P_SOUT1, INPUT);
  pinMode(P_RST, OUTPUT);
  pinMode(P_SCLK, OUTPUT);
  pinMode(P_CFGCLK, OUTPUT);
  pinMode(P_MODE, OUTPUT);
  pinMode(P_PRIME, OUTPUT);
  pinMode(P_LOOPNK_EN, OUTPUT);
  pinMode(P_CFGIN, OUTPUT);

}

void loop() {
  if (stringComplete) {
    splitInputString();
    
    if (inputString.indexOf("blink") != -1) {
      blink();
      Serial.println("OK!");
    }

    if (inputString.indexOf("LMKWrite") != -1) {
      LMKWrite(strtoul(inputString_split[1], NULL, 10));
      Serial.println("OK!");
    }

    if (inputString.indexOf("LMKRead") != -1) {
      LMKRead(strtoul(inputString_split[1], NULL, 10));
    }

    if (inputString.indexOf("WriteConfig") != -1) {
      WriteConfig(strtoul(inputString_split[1], NULL, 10));
      Serial.println("OK!");
    }

    if (inputString.indexOf("WriteGPIO") != -1) {
       int pin_write = -1;
       if (strcmp(inputString_split[1], "RST") == 0) pin_write = P_RST;
       if (strcmp(inputString_split[1], "MODE") == 0) pin_write = P_MODE;
       if (strcmp(inputString_split[1], "PRIME") == 0) pin_write = P_PRIME;
       if (strcmp(inputString_split[1], "LOOPNK_EN") == 0) pin_write = P_LOOPNK_EN;

       if (pin_write > -1) {
            if (inputString_split[2][0] == '1') {
                digitalWrite(pin_write,  HIGH);
            }
            else if (inputString_split[2][0] == '0') {
                digitalWrite(pin_write,  LOW);
            }
            else {
                digitalWrite(pin_write,  LOW);
                digitalWrite(pin_write,  HIGH);
                digitalWrite(pin_write,  LOW);
            }
       }
       Serial.println("Write pin "+String(pin_write)+"("+inputString_split[1]+") to "+inputString_split[2]+" OK!");
    }

    if (inputString.indexOf("ReadSOUT") != -1) {
      ReadSOUT();
    }

    // Clear the string for new input:
    inputString = "";
    stringComplete = false;
  }
}

void ReadSOUT() {

  // in total 133 bits, use 5 of 32 bit integers
  uint32_t value1[5] = {0};
  uint32_t value2[5] = {0};
  for(int iv=0; iv<5; iv++){
    for (int i = 0; i < 32; i++) {
        value1[iv] <<= 1;
        value2[iv] <<= 1;
        if(iv == 4 && i > 5) continue;
        digitalWrite(P_SCLK, HIGH);
        if (digitalRead(P_SOUT1) == HIGH) value1[iv] |= 1 ;
        digitalWrite(P_SCLK, LOW);
        if (digitalRead(P_SOUT2) == HIGH) value2[iv] |= 1 ;
    }
    //delay(100);
  }

  String outString1 = String(value1[0], HEX);
  String outString2 = String(value2[0], HEX);
  for(int iv=1; iv<5; iv++){
    outString1 = outString1 + String(",") + String(value1[iv], HEX);
    outString2 = outString2 + String(",") + String(value2[iv], HEX);
  }
   
  Serial.println(outString1+";"+outString2);
}

void WriteConfig(uint32_t value) {
  // write value to pin P_CFGIN, with clock on pin P_CFGCLK
  digitalWrite(P_CFGCLK, LOW);
  digitalWrite(P_CFGIN, LOW);

  for (int i = 31; i >= 6; i--) {  // Start from the most significant bit, only send 25 MSBs
    // Set data pin based on current bit value
    digitalWrite(P_CFGIN, (value & (1UL << i)) ? HIGH : LOW);
    
    // Pulse the clock pin
    digitalWrite(P_CFGCLK, HIGH);
    digitalWrite(P_CFGCLK, LOW);
  }

  // CLK and IN back to low
  digitalWrite(P_CFGCLK, LOW);
  digitalWrite(P_CFGIN, LOW);
}

void LMKRead(uint32_t regis) {
    uint32_t value = 0; // This will store the read value
    uint32_t R31 = (0 << 21) | (regis << 16) | (1 << 5) | 31; 
    LMKWrite(R31);
    for (int i = 0; i < 32; i++) {
        digitalWrite(LMK_SCL, HIGH); // Set the clock pin high

        digitalWrite(LMK_SCL, LOW); // Set the clock pin low

        value <<= 1; // Shift all bits left by one place
        if (digitalRead(LMK_SDO) == HIGH && i < 27) {
            value |= 1; // If the data pin is high, set the least significant bit of 'value'
        }
    }
    uint32_t value2 = value | regis;
    Serial.println(value2);
}

void LMKWrite(uint32_t value) {
  // write value to pin LMK_SDI, with clock on pin LMK_SCL
  digitalWrite(LMK_SCL, LOW);
  digitalWrite(LMK_SDI, LOW);
  digitalWrite(LMK_LATCH, LOW);
  delay(1);

  for (int i = 31; i >= 0; i--) {  // Start from the most significant bit
    // Set data pin based on current bit value
    digitalWrite(LMK_SDI, (value & (1UL << i)) ? HIGH : LOW);
    
    // Pulse the clock pin
    digitalWrite(LMK_SCL, HIGH);
    digitalWrite(LMK_SCL, LOW);
  }

  // SCL and SDI back to low
  digitalWrite(LMK_SCL, LOW);
  digitalWrite(LMK_SDI, LOW);
  // toggle LATCH high once
  digitalWrite(LMK_LATCH, HIGH);
  digitalWrite(LMK_LATCH, LOW);
}

void blink() {
  digitalWrite(LED, HIGH);
  delay(10);
  digitalWrite(LED, LOW);
  delay(10);
}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == '>' || inChar == '\n') {              
      stringComplete = true;
    }
    else if (inChar == '<') {
      stringComplete = false;
      inputString = "";
    }
    else {
      inputString += inChar;
    }
  }
}

void splitInputString() {
    memset(inputString_split, 0, sizeof(inputString_split));
    
    int prevIndex = 0;
    int partIndex = 0;

    while (partIndex < MAX_PARTS) {
        int separatorIndex = inputString.indexOf(":", prevIndex);
        String part = "";
        
        if (separatorIndex == -1) part = inputString.substring(prevIndex);
        else part = inputString.substring(prevIndex, separatorIndex);
        
        if(part.length() > MAX_STRING_SIZE - 1) part = part.substring(0, MAX_STRING_SIZE - 1);  // Truncate if needed
        
        part.toCharArray(inputString_split[partIndex], part.length() + 1);

        if (separatorIndex == -1) break;
        
        prevIndex = separatorIndex + 1; 
        partIndex++;
    }
}



