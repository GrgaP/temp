#ifndef BUG_H		// ��������� ��������� ��������� ����� ������
#define BUG_H

#include "main.h"

#define bug_count 5	// ���������� ���������� ���� ��� ������������ ����

extern volatile u8 bug[bug_count];
extern volatile u8 bug_actual;

////////////////////////////////////////////////
//    ������ ���������� ��������� �������     //
////////////////////////////////////////////////
void bug_init (void);
u8 bug_get_cmd (void);

#endif



