

// ARDUINO MPU6050 to NMEA pitch & roll per opencpn

#include <SPI.h>
#include <Wire.h>
#include <PString.h>
#define MPU 0x68  // I2C address of the MPU-6050

double AcX,AcY,AcZ;
int Pitch, Roll;

float r = M_PI/180.0f; // degrees to radians  
float d = 180.0f/M_PI; // radians to degrees  


void setup(){
  Serial.begin(4800);
  init_MPU(); // Inizializzazione MPU6050
}

// calculate checksum function (thanks to https://mechinations.wordpress.com)  
 byte checksum(char* str)   
 {  
   byte cs = 0;   
   for (unsigned int n = 1; n < strlen(str) - 1; n++)   
   {  
     cs ^= str[n];  
   }  
   return cs;  
 }

void loop()
{
  FunctionsMPU(); // Acquisisco assi AcX, AcY, AcZ.
    
  Roll = FunctionsPitchRoll(AcX, AcY, AcZ);   //Calcolo angolo Roll
  Pitch = FunctionsPitchRoll(AcY, AcX, AcZ);  //Calcolo angolo Pitch

/*
  Serial.print("Pitch: "); Serial.print(Pitch);
  Serial.print("\t");
  Serial.print("Roll: "); Serial.print(Roll);
  Serial.print("\n");
*/
  char shrSentence [50];
  byte csp;
  PString strp(shrSentence, sizeof(shrSentence));
  strp.print("$IIXDR,A,"); 
  strp.print(lround(Roll)); 
  strp.print(",D,ROLL,A,");
  strp.print(lround(Pitch)); 
  strp.print(",D,PTCH,0*");
  csp = checksum(shrSentence);
  if (csp < 0x10) strp.print('0');  
  strp.print(csp, HEX);  
  Serial.println(shrSentence);
  delay(1000);
}

void init_MPU(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  delay(1000);
}

//Funzione per il calcolo degli angoli Pitch e Roll
double FunctionsPitchRoll(double A, double B, double C){
  double DatoA, DatoB, Value;
  DatoA = A;
  DatoB = (B*B) + (C*C);
  DatoB = sqrt(DatoB);
  
  Value = atan2(DatoA, DatoB);
  Value = Value * 180/3.14;
  
  return (int)Value;
}

//Funzione per l'acquisizione degli assi X,Y,Z del MPU6050
void FunctionsMPU(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,6,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
}
