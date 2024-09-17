#ifndef __SERVO_H
#define __SERVO_H

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "stm32f10x_tim.h"
#include "FreeRTOS.h"
#include "task.h"

#define M_PI    3.14159265358979323846
#define M_PI_2  1.57079632679489661923

#define L0      7.5
#define L1      2.0 
#define L2      10.5 
#define L3      14.8 
#define L4      6.5
#define L5      11.0

#define ANGLE_MIN -90.0
#define ANGLE_MAX 90.0
#define PULSE_MIN 500
#define PULSE_MAX 2500
#define ANGLE_TO_PULSE_SCALE ((PULSE_MAX - PULSE_MIN) / (ANGLE_MAX - ANGLE_MIN))
#define NUM_SERVOS 6

typedef struct {
    TIM_TypeDef* tim;
    void (*setCompare)(TIM_TypeDef*, uint16_t);
} Servo;

extern Servo servos[NUM_SERVOS];


double calculate_q1(double x, double y);
void calculate_q3(double q1, double x, double y, double z, double s234, double c234, double* q3_values);
double calculate_q2(double q1, double q3, double x, double y, double z, double s234, double c234);
double calculate_q4(double q1, double q2, double q3, double r13, double r23, double r33);
double calculate_q5(double q1, double r11, double r21, double r12, double r22);


void calculateAngles(double x, double y, double z, double* angles);
uint16_t angleToPulse(double angle);
void pwmControl(double targetAngles[], double extraAngle, double step, uint32_t delay_ms, double* finalAngles);


#endif
