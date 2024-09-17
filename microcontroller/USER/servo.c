#include "servo.h"

Servo servos[NUM_SERVOS] = {
    {TIM3, TIM_SetCompare1},
    {TIM3, TIM_SetCompare2},
    {TIM3, TIM_SetCompare4},
    {TIM4, TIM_SetCompare1},
    {TIM4, TIM_SetCompare2},
    {TIM4, TIM_SetCompare4}
};

double calculate_q1(double x, double y) {
    return atan2(y, x);
}

void calculate_q3(double q1, double x, double y, double z, double s234, double c234, double* q3_values) {
    double c3 = (pow((cos(q1) * x + sin(q1) * y - s234 * L4), 2) + pow((z - (L0 + L1) + c234 * L4), 2) - L2 * L3) / (2 * L2 * L3);

    double s3_positive = sqrt(1 - pow(c3, 2));
    double s3_negative = -sqrt(1 - pow(c3, 2));

    q3_values[0] = atan2(s3_positive, c3);
    q3_values[1] = atan2(s3_negative, c3);
}

double calculate_q2(double q1, double q3, double x, double y, double z, double s234, double c234) {
    double c2 = (cos(q1) * x + sin(q1) * y + s234 * L4) / (cos(q3) * L3 + L2);
    double s2 = (z - (L0 + L1) - c234 * L4) / (sin(q3) * L3 + L2);
    return atan2(s2, c2);
}

double calculate_q4(double q1, double q2, double q3, double r13, double r23, double r33) {
    double s4 = -(cos(q1 + q2 + q3) * r13 + cos(q2 + q3) * sin(q1) * r23 + sin(q2 + q3) * r33);
    double c4 = -(sin(q2 + q3) * cos(q1) * r13 + sin(q1 + q2 + q3) * r23 - cos(q2 + q3) * r33);
    return atan2(s4, c4);
}

double calculate_q5(double q1, double r11, double r21, double r12, double r22) {
    double s5 = sin(q1) * r11 - cos(q1) * r21;
    double c5 = sin(q1) * r12 - cos(q1) * r22;
    return atan2(s5, c5);
}

void calculateAngles(double x, double y, double z, double* angles) {
    double q1, q2, q3, q4, q5, s234, c234;

    double R[3][3] = {
        {1, 0, 0},
        {0, -1, 0},
        {0, 0, -1}
    };

    double r11  = R[0][0];
    double r12  = R[0][1];
    double r13  = R[0][2];
    double r21  = R[1][0];
    double r22  = R[1][1];
    double r23  = R[1][2];
    double r33  = R[2][2];

    q1 = calculate_q1(x, y);

    s234 = -(cos(q1) * r13 + sin(q1) * r23);
    c234 = r33;

    double q3_values[2];
    calculate_q3(q1, x, y, z , s234, c234, q3_values);

    for (uint8_t i = 0; i < sizeof(q3_values) / sizeof(q3_values[0]); i++) {
        q3 = q3_values[i];
        q2 = calculate_q2(q1, q3, x, y, z, s234, c234);
        q4 = calculate_q4(q1, q2, q3, r13, r23, r33) + M_PI_2;
        q5 = calculate_q5(q1, r11, r21, r12, r22);

        if (q1 >= -M_PI_2 && q1 <= M_PI_2 && q2 >= -M_PI_2 && q2 <= M_PI_2 && q3 >= -M_PI_2 && q3 <= M_PI_2 && q4 >= -M_PI_2 && q4 <= M_PI_2 && q5 >= -M_PI_2 && q5 <= M_PI_2) {
            angles[0] = q1 * (180 / M_PI);
            angles[1] = q2 * (180 / M_PI);
            angles[2] = q3 * (180 / M_PI);
            angles[3] = q4 * (180 / M_PI);
            angles[4] = q5 * (180 / M_PI);
        }
    }
}

uint16_t angleToPulse(double angle) {
    angle = fmax(ANGLE_MIN, fmin(angle, ANGLE_MAX));
    return (uint16_t)((angle - ANGLE_MIN) * ANGLE_TO_PULSE_SCALE + PULSE_MIN);
}

void pwmControl(double targetAngles[], double extraAngle, double step, uint32_t delay_ms, double* finalAngles) {
    static uint16_t currentPulses[NUM_SERVOS] = {1055, 2500, 778, 2444, 2500, 722};
    int done;
    do {
        done = 1;
        for (unsigned int i = 0; i < NUM_SERVOS; i++) {
            uint16_t targetPulse;
            if (i < NUM_SERVOS - 1) {
                targetPulse = angleToPulse(targetAngles[i]);
            } else {
                targetPulse = angleToPulse(extraAngle);
            }

            if (abs(targetPulse - currentPulses[i]) > step) {
                currentPulses[i] += (targetPulse >= currentPulses[i]) ? step : -step;
                done = 0;
            } else {
                currentPulses[i] = targetPulse;
            }
            vTaskDelay(pdMS_TO_TICKS(delay_ms));
            servos[i].setCompare(servos[i].tim, currentPulses[i]);
        }
    } while (!done);

    for (unsigned int i = 0; i < NUM_SERVOS; i++) {
        finalAngles[i] = (currentPulses[i] - PULSE_MIN) / ANGLE_TO_PULSE_SCALE + ANGLE_MIN;
    }
}
