/*==========================================================================================================
 ===========================================================================================================*/
#ifndef XC_KEYPAD_4x4_LIBRARY_H
#define	XC_KEYPAD_4x4_LIBRARY_H

/*==========================================================================================================
 ===========================================================================================================*/
#include <xc.h> // include processor files - each processor file is guarded.  

#define _XTAL_FREQ 8000000

#define ROW0_PIN LATBbits.LATB7
#define ROW1_PIN LATBbits.LATB6
#define ROW2_PIN LATBbits.LATB5
#define ROW3_PIN LATBbits.LATB4
#define COL0_PIN PORTBbits.RB3
#define COL1_PIN PORTBbits.RB2
#define COL2_PIN PORTBbits.RB1
#define COL3_PIN PORTBbits.RB0

#define ROW0_DIR TRISBbits.TRISB7
#define ROW1_DIR TRISBbits.TRISB6
#define ROW2_DIR TRISBbits.TRISB5
#define ROW3_DIR TRISBbits.TRISB4
#define COL0_DIR TRISBbits.TRISB3
#define COL1_DIR TRISBbits.TRISB2
#define COL2_DIR TRISBbits.TRISB1
#define COL3_DIR TRISBbits.TRISB0

/*==========================================================================================================
 ===========================================================================================================*/
void Keypad_Init(void);
char Keypad_Get_Char(void);

/*==========================================================================================================
 ===========================================================================================================*/
#endif	/* XC_KEYPAD_4x4_LIBRARY_H */

