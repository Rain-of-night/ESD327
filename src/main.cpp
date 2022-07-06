#include <Arduino.h>

//引脚定义
#define LIGHT_POWER 26
#define ADC_LIGHT 39

#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
#define SDMMC_DATA2  12
#define SDMMC_DATA3  13
#define SDMMC_CMD    15
#define SDMMC_CLK    14
#define SDMMC_DATA0  2
#define SDMMC_DATA1  4
#define SDMMC_CD     25

//#include <SD.h>
//#include <SPI.h>
//#define SDCARD_SS 13
//#define SDCARD_CLK 14
//#define SDCARD_MOSI 15
//#define SDCARD_MISO 2
//SPIClass sdSPI(VSPI);

#include <Wire.h>
// SHT30 I2C address is 0x44(68)
#define Addr 0x44

void setup() 
{
  Serial.begin(9600);
  Serial.println();

//光敏电阻测量初始化
  //设置光敏电阻测量系统控制引脚
  pinMode(LIGHT_POWER, OUTPUT);
  //设置光敏电阻电压采集，采样位数16bit
  adcAttachPin(ADC_LIGHT);
  adcStart(ADC_LIGHT);
  analogReadResolution(16);
  
//SHT30  I2C初始化，作为主机
  Wire.begin();

//挂载SD卡文件系统
  if (!SD_MMC.begin())
  {
    Serial.println("存储卡挂载失败");
    return;
  }
  else
  {
    SD_MMC.end();
  }
};

void loop()
{
//光敏电阻测量
  //LIGHT_POWER=LOW 拉低为光敏电阻测量系统上电
  digitalWrite(LIGHT_POWER, LOW);
  delay(500);
  //换算电压值，16bit的65535对应满量程的3.3V
  float Light_RES = analogRead(ADC_LIGHT) * 3.3 / 65535;
  delay(500);
  //输出测试数据
  Serial.print("LightRES Volage: ");
  Serial.print(Light_RES);
  Serial.println("V");
  delay(500);

//SHT30 温湿度计测量
  //开始 I2C 通信
  Wire.beginTransmission(Addr);
  //发送测量命令
  Wire.write(0x2C);
  Wire.write(0x06);
  //停止I2C通信
  Wire.endTransmission();
  delay(500);
  // 接收 6 bytes 数据
  Wire.requestFrom(Addr, 6);
  // 读 6 bytes 数据
  // cTemp msb, cTemp lsb, cTemp crc, humidity msb, humidity lsb, humidity crc
  unsigned int data[6];
  if (Wire.available() == 6)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  }
  // 转换数据格式
  float cTemp = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
  float humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
  // Output data to serial monitor
  Serial.print("Relative Humidity : ");
  Serial.print(humidity);
  Serial.println(" %RH");
  Serial.print("Temperature in Celsius : ");
  Serial.print(cTemp);
  Serial.println(" C");


//SD卡测试
  


  Serial.println(" ");
  delay(10000);
};