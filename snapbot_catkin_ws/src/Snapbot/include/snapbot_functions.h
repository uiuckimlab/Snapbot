#ifndef SNAPBOT_FUNCTIONS_H
#define SNAPBOT_FUNCTIONS_H

#include "ros/ros.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>
#include <fcntl.h>
// #include <sys/ioctl.h>
// #include <stdint.h>
// #include <sys/mman.h>
// #include <linux/fb.h>
// #include <time.h>
#include "dynamixel_sdk/dynamixel_sdk.h"
#include "snapbot_functions.h"

#define STDIN_FILENO 0

// DYNAMIXEL ADDRESS TABLE
#define ADDR_Oper_Mode 11
#define ADDR_Status_Level 68
#define ADDR_TORQUE_ENABLE 64
#define ADDR_LED 65
#define ADDR_POS_D 80
#define ADDR_POS_I 82
#define ADDR_POS_P 84
#define ADDR_GOAL_VEL 104
#define ADDR_PROFILE_ACCL 108
#define ADDR_PROFILE_VEL 112
#define ADDR_GOAL_POSITION 116
#define ADDR_PRESENT_CURRENT 126
#define ADDR_PRESENT_VEL 128
#define ADDR_PRESENT_POSITION 132

// XL320 Parameters
#define ADDR_320_MODE 11
#define ADDR_320_TORQUE 24
#define ADDR_320_LED 25
#define ADDR_320_D 27
#define ADDR_320_I 28
#define ADDR_320_P 29
#define ADDR_320_POS_GOAL 30
#define ADDR_320_POS_PRESENT 37

// Data Byte Length
#define LEN_GOAL_POSITION 4
#define LEN_PRESENT_POSITION 4
#define LEN_PRESENT_VELOCITY 4
#define LEN_PRESENT_CURRENT 2
// 320 Data Length
#define LEN_320_MODE 1
#define LEN_320_TORQUE 1
#define LEN_320_D 1
#define LEN_320_I 1
#define LEN_320_P 1
#define LEN_320_POS_GOAL 2
#define LEN_320_POS_PRESENT 2

#define LEN_IND_WRITE (LEN_GOAL_POSITION)
#define LEN_IND_READ (LEN_PRESENT_POSITION + LEN_PRESENT_CURRENT + LEN_PRESENT_VELOCITY)

// Indirect Address Parameters
#define ADDR_INDADDR_WRITE 168
#define ADDR_INDADDR_READ_POS (ADDR_INDADDR_WRITE + 2 * LEN_GOAL_POSITION)
#define ADDR_INDADDR_READ_VEL (ADDR_INDADDR_READ_POS + 2 * LEN_PRESENT_POSITION)
#define ADDR_INDADDR_READ_CUR (ADDR_INDADDR_READ_VEL + 2 * LEN_PRESENT_VELOCITY)

#define ADDR_INDDATA_WRITE 224
#define ADDR_INDDATA_READ_POS (ADDR_INDDATA_WRITE + LEN_GOAL_POSITION)
#define ADDR_INDDATA_READ_VEL (ADDR_INDDATA_READ_POS + LEN_PRESENT_POSITION)
#define ADDR_INDDATA_READ_CUR (ADDR_INDDATA_READ_VEL + LEN_PRESENT_VELOCITY)

#define TORQUE_ENABLE 1  // Value for enabling the torque
#define TORQUE_DISABLE 0 // Value for disabling the torque
#define POS_MODE 3

// Protocol version
#define PROTOCOL_VERSION 2.0      // See which protocol version is used in the Dynamixel
#define DEVICENAME "/dev/ttyUSB0" // Check which port is being used on your controller
#define BAUDRATE 1000000
#define PI 3.1412
#define ESC_ASCII_VALUE 0x1b
#define BILLION 1000000000L
#define MILLION 1000000L

int getch();
int kbhit(void);
char getKey();
// void IK(int motor_angle[], float x, float y, float theta_0);
int *IK(float x, float y, float theta_0);

void write1(int DXL_ID[], int DXL_num, int ADDR, int CMD);
void write2(int DXL_ID[], int DXL_num, int ADDR, int CMD);
void write4(int DXL_ID[], int DXL_num, int ADDR, int CMD);
void ind_addr(int DXL_ID[], int DXL_num, int ADDR_IND, int ADDR_DIR, int LEN_DATA);
void addparam_read(int DXL_ID[], int DXL_num);
void addparam_write(int ID, int VALUE, int LEN_DATA);
int open_port();
int close_port(int DXL_ID[12], int DXL_num);
int DXL_setting(int DXL_ID[12], int DXL_num);
void dxl_scan(int DXL_ID[12]);
void conf_check(int DXL_ID_LEG[12], int DXL_ID[12], int DXL_num);
int conf_simple(int DXL_ID_input[], int DXL_num);
void Leg_assign(int DXL_LEG[], int DXL_LEG_Input[], int DXL_num_Input);
void Leg_minus(int DXL_ID_saved[], int DXL_ID_new[], int DXL_num, int DXL_LEG[], int DXL_ID[]);
void Leg_plus(int DXL_ID_saved[], int DXL_ID_new[], int DXL_num, int DXL_LEG[], int DXL_ID[]);
void LEG1(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot);
void LEG2(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot);
void LEG3(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot);
void LEG4(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot);
void LEG5(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot);
void LEG6(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot);
int DXL_Read(int DXL_ID[], int DXL_num, int DXL_pos[]);
#endif
