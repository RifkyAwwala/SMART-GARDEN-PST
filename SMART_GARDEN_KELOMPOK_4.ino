#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal.h>
// Mendefinisikan pin dan tipe sensor DHT
#define DHTPIN D3
#define DHTTYPE DHT22
#define PUMP D5
DHT dht(DHTPIN, DHTTYPE);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Harus diisi
#define FIREBASE_HOST "https://iot-tubes-9509f-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "xQAcyS5i0cNWklu9CSpmTEk9uPrf5KZcCH4lPI5w"
#define WIFI_SSID "Sayaa"
#define WIFI_PASSWORD "arya1235"

// mendeklarasikan objek data dari FirebaseESP8266
String jsonStr;
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;


void setup() {
// initialize the LCD
  

  // Turn on the blacklight and print a message.

  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUMP, OUTPUT);
  dht.begin();
  lcd.begin(16, 2);

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
  config.api_key = FIREBASE_HOST;
  config.database_url = FIREBASE_AUTH;
    /* Sign up */
  if (Firebase.signUp(&config,&auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Firebase.begin(&config, &auth);
  // Firebase.reconnectWiFi(true);

}

void loop() {

  // Sensor DHT11 membaca suhu dan kelembaban
  int m = analogRead(A0);
  int soilmoisture ;
  soilmoisture = map(m,577,133,0,100);
  int ldr = digitalRead(D6);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  // Memeriksa apakah sensor berhasil mambaca suhu dan kelembaban
  if (isnan(t) || isnan(h) || isnan(m)) {
    Serial.println("Gagal membaca sensor DHT11");
    return;
  }

  // Menampilkan suhu dan kelembaban pada serial monitor
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.println(" *C");
  lcd.print("Suhu: ");
  lcd.print(t);
  lcd.print(" *C");
  Serial.print("Kelembaban: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Kelembaban tanah: ");
  Serial.print(m);
  Serial.println(" %");
  Serial.println();

    //PRINT LCD
      lcd.clear();
      lcd.print("penyiram tanaman");
      delay (1000);
      lcd.clear();

      lcd.print("Klmbn T : ");
      lcd.print(m);
      lcd.println(" %");
      delay (2000);
      lcd.clear();

      lcd.print("Suhu   : ");
      lcd.print(t);
      lcd.println(" C");

      lcd.setCursor(0,1);
      lcd.print("klmbn : ");
      lcd.print(h);
      lcd.println(" %");
      delay (1000);
  // Memberikan status suhu dan kelembaban kepada firebase
  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/suhu", t)) {
    Serial.println("Suhu terkirim");
  } else {
    Serial.println("Suhu tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban", h)) {
    Serial.println("Kelembaban terkirim");
  } else {
    Serial.println("Kelembaban tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Hasil_Pembacaan/kelembaban tanah", m)) {
    Serial.println("Kelembaban tanah terkirim");
    Serial.println();
  } else {
    Serial.println("Kelembaban tanah tidak terkirim");
    Serial.println("Karena: " + firebaseData.errorReason());
  }

     if (Firebase.getString(firebaseData, "/Hasil_Pembacaan/Sistem_Manual")){
    if  (firebaseData.dataType() == "string"){
      String FBStatus = firebaseData.stringData();//Sistem otomatis
      if (FBStatus == "OFF"){
        Serial.println("............ Sistem Otomatis ...........");
      lcd.clear();
      lcd.print("Sistem Otomatis");                 
      lcd.setCursor(0,1);
      lcd.print(" ");

      if(m < 25){
        digitalWrite(PUMP, LOW); //ON POMPA
        Firebase.setString(firebaseData,"/Hasil_Pembacaan/PUMP","ON");
        Serial.println("...........................  POMPA ON");
        lcd.setCursor(0,1);
        lcd.print("POMPA ON");}

       else{
        digitalWrite(PUMP, HIGH); //OFF LAMPU
        Firebase.setString(firebaseData,"/Hasil_Pembacaan/PUMP","OFF");
        Serial.println("...........................  POMPA OFF");
        lcd.setCursor(0,1);
        lcd.print("POMPA OFF");}
        Serial.println(" ");

        //ldr SENSOR
        if(ldr == 1)
        {
          digitalWrite(LED_BUILTIN, LOW);
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/mode malam","ON");
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/LAMPU","ON");
          delay(500); 
            lcd.clear();
            lcd.print("MODE MALAM ON");
        }
        else if(ldr == 0){
          digitalWrite(LED_BUILTIN, HIGH);
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/mode malam","OFF");
          Firebase.setString(firebaseData,"/Hasil_Pembacaan/LAMPU","OFF");
          delay(500); 
            lcd.clear();
            lcd.print("MODE MALAM OFF");
            Serial.println(" ");}
        }
        if (FBStatus == "ON"){ //SISTEM MANUAL
        Serial.println("............. Sistem Manual ............");
        lcd.clear();
        lcd.print("Sistem Manual");  
        lcd.setCursor(0,1);
        lcd.print(" ");

        if (Firebase.getString(firebaseData, "/Hasil_Pembacaan/PUMP")){
          if  (firebaseData.dataType() == "string"){
            String FBStatus = firebaseData.stringData();
            if (FBStatus == "OFF"){
            Serial.println("...........................  POMPA OFF");                     
            lcd.setCursor(0,1);
            lcd.print("POMPA OFF");
            digitalWrite(PUMP, HIGH);}
            
            else if (FBStatus == "ON"){
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
            if (FBStatus == "OFF"){
              Serial.println("...........................  LAMPU OFF");                     
              lcd.setCursor(0,1);
              lcd.print("LAMPU OFF");
              digitalWrite(LED_BUILTIN, HIGH);
            }
            else if (FBStatus == "ON"){
              Serial.println("...........................  LAMPU ON");
                    lcd.setCursor(0,1);
                    lcd.print("LAMPU ON");
                    
                  digitalWrite(LED_BUILTIN, LOW); 
                  Serial.println(" ");
            }
          }
          else {Serial.println("Salah kode LAMPU! isi dengan data ON/OFF");}
        }
        }
      }
      else {Serial.println("Salah kode Sistem_Manual! isi dengan data ON/OFF");}
    }
  }
