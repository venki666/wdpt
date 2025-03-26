#include <M5StickCPlus.h>
#undef ESP32
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/String.h>
#include <std_msgs/Char.h>
#include <std_msgs/Empty.h>
#include <sensor_msgs/Range.h>
#include <sensor_msgs/Illuminance.h>
#include <std_msgs/ColorRGBA.h>
#define ESP32
#include <Wire.h>
#include <Adafruit_VL6180X.h>
#include <Adafruit_NeoPixel.h>

ros::NodeHandle nh;
sensor_msgs::Range range_msg;
sensor_msgs::Illuminance lux_msg;
std_msgs::ColorRGBA light_msg;
std_msgs::Empty trigger_msg;

ros::Publisher pub_range("/range_data", & range_msg);
ros::Publisher pub_lux("/lux_data", & lux_msg);
ros::Publisher pub_light("/light_data", & light_msg);
void messageCb(const std_msgs::Empty & drop_msg);
ros::Subscriber <std_msgs::Empty> drop_sub("drop", messageCb);

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 26 // For Neopixel
#define TOUT       36 // for dropper trigger
//#define TOUT 10 // Red LED for testing purpose
#define PIN2 37 // Button for manual trigger

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 12 // Popular NeoPixel ring size
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

Adafruit_VL6180X vl = Adafruit_VL6180X();
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint8_t range;
uint8_t status;
unsigned long range_timer;

uint32_t lux;
uint32_t variance = 0;



void messageCb(const std_msgs::Empty & drop_msg) {
  nh.loginfo("Command/Triggered");
  digitalWrite(TOUT, HIGH);
  delay(1000); // reduce based on droplet size
  digitalWrite(TOUT, LOW);
}

void setup() {
  M5.begin();
  Wire.begin();
  nh.initNode();
  nh.advertise(pub_range);
  nh.advertise(pub_lux);
  nh.advertise(pub_light);
  nh.subscribe(drop_sub);
  // wait controller to be connected
  while (!nh.connected()) {
    nh.spinOnce();
  }
  // Wire.begin();
  nh.loginfo("Sensor test!");

  pinMode(PIN2, INPUT);
  pinMode(TOUT, OUTPUT);
  digitalWrite(TOUT, LOW);

  if (!vl.begin()) {
    nh.loginfo("VL6180 Sensor not found");
    while (1);
  } else
    nh.loginfo("VL6180 Sensor found");

  nh.loginfo("vl6810x API serial node started");
  // fill static range message fields
  range_msg.radiation_type = sensor_msgs::Range::INFRARED;
  range_msg.header.frame_id = "fixed_frame";
  range_msg.field_of_view = 0.44; //25 degrees
  range_msg.min_range = 10.0;
  range_msg.max_range = 255.0;

  lux_msg.header.frame_id = "fixed_frame";

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(100);
  pixels.clear(); // Set all pixel colors to 'off'

  light_msg.r = 255;
  light_msg.g = 255;
  light_msg.b = 255;
  light_msg.a = 100;

}
int lastState = HIGH;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
void loop() {
  M5.update();
  if ((millis()-range_timer) > 50)
  {
   currentState = digitalRead(PIN2);
   if (lastState == HIGH && currentState == LOW)
   {
    nh.loginfo("Button/Triggered \n");
    digitalWrite(TOUT, HIGH);
    delay(1000);
    digitalWrite(TOUT, LOW);
   }
  // save the the last state
  lastState = currentState;

  range = vl.readRange();
  status = vl.readRangeStatus();
  if (status == VL6180X_ERROR_NONE) { // phase failures have incorrect data
    range_msg.range = range; // in mm 
    range_msg.header.stamp = nh.now();
    pub_range.publish( & range_msg);
  } else {
    nh.logwarn("TIMEOUT/Out of range"); // if out of range, send fixe max value
    range_msg.range = 255; // in mm 
    range_msg.header.stamp = nh.now();
    pub_range.publish( & range_msg);
  }
  float lux = vl.readLux(VL6180X_ALS_GAIN_5);
  lux_msg.illuminance = lux;
  lux_msg.variance = 0;
  pub_lux.publish( & lux_msg);
  long litn = map(lux,0,4000,0,255);
  pixels.setBrightness(litn);
  pixels.fill(pixels.Color(255, 255, 255), 0, 12);
  pixels.show();
  light_msg.a = litn;
  pub_light.publish( & light_msg);
  range_timer =  millis();    
 }
 nh.spinOnce();
}