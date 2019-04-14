
#include <lpc214x.h>
void InitClocks(void);
void SetupPLL0(void);
void feedSeq(void);
void connectPLL0(void);
#define PLOCK 0x00000400 // for PLL
#define THRE (1<<5) // Transmit Holding Register Empty
#define NEW_LINE 0x0a // Character for new line .. analogous to '\n'

void InitUART0(void);
void U0WriteData(char data);
void InitClocks(void); // code given in project files

int main(void)
{
    char msg[] = "WELCOME TO INDIA\r\n" ; 
    int c=0; // counter
    
    InitClocks(); // Set CCLK=60Mhz and PCLK=60Mhz 
    InitUART0();
    
    for(;;)     
    {
        while( msg[c]!='\0' )
        {
            U0WriteData(msg[c]);
            c++;
        }
        U0WriteData(NEW_LINE); //get to the next line below
        c=0; // reset counter       
    }

}

void InitUART0(void)
{
    PINSEL0 = 0x5;  /* Select TxD for P0.0 and RxD for P0.1 */
    U0LCR = 3 | (1<<7) ; /* 8 bits, no Parity, 1 Stop bit | DLAB set to 1  */
    U0DLL = 110;
    U0DLM = 1;   
    U0LCR &= 0x0F; // Set DLAB=0 to lock MULVAL and DIVADDVAL
    //BaudRate is now ~9600 and we are ready for UART communication!
}

void U0WriteData(char data)
{
    while ( !(U0LSR & THRE ) ); // wait till the THR is empty
    // now we can write to the Tx FIFO
    U0THR = data;
}

void InitClocks(void)
{
    SetupPLL0();
    feedSeq(); //sequence for locking PLL to desired freq.
    connectPLL0();
    feedSeq(); //sequence for connecting the PLL as system clock
   
    //SysClock is now ticking @ 60Mhz!
       
    VPBDIV = 0x01; // PCLK is same as CCLK i.e 60Mhz

    //PLL0 Now configured!
}

void SetupPLL0(void)
{
    //Note : Assuming 12Mhz Xtal is connected to LPC2148.
    
    PLL0CON = 0x01; // PPLE=1 & PPLC=0 so it will be enabled
                    // but not connected after FEED sequence

    PLL0CFG = 0x24; // set the multipler to 5
                    
}

void feedSeq(void)
{
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
}

void connectPLL0(void)
{
    // check whether PLL has locked on to the  desired freq by reading the lock bit
    // in the PPL0STAT register

    while( !( PLL0STAT & PLOCK ));

    // now enable(again) and connect
    PLL0CON = 0x03;
}
