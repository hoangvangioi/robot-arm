#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "usart.h"
#include "timer.h"
#include "servo.h"
#include "delay.h"

#define QUEUE_LENGTH 10
QueueHandle_t xQueue;

void USART_Task(void *pvParameters) __attribute__((noreturn));
void Servo_Task(void *pvParameters) __attribute__((noreturn));

int main(void)
{
    USART1_Init(115200);
    TIM_Init();
    Delay_Init();

    xTaskCreate(USART_Task, "USART Task", 128, NULL, 1, NULL);
    xTaskCreate(Servo_Task, "Servo Task", 128, NULL, 1, NULL);

    xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint16_t));

    vTaskStartScheduler();

    while(1) {}
}

void USART_Task(void *pvParameters)
{
    (void)pvParameters;
    uint16_t RX_BUF;
    char message[20];
    char buffer[3]; // Dùng để lưu trữ 2 ký tự số và ký tự kết thúc chuỗi ('\0')
    int i = 0;

    while(1)
    {
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
        {
            RX_BUF = USART_ReceiveData(USART1) - '0';

            // Kiểm tra xem ký tự nhận được có phải là ký tự số không
            if(RX_BUF >= 0 && RX_BUF <= 9)
            {
                buffer[i++] = RX_BUF + '0'; // Chuyển đổi số nguyên thành ký tự số

                // Kiểm tra xem buffer đã chứa 2 ký tự số chưa
                if(i == 2)
                {
                    buffer[i] = '\0'; // Thêm ký tự kết thúc chuỗi
                    RX_BUF = atoi(buffer); // Chuyển đổi chuỗi thành số nguyên

                    if(RX_BUF >= 11 && RX_BUF <= 99)
                    {
                        xQueueSend(xQueue, &RX_BUF, portMAX_DELAY);
                        strcpy(message, "Received data\r\n");
                        USART_SendString(USART1, message);
                    }
                    else
                    {
                        strcpy(message, "Invalid value\r\n");
                        USART_SendString(USART1, message);
                    }

                    // Đặt lại chỉ số i cho buffer
                    i = 0;
                }
            }
            else
            {
                strcpy(message, "Invalid value\r\n");
                USART_SendString(USART1, message);
                i = 0; // Đặt lại chỉ số i cho buffer
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Servo_Task(void *pvParameters)
{
    (void)pvParameters;
    uint16_t receivedValue;
    double finalAngles[6];
    
    while(1)
    {
        if(xQueueReceive(xQueue, &receivedValue, portMAX_DELAY) == pdPASS)
        {
            double angles[6] = {0, 0, 0, 0, 0};
            uint8_t isValid = 0;

            if (isValid == 1) {
  
                double a3[6] = {-32, 90, -65, 85, 90};
                pwmControl(a3, -70.0, 16, 6, finalAngles);
                vTaskDelay(20);

                double a4[6] = {-25, 90, -65, 85, 90};
                pwmControl(a4, -70.0, 16, 6, finalAngles);
                vTaskDelay(20);

                angles[1] += 5;
                pwmControl(angles, -25.0, 16, 6, finalAngles);
                vTaskDelay(500);

                double a1[6] = {-40, 90, -65, 85, 90};
                pwmControl(a1, -25.0, 16, 6, finalAngles);

                double a2[6] = {-40, 90, -65, 85, 90};
                pwmControl(a2, -70.0, 16, 6, finalAngles);
                vTaskDelay(2000);

            }
            if(isValid == 0) {
                double a0[6] = {-40, 90, -65, 85, 90};
                pwmControl(a0, -70.0, 16, 6, finalAngles);
                vTaskDelay(1000);
            }
        }
    }
}
