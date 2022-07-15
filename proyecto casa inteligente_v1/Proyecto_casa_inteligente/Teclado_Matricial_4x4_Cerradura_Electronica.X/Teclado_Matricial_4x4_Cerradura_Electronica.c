/*===========================================================================================================================
 * Codigo tomado y adaptado del canal de YouTube: jorge APC
 * modificado por: Camila Andrea Quintero
 *                 Yimmer Mezu Castro 
 *                 Jhonatan Villegas 
 *                 Juan Carlos Camacho Muñoz
 ==========================================================================================================================*/
///////////////////////////////////////////////////   dht11 /////////////////////////////////////////////////////////////////
#include <pic18f4550.h>
#define Data_Out LATA0              /* assign Port pin for data*/
#define Data_In PORTAbits.RA0       /* read data from Port pin*/           
#define Data_Dir TRISAbits.RA0      /* Port direction */
#define _XTAL_FREQ 8000000          /* define _XTAL_FREQ for using internal delay */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DHT11_Start();
void DHT11_CheckResponse();
char DHT11_ReadData();

///////////////////////////////////    LDR ////////////////////////////////////////////////////////////////////////////////

#define LDR_sensor  TRISAbits.RA1     //asignamos el nombre LDR al pin RA1                                               
#define lampara     LATDbits.LATD0    // Asignamos el nombre "Lampara" al pin LATD0.                                     
unsigned char      buffer_ldr[16];    //alamcenar el mensaje a mostar en el lcd   

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <xc.h>                       // Incluimos todos los registros del PIC18F4550.
#include <stdint.h>                   // Incluimos esta librería para trabajar con variables enteras.
#include <stdio.h>                    // Incluimos esta librería para trabajar con perifericos de entrada/salida.
#include <string.h>                   // Incluimos esta librería para trabajar con cadenas de caracteres.
#include "Bits_Configuracion.h"       // Incluimos el archivo de cabecera para los fusibles.
#include "LCD_16x2_Library.h"         // Incluimos la librería LCD 16x2.
#include "KeyPad_4x4_Library.h"       // Incluimos la librería Keypad 4x4.

#define TRIS_Leds   TRISC             // Asignamos el nombre "TRIS_Leds" al registro TRISC.
#define LAT_Leds    LATC              // Asignamos el nombre "LAT_Leds" al registro LATC.
#define Led_Green   LATCbits.LATC0    // Asignamos el nombre "Led_Green" al pin RC0.
#define Led_Red     LATCbits.LATC1    // Asignamos el nombre "Led_Red" al pin RC1.
#define ventilador  LATCbits.LATC2    // Asignamos el nombre "Buzzer" al pin RC2.
#define Led_calor   LATCbits.LATC7    // Asignamos el nombre "led_calor" al pin RC7.

/*==========================================================================================================
 ===========================================================================================================*/

uint8_t Buffer_LCD[16];               // Arreglo de tipo entero 8 bits, Almacena el formato de las variables a mostrar en la pantalla LCD.
uint8_t Contador_Tecla=0;             // Variable de tipo entero,cuenta las veces que se pulsa una tecla. 
char K;                               // Variable de tipo caracter, aqui se almacena el valor de la tecla presionada.
char Password[5];                     // Arreglo de tipo caracter, aqui se almacena la contraseña generada por el usuario.
char clave[5]="2501";                 // Arreglo de tipo caracter, aqui se almacena la clave para ingresar.
unsigned long Tiempo_Apertura=3000;   // Tiempo en segundos que estará aperturada la puerta.
unsigned long Tiempo_Incorrecto=2000; // Tiempo en el que aparecerá el mensaje de contraseña incorrecta en la LCD.
unsigned long tiempo_bloqueo=20;      //Tiempo de bloqueo del usuario en segundos

/*==========================================================================================================
 ===========================================================================================================*/

void Configuracion_Registros (void);  // Función para configurar los registros del PIC18F4550 necesarios en este programa.
void dht11(void);                     // Funcion para administrar el confort de la casa
int LDR_funcion(void);                // Funcion para administar el ldr
void encender_lampara(int);           // Funcion para encender la lampara

/*==========================================================================================================
 ===========================================================================================================*/

void main(void) 
{
    char tiempo[30];                  //almacenamos el tiempo de bloqueo a mostrar en el lcd
    int  intentos =0 ;                //Variable para contar el numero de intentos
    OSCCON=0x72;
    Configuracion_Registros();        // Llamamos a la función "Configuracion_Registros".
    lcd_init();                       // Inicializamos la pantalla LCD 16x2.
    Keypad_Init();                    // Inicializamos el Keypad 4x4.
    LAT_Leds&=~((1<<0)|(1<<1)|(1<<2));// Inicialmente los leds y el buzzer estaran apagados.
    LAT_Leds&=~(1<<7);                // Inicialmente el pin RC7 estará desenergizado 
    
    Led_Red=1;                        //encendemos el led indicador de la cerradura ON=abierto off=cerrado
    lcd_clear();                      // Limpiamos la pantalla LCD.
    lcd_gotoxy(1,1);                  // Posicionamos el cursor en fila 1, columna 1.
    lcd_putc("  PROYECTO CASA");     // Mostramos el mensaje en la pantalla LCD.
    lcd_gotoxy(2,1);                  // Posicionamos el cursor en fila 2, columna 1.
    lcd_putc("   INTELIGENTE");     // Mostramos el mensaje en la pantalla LCD.   
    __delay_ms(2000);                 // Retardo de tiempo.
    lcd_gotoxy(1,1);                  // Posicionamos el cursor en fila 1, columna 1.
    lcd_putc("    CERRADURA   ");     // Mostramos el mensaje en la pantalla LCD.
    lcd_gotoxy(2,1);                  // Posicionamos el cursor en fila 2, columna 1.
    lcd_putc("   ELECTRONICA  ");     // Mostramos el mensaje en la pantalla LCD.   
    __delay_ms(2000);                 // Retardo de tiempo.
    lcd_clear();                      // Limpiamos la pantalla LCD.
    
    while(1)                          // Bucle infinito.
    {
        lcd_gotoxy(1,1);              // Posicionamos el cursor en fila 1, columna 1.
        lcd_putc("INGRESE PASSWORD"); // Mostramos el mensaje en la pantalla LCD.
        while(Contador_Tecla<4)       // Mientras la variable "Contador_Tecla" sea menor que 4
        {
            K=Keypad_Get_Char();      // Cargamos el valor de la tecla presionada en "K".
            if(K!=0)                  // Si "K" es diferente de 0, verifica si una tecla es presionada.  
            {
                Password[Contador_Tecla]=K;    // Almacena las teclas presionadas (valores de K) en el arreglo "Password".
                lcd_gotoxy(2,1+Contador_Tecla);// Posicionamos el cursor en fila 1, columna respectiva.
                lcd_write_char('*');  // Mostramos el mensaje en la pantalla LCD.
                Contador_Tecla++;     // Incrementamos en una unidad el valor de "Contador_Tecla".
            }
        }
        __delay_ms(200);              // Retardo de tiempo.
        lcd_clear();                  // Limpiamos la pantalla LCD.
        
        if(!strcmp(Password,clave))// La función "strcmp" sirve para comparar dos cadenas y así saber si son iguales o son diferentes. 
        
        {                             // Comparamos si la contraseña es correcta.
            lcd_gotoxy(1,1);          // Posicionamos el cursor en fila 1, columna 1.
            lcd_putc(" clave correcta "); // Mostramos el mensaje en la pantalla LCD.
            lcd_gotoxy(2,1);          // Posicionamos el cursor en fila 2, columna 1.
            lcd_putc("   BIENVENIDO   "); // Mostramos el mensaje en la pantalla LCD.
            Led_Green=1;              // Ponemos a nivel alto el pin RC0.
            Led_Red=0;                // Ponemos a nivel bajo el pin RC1.
            __delay_ms(Tiempo_Apertura);  // Retardo de tiempo, apertura de puerta.
            Led_Green=0;              // Ponemos a nivel bajo el pin RC0.
            Led_Red=1;                // Ponemos a nivel alto el pin RC1.
            dht11();                  //llamamos a la funcion confort 
  ///////////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////// 
        }
        else 
        {
            intentos+=1;                    //incrementamos el numero de intentos
            lcd_gotoxy(1,1);                // Posicionamos el cursor en fila 1, columna 1.
            lcd_putc("     CLAVE     ");  // Mostramos el mensaje en la pantalla LCD.
            lcd_gotoxy(2,1);                // Posicionamos el cursor en fila 2, columna 1.
            lcd_putc("   INCORRECTA   ");   // Mostramos el mensaje en la pantalla LCD.
            Led_Green=0;                    // Ponemos a nivel bajo el pin RC0.
            __delay_ms(Tiempo_Incorrecto);  // Retardo de tiempo, apertura de puerta.
           
            
            if(intentos==3)                 //bloqueamos al usuario si intentos igual 3
            {
                lcd_clear();
                for(int j=tiempo_bloqueo;j>=0;j--){
                lcd_gotoxy(1,1); 
                lcd_putc("  MAX INTENTOS");
               
                lcd_gotoxy(2,1);    
                sprintf(tiempo,"  BLOQUEADO %ds",j);
                lcd_putc(tiempo);        
                __delay_ms(1000);
                lcd_clear();
                }
                break; // salimos del bucle while
            }
        }
        Contador_Tecla=0;                   // Reiniciamos el valor de la variable "Contador_Tecla".
        //Led_Green=0;                        // Ponemos a nivel bajo el pin RC0.
        //Led_Red=0;                    // Ponemos a nivel bajo el pin RC1.
        Led_calor=0;
        lcd_clear();                  // Limpiamos la pantalla LCD.
    }
    return;
}

/*==========================================================================================================
 ===========================================================================================================*/

void Configuracion_Registros (void) // Función para configurar los registros del PIC18F4550 necesarios en este programa.
{
    ADCON1bits.PCFG=0b1111;         // Deshabilitamos las entradas analógicas de los puertos A y B.
    TRIS_Leds&=~((1<<0)|(1<<1)|(1<<2));// Configuramos los pines RC0, RC1 y RC2 como salidas digitales. 
    TRIS_Leds&=~(1<<7);             // Configuramos el pin RC7 como salida digital. 
}

//////////////////////////////////////////////////////////////////////////////////////
void dht11(){
    int ldr_ouput=0;                  //allmacena la salida del ldr poca luz=1 , luz alta=0
    Configuracion_Registros();        // Llamamos a la función "Configuracion_Registros".
    lcd_init();                       // Inicializamos la pantalla LCD 16x2.
    char RH_Decimal,RH_Integral,T_Decimal,T_Integral;
    char Checksum;
    char value[10]; 
    while(1)
    {
    ldr_ouput=LDR_funcion();     
    DHT11_Start();                  /* send start pulse to DHT11 module */
    DHT11_CheckResponse();          /* wait for response from DHT11 module */ 
    
    RH_Integral = DHT11_ReadData(); /* read Relative Humidity's integral value */
    RH_Decimal = DHT11_ReadData();  /* read Relative Humidity's decimal value */
    T_Integral = DHT11_ReadData();  /* read Temperature's integral value */
    T_Decimal = DHT11_ReadData();   /* read Relative Temperature's decimal value */
    Checksum = DHT11_ReadData();    /* read 8-bit checksum value */
    
    encender_lampara(ldr_ouput);
    
    if(T_Integral<=20)
    {   
        ventilador=1; //encendemos el ventilador
        Led_calor=1; //encedemos el calefactor representado por un led
          lcd_clear();
        lcd_gotoxy(1,1);
        lcd_putc("   Temp baja");
        lcd_gotoxy(2,1);
        sprintf(value,"Temp:%d",T_Integral);
        lcd_putc(value);
        lcd_gotoxy(2,8); 
        sprintf(value,"  Hum:%d",RH_Integral,"%");
        lcd_putc(value);
        __delay_ms(3000);      
    }
    else if(T_Integral>=30)
    {   
        Led_calor=0;    //apagamos el led_calor
        ventilador=1;   //encendemos el ventilador
        lcd_clear();
        lcd_gotoxy(1,1);
        lcd_putc("   Temp alta");
        lcd_gotoxy(2,1);
        sprintf(value,"Temp:%d",T_Integral);
        lcd_putc(value);
        lcd_gotoxy(2,8); 
        sprintf(value,"  Hum:%d",RH_Integral,"%");
        lcd_putc(value);
        __delay_ms(3000);  
    }
    else
    { 
        Led_calor=0;                //apagamos el calefactor
        ventilador=0;               //apagamos el ventilador 
        lcd_clear();
        sprintf(value,"Temp:%d",T_Integral);
        lcd_putc(value);
        lcd_gotoxy(2,1); 
        sprintf(value,"humedad: %d",RH_Integral,"%");
        lcd_putc(value);
         __delay_ms(500);
        
    }
     
    }
}
////////////////////////////////    DHT11 FUNCIONES ///////////////////////////////////////
char DHT11_ReadData()
{
  char i,data = 0;  
    for(i=0;i<8;i++)
    {
        while(!(Data_In & 1));      /* wait till 0 pulse, this is start of data pulse */
        __delay_us(30);         
        if(Data_In & 1)             /* check whether data is 1 or 0 */    
          data = ((data<<1) | 1); 
        else
          data = (data<<1);  
        while(Data_In & 1);
    }
  return data;
}

void DHT11_Start()
{    
    Data_Dir = 0;       /* set as output port */
    Data_Out = 0;       /* send low pulse of min. 18 ms width */
    __delay_ms(18);
    Data_Out = 1;       /* pull data bus high */
    __delay_us(20);
    Data_Dir = 1;       /* set as input port */    
//    LED = 14;
}

void DHT11_CheckResponse()
{
    while(Data_In & 1);     /* wait till bus is High */     
    while(!(Data_In & 1));  /* wait till bus is Low */
    while(Data_In & 1);     /* wait till bus is High */
}
////////////////////////////////////////////////////////////////////////////////
int LDR_funcion()
{
    int ldr_data=0;   
    LDR_sensor=1;           //configuramos como entrada 
    
    ldr_data=PORTAbits.RA1;  //leemos el pin y guardamos en la variable ldr_data
    
    return ldr_data;           
}
////////////////////////////////////////////////////////////////////////////////
void encender_lampara(int ldr_output)
{
    if(ldr_output==0) //si hay poca luz encendemos lampara
    {
        lampara=1;
    }
    else            //si hay luz apagamos lampara
    {
        lampara=0;
    }
}
////////////////////////////////////////////////////////////////////////////////


