

#include <rdm6300.h>
#define RDM6300_RX_PIN 3
Rdm6300 rdm6300;

//servo
#include <Servo.h>
Servo myservo;
int a, f = 0, f1 = 0;

//wifi
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "low-cast-iot-home-automation.firebaseio.com"
#define FIREBASE_AUTH "Xc7vmeakCLEC49hdWiH8a6pfX2uaHAJIBK3vqRSv"
#define WIFI_SSID "rezvee"
#define WIFI_PASSWORD "23456789"


String slot_1, slot_2, slot_3, slot_4, slot_5, slot_6, slot_7, slot_8;

int flag, set = 0, flag1 = 0, set1 = 0;
String tag1;

int flag2 = 0;

int now;

void setup() {
  Serial.begin(9600);
  Serial.println("INIT DONE");

  ssrfid.begin(9600);
  ssrfid.listen();

  myservo.attach(5);
  pinMode(13, INPUT);
  myservo.detach();



  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}


int i = 0;

void ser() {

  Serial.print("Set1= ");
  Serial.println(set1);

  Serial.print("count = ");
  Serial.println(i);

  if (i != set1)
  {
    myservo.attach(5);
  }
  a = digitalRead(13);
  Serial.println(a);
  if (a == 1)
  {
    if (f == 0)
    {
      i++;  // de-bounceing problem
      Serial.println(i);
      f = 1;
    }


    if (i == set1)
    {
      set1 = 0;
      i = 0;
      f1 = 0;
      delay(320);
      myservo.detach();
    }

  }
  else
  {
    f = 0;

  }

  myservo.write(0);
  delay(10);

}


void firebasereconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}





void RFIDread() {
  if (ssrfid.available() > 0) {


    bool call_extract_tag = false;

    int ssvalue = ssrfid.read();
    if (ssvalue == -1) {
      return;
    }
    if (ssvalue == 2) {
      buffer_index = 0;
    } else if (ssvalue == 3) {
      call_extract_tag = true;
    }
    if (buffer_index >= BUFFER_SIZE) {
      Serial.println("Error: Buffer overflow detected!");
      return;
    }

    buffer[buffer_index++] = ssvalue;
    if (call_extract_tag) {
      if (buffer_index == BUFFER_SIZE) {
        unsigned tag = extract_tag();
      } else {
        buffer_index = 0;
        return;
      }
    }
  }

}



unsigned extract_tag() {

  uint8_t *msg_data = buffer + 1;
  uint8_t *msg_data_tag = msg_data + 2;
  char* pointer = (char*) msg_data_tag;
  pointer[DATA_TAG_SIZE] = '\0';
  long tag = strtol(pointer, NULL, 16);
  Serial.print("Card No: ");
  Serial.println(tag);

  tag1 = (String)tag;


  myservo.detach();
  now = Firebase.getInt("/now/");
  Serial.print("now= ");
  Serial.println(now);
  myservo.detach();


  if (slot_1 == tag1)
  {
    flag = 1;
    Serial.print("Matched 1");
  }

  else if (slot_2 == tag1)
  {
    flag = 2;
    Serial.print("Matched 2");
  }

  else if (slot_3 == tag1)
  {
    flag = 3;
    Serial.print("Matched 3");
  }
  else if (slot_4 == tag1)
  {
    flag = 4;
    Serial.print("Matched 4");
  }

  else if (slot_5 == tag1)
  {
    flag = 5;
    Serial.print("Matched 5");
  }

  else if (slot_6 == tag1)
  {
    flag = 6;
    Serial.print("Matched 6");
  }

  else if (slot_7 == tag1)
  {
    flag = 7;
    Serial.print("Matched 7");
  }

  else if (slot_8 == tag1)
  {
    flag = 8;
    Serial.print("Matched 8");
  }
  else

  {

    Serial.println("here");


    if (slot_1 == "Empty")
    {
      Serial.print("put 1");
      flag = 1;
      Firebase.setString("/stole/stole_1/", tag1);
    }
    else if (slot_2 == "Empty")
    {
      flag = 2;
      Firebase.setString("/stole/stole_2/", tag1);
    }

    else if (slot_3 == "Empty")
    {
      flag = 3;
      Firebase.setString("/stole/stole_3/", tag1);
    }
    else if (slot_4 == "Empty")
    {
      flag = 4;
      Firebase.setString("/stole/stole_4/", tag1);
    }

    else if (slot_5 == "Empty")
    {
      flag = 5;
      Firebase.setString("/stole/stole_5/", tag1);
    }

    else if (slot_6 == "Empty")
    {
      flag = 6;
      Firebase.setString("/stole/stole_6/", tag1);
    }

    else if (slot_7 == "Empty")
    {
      flag = 7;
      Firebase.setString("/stole/stole_7/", tag1);
    }

    else if (slot_8 == "Empty")
    {
      flag = 8;
      Firebase.setString("/stole/stole_8/", tag1);
    }
  }

  if (now > flag)

  {
    set = flag + 8 - now;
    if (f1 == 0)
    {
      f1 = 1;
      set1 = set;
    }
    Serial.print("Set= ");
    Serial.println(set);
    Firebase.set("/now/", flag);
  }
  else {
    set = flag - now;
    Serial.print("Set= ");
    Serial.println(set);
    if (f1 == 0)
    {
      f1 = 1;
      set1 = set;
    }

    if (set != 0)
    {
      if (f1 == 0)
      {
        f1 = 1;
        set1 = set;
      }
      Firebase.set("/now/", flag);
    }
    else
    {
      Firebase.set("/now/", now);
    }
  }


  return tag;
}


void loop() {

  if (Firebase.failed())
  {
    Serial.print("setting number: ");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }

  slot_1 = Firebase.getString("/stole/stole_1/");
  slot_2 = Firebase.getString("/stole/stole_2/");
  slot_3 = Firebase.getString("/stole/stole_3/");
  slot_4 = Firebase.getString("/stole/stole_4/");
  slot_5 = Firebase.getString("/stole/stole_5/");
  slot_6 = Firebase.getString("/stole/stole_6/");
  slot_7 = Firebase.getString("/stole/stole_7/");
  slot_8 = Firebase.getString("/stole/stole_8/");
  Serial.println(slot_1);
  Serial.println(slot_2);
  Serial.println(slot_3);
  Serial.println(slot_4);
  Serial.println(slot_5);
  Serial.println(slot_6);
  Serial.println(slot_7);
  Serial.println(slot_8);

  if (ssrfid.available() > 0) {
    RFIDread();
    delay(4000);
    Serial.println("Processing");
  }

  //servo function
  ser();


}
