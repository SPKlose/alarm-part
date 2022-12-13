//sender phone number with country code
const String PHONE = "+4599999999";

//GSM Module RX pin to ESP32 Pin 2
//GSM Module TX pin to ESP32 Pin 4
#define rxPin 4
#define txPin 2

#define BAUD_RATE 9600
HardwareSerial sim800(1);
#define PIN_TO_CONTROL 19
#define RELAY_1 25
#define RELAY_2 26
#define RELAY_3 27
#define RELAY_4 33
//
String smsStatus, senderNumber, receivedDate, msg;
boolean isReply = false;

void setup() {
  pinMode(RELAY_1, OUTPUT);  //fyr
  pinMode(RELAY_2, OUTPUT);  //lys
  pinMode(RELAY_3, OUTPUT);  //pumpe
  pinMode(RELAY_4, OUTPUT);  //ekstra
  pinMode(PIN_TO_CONTROL, OUTPUT);
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
  delay(10000);  // 10 sekunder ekstra tid for modem

  Serial.begin(9600);
  Serial.println("esp32 serial initialize");

  sim800.begin(BAUD_RATE, SERIAL_8N1, rxPin, txPin);
  Serial.println("SIM800L serial initialize");

  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";

  sim800.print("AT+CMGF=1\r");  //SMS text mode
  delay(1000);
}

void loop() {
  //////////////////////////////////////////////////
  while (sim800.available()) {
    parseData(sim800.readString());
  }
  //////////////////////////////////////////////////
  while (Serial.available()) {
    sim800.println(Serial.readString());
  }
  String sms = Serial.readString();

  

/////////////////////////////////////////////////
}  //main loop ends

//***************************************************
void parseData(String buff) {
  Serial.println(buff);

  unsigned int len, index;
  //////////////////////////////////////////////////
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();
  //////////////////////////////////////////////////

  //////////////////////////////////////////////////
  if (buff != "OK") {
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);

    if (cmd == "+CMTI") {
      index = buff.indexOf(",");
      String temp = buff.substring(index + 1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      sim800.println(temp);
    } else if (cmd == "+CMGR") {
      extractSms(buff);


      if (senderNumber == PHONE) {
        doAction();
      }
    }
    //////////////////////////////////////////////////
  } else {
    //The result of AT Command is "OK"
  }
}

//************************************************************
void extractSms(String buff) {
  unsigned int index;

  index = buff.indexOf(",");
  smsStatus = buff.substring(1, index - 1);
  buff.remove(0, index + 2);

  senderNumber = buff.substring(0, 11);
  buff.remove(0, 19);

  receivedDate = buff.substring(0, 20);
  buff.remove(0, buff.indexOf("\r"));
  buff.trim();

  index = buff.indexOf("\n\r");
  buff = buff.substring(0, index);
  buff.trim();
  msg = buff;
  buff = "";
  msg.toLowerCase();
}

void doAction() {
  if (msg == "fyr off") {
    digitalWrite(RELAY_1, HIGH);
    Reply("fyr OFF");
  } else if (msg == "fyr on") {
    digitalWrite(RELAY_1, LOW);
    Reply("fyr ON");
  } else if (msg == "lys off") {
    digitalWrite(RELAY_2, HIGH);
    Reply("lys OFF");
  } else if (msg == "lys on") {
    digitalWrite(RELAY_2, LOW);
    Reply("lys ON");
  } else if (msg == "pumpe off") {
    digitalWrite(RELAY_3, HIGH);
    Reply("pumpe OFF");
  } else if (msg == "pumpe on") {
    digitalWrite(RELAY_3, LOW);
    Reply("pumpe ON");
  } else if (msg == "ekstra off") {
    digitalWrite(RELAY_4, HIGH);
    Reply("ekstra OFF");
  } else if (msg == "ekstra on") {
    digitalWrite(RELAY_4, LOW);
    Reply("ekstra ON");
  } else if (msg == "arm") {
    digitalWrite(PIN_TO_CONTROL, HIGH);     
    Reply("ARMED"); 
  } else if (msg == "disarm") {
    digitalWrite(PIN_TO_CONTROL, LOW);
     Reply("DISARMED");
  }

  smsStatus = "";
  senderNumber = "";
  receivedDate = "";
  msg = "";
}

void Reply(String text) {
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A);  //ascii code for ctrl-26 //sim800.println((char)26); //ascii code for ctrl-26
  delay(1000);
  Serial.println("SMS Sent Successfully.");
  delay(1000);
  sim800.println("AT+CMGD=1,4");
  delay(1000);
  sim800.println("AT+CMGDA= \"DEL ALL\"");
  delay(1000);
}
