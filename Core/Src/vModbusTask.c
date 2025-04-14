#include "mb.h"

void vModbusTask(void *pvParameters) {
    eMBErrorCode eStatus;

    eStatus = eMBInit(MB_RTU, 1, 1, 115200, MB_PAR_NONE); // 从站地址 1
    eStatus = eMBEnable();

    for (;;) {
        eMBPoll();
        vTaskDelay(1); // 避免 CPU 独占
    }
}

void vStartModbusTask(void) {
    xTaskCreate(vModbusTask, "Modbus", 512, NULL, 5, NULL);
}
