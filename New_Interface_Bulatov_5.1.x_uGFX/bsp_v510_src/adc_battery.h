#ifndef ADC_BATTERY_H		// ��������� ��������� ��������� ����� ������
#define ADC_BATTERY_H

#include "main.h"

#define REFERENCE_VOLTAGE 1200	// ������� ���������� 1,2�
#define VOLTAGE_DIVIDER 11.0F	// �������� � ���� ��������� ���������� 1:11
//#define ADC_CH_BATT 4			// ����� ���, � �������� ���������� ���������� ����������
//#define ADC_CH_REF 6			// ����� ���, � �������� ���������� ������� ����������

extern u16 batt_voltage_mV;
extern float batt_percent;
extern u8 batt_check_timer;
extern u8 batt_show_timer;
extern u8 batt_show;

////////////////////////////////////////////////
//    ������ ���������� ��������� �������     //
////////////////////////////////////////////////
void ADC_init (void);
void battery_check (void);
void battery_icon (void);

#endif