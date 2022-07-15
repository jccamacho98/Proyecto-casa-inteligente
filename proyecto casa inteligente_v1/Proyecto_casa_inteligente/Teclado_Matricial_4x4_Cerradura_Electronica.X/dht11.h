/* #include "dht11.h"
*
* Creada por: Ing. Abiezer Hernandez O.
* Fecha de creacion: 10/11/2020
* Electronica y Circuitos
*
*/

#include <xc.h>
#define _XTAL_FREQ 48000000

#define DHT11_PIN_PORT 	PORTBbits.RB0
#define DHT11_PIN_LAT 	LATBbits.LB0
#define DHT11_PIN_DIR 	TRISBbits.TRISB0

void DHT11_Start(void);
void DHT11_Response(void);
int DHT11_Read_Byte(void);
short DHT11_Read_Data(float *temp, float *hum);
unsigned DHT11_Join_Data(unsigned h, unsigned l);