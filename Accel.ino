#include <SparkFun_ADXL345.h>
#include<string.h>


char b[25];
short int c;
 int x, y, z;
int  base = 800, xb = 0, yb = 0, zb = 0, xa = 0, ya = 0, za = 130;


ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION


void setup() {

  Serial.begin(115200);                 
  delay(5000);
  Serial.print("AT+GPS=1\r\n");       //Enabling GPS 
  delay(200);
  Serial.print("AT+CGATT=1\r\n");     //Enabling GPRS
  delay(100);



  adxl.powerOn();                     //Turning on ADXL345

  adxl.setRangeSetting(4);           //Setting the range of measurement on 4g
  

  adxl.setSpiBit(0);                  //For SPI Connection

}

short int ax = 0;
short int ay = 0;
short int az = 0;
char lat[9], lon[9];


void loop() {

  Serial.print("AT+GPS=1\r\n");
  delay(100);
  Serial.print("AT+GPSRD=1\r\n");   //Reading GPS Data Every 1 Second to Ensure GPS Connection
  delay(8000);
  Serial.print("AT+GPSRD=0\r\n");   //Disable Reading
  c = 0;
  while (Serial.available() > 0)
  {
    b[c] = Serial.read();
    c += 1;
    if (c == 25) c =0 ;
  }


  Serial.print("AT+LOCATION=2\r\n");  //Getting Location in Longitude and Latitude ( If GPS is Not Connected Shows "GPS NOT FIXED")
  delay(100);
  for ( int i = 0; i < 9; i ++){      //If GPS Is not Fixed
      lon[i] = '0';
      lat[i] = '0';
  }
  c = 0;
  while (Serial.available() > 0) {
    
    b[c] = Serial.read();
    c += 1;
    if ( c == 25 ) c =0;
    
    
  }
  if ((b[0]>=48) && (b[0]<= 57)) {
     for ( int i = 0; i <9 ; i++){
      lon[i] = b[i];
      lat[i] = b[10+i];
     }
  }
 






  // Accelerometer Readings

  adxl.readAccel(&x, &y, &z);      //Getting Data from ADXL345


  Serial.print("AT+CIPSTART=\"TCP\",\"81.31.169.13\",8080\r\n");    //Connecting to the Server
  delay(2000);
  c = 0;
  while (Serial.available() > 0) {
    
    b[c] = Serial.read();
    c += 1;
    if ( c == 25 ) c =0;
    
  }

  // C is for compuning the length of the JSON data we want to send since ATMega16 deos not support STRING  
  c = 73 + 18;
  if ((x-xa) < 0) c += 1;
  if ((y-ya) < 0) c += 1;
  if ((z-za) < 0) c += 1;

  c += 3;
  if (abs(x-xa) >9) c+=1;
  if (abs(x-xa) >99) c+=1;
  if (abs(y-ya) >9) c+=1;
  if (abs(y-ya) >99) c+=1;
  if (abs(z-za) >9) c+=1;
  if (abs(z-za) >99) c+=1;
  
  Serial.print("AT+CIPSEND\r\n");     //Start Sending Data
  delay(800);
  Serial.print("POST /api/v1/addData HTTP/1.1\r\n");
  Serial.print("Host: 81.31.169.13\r\n");
  Serial.print("Content-Type: application/json\r\n");
  delay(100);
  Serial.print("Content-Length:");
  //  Serial.print(json.length());
  Serial.print(c);
  //  Serial.print(strlen(token));
  Serial.print("\r\n\r\n");
  //  Serial.print(json);
  
  // Start Seding in JSON Format 
  Serial.print( "{\"token\":\"pc885qf5bpywjiu8\",\"lon\":\"");
  delay(100);
  for (int i =0; i<9; i++)
    Serial.print(lon[i]);
  Serial.print("\",\"lat\":\"");
  for (int i =0; i<9; i++)
    Serial.print(lat[i]);
  Serial.print("\",\"a_x\":\"");
  Serial.print(x-xa);
  Serial.print("\",\"a_y\":\"");
  Serial.print(y-ya);
  Serial.print("\",\"a_z\":\"");
  Serial.print(z-za);
  Serial.print( "\"}");


  delay(100);
  Serial.print(''); // This character is "CTRL + Z" And Is Necessary for Indicating That the Data We Want To Send Ends Here
  delay(500);
   c = 0;
  while (Serial.available() > 0) {
    
    b[c] = Serial.read();
    c += 1;
    if ( c == 25 ) c =0;
    
  }

  inact();    // This is the mentioned function for making the base variables for Accelaration. If data won't change that much in a period of Time the base will be updated 


}


void inact() {

  base = base + abs(x - xb) + abs(y - yb) + abs(z - zb) - 10;
  xb = x;
  yb = y;
  zb = z;
  if (base < 0 ) {
    base = 800;
    xa = xb;
    ya = yb;
    za = zb;
  }
  if (base > 800)
    base = 800;

}
