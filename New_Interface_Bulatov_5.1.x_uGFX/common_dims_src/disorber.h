#ifndef DISORBER_H  // ��������� ��������� ��������� ����� ������
#define DISORBER_H

#include "main.h"

extern volatile u8 DISOR_Flag;
extern volatile u8 Disorber_sec;

// ��������� �������
void Disorber_On_Off (void);
void Disorber_Update (void);

#endif