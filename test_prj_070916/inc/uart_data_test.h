#ifndef __UART_DATA_TEST_H
#define __UART_DATA_TEST_H

extern osSemaphoreId semSerialCommReceivedHandle;
extern xQueueHandle uart_data_ptr_queue;//������� �������� ��������� �� ������ � ������� ����������� �� UART

void svd_uart_module_cfg(void);

#endif /* __UART_DATA_TEST_H */