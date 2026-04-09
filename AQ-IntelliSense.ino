#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ===== ADD: DHT22 ===== */
#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
/* ===================== */

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define MQ135_PIN 32

/* ===== ADD: MQ-7 ===== */
#define MQ7_PIN 33
float RL_MQ7 = 10.0;
float R0_MQ7 = 10.0;   // adjust after clean-air calibration
static unsigned long lastMQ7Read = 0;
static float copppm_frozen = 0;
static int adc7_frozen = 0;  // ADDED: frozen ADC for MQ-7
/* ==================== */

/* ===== ADD: DUST SENSOR ===== */
#define MEASURE_PIN 34
#define LED_POWER   25

int samplingTime = 280;
int deltaTime    = 40;
int sleepTime    = 9860;

static unsigned long lastDustRead = 0;
static float dustDensity_frozen = 0;
static int dustADC_frozen = 0;
/* =========================== */

/* --- YOUR CALCULATED VALUES --- */
float RL = 10.0;
//float R0 = 0.425;        
float R0 = 4.25;

/* ===== ADD: DISPLAY STATE ===== */
enum ScreenMode { SHOW_MQ135, SHOW_MQ7, SHOW_DHT, SHOW_DUST };
ScreenMode currentScreen = SHOW_MQ135;

int xOffset = 0;
bool scrolling = false;
unsigned long lastSwitchTime = 0;
/* ============================== */

// ===== ADD: MQ-135 FREEZE CONTROL =====
static unsigned long lastMQRead = 0;
static int adc_frozen = 0;
static float co2ppm_frozen = 0;
// =====================================

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db); 
  
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  /* ===== ADD ===== */
  dht.begin();
  lastSwitchTime = millis();
  /* ============== */

  /* ===== ADD: DUST INIT ===== */
  pinMode(LED_POWER, OUTPUT);
  digitalWrite(LED_POWER, HIGH);
  /* ========================== */

}

void loop() {

// ===== MQ-135 UPDATE ONLY ONCE PER 10 SECOND =====
if (millis() - lastMQRead >= 10000) {

  int adc = analogRead(MQ135_PIN);

  //float Vadc = (adc / 4095.0) * 3.3;
  //float Vadc = (adc / 4095.0) * 1.39;
  //float Vadc = (adc / 4095.0) * 4.21;
  float Vadc = (adc / 4095.0) * 4.23;

  // Based on your data (Vadc 1.31V -> Vout 1.98V), your multiplier is ~1.51
  //float Vout = Vadc * 1.511; 
  //float Vout = Vadc * 2.47;
  float Vout = Vadc * 2.445;

  // Formula: Rs = (Vcc * RL / Vout) - RL
  // float Rs = ((5.0 * RL) / Vout) - RL;
  //float Rs = 1.87;
  float Rs = RL * ((5 / Vout) - 1);
  float ratio = Rs / R0;
  float co2ppm = 110.47 * pow(ratio, -2.862);

  // FREEZE VALUES
  adc_frozen = adc;
  co2ppm_frozen = co2ppm;

  lastMQRead = millis();
}

/* ===== MQ-7 UPDATE (RELATIVE CO) ===== */
  if (millis() - lastMQ7Read >= 5000) {

    int adc7 = analogRead(MQ7_PIN);
    adc7_frozen = adc7;  // ADDED: store ADC value

    float Vadc7 = (adc7 / 4095.0) * 3.3;
    float Rs7 = RL_MQ7 * ((5.0 / Vadc7) - 1.0);
    float ratio7 = Rs7 / R0_MQ7;

    // Empirical MQ-7 CO curve (relative)
    float co_ppm = 100 * pow(ratio7, -1.5);

    copppm_frozen = co_ppm;
    lastMQ7Read = millis();
  }

  // ===== ADD: DUST SENSOR UPDATE =====
if (millis() - lastDustRead >= 2000) {

  digitalWrite(LED_POWER, LOW);
  delayMicroseconds(samplingTime);

  int adcDust = analogRead(MEASURE_PIN);

  delayMicroseconds(deltaTime);
  digitalWrite(LED_POWER, HIGH);
  delayMicroseconds(sleepTime);

  float voltage = adcDust * (3.3 / 4095.0);
  float dustDensity = 170 * voltage - 0.1;
  if (dustDensity < 0) dustDensity = 0;

  dustADC_frozen = adcDust;
  dustDensity_frozen = dustDensity;

  lastDustRead = millis();
}
// ====================================

// =============================================

  //Serial.print("ADC MQ135: "); Serial.print(adc_frozen);
  //Serial.print(" | ADC MQ7: "); Serial.print(adc7_frozen);  // ADDED
  //Serial.print(" | Vout: "); Serial.print(Vout);
  //Serial.print(" | Rs: "); Serial.print(Rs, 3);
  //Serial.print(" | Ratio: "); Serial.print(ratio, 2);
  //Serial.print(" | R0: "); Serial.print(R0);
  //Serial.print(" | CO2: "); Serial.print(co2ppm_frozen, 0);
  Serial.print("ADC MQ135: "); Serial.print(adc_frozen);
Serial.print(" | ADC MQ7: "); Serial.print(adc7_frozen);
Serial.print(" | ADC DUST: "); Serial.print(dustADC_frozen);
Serial.print(" | MQ135 CO2: "); Serial.print(co2ppm_frozen, 0);
Serial.print(" ppm | MQ7 CO: "); Serial.print(copppm_frozen, 0);
Serial.println(" ppm");

Serial.print("Dust Density: ");
Serial.print(dustDensity_frozen);
Serial.println(" ug/m3");
  /* ====================================================== */

  /* ===== ADD: DHT22 READ (SAFE RATE) ===== */
  static unsigned long lastDHTRead = 0;
  static float temperature = 0, humidity = 0;

  if (millis() - lastDHTRead > 2000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      humidity = h;
      temperature = t;
    }
    lastDHTRead = millis();
  }
  /* ====================================== */

/* ===== DRAW SCREEN ===== */
display.clearDisplay();  // clear every loop

if (currentScreen == SHOW_MQ135) {
    // --- Big Title ---
    display.setTextSize(2);
    display.setCursor(128 - xOffset, 0);
    display.print("CO2");

    // --- Horizontal line (scrolls with text) ---
    display.drawFastHLine(128 - xOffset, 18, 128, SSD1306_WHITE);

    // --- Smaller reading ---
    display.setTextSize(1);
    display.setCursor(128 - xOffset, 28);
    display.print((int)co2ppm_frozen);
    display.println(" PPM");

} else if (currentScreen == SHOW_MQ7) {

    display.setTextSize(2); 
    display.setCursor(128 - xOffset, 0);
    display.print("CO");

    display.drawFastHLine(128 - xOffset, 18, 128, SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(128 - xOffset, 28);
    display.print(copppm_frozen, 2);
    display.println(" PPM");

  }
  else if (currentScreen == SHOW_DHT) { // DHT22 Screen
    // --- Big Title: Temperature ---
    display.setTextSize(1);
    display.setCursor(128 - xOffset, 0);
    display.print("Temp: ");
    display.print(temperature, 1);
    display.println(" C");

    // --- Horizontal line ---
    display.drawFastHLine(128 - xOffset, 22, 128, SSD1306_WHITE);

    // --- Smaller Humidity ---
    display.setTextSize(1);
    display.setCursor(128 - xOffset, 28);
    display.print("Humidity: ");
    display.print(humidity, 1);
    display.println(" %");

}else if (currentScreen == SHOW_DUST) {

  display.setTextSize(2);
  display.setCursor(128 - xOffset, 0);
  display.print("DUST");

  display.drawFastHLine(128 - xOffset, 18, 128, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(128 - xOffset, 28);
  display.print(dustDensity_frozen, 1);
  display.println(" ug/m3");

}

display.display();  // update OLED

  /* ===== ADD: SCROLL ===== */
if (!scrolling && millis() - lastSwitchTime > 2000) {
    scrolling = true;
    xOffset = 0;             // start at 0
    lastSwitchTime = millis();
}

if (scrolling) {
    xOffset += 2;            // speed of scroll
    if (xOffset > 160) {     // enough to fully scroll off screen
        xOffset = 0;
        scrolling = false;
        if (currentScreen == SHOW_MQ135) currentScreen = SHOW_MQ7;
        else if (currentScreen == SHOW_MQ7) currentScreen = SHOW_DHT;
        else if (currentScreen == SHOW_DHT) currentScreen = SHOW_DUST;
        else currentScreen = SHOW_MQ135;
        lastSwitchTime = millis();
    }
}

/* ===================================== */

  delay(60);   // smooth animation
}
