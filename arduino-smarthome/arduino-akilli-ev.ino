#include <dht11.h>  // DHT11 kütüphanesi
#include <Servo.h>

#define ledPin1 2  // İlk LED pin
#define ledPin2 3  // İkinci LED pin

#define echoPin2 4  // İkinci mesafe sensörü pinleri
#define trigPin2 5
#define buzzerPin2 9  // İkinci mesafe sensörü için buzzer pin

#define echoPin3 6  // Üçüncü mesafe sensörü pinleri
#define trigPin3 7
#define buzzerPin3 10  // Üçüncü mesafe sensörü için buzzer pin

#define waterSensorPin A0  // Su sensörü analog pini
#define soilSensorPin A1   // Toprak nem sensörü analog pini
#define servoPin 11        // Servo motor pin
#define servoPin2 8        // Servo motor pin2

#define pirSensorPin 12  // PIR sensörü pini
#define pirBuzzerPin 13  // PIR sensörü için buzzer pin

#define lm35Pin A2      // LM35 sıcaklık sensörü pin

int maximumRange = 50;
int minimumRange = 0;
int detectionRange = 6;                // Engel algılanacak mesafe (cm)
unsigned long lastDetectionTime1 = 0;  // İlk sensör için son engel algılama zamanı
unsigned long lastDetectionTime2 = 0;  // İkinci sensör için son engel algılama zamanı
unsigned long lastDetectionTime3 = 0;  // Üçüncü sensör için son engel algılama zamanı

Servo servoMotor;
Servo servoMotor2;
bool isWaterDetected = false;  // Su algılama durumu
char receivedChar;

void setup() {

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(buzzerPin2, OUTPUT);

  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(buzzerPin3, OUTPUT);

  pinMode(waterSensorPin, INPUT);  // Su sensörünü giriş pini olarak ayarla
  pinMode(soilSensorPin, INPUT);   // Toprak nem sensörünü giriş pini olarak ayarla

  servoMotor.attach(servoPin);  // Servo motoru ilgili pine bağla
  servoMotor.write(0);          // Servo motoru başlangıç pozisyonuna getir (kapalı)

  servoMotor2.attach(servoPin2);  // Servo motoru ilgili pine bağla
  servoMotor2.write(90);          // Servo motoru başlangıç pozisyonuna getir (kapalı)

  pinMode(pirSensorPin, INPUT);   // PIR sensörünü giriş pini olarak ayarla
  pinMode(pirBuzzerPin, OUTPUT);  // PIR buzzer'ını çıkış pini olarak ayarla

  pinMode(ledPin1, OUTPUT); // İlk LED pinini çıkış olarak ayarla
  pinMode(ledPin2, OUTPUT); // İkinci LED pinini çıkış olarak ayarla

  Serial.begin(9600);  // Serial portu başlat
}

void loop() {
  int distance2 = mesafe(echoPin2, trigPin2, maximumRange, minimumRange);
  int distance3 = mesafe(echoPin3, trigPin3, maximumRange, minimumRange);
  Serial.println("");
  Serial.print(distance2);
  Serial.print(",");
  Serial.print(distance3);
  Serial.print(",");

  // İkinci mesafe sensörü için kontrol
  if (distance2 > minimumRange && distance2 < detectionRange) {
    digitalWrite(buzzerPin2, HIGH);
    lastDetectionTime2 = millis();
  } else {
    if (millis() - lastDetectionTime2 > 5000) {
      digitalWrite(buzzerPin2, LOW);
    }
  }

  // Üçüncü mesafe sensörü için kontrol
  if (distance3 > minimumRange && distance3 < detectionRange) {
    digitalWrite(buzzerPin3, HIGH);
    lastDetectionTime3 = millis();
  } else {
    if (millis() - lastDetectionTime3 > 5000) {
      digitalWrite(buzzerPin3, LOW);
    }
  }

  // Su sensörü kontrolü
  int waterValue = analogRead(waterSensorPin);
  Serial.print(waterValue);
  Serial.print(",");

  if (waterValue > 300) {     // Su algılandığında (su sensörü değeri 500'den büyükse)
    if (!isWaterDetected) {   // Eğer su yeni algılandıysa
      servoMotor.write(360);  // Pencereyi kapat (servoyu 90 derece döndür)
      isWaterDetected = true;
    }
      Serial.print("kapali");
      Serial.print(",");
  } else {                  // Su algılanmadığında
    if (isWaterDetected) {  // Eğer su algılaması durduysa
      servoMotor.write(0);  // Pencereyi aç (servoyu başlangıç pozisyonuna getir)
      isWaterDetected = false;
    }
      Serial.print("acik");
      Serial.print(",");
  }

  // PIR sensörü kontrolü
  int pirState = digitalRead(pirSensorPin);
  Serial.print(pirState);
  Serial.print(",");

  if (pirState == HIGH) {              // Hareket algılandığında
    digitalWrite(pirBuzzerPin, HIGH);  // PIR buzzer'ını çalıştır
  } else {
    digitalWrite(pirBuzzerPin, LOW);  // PIR buzzer'ını kapat
  }

  // Toprak nem sensörü kontrolü
  int soilMoisture = analogRead(soilSensorPin);
  float nemYuzdesi = (1.0 - (soilMoisture / 1023.0)) * 100.0; // Yüzdelik nem değerini hesapla
  Serial.print(nemYuzdesi);
  Serial.print(",");

    // LM35 sıcaklık sensörü kontrolü
  int lm35Value = analogRead(lm35Pin);
  float voltage = lm35Value * (5.0 / 1023.0);  // Sensörün çıkış gerilimini hesapla (0-5V arası)
  float temperatureC = (voltage - 0.5) * 100.0; // Sıcaklık değerini derece Celsius cinsinden hesapla

  Serial.print(temperatureC);
  Serial.print(",");

  //Koridor Ledleri
 if (Serial.available() > 0) {
    receivedChar = Serial.read();
    if (receivedChar == '1') {
      digitalWrite(ledPin1, HIGH);
    } else if (receivedChar == '2') {
      digitalWrite(ledPin1, LOW);
    } else if (receivedChar == '3') {
      digitalWrite(ledPin2, HIGH);
    } else if (receivedChar == '4') {
      digitalWrite(ledPin2, LOW);
    } else if (receivedChar == '5') {
      servoMotor2.write(180);
    } else if (receivedChar == '6') {
      servoMotor2.write(90);
    }
  }

   // Arduino'dan gelen verileri Node.js sunucusuna gönder

  Serial.print(distance2 > minimumRange && distance2 < detectionRange ? 1 : 0); // 1 if otopark dolu, 0 if boş
  Serial.print(",");
  Serial.print(distance3 > minimumRange && distance3 < detectionRange ? 1 : 0); // 1 if otopark dolu, 0 if boş

  //servoMotor2.write(180);

  delay(100);  // Seri iletişimde fazla yazdırma önlemi
  //servoMotor2.write(0);
}

int mesafe(int echoPin, int trigPin, int maxrange, int minrange) {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delay(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  delay(50);

  if (distance >= maxrange || distance <= minrange)
    return 0;

  return distance;
}