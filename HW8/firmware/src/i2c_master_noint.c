#include<xc.h>
#include "i2c_master_noint.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be callled in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

void i2c_master_setup(void) {
  I2C2BRG = 235;            // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2
                                    // look up PGD for your PIC32
  I2C2CONbits.ON = 1;               // turn on the I2C1 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C2CONbits.RSEN = 1;           // send a restart
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}

void initExpander(unsigned char addr)
{
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    //i2c_master_setup();
    
    i2c_master_start();
    i2c_master_send(addr<<1);
    i2c_master_send(0x00);
    i2c_master_send(0b11110000);
    i2c_master_stop();
}

void setExpander(unsigned char addr, unsigned char pin, unsigned char level)
{
    unsigned char val = 0x00;
    val = (val|level)<<pin;
    unsigned char reg = 0x0A;
    writei2c(addr,reg,val);
    
}

unsigned char getExpander(unsigned char addr)
{
    unsigned char val = readi2c(addr,0x09);
    return val;
}

unsigned char readi2c(unsigned char addr, unsigned char reg)
{
    unsigned char r = 0;
    i2c_master_start();
    i2c_master_send(addr<<1);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(addr<<1|1);
    r=i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return r;
}

void writei2c(unsigned char addr, unsigned char reg, unsigned char val)
{
    i2c_master_start();
    i2c_master_send(addr<<1);
    i2c_master_send(reg);
    i2c_master_send(val);
    i2c_master_stop();
}

void initIMU(unsigned char addr){
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;
    writei2c(addr, 0x0B, 0b00000010);
    writei2c(addr, 0x10, 0b10000000); 
    writei2c(addr, 0x11, 0b10000000);  
    writei2c(addr, 0x12, 0b100); 
}

void i2cMultipleRead(unsigned char addr, unsigned char reg, unsigned char *data)
{
    int i = 0;
    i2c_master_start();
    i2c_master_send(addr<<1);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(addr<<1|1);
    for(i=0;i<14;i++)
    {
        data[i]=i2c_master_recv();
        if(i<13)
        {
            i2c_master_ack(0);
        }
        else
        {
            i2c_master_ack(1);
        }
    }
    
    i2c_master_stop();
}

