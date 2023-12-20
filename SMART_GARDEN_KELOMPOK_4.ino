#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Mendefinisikan pin 
#define PUMP D5
#define LAMP D7
#define m A0
#define l D6
// Koneksi ke Firebase & wifi
#define FIREBASE_HOST "https://iot-tubes-9509f-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "xQAcyS5i0cNWklu9CSpmTEk9uPrf5KZcCH4lPI5w"
#define WIFI_SSID "Sayaa"
#define WIFI_PASSWORD "arya1235"

// mendeklarasikan objek data dari FirebaseESP8266
String jsonStr;
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200); //kecepatan pembacaan port
  pinMode(LAMP, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(m, INPUT);
  pinMode(l, INPUT);
  lcd.begin();

  // Koneksi ke Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcd.print("Connecting...");
    delay (1000);
    lcd.clear();
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
      lcd.print("     READY");
      lcd.setCursor(0,1);
      lcd.print("IP: ");
      lcd.println(WiFi.localIP());
      delay (1000);
      lcd.clear();
  Serial.println();
  Serial.println(" ");


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  if (Firebase.getInt(firebaseData, "/Hasil_Pembacaan/set_point")){
    if  (firebaseData.dataTypeEnum() == firebase_rtdb_data_type_integer){
      int set_point = firebaseData.to<int>();
}

void loop() {
  
  int soilmoisture = analogRead(m);
  int soilmoisturepercen = map(soilmoisture,0,255,0,100);
  int ldr = digitalRead(l);
  

  // Memeriksa apakah sensor berhasil mambaca kelembaban
  if ( isnan(soilmoisturepercen)) {
    Serial.println("Gagal membaca sensor soilmoisture");
    return;
  }

  // Menampilkan suhu dan kelembaban pada serial monitor
  Serial.print("Kelembaban tanah: ");
  Serial.print(soilmoisturepercen);
  Serial.println(" %");
  Serial.println();

    //PRINT LCD
      lcd.clear();
      lcd.print("penyiram tanaman");
      delay (1000);
      lcd.clear();
      
      lcd.setCursor(0,0);
      lcd.print("Klmbn T : ");
      lcd.print(soilmoisturepercen);
      lcd.println(" %");
      delay (500);
      lcd.clear();

  // Memberikan status kelembaban kepada firebase
  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban tanah", soilmoisturepercen)) {
    Serial.println("Kelembaban tanah terkirim");
    Serial.println();
  } else {
    Serial.println("Kelembaban tanah tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

     if (Firebase.getString(firebaseData, "/Hasil_Pembacaan/Sistem_Manual")){
    if  (firebaseData.dataType() == "string"){
      String FBStatus = firebaseData.stringData();//Sistem otomatis
      if (FBStatus == "false"){
        Serial.println("............ Sistem Otomatis ...........");
      lcd.clear();
      lcd.print("Sistem Otomatis");                 
      lcd.setCursor(0,1);
      lcd.print(" ");

      if(soilmoisturepercen < set_point){
        digitalWrite(PUMP, LOW); //ON POMPA
        Firebase.setString(firebaseData,"/Hasil_Pembacaan/PUMP","ON");
        Serial.println("...........................  POMPA ON");
        lcd.setCursor(0,1);
        lcd.print("POMPA ON");}

       else{
        digitalWrite(PUMP, HIGH); //OFF POMPA
        Firebase.setString(firebaseData,"/Hasil_Pembacaan/PUMP","OFF");
        Serial.println("...........................  POMPA OFF");
        lcd.setCursor(0,1);
        lcd.print("POMPA OFF");}
        Serial.println(" ");

        //ldr SENSOR
        if(ldr == 1)
        {
          digitalWrite(LAMP, LOW);
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/mode malam","ON");
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/LAMPU","ON");
          Serial.println("LAMPU ON");
          delay(500); 
            lcd.clear();
            lcd.print("MODE MALAM ON");
        }
        else if(ldr == 0){
          digitalWrite(LAMP, HIGH);
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/mode malam","OFF");
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/LAMPU","OFF");
          Serial.println("LAMPU OF");
          delay(500); 
            lcd.clear();
            lcd.print("MODE MALAM OFF");
            Serial.println(" ");}
        }
        else if (FBStatus == "true"){ //SISTEM MANUAL
        Serial.println("............. Sistem Manual ............");
        lcd.clear();
        lcd.print("Sistem Manual");  
        lcd.setCursor(0,1);
        lcd.print(" ");

        if (Firebase.getString(firebaseData, "/Hasil_Pembacaan/PUMP")){
          if  (firebaseData.dataType() == "string"){
            String FBStatus = firebaseData.stringData();
            if (FBStatus == "false"){
            Serial.println("...........................  POMPA OFF");                     
            lcd.setCursor(0,1);
            lcd.print("POMPA OFF");
            digitalWrite(PUMP, HIGH);}
            
            else if (FBStatus == "true"){
              Serial.println("...........................  POMPA ON");
                    lcd.setCursor(0,1);
                    lcd.print("POMPA ON");
                    
                  digitalWrite(PUMP, LOW); 
                  Serial.println(" ");
            }
            
          }
          else {Serial.println("Salah kode POMPA! isi dengan data ON/OFF");}
        }
        if (Firebase.getString(firebaseData, "/Hasil_Pembacaan/LAMPU")){
          if  (firebaseData.dataType() == "string"){
            String FBStatus = firebaseData.stringData();
            if (FBStatus == "false"){
              Serial.println("...........................  LAMPU OFF");                     
              lcd.setCursor(0,1);
              lcd.print("LAMPU OFF");
              digitalWrite(LAMP, HIGH);
            }
            else if (FBStatus == "true"){
              Serial.println("...........................  LAMPU ON");
                    lcd.setCursor(0,1);
                    lcd.print("LAMPU ON");
                    
                  digitalWrite(LAMP, LOW); 
                  Serial.println(" ");
            }
          }
          
        }
        }
      }
    }
  }
