#ifndef TASK_MANAGER_H		// ��������� ��������� ��������� ����� ������
#define TASK_MANAGER_H

#include "main.h"

extern u32 tasks;

#define Task_add(x) (tasks|=x)
#define Task_del(x) (tasks&=~x)

// ����-��������� �����
// �������� 32 ������ (1 ��� = 1 ������)
#define TASK_NONE				0		// ��� ������
#define TASK_DISORBER_ON_OFF	0x01	// ��������/��������� ��������
#define TASK_GAN				0x02	// ����������� ������������� ���
#define TASK_LCD_UPDATE			0x04	// �������� ������� �� ������
#define TASK_SETTINGS_WRITE		0x08	// ��������� ��������� �� ����-������
#define TASK_CALIBRATION		0x10	// ��������� ������ ����������� ��� ������ ��������
#define TASK_MENU_STOP			0x20	// ��������, ������� ���� ������� ��� ������ �� ����
#define TASK_BATT_CHECK			0x40	// ��������� ������ ������ �������
#define TASK_BATT_ICON			0x80	// ���������� �� ������� ������ ���������
#define TASK_BUZZER_OFF			0x100	// ��������� �������
#define TASK_BUZZER_ON			0x200	// �������� �������
#define TASK_FABRICATION_LOAD		0x400   // ��������� ��������� ���������
#define TASK_FB_WR_EN			0x800
#define TASK_NO3				0x1000
#define TASK_NO4				0x2000
#define TASK_NO5				0x4000
#define TASK_NO6				0x8000

////////////////////////////////////////////////
//    ������ ���������� ��������� �������     //
////////////////////////////////////////////////

#endif