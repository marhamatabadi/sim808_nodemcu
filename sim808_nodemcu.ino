#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial mySerial(10,11 );// RX, TX
String data[5];
String state,timegps,latitude,longitude;
String Ooperator,balance;
String Phone = "xxxxxxxx"; // like a 0938*****75
 //---------------------------------------------------------

String sendData (String command , const int timeout ,boolean debug){
String response = "";
mySerial.println(command);
long int time = millis();
int i = 0;
 
while ( (time+timeout ) > millis()){
while (mySerial.available()){
char c = mySerial.read();
response +=c;
}
}
if (debug) {
Serial.print(response);
}
return response;
}

 //---------------------------------------------------------
 void sendTabData(String command , const int timeout , boolean debug){

  mySerial.println(command);
  long int time = millis();
  int i = 0;
  
  while((time+timeout) > millis()){
      while(mySerial.available()){
          char c = mySerial.read();
          if (c != ',') {
              data[i] +=c;
              delay(100);
              } else {
                i++;
                }
      }
  }
  if (debug) {
      state = data[1];
      timegps = data[2];
      latitude = data[3];
      longitude =data[4];
  }
}
 //---------------------------------------------------------
 String Get_PhoneNumber(String Number){
 
     String word1 = "+98";
     String word2 = "\",\"\",\"";
     int Word1 = Number.indexOf(word1) + word1.length() ; 
     int Word2 = Number.lastIndexOf(word2);
       if(Word1 ==-1 or Word2==-1){ 
          return "0" ; 
       }else{
         Number =  "0" + Number.substring(Word1 , Word2 ); 
       }  
      return Number;
}
 //---------------------------------------------------------
 void Operator_Setting(String Operator)
{
 
  String  oprator = "Irancell";
    if(oprator == "Irancell"){
      mySerial.println("AT+CUSD=1,\"*555*4*3*2#\"");
    }
      delay(3000);
      mySerial.println("AT+CUSD=2");
 
}
 //---------------------------------------------------------
 String Balance(String Operator){
       if(Operator=="Irancell"){
       mySerial.println("AT+CUSD=1,\"*141*1#\"");
       mySerial.setTimeout(6000);
       String Balance_Read = mySerial.readString();
       delay(100);
       mySerial.println("" );
       String firsword = "Credit";
       String lastword = "WOW";
       int Word1 = Balance_Read.indexOf(firsword) + firsword.length() ; 
       int Word2 = Balance_Read.lastIndexOf(lastword);
         if(Word1 ==-1 or Word2==-1){ 
            return "0" ; 
         }else{
           Balance_Read =  Balance_Read.substring(Word1 , Word2);
         } 
       delay(100);
       Balance_Read.replace("Rial", "");
       Balance_Read.replace(".", "");
       Balance_Read.replace("IRR", "");
       Balance_Read.trim();
       return Balance_Read;
     }
}
 //---------------------------------------------------------
 String Operator(){
    delay(50);
     mySerial.println("AT+COPS?");
     mySerial.setTimeout(100) ;
     String input = mySerial.readString();
     mySerial.setTimeout(10) ;
     input = "";
     
    input.trim();
    delay(10);
      if(input=="43235"){
         return "Irancell";
      }else{
        return "Not Recognized";
      }
}

 //---------------------------------------------------------
 String receivemessage(){

    if(mySerial.find("CNMI")>=0){  
    mySerial.println("AT+CMGR=1") ; 
    delay(10);
    mySerial.setTimeout(2000) ;
    String input = mySerial.readString();
    delay(500);
    String number ="0";
     String firstword = "\",\"";
     String lastword = "\",\"\",\"";
     int n1 = input.indexOf(firstword) + firstword.length() ; 
     int n2 = input.lastIndexOf(lastword);
      if(n1>-1  and n2>-1){
         number = input.substring(n1 , n2); 
      }
      if(number=="0"){ 
      return "0";
      }else{
       mySerial.println("AT+CMGD=1,4") ; 
       return input;
      }
    }

}
  //---------------------------------------------------------
  void GSM_Initilaize() {
  boolean gsm_Init = 1;
  while (gsm_Init > 0) {
    mySerial.println("AT");
    while (mySerial.available()) {
      if (mySerial.find("OK") > 0)
        gsm_Init = 0;
    }
    delay(100);
  }
 
  boolean Ready = 1;
  while (Ready > 0) {
    mySerial.println("AT+CPIN?");
    while (mySerial.available()) {
      if (mySerial.find("+CPIN: READY") > 0)
        Ready = 0;
    }
    delay(1000);
  }
 
  mySerial.write("AT+CMGF=1\r\n");
  delay(1000);
}
 //---------------------------------------------------------
 int Send_Message(String phone , String text){                                   
        mySerial.print("AT+CMGF=1");
        delay(1000);
        mySerial.print("AT+CMGS="); 
        mySerial.print("\"") ; 
        mySerial.print(phone);
        mySerial.print("\"") ; 
        mySerial.print("\r\n");
        delay(100);
        mySerial.print(text);
        delay(100);
        mySerial.print(char(26));
        delay(100);
        return 1;
}

 //---------------------------------------------------------
void setup() {
     Serial.begin(9600);
     mySerial.begin(9600);
     mySerial.println("AT&F");            
     delay(100);                          
     mySerial.println("AT+CMGF=1");    
     mySerial.setTimeout(100);
     sendData("AT+CGNSPWR=1",1000,DEBUG);
     delay(50);
 
     while(1){               
  
   Ooperator = Operator();
     if(Ooperator=="nothing"){
               Serial.println(Ooperator);
    }// end if operator
      else{
               Serial.println(Ooperator);                  
               Operator_Setting(Ooperator);  
               GSM_Initilaize(); 
               balance= Balance(Ooperator);
            
            //  Send_Message(Phone, Balance);
              Send_Message(Phone, "Welcome to SIM808");
             
              Serial.println(balance);
         
     return;    
    } 
  }// end while
 
  
} // end of Main
 //--------------------------------------------------------- 
void loop() {
       String Read_Message = receivemessage();
       if(Read_Message != "0"){ 
              String phones = Get_PhoneNumber(Read_Message);
              delay(100);
               sendTabData("AT+CGNSINF",1000,DEBUG);
               delay(300);
                  if (state !=0) {
                          Serial.println("State    :"+state);
                          Serial.println("Time     :"+timegps);
                          Serial.println("Latitude :"+latitude);
                          Serial.println("Longitude :"+longitude);
                      }
      delay(1000);
      //send_sms(Phone , "http://maps.google.com/maps?q=loc:"+latitude+","+longitude);
      Serial.println("http://maps.google.com/maps?q=loc:"+latitude+","+longitude);
         
   }
 
}

//---------------------------------------------------------
