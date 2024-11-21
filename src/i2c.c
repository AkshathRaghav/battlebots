#include "i2c.h"

//===========================================================================
// Send a START bit.
//===========================================================================
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir) {
    // 0. Take current contents of CR2 register. 
    uint32_t tmpreg = I2C1->CR2;

    // 1. Clear the following bits in the tmpreg: SADD, NBYTES, RD_WRN, START, STOP
    tmpreg &=~ (I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    // 2. Set read/write direction in tmpreg.
    if(dir == 0x0)
    {
        tmpreg &=~ I2C_CR2_RD_WRN;
    }   
    else 
    {
        tmpreg |= I2C_CR2_RD_WRN;

    }
    // 3. Set the target's address in SADD (shift targadr left by 1 bit) and the data size.
    tmpreg |= ((targadr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);

    // 4. Set the START bit.
    tmpreg |= I2C_CR2_START;

    // 5. Start the conversion by writing the modified value back to the CR2 register.
    I2C1->CR2 = tmpreg;
}

//===========================================================================
// Send a STOP bit.
//===========================================================================
void i2c_stop(void) {
// 0. If a STOP bit has already been sent, return from the function.
// Check the I2C1 ISR register for the corresponding bit.
    if((I2C1->ISR & I2C_ISR_STOPF))
    {
        return;
    }
    // if(I2C1->CR2 & I2C_CR2_STOP) TRY IN CASE SHRIKAR.
// 1. Set the STOP bit in the CR2 register.
    I2C1->CR2 |= I2C_CR2_STOP;
// 2. Wait until STOPF flag is reset by checking the same flag in ISR.
    while(!(I2C1->ISR & I2C_ISR_STOPF))
    {
        //DO !(I2C1->ISR & I2C_ISR_STOPF) IF GETTING STUCK HERE!
        //wait for stop to be asserted
    }
// 3. Clear the STOPF flag by writing 1 to the corresponding bit in the ICR.
    I2C1->ICR |= I2C_ICR_STOPCF;
}

//===========================================================================
// Wait until the I2C bus is not busy. (One-liner!)
//===========================================================================
void i2c_waitidle(void) {
  while(I2C1->ISR & I2C_ISR_BUSY){
    //wait till i2c is not busy.
  }
}

//===========================================================================
// Send each char in data[size] to the I2C bus at targadr.
//===========================================================================
int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size) {
    i2c_waitidle(); //waits until i2c is idle
    // i2c_start(uint32_t targadr, uint8_t size, uint8_t dir) 
    i2c_start(targadr, size, 0); //check 0 for dir. 0: write transfer
    
    for(int i = 0; i<size; i++)
    {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack()) {
                i2c_clearnack();
                i2c_stop();
                return -1;
            }
        }

        I2C1->TXDR = data[i] & I2C_TXDR_TXDATA; //masking data[i] with I2C_TXDR_TXDATA. CHECK?
    }

    while (!(I2C1->ISR & I2C_ISR_TC) && !(I2C1->ISR & I2C_ISR_NACKF))
    {
        //wait till TC and NACKF flags are not set.
    }

    if((I2C1->ISR & I2C_ISR_NACKF))
    {
        return -1; //target device did not ack data
    }

    i2c_stop();

    return 0;
}

//===========================================================================
// Receive size chars from the I2C bus at targadr and store in data[size].
//===========================================================================
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size) {
    char* temp_data = (char*) data;
    i2c_waitidle();
    i2c_start(targadr, size, 1); //check 1 for dir. 1: read transfer

    for(int i = 0; i< size ; i++)
    {
        int count = 0;
        while ((I2C1->ISR & I2C_ISR_RXNE) == 0) {
            count += 1;
            if (count > 1000000)
                return -1;
            if (i2c_checknack()) {
                i2c_clearnack();
                i2c_stop();
                return -1;
            }
        }
        temp_data[i] = I2C1->RXDR & I2C_RXDR_RXDATA;
    }
    return 0;

}

//===========================================================================
// Clear the NACK bit. (One-liner!)
//===========================================================================
void i2c_clearnack(void) {
    I2C1->ICR &=~ I2C_ICR_NACKCF;
}

//===========================================================================
// Check the NACK bit. (One-liner!)
//===========================================================================
int i2c_checknack(void) {
    if(I2C1->ISR & I2C_ISR_NACKF) return 1;
    else return 0;
}

//===========================================================================
// EEPROM functions
// We'll give these so you don't have to figure out how to write to the EEPROM.
// These can differ by device.

//LOC HAS TO BE MULTIPLE OF 32
//WRITE TO EEPROM
//wait for nanowait(10000000) * 2
//READ EEPROM
//DISPLAY

void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
    uint8_t bytes[34];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    for(int i = 0; i<len; i++){
        bytes[i+2] = data[i];
    }
    i2c_senddata(EEPROM_ADDR, bytes, len+2);
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
    // ... your code here
    uint8_t bytes[2];
    bytes[0] = loc>>8;
    bytes[1] = loc&0xFF;
    i2c_senddata(EEPROM_ADDR, bytes, 2);
    i2c_recvdata(EEPROM_ADDR, data, len);
}

int Num_Games_Updater()
{
    uint8_t games_played;  
    char data;      
    // Read the games_played from the EEPROM at the specified location
    eeprom_read(LOC, &data, 1);  // Length is 1 byte
    nano_wait(1000000);
    // Convert the `char` data back to `uint8_t`
    games_played = (uint8_t)data;
    games_played++;
    data = (char)games_played;
    eeprom_write(LOC, &data, 1);
    nano_wait(1000000);
    return games_played;
}
