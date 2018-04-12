/*****************************************************************************
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME.: keyboard.c
 *
 * PROJECT....: EMP_ASS5
 *
 * DESCRIPTION: See module specification file (.h-file).
 *
 * Change Log:
 ******************************************************************************
 * Date			Id	  Change
 * DD MMM, YYYY
 * ----------------------------------------------------------------------------
 * 11. apr. 2018	HLH   Module created.
 *
 *****************************************************************************/

/***************************** Include files ********************************/
#include <stdint.h>
#include "emp_type.h"
#include "keyboard.h"
#include "tm4c123gh6pm.h"
#include "glob_def.h"
#include "rtc.h"
#include "tmodel.h"
/*****************************    Defines    ********************************/
#define ast 0x41
#define has 0x11
#define zer 0x21
#define one 0x48
#define two 0x28
#define thr 0x18
#define fou 0x44
#define fiv 0x24
#define six 0x14
#define sev 0x42
#define eig 0x22
#define nin 0x12

/*****************************   Constants   ********************************/

/*****************************   Variables   ********************************/

/*****************************   Functions   *********************************
 *   Function : See General module specification (general.h-file).
 *****************************************************************************/
void keyboard_init()
{
#ifndef E_PORTA
#define E_PORTA
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0; // Clock gate port A
#endif

#ifndef E_PORTE
#define E_PORTE
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // Clock gate port E
#endif

    GPIO_PORTA_DIR_R |= 0x1C;               // Set A pins as output
    GPIO_PORTE_DIR_R &= 0x00;               // Set E pins as input
    GPIO_PORTA_DEN_R |= 0x1C;               // Enable digital function A2-4
    GPIO_PORTE_DEN_R |= 0x0F;               // Enable digital function E0-3

}

INT8U kp_scan(void)
{
    INT8U iter = 3, imask[3] = { 0x10, 0x20, 0x40 };
    INT8U mask[3] = { 0x04, 0x08, 0x10 };
    INT8U answer = 0, data = 0;
    do
    {
        iter--;
        GPIO_PORTA_DATA_R |= mask[iter];
        data = GPIO_PORTE_DATA_R;
        GPIO_PORTA_DATA_R &= 0x00;
        if (data)
        {
            answer += imask[iter];
            answer += data;
            iter = 0;
        }
    }
    while (iter);
    return answer;
}

void keyboard_read_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Task to read keyboard in put into queue.
 ******************************************************************************/
{
    static INT8U s_kp_value = 0, rep = 0, count = 0;
    INT8U kp_value = 0, ch = 0x00;
    kp_value = kp_scan();
    if ((kp_value) && !(rep))
    {
        rep++;
        count = 0;
        s_kp_value = kp_value;
    }
    if (rep)
    {
        if (kp_value == s_kp_value)
        {
            count++;
        }
        if (++rep == 4)
        {
            rep = 0;
            if (count >= 2)
            {
                switch (s_kp_value)
                {
                case ast:
                    ch = '*';
                    break;
                case has:
                    ch = '#';
                    break;
                case zer:
                    ch = '0';
                    break;
                case one:
                    ch = '1';
                    break;
                case two:
                    ch = '2';
                    break;
                case thr:
                    ch = '3';
                    break;
                case fou:
                    ch = '4';
                    break;
                case fiv:
                    ch = '5';
                    break;
                case six:
                    ch = '6';
                    break;
                case sev:
                    ch = '7';
                    break;
                case eig:
                    ch = '8';
                    break;
                case nin:
                    ch = '9';
                    break;
                default:
                    break;
                }
                put_queue(Q_KEYBOARD, ch, WAIT_FOREVER);
                wait(100);
            }

        }
    }
}

void keyboard_update_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
/*****************************************************************************
 *   Input    :
 *   Output   :
 *   Function : Task to update RTC clock from keyboard input queues.
 ******************************************************************************/
{
    static INT8U i = 0;
    static INT8U inbuff[7];
    INT8U ch;

    if (get_queue( Q_KEYBOARD, &ch, WAIT_FOREVER))
    {
        if (i < 6)
            inbuff[i++] = ch;
        if (ch == '*')
        {
            i = 0;
        }
        if ((ch == '#') || (i >= 6))
        {
            set_hour((inbuff[1] - '0') * 10 + inbuff[2] - '0');
            set_min((inbuff[3] - '0') * 10 + inbuff[4] - '0');
            set_sec((inbuff[5] - '0') * 10 + inbuff[6] - '0');
            i= 0 ;
        }
    }
}

/****************************** End Of Module *******************************/
