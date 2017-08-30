#ifndef TASK_MANAGER_H		// Блокируем повторное включение этого модуля
#define TASK_MANAGER_H

#include "main.h"

extern u32 tasks;

#define Task_add(x) (tasks|=x)
#define Task_del(x) (tasks&=~x)

// Типа-диспетчер задач
// Максимум 32 задачи (1 бит = 1 задача)
#define TASK_NONE				0		// Нет задачи
#define TASK_DISORBER_ON_OFF	0x01	// Включить/отключить десорбер
#define TASK_GAN				0x02	// Отображение осциллограммы ГАН
#define TASK_LCD_UPDATE			0x04	// Обновить дисплей из буфера
#define TASK_SETTINGS_WRITE		0x08	// Сохранить настройки во флеш-память
#define TASK_CALIBRATION		0x10	// Установка границ обнаружения под разные вещества
#define TASK_MENU_STOP			0x20	// Действия, которые надо сделать при выходе из меню
#define TASK_BATT_CHECK			0x40	// Измерение уровня заряда батареи
#define TASK_BATT_ICON			0x80	// Отобразить на дисплее иконку батарейки
#define TASK_BUZZER_OFF			0x100	// Отключить пищалку
#define TASK_BUZZER_ON			0x200	// Включить пищалку
#define TASK_FABRICATION_LOAD		0x400   // Загрузить фабричные настройки
#define TASK_FB_WR_EN			0x800
#define TASK_NO3				0x1000
#define TASK_NO4				0x2000
#define TASK_NO5				0x4000
#define TASK_NO6				0x8000

////////////////////////////////////////////////
//    Секция прототипов локальных функций     //
////////////////////////////////////////////////

#endif