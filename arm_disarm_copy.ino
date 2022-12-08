
const String PHONE = "+9999999999";

#define rxPin 4
#define txPin 2
#define BAUD_RATE 9600
HardwareSerial sim800(1);

#define DOOR_SENSOR_PIN 25
#define PIR_SENSOR_PIN 14

String smsStatus,senderNumber,receivedDate,msg;
//

//
bool doorSensor = false;
bool pirSensor = false;

void setup() {

  pinMode(DOOR_SENSOR_PIN, INPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
}

void loop() {
 //////////////////////////////////////////////////
while(sim800.available()){
  parseData(sim800.readString());
}
//////////////////////////////////////////////////
while(Serial.available())  {
  sim800.println(Serial.readString());
}
//////////////////////////////////////////////////
} //main loop ends

//***************************************************
void parseData(String buff){
  Serial.println(buff);

  unsigned int len, index;
  //////////////////////////////////////////////////
  //Remove sent "AT Command" from the response string.
  index = buff.indexOf("\r");
  buff.remove(0, index+2);
  buff.trim();
  //////////////////////////////////////////////////
  
  //////////////////////////////////////////////////
  if(buff != "OK"){
    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();
    
    buff.remove(0, index+2);
    
    if(cmd == "+CMTI"){
      //get newly arrived memory location and store it in temp
      index = buff.indexOf(",");
      String temp = buff.substring(index+1, buff.length()); 
      temp = "AT+CMGR=" + temp + "\r"; 
      //get the message stored at memory location "temp"
      sim800.println(temp); 
    }
    else if(cmd == "+CMGR"){
      extractSms(buff);
      
      
      if(senderNumber == PHONE){
        doAction();
      }
    }
  //////////////////////////////////////////////////
  }
  else{
  }
}

//************************************************************
void extractSms(String buff){
   unsigned int index;
   
    index = buff.indexOf(",");
    smsStatus = buff.substring(1, index-1); 
    buff.remove(0, index+2);
    
    senderNumber = buff.substring(0, 11);
    buff.remove(0,19);
   
    receivedDate = buff.substring(0, 20);
    buff.remove(0,buff.indexOf("\r"));
    buff.trim();
    
    index =buff.indexOf("\n\r");
    buff = buff.substring(0, index);
    buff.trim();
    msg = buff;
    buff = "";
    msg.toLowerCase();
}

  if (digitalRead(DOOR_SENSOR_PIN) == HIGH) {
    doorSensor = true;
  }

  if (digitalRead(PIR_SENSOR_PIN) == HIGH) {
    pirSensor = true;
  }
}

 void doAction(){
  if (message == "ARM") {
    doorSensor = false;
    pirSensor = false;
    gsm.sendSMS(number, "Door and PIR sensors armed");
  } else if (message == "DISARM") {
    doorSensor = true;
    pirSensor = true;
    gsm.sendSMS(number, "Door and PIR sensors disarmed");
  }
}
