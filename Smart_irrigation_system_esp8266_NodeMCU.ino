#define BLYNK_TEMPLATE_ID "TMPL3kjw27icm"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System"
#define BLYNK_AUTH_TOKEN "ihVwPbYodrkgA0Dp8sXCumat4hQx_MhV"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define dhtpin D4
#define soil A0
#define ldr D3
#define light D0
#define relay D5

DHT dht(dhtpin, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

char ssid[] = "FTTH-39D6";
char pass[] = "4321043210";

bool manualRelayControl = false;
bool manualLightControl = false;

void setup() {

  Serial.begin(9600);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.init();
  lcd.backlight();

  pinMode(soil, INPUT);
  pinMode(ldr, INPUT);
  pinMode(light, OUTPUT);
  pinMode(relay, OUTPUT);

  digitalWrite(light, LOW);
  digitalWrite(relay, LOW);

  lcd.print("Smart Irrigation");
  delay(2000);
  lcd.clear();
}

BLYNK_WRITE(V3) {
  manualRelayControl = param.asInt();
  if (manualRelayControl) {
    digitalWrite(relay, LOW);
  } else {
    digitalWrite(relay, HIGH);
  }
}

BLYNK_WRITE(V4) {
  manualLightControl = param.asInt();
  if (manualLightControl) {
    digitalWrite(light, LOW);
  } else {
    digitalWrite(light, HIGH);
  }
}

void loop() {
  Blynk.run();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(soil);
  int ldrValue = digitalRead(ldr);
  int invert=soilMoisture-1023;
  int state=map(invert,0,1023,0,100)*(-1);


  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Soil Moisture: ");
  Serial.println(state);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  if (state < 30) { 
    lcd.print("Low Moisture!  ");
    if (!manualRelayControl) {
      digitalWrite(relay, LOW);
    }
  } else {
    lcd.print("Moisture OK    ");
    if (!manualRelayControl) {
      digitalWrite(relay, HIGH);
    }
  }


  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, state);


  if (!manualLightControl) { 
    if (ldrValue == LOW) { 
      digitalWrite(light, HIGH);
    } else {
      digitalWrite(light, LOW);
    }
  }
}
