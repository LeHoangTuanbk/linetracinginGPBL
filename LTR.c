#include <xc.h>
#include <pic18f2553.h>


void init(){
       //set ports as digital ports 
    ADCON1 = 0x0F;

   //set ports as input or output
    TRISA = 0x20;            //  portA   RA0,1,2,3,4:output, RA5:input   0010 0000(2) = 20(16) 
    TRISB = 0xFF;            //  portB   all input 1111 1111(2) = FF(16)
    TRISC = 0x00;            //  portC   RC all output
   
      //reset to port
    PORTA = 0x00;            //  portA   reset output
    PORTB = 0x00;            //  portB    reset output
    PORTC = 0x00;            //  portC    reset output
}

void init_timer(){
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 1;
    T0CONbits.T0PS2 = 1;
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS0 = 1;
    
    TMR0H=0xFE;
    TMR0L=0x0C;
    
    T0CONbits.TMR0ON = 1;
}
//bat/tat dong co
char flag = 0;
//dang giu nut start hay khong
char flag_start = 0;
//co trang thai truoc do
char flag_before = 0;
//dem chu ki
char count = 0;

//toc do dong co 1, 2
char v1 = 0, v2 = 0;


//dieu khien dong co
__interrupt() void ISR(void){
    if (INTCONbits.TMR0IF == 1){
        T0CONbits.TMR0ON = 0;
    
        if (count >= 10){
            count = 0;
        }

        if (flag){
    //        dieu khien dong co 1
            if (v1 > count)
                PORTCbits.RC0 = 1;
            else 
                PORTCbits.RC0 = 0;
    //        dieu khien dong co 2
            if (v2 > count)
                PORTCbits.RC1 = 1;
            else 
                PORTCbits.RC1 = 0;
        }

        count++;
        TMR0H=0xFE;
        TMR0L=0x0C;

        T0CONbits.TMR0ON = 1;
        INTCONbits.TMR0IF = 0;
    } else {
        flag = !flag;
    }
    
}

void left_down(void){
    v1 = 10;
    v2 = 1;
}

void small_left_down(void){
    v1 = 7;
    v2 = 3;
}

void right_down(void){
    v1 = 1;
    v2 = 10;
}

void small_right_down(void){
    v1 = 3;
    v2 = 7;
}

void straight(void){
    v1 = 8;
    v2 = 8;
}

void led_sens(void)
{
    char leds = PORTBbits.RB0 + PORTBbits.RB1 + PORTBbits.RB2 + PORTBbits.RB3 + PORTBbits.RB4;
    char flag_led = 1;
    
    if (leds < 3)
        flag_led = 0;
    
    PORTAbits.RA0 = PORTBbits.RB0 == flag_led;
    PORTAbits.RA1 = PORTBbits.RB1 == flag_led;            
    PORTAbits.RA2 = PORTBbits.RB2 == flag_led;           
    PORTAbits.RA3 = PORTBbits.RB3 == flag_led;
    PORTAbits.RA4 = PORTBbits.RB4 == flag_led;       
}

void DieuKhien(int dk){
    char trai = PORTBbits.RB0 + PORTBbits.RB1;
    char phai = PORTBbits.RB3 + PORTBbits.RB4;
    
    if (dk){
        if (PORTBbits.RB2==0){
            straight();
            flag_before = 0;
        } else if(trai > phai && PORTBbits.RB4==0){ 
           /*White White White White black*/
            left_down();/* turn left */
            flag_before = -2;
        }    
        else if(trai > phai && PORTBbits.RB4==1){ 
           /*White White White black White */
            small_left_down();/* turn left */
            flag_before = -1;
        }
        else if(PORTBbits.RB0==1  &&  trai < phai){ 
           /*White black White White  White */
            small_right_down();/* turn right */
            flag_before = 1;
        }
        else if(PORTBbits.RB0==0  &&  trai < phai){ 
           /* black White White White  White */
            right_down();/* turn right */
            flag_before = 2;
        }
        else{ /* The other case */
            switch(flag_before){
                case -2:
                    left_down();
                    break;
                case -1:
                    small_left_down();
                    break;
                case 0:
                    straight();
                    break;
                case 1:
                    small_right_down();
                    break;
                case 2:
                    right_down();
                    break;
                default:
                    straight();
                    break;
            }
        }
    } else {
        v1 = 0;
        v2 = 0;
    }
}

void main(void){
    init();
   
    init_timer(); //initialize the timer

//    while(PORTAbits.RA5==1){
//       led_sens();
//   }
    flag = 1;
    
    while(1){

        led_sens();
        
        DieuKhien(flag);
    }
}