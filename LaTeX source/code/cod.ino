#include <Arduino_FreeRTOS.h>
#include <semphr.h>  
#include "Simpletimer.h"
SemaphoreHandle_t SemPLIN, SemNOAPTE, SemSB, SemGOL;


void TaskLightRead( void *pvParameters );
void TaskProd( void *pvParameters );
void TaskCons( void *pvParameters );
long b[5] = {0, 0, 0, 0, 0}, z[5] = {0, 0, 0, 0, 0}, zn[5] = {0, 0, 0, 0, 0}, aux[5] = {0, 0, 0, 0, 0}, bc[5] = {0, 0, 0, 0, 0}, bn[5] = {0, 0, 0, 0, 0};
float v[4] = {0, 0, 0, 0}, vint[4] = {0, 0, 0, 0}, auxv[4] = {0, 0, 0, 0};
int c[4] = {0, 0, 0, 0}, lum[3] = {0, 0, 0}, f[5] = {0, 0, 0, 0, 0}, zf[5] = {0, 0, 0, 0, 0};
int timp = 0;
long t[4] = {0, 0, 0, 0};
int d = 14;
void setup() {

  Serial.begin(19200);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);    
    pinMode(7, OUTPUT);
    pinMode(49, INPUT);
    pinMode(50, INPUT);
    pinMode(51, INPUT);
    pinMode(52, INPUT);
    pinMode(53, INPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  
  if ( SemPLIN == NULL )  
  {
    SemPLIN = xSemaphoreCreateBinary(); 
    if ( ( SemPLIN ) != NULL )
      xSemaphoreGive( ( SemPLIN ) ); 
  }

  if ( SemGOL == NULL ) 
    SemGOL = xSemaphoreCreateBinary();  

  if ( SemSB == NULL )  
  {
    SemSB = xSemaphoreCreateBinary();  
    if ( ( SemSB ) != NULL )
      xSemaphoreGive( ( SemSB ) ); 
  }

  if ( SemNOAPTE == NULL )  
    SemNOAPTE = xSemaphoreCreateBinary(); 

  xTaskCreate(TaskLightRead, "Light", 128, NULL, 1, NULL );
    
  xTaskCreate(TaskProd, "Producator",  128,  NULL,  1, NULL);

  xTaskCreate(TaskCons, "Consumator", 128, NULL, 1, NULL);

}
void loop() {
}
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskProd( void *pvParameters __attribute__((unused)) )  
{
  for (;;) 
  { 
    xSemaphoreTake(SemNOAPTE, portMAX_DELAY);

    bc[0] = digitalRead(49);
    bc[1] = digitalRead(50);
    bc[2] = digitalRead(51);
    bc[3] = digitalRead(52);
    bc[4] = digitalRead(53);

    xSemaphoreTake(SemPLIN,portMAX_DELAY);
    xSemaphoreTake(SemSB,portMAX_DELAY);

     for(int in = 0; in < 5; in ++){
        if (f[in] == 1){
          b[in]= bn[in];
          bn[in] = 0;
          f[in] = 2;
        }
        if (aux[in] - bc[in] == 1) {
            if (in <= 3) {
              c[in] ++;
            }
            if ((in >= 1) && c[in-1]>0){
              c[in - 1] --;
             if ((bn[in-1] != 0) &&(c[in - 1] == 1)){
                  f[in-1] = 1;
               } 
           }      
            if (in == 4)
              b[in] = millis();
            else if ((in <= 3) && (c[in] == 1)){ 
                  b[in] = millis();
            }
            else if ((in <= 3) && (c[in] > 1)){
                bn[in] = millis();
            }
         }
          aux[in] = bc[in];      
      }
          xSemaphoreGive(SemSB);
          xSemaphoreGive(SemGOL);
        
          vTaskDelay(1);  
   }
}

void TaskCons( void *pvParameters __attribute__((unused)) )  
{
  for (;;)
  {

    xSemaphoreTake(SemGOL, portMAX_DELAY);
    // Transfer date buffer
    xSemaphoreTake(SemSB, portMAX_DELAY);
    for (int i = 0; i < 5; i ++) { 
      z[i] = b[i];
      zn[i] = bn[i];
    }

    xSemaphoreGive(SemSB);
    xSemaphoreGive(SemPLIN);
    //  CALCULE
    for (int i = 0; i < 4; i ++) {
      if (i==3)
        t[i] = z[i + 1] - z[i];
      else if (c[i + 1] == 1)
        t[i] = z[i + 1] - z[i];
      else if (c[i + 1] == 2)
        t[i] = zn[i + 1] - z[i];
      
      if (t[i] != 0)
        vint[i] = 10*d * 3.6/((float)t[i]);
      else if (t[i] == 0) 
        vint[i] = 0;
      if (auxv[i] != vint[i]){
        if ((f[i] != 2)&&(vint[i]>=0)){
          v[i] = vint[i];
          Serial.print("  t[");Serial.print(i);Serial.print("] = ");
          Serial.print(t[i]);
          Serial.print("  ||||");
          for (int j = 0; j < 4; j ++) {
            if (j==i){
              Serial.print("<v[");Serial.print(j);Serial.print("] = ");
              Serial.print(v[j]);
              Serial.print(">");
            }
            else {
              Serial.print(" v[");Serial.print(j);Serial.print("] = ");
              Serial.print(v[j]);     
              Serial.print(" ");
            }
           }
        Serial.println("");
        }
        else {Serial.print("  z[");Serial.print(i);Serial.print("] = ");
          Serial.print(z[i]);
          Serial.print("  zn[");Serial.print(i);Serial.print("] = ");
          Serial.print(zn[i]);
          Serial.print("  z[");Serial.print(i+1);Serial.print("] = ");
          Serial.print(z[i+1]);
          Serial.print("  zn[");Serial.print(i+1);Serial.print("] = ");
          Serial.print(zn[i+1]);
          Serial.print("  t[");Serial.print(i);Serial.print("] = ");
          Serial.print(t[i]);
          Serial.print(" ***");
        Serial.println("");
          f[i] = 0;
        }
      }  
      auxv[i] = vint[i];
      
      if (i > 0){ 
        if (lum[i - 1] < 3)
          lum[i - 1] = 3;
        if ((c[i] == 0) && ( lum[i - 1] > 3)) {
            if (timp == 0) {
              if (lum[i - 1] > 8)
                lum[i - 1] = lum[i - 1] * 0.7;
              else
                lum[i - 1] --;
            }
            timp ++;
            timp = timp % 5;      
        }
        else if (c[i] == 0) {     
        }
        else if (c[i] == 1){
          if (v[i-1] > 4){ 
            lum[i - 1] = 255;      
          }
          else if (v[i-1] > 1){ 
             if (lum[i - 1] > 150)
               lum[i - 1] =  lum[i - 1] - 2;
             else 
               lum[i - 1] = 150;
          }           
          else if (v[i-1] > 0){ 
             if (lum[i - 1] > 40)
               lum[i - 1] =  lum[i - 1] - 2;
             else 
               lum[i - 1] = 40;
          }
         }
         else if (c[i] == 2){
          if ((v[i-1] > 4)&& (lum[i - 1] < 150)){ 
            lum[i - 1] = 255;      
          }
          else if ((v[i - 1] > 1) && (lum[i - 1] < 150)){ 
            lum[i - 1] = 150;
          }           
         }  
         analogWrite(4 + i, lum[i - 1]);
      }


   }
  }
}
void TaskLightRead( void *pvParameters __attribute__((unused)) ) 
{

  for (;;)
  {
    int FotoRez = analogRead(A0);
      if (FotoRez>80)
        xSemaphoreGive( SemNOAPTE );
      else {
        Serial.println(FotoRez);
        analogWrite (5, 0);
        analogWrite (6, 0);
        analogWrite (7, 0);
        timp = 0;
        b[0] = 0; b[1] = 0; b[2] = 0; b[3] = 0; b[4] = 0;
        f[0] = 0; f[1] = 0; f[2] = 0; f[3] = 0; f[4] = 0;
        bn[0] = 0; bn[1] = 0; bn[2] = 0; bn[3] = 0; bn[4] = 0;
        c[0] = 0; c[1] = 0; c[2] = 0; c[3] = 0;
        lum[0] = 0; lum[1] = 0; lum[2] = 0;
        v[0] = 0; v[1] = 0; v[2] = 0; v[3] = 0;
        vint[0] = 0; vint[1] = 0; vint[2] = 0; vint[3] = 0;
        t[0] = 0; t[1] = 0; t[2] = 0; t[3] = 0;
        aux[0] = 0; aux[1] = 0; aux[2] = 0; aux[3] = 0; aux[4] = 0;
        auxv[0] = 0; auxv[1] = 0; auxv[2] = 0; auxv[3] = 0;
      }
  }
}