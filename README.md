
// work on outputs ddr, led-lights, led-interrupt timer, modify, include all libraries currently using, fix baud rate, fix timing
#include <util/delay.h>

struct data{
unsigned int byte1;
unsigned int byte2;
unsigned int byte3;
unsigned int time_delay;
}


int main(void) {

USART_Init (31250);

unsigned int start_address=0;                                            // start address that holds recorded data
unsigned int last_address;                                               // last address in EEPROM that holds valid data
unsigned int curr_rec_address=start_address;                             // next address available to hold recorded data
unsigned int curr_play_address=start_address;                            // address of the next recorded note to be played
unsigned int time2;

while (1) {
 
 struct data* note;
 
 //recording
 if (PINA & 1) {
 	
 	//initialize timer
 	TIMER_Init();
 	
 	//store received data
 	EEPROM_Write(curr_rec_ddress, USART_Read(note) );

 	
 }
 //playing
 if (PINA & (1<<1)) {
 	
 	//last recorded note was played, start over
 	if (curr_play_address>last_address) curr_play_address = start_address;
 	
 	//retrieve data
 	data= EEPROM_Read(curr_play_address);
 	
 	/*
 	if (PINA & (1<<2)){
 		//modify
 	}
 	*/
 	
 	 //delay by the time in between notes
 	_delay_ms (data->time_delay/15.625);
 	
 	//transmit the data
 	USART_Write(data);
 	
 }
 
}
}



//************************************************* ~ EEPROM BYTE ~ **************************************************//


void EEPROM_Write (unsigned int address, struct data* note){

while (EECR & (1 << EEWE) );                                 // wait to write
EEAR= address;                                               // set address to be written
EEDR= note->byte1;                                           // set byte1 as data to be written
while (EECR & (1 << EEWE) );                                 // wait to write
EEAR= address+1;                                             // set address to be written
EEDR= note->byte2;                                           // set byte2 as data to be written
while (EECR & (1 << EEWE) );                                 // wait to write
EEAR= address+2;                                             // set address to be written
EEDR= note->byte3;                                           // set byte3 as data to be written
while (EECR & (1 << EEWE) );                                 // wait to write
EEAR= address+3;                                             // set address to be written
EEDR= note->time_delay;                                      // set time_delay as data to be written

EECR |= (1 << EEMWE);

//start write
EECR |= (1 << EEWE);

curr_rec_address=address+4;
last_play_address=address+3;

}



struct data* EEPROM_Read (unsigned int address){

struct data* temp;

while (EECR & (1 << EEWE) );                                 // wait to read
EEAR= address;                                               // set address to be read
temp->byte1=EEDR;                                            // set byte1
while (EECR & (1 << EEWE) );                                 // wait to read
EEAR= address-1;                                             // set address to be read
temp->byte2=EEDR;                                            // set byte2
while (EECR & (1 << EEWE) );                                 // wait to read
EEAR= address-2;                                             // set address to be read
temp->byte3=EEDR;                                            // set byte3
while (EECR & (1 << EEWE) );                                 // wait to read
EEAR= address-3;                                             // set address to be read
temp->time_delay=EEDR;                                       // set time_delay

//start reading
EECR |= (1<<EERE);

curr_play_address=address-4;

return temp;
}

//************************************************* ~ EEPROM STRUCT ~ **************************************************//


void EEPROM_Write (unsigned int address, struct data* note){

while (EECR & (1 << EEWE) );                                 // wait to write
EEAR= address;                                               // set address to be written
EEDR= *note;                                        
                                

EECR |= (1 << EEMWE);

//start write
EECR |= (1 << EEWE);

curr_rec_address=address+4;                                  
last_play_address=address+3;

}



struct data* EEPROM_Read (unsigned int address){

struct data* temp;

while (EECR & (1 << EEWE) );                                 // wait to read
EEAR= address;                                               // set address to be read
*temp=EEDR;                                         

//start reading
EECR |= (1<<EERE);

curr_play_address=address-4;

return temp;
}


//************************************************* ~ TIMER ~ **************************************************//


unsigned int Read_TCNT1(){

unsigned char sreg;
unsigned int i;

sreg=SREG;
_CLI();

i=TCNT1;

SREG= sreg;

if (i>TOP) TIMER_Init;

return i;
}



void Write_TCNT1( unsigned int i){

unsigned char sreg;
unsigned int i;

sreg=SREG;
_CLI();

TCNT1=i;

SREG= sreg;

}




void TIMER_Init(){

//set prescaler of 256
TCCR1B |= (1<<CS12);
TCNT1=0;

//timer value equivalent to 4s
TOP=62500;

}



//************************************************* ~ USART ~ **************************************************//


void USART_Init (unsigned int baud){

//set baud rate
UBRRH= (unsigned char) (baud>>8);
UBRRL= (unsigned char) baud;

//set receiver and transmitter
UCSRB= (1<<RXEN) | (1<<TXEN);

//set 8 data bits, 
UCSRC= (3<<UCSZ0);

}


struct data* USART_Read(struct data* note){

while ( !(UCSRA & (1<<RXC)) );                                 //wait to receive
note->time_delay= Read_TCNT1();                                //set time delay in between notes
note->byte1=UDR;                                               //set byte1 of the note message
while ( !(UCSRA & (1<<RXC)) );                                 //wait to receive
note->byte2=UDR;                                               //set byte2 of the note message
while ( !(UCSRA & (1<<RXC)) );                                 //wait to receive
note->byte3=UDR;                                               //set byte3 of the note message


return note;                                                   //return pointer to the data structure
}



void USART_Write(struct data* note){

while ( !( UCSRA & (1<<UDRE)) );                                 //wait to send      
UDR=note->byte1;                                                 //send byte1 of the note message
while ( !( UCSRA & (1<<UDRE)) );                                 //wait to send
UDR=note->byte2;                                                 //send byte2 of the note message
while ( !( UCSRA & (1<<UDRE)) );                                 //wait to send
UDR=note->byte3;                                                 //send byte3 of the note message
}
