#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPX4250AP.h>

// OLED FeatherWing buttons map to different pins depending on board.
// The I2C (Wire) bus may also be different.
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
  #define WIRE Wire
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
  #define WIRE Wire
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
  #define WIRE Wire
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
  #define WIRE Wire
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
  #define WIRE Wire
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  #define BUTTON_A  9
  #define BUTTON_B  8
  #define BUTTON_C  7
  #define WIRE Wire1
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
  #define WIRE Wire
#endif

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &WIRE);
MPX4250AP *turboSensor;

#define V3_3 0
double peak_boost = 0.0;
int it = 0;
int seconds = 0;
int adc_value = 0;
double display_boost = 0.0;

void setup() {
  Serial.begin(9600);

  Serial.println("Turbo Boost Startup");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3D for 128x64

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  //display.display();
  //delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize car sensors
  turboSensor = new MPX4250AP(A0, V3_3);
}

void loop() {
  display.clearDisplay();
  // Turbo
  double turboPressure = turboSensor->getPressure();
  // Hold peak pressure for 5 seconds
  if (seconds - peak_pressure_seconds > 5) {
    peak_boost = 0.0;
  }

  if (turboPressure > peak_boost) {
    peak_pressure_seconds = seconds;
    peak_boost = turboPressure;
  }

  // Read value every 500ms
  if (it % 5 == 0) {
    adc_value = analogRead(A0);
    display_boost = turboPressure;
  }

  // Center (text size 2)
  // Each letter 8px width
  // Each space between leter 2px width
  int turbo_x_px = (128/2)-((5*8+4*2)/2);
  display.setCursor(turbo_x_px, 0);
  display.setTextSize(2);
  display.println("Turbo");

  //display.drawPixel(64, 49, SSD1306_WHITE);
  //display.drawFastHLine(64, 50, 20, SSD1306_WHITE);
  
  // Gauge
  display.drawRect(10, 20, 108, 16, SSD1306_WHITE);
  // Needle
  int max_needle_value_px = 106;
  int zero_needle_value_px = 0;
  int needle_value_px = max(0.0, min(turboPressure, 0.7))/0.7*106;

  display.fillRect(11, 21, needle_value_px, 14, SSD1306_WHITE);

  // Inner values 
  // space_between each 0.1 bar
  int number_of_zero_one_bars = 7;
  double bar_space = 108/number_of_zero_one_bars;

  for(int i = 1; i<=6; i++) {
    display.drawPixel(10 + i*bar_space, 21, SSD1306_WHITE);
    display.drawPixel(10 + i*bar_space, 21+13, SSD1306_WHITE);
  }

  // Values (only change very 500ms)
  display.setTextSize(2);
  display.setCursor(12,40);
  display.print(display_boost, 1);
  display.setTextSize(1);
  display.println(" bar");
  
  display.setCursor(74,49);
  display.print("Peak: ");
  display.println(peak_boost, 1);

  display.setCursor(74,57);
  display.print("ADC: ");
  display.println(adc_value);
  
  // TODO: Out of bounds
  // display.drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)

  delay(100);
  if (it == 9) { it = 0; seconds += 1; } else { it += 1; }
  
  yield();
  display.display();
}


