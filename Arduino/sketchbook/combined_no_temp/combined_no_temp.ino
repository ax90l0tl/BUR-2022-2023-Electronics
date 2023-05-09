// Messages and Libraries
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//ROS
#include <ros.h>
//ROS message files
#include <rov/cmd_thruster.h>
//#include <geometry_msgs/Pose.h>
#include <std_msgs/Int8.h>
//#include <Adafruit_BNO08x.h>

/*********************************
------------IMU-Setup-------------
*********************************/
// For SPI mode, we also need a RESET 
// but not for I2C or UART
//#define BNO08X_RESET -1

//Adafruit_BNO08x  bno08x(BNO08X_RESET);
//sh2_SensorValue_t sensorValue;

/*********************************
---------Temp-Sensor-Setup--------
*********************************/
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 54
//// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
//// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
//// arrays to hold device address
DeviceAddress insideThermometer;

/*********************************
--------Motor-Button-Setup---------
*********************************/
//Motor and Button Setup
Servo motor[6];
const byte motor_pin_begin = 6;
const byte util_pin_begin = 2;

//ROS Node
ros::NodeHandle  nh;

//callback function for cmd_thruster
void msg_Cb( const rov::cmd_thruster &msg){
  for(int i =0; i< 6; i++){
    motor[i].writeMicroseconds(msg.thruster_val[i]*400 + 1500);
  }
  
  for(int i =0; i< 4; i++){
      digitalWrite(i+util_pin_begin, msg.buttons[i]);    
  }
}

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  float tempF = sensors.getTempF(deviceAddress);
  if(tempF == DEVICE_DISCONNECTED_C) 
  {
    return(-255);
  }
  return(tempF);
}

//geometry_msgs::Pose pose_msg;
std_msgs::Int8 temp_msg;
//ros::Publisher rotationPublisher("imu_output", &pose_msg);
ros::Subscriber<rov::cmd_thruster> sub("cmd_thruster", &msg_Cb );
ros::Publisher tempPublisher("temperature", &temp_msg);

void setup()
{ 
  //IMU Setup
//  if (!bno08x.begin_I2C()) {
//    while (1) { delay(10); }
//  }
  nh.getHardware()->setBaud(115200);
//  bno08x.enableReport(SH2_GYRO_INTEGRATED_RV);
//  bno08x.enableReport(SH2_ACCELEROMETER);

  //Temp Sensor Setup
  sensors.begin();
  sensors.getDeviceCount();
  sensors.getAddress(insideThermometer, 0);
  sensors.setResolution(insideThermometer, 9);
  
  //ROS Setup
  nh.initNode();
  nh.subscribe(sub);
//  nh.advertise(rotationPublisher);
  nh.advertise(tempPublisher);

  //Motor Assignments
  for(int i = 0; i<6; i++){
    motor[i].attach(i+motor_pin_begin);
    motor[i].writeMicroseconds(1500); // 1500 for the thrusters  
  }
    for(int i = 0; i<4; i++){
      pinMode(i+util_pin_begin, OUTPUT);
  }
}  

void loop()
{ 
//  if (! bno08x.getSensorEvent(&sensorValue)) {
//    return;
//  }
//  switch (sensorValue.sensorId) {   
//    case SH2_GYRO_INTEGRATED_RV:
//      pose_msg.orientation.x = sensorValue.un.gyroIntegratedRV.i;
//      pose_msg.orientation.y = sensorValue.un.gyroIntegratedRV.j;
//      pose_msg.orientation.z = sensorValue.un.gyroIntegratedRV.k;
//      pose_msg.orientation.w = sensorValue.un.gyroIntegratedRV.real;
//      break;
//    case SH2_ACCELEROMETER:
//      pose_msg.position.x = sensorValue.un.accelerometer.x;
//      pose_msg.position.y = sensorValue.un.accelerometer.y;
//      pose_msg.position.z = sensorValue.un.accelerometer.z;
//      break;
//  }
  sensors.requestTemperatures(); // Send the command to get temperatures
    // It responds almost immediately. Let's print out the data
  temp_msg.data = printTemperature(insideThermometer);
  tempPublisher.publish(&temp_msg);
//  rotationPublisher.publish( &pose_msg ); 
  nh.spinOnce();
  
  delay(10);
}
