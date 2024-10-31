#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Adafruit_NeoPixel.h>
#include "math.h"
#include "DHT.h"

#define LED_PIN    10
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 100
const int trigPin = 5;
const int echoPin = 6;
const int dhtPin = 12;
#define DHTPIN 12
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int counting = 0;
int off = 0;
long duration;
int distanceCm, distanceInch;
int brightness = 0;
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
void setup() {
  // Start the serial connection for debugging
  Serial.begin(9600);
  
  // Initialize the Arduino IoT Cloud properties
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // ------------------------------------------------
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(brightness); // Set BRIGHTNESS to about 1/5 (max = 255)
  //---------------------------------------------------------------------
  dht.begin();
  pinMode(dhtPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
//เช็คค่าที่ได้จาก dashboard ค่าที่ได้ออกมาเป็น r, g, b
  ArduinoCloud.update();
  uint8_t r, g, b, r1, g1, b1;
  color.getValue().getRGB(r, g, b);
  colorWipe(strip.Color(r,   g,   b), 10);
  Serial.print("Color:");
  Serial.print(" Red ");
  Serial.print(r);
  Serial.print(" Green ");
  Serial.print(g);
  Serial.print(" Blue ");
  Serial.println(b);

  color_light.getValue().getRGB(r1, g1, b1);
  Serial.print("Color_light:");
  Serial.print(" Red ");
  Serial.print(r1);
  Serial.print(" Green ");
  Serial.print(g1);
  Serial.print(" Blue ");
  Serial.println(b1);
  //-------------------------------
  digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Set the trigPin HIGH for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Read the echoPin, return the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculate the distance in cm and inches
    distanceCm = duration * 0.034 / 2;
    distanceInch = duration * 0.0133 / 2;

    // Print the distances to the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");

    distance = distanceCm;
  // if (brightness == 0){
  //   brightness = ceil(distance/100.0*150.0);
  //   brightness = 10 > brightness ? 10:brightness;
  //   strip.setBrightness(brightness);
  //   off = 0;
  // }
  if (off >= 2){
    brightness = 0;
    off = 0;
    strip.setBrightness(brightness);
  }
  else{
    if (distance <= 100){
      brightness = ceil(distance/100.0*150.0);
      brightness = 10 > brightness ? 10:brightness;
      strip.setBrightness(brightness);
      off = 0;
    }
    else{
      off++;
    }
  }

  // Sensor Reading
  float h = dht.readHumidity();
  // Read temperature as Celcius
  float t = dht.readTemperature();
  // Compute heat index in Celcius (isFahrenheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.println(t);
  Serial.println(hic);

  temperature = t;
    //-----------------------------------------
  delay(500);
}
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}