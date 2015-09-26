
/*
小车电机的驱动程序
IN1、2控制左侧电机 3 4 控制右侧电机
ENA 左侧电机 ENB 右侧电机
 */

#include "stm32f10x.h"

void MotoInit(void);


void LeftStop(void);
void LeftBack(void);
void LeftForward(void);
void RightStop(void);
void RightBack(void);
void RightForward(void);

void RightSetSpeed(float D);
void LeftSetSpeed(float D);






