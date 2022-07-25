#define PWM 11
#define DIR 12

#define pinA 2  
#define pinB 3 

#define pinA_A A1 
#define pinB_A A0

#define readA bitRead(PIND,pinA)  //faster than digitalRead()
#define readB bitRead(PIND,pinB)

#define BAUDRATE 9600

#define SINGLE_INTERRUPT 1	// values for setting mode in encoder setup
#define DUAL_INTERRUPT 2
