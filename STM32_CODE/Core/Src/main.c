#include "main.h"
#include "math.h"
#include "string.h"
#include "stdio.h"

#define MPU6050_ADDR 0xD0
#define EEPROM_ADDR  0xA0

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* ===== STRUCT ===== */
typedef struct {
    float acc;
    float lat;
    float lon;
    uint32_t timestamp;
} CrashData;

/* ===== GLOBAL VARIABLES ===== */
int16_t ax, ay, az;
float latitude = 28.6139;
float longitude = 77.2090;
uint8_t crash_flag = 0;
uint16_t eeprom_index = 0;

/* ===== MPU6050 READ ===== */
void MPU6050_Read()
{
    uint8_t data[6];
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x3B, 1, data, 6, 1000);

    ax = (data[0]<<8)|data[1];
    ay = (data[2]<<8)|data[3];
    az = (data[4]<<8)|data[5];
}

/* ===== ACCELERATION ===== */
float get_acceleration()
{
    float Ax = ax / 16384.0;
    float Ay = ay / 16384.0;
    float Az = az / 16384.0;
    return sqrt(Ax*Ax + Ay*Ay + Az*Az);
}

/* ===== UART SEND ===== */
void send_to_esp(float lat, float lon, float acc)
{
    char msg[100];
    sprintf(msg, "CRASH,%.4f,%.4f,%.2f\n", lat, lon, acc);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 1000);
}

/* ===== EEPROM WRITE ===== */
void EEPROM_Write(uint16_t addr, uint8_t *data, uint16_t size)
{
    HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR, addr, 2, data, size, 1000);
    HAL_Delay(10);
}

/* ===== SAVE CRASH ===== */
void save_crash_data(float acc, float lat, float lon)
{
    CrashData crash;
    crash.acc = acc;
    crash.lat = lat;
    crash.lon = lon;
    crash.timestamp = HAL_GetTick();

    EEPROM_Write(eeprom_index, (uint8_t*)&crash, sizeof(CrashData));

    eeprom_index += sizeof(CrashData);
    if(eeprom_index >= 4096) eeprom_index = 0;
}

/* ===== MAIN ===== */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();

    /* MPU6050 INIT */
    uint8_t data = 0;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1, &data, 1, 1000);

    while (1)
    {
        MPU6050_Read();
        float acc = get_acceleration();

        if(acc > 3.5 && crash_flag == 0)
        {
            crash_flag = 1;

            send_to_esp(latitude, longitude, acc);
            save_crash_data(acc, latitude, longitude);
        }

        HAL_Delay(200);
    }
}
