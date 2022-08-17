#include "snapbot_functions.h"
using namespace dynamixel;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "snapbot_main");
  ros::NodeHandle n;
  ros::WallTime start_t, end_t, t0;

  int DXL_pos[12] = {0};

  int DXL_ID[12] = {0};
  int DXL_ID_new[12] = {0};
  int DXL_ID_temp[12] = {0};
  int DXL_LEG_temp[12] = {0};
  int DXL_LEG[12] = {0};

  int DXL_num = 0, DXL_num_new = 0;
  char input = 0;
  double x_amp = 0.0, y_amp = 0.0, theta_rot = 0.0, r_rot_temp = 0.0, trn = 0.0, step_time = 0.2, theta_p = PI / 4, R = 0.0;
  double theta_rot_new = 0.0, x_amp_new = 0.0, y_amp_new = 0.0, r_rot_temp_new = 0.0, R_new = 235.0, theta_p_new = PI / 4, trn_new = 0.0, step_time_new = 0.2;
  double t_elapsed = 0, t_loop = 0, freq = PI, t_old = 0.0, i_step = 0.0, t = 0.0;
  int com_err = 0, tick = 0, new_port = 0;
  int amp_decay;
  open_port();

  //////////////////////////////////////////////////  Initial Configuration Check Start ///////////////////////////////////////////////////////
  dxl_scan(DXL_ID);
  for (int i = 0; i < 13; i++)
  {
    if (DXL_ID[i] != 0)
    {
      DXL_num++;
    }
  }
  // If number of DXL is not zero, execute configuration check and leg assign
  if (DXL_num != 0)
  {
    conf_check(DXL_LEG_temp, DXL_ID, DXL_num);
    Leg_assign(DXL_LEG, DXL_LEG_temp, DXL_num);
    int DXL_count = 0;
    for (int i = 0; i < 12; i++)
    {
      if (DXL_LEG[i] != 0)
      {
        DXL_ID[DXL_count] = DXL_LEG[i];
        DXL_count++;
      }
    }
    for (int i = 0; i < 12; i++)
    {
      printf("DXL_ID:%d     ", DXL_ID[i]);
    }
  }
  //////////////////////////////////////////////////  Initial Configuration Check Completed ///////////////////////////////////////////////////////
  DXL_setting(DXL_ID, DXL_num);
  t0 = ros::WallTime::now();
  ros::Rate loop_rate(100);
  while (ros::ok())
  {
    start_t = ros::WallTime::now();
    tick += 1;
    input = getKey();
    if (input == ESC_ASCII_VALUE)
    {
      break;
    }

    switch (input)
    {
    case 'w':
      x_amp_new = 4.0;
      y_amp_new = 4.0;
      theta_rot_new = 0.0;
      r_rot_temp_new = 0.0;
      break;

    case 's':
      x_amp_new = -4.0;
      y_amp_new = 4.0;
      theta_rot_new = 0.0;
      r_rot_temp_new = 0.0;
      break;

    case 'd':
      trn_new += 0.1;
      break;

    case 'a':
      trn_new -= 0.1;
      break;

    case 'q':
      x_amp_new = 4.0;
      y_amp_new = 4.0;
      theta_rot_new = 0.1;
      r_rot_temp_new = 0.0;
      break;

    case 'e':
      x_amp_new = 4.0;
      y_amp_new = 4.0;
      theta_rot_new = -0.1;
      r_rot_temp_new = 0.0;
      break;

    case 'z':
      x_amp_new = -4.0;
      y_amp_new = 4.0;
      theta_rot_new = 0.1;
      r_rot_temp_new = 0.0;
      break;

    case 'c':
      x_amp_new = -4.0;
      y_amp_new = 4.0;
      theta_rot_new = -0.1;
      r_rot_temp_new = 0.0;
      break;

    case 't':
      x_amp = 0.0;
      y_amp_new = 0.0;
      trn_new = 0.0;
      theta_rot_new = 0.0;
      r_rot_temp_new = 3.0;
      break;

    case 'r':
      x_amp = 0.0;
      y_amp_new = 0.0;
      trn_new = 0.0;
      theta_rot_new = 0.0;
      r_rot_temp_new = -3.0;
      break;

    case 'v':
      step_time_new -= 0.02;
      if (step_time_new <= 0.02)
      {
        step_time_new = 0.02;
      }
      break;

    case 'b':
      step_time_new += 0.02;
      break;

    case 49:
      theta_p_new -= 0.025;
      break;

    case 50:
      theta_p_new += 0.025;
      break;

    case 51:
      R_new += 5.0;
      break;

    case 52:
      R_new -= 5.0;
      break;
    }

    R = R_new;
    theta_p = theta_p_new;

    if (t_elapsed > 0.5 && t_elapsed < 1.0)
    {
      write4(DXL_ID, DXL_num, ADDR_PROFILE_VEL, 800);
      write4(DXL_ID, DXL_num, ADDR_PROFILE_ACCL, 800);
    }
    else if (t_elapsed > 1.0)
    {
      if ((fabs(x_amp) < 0.4) && (fabs(r_rot_temp) < 0.4)&& (fabs(y_amp) < 0.4) )
      {
        write4(DXL_ID, DXL_num, ADDR_PROFILE_VEL, 800);
        write4(DXL_ID, DXL_num, ADDR_PROFILE_ACCL, 800);
      }
    }
 
    if (x_amp > 0.0)
    {
      x_amp = x_amp + x_amp_new - 0.40;
    }
    else if (x_amp < 0.0)
    {
      x_amp = x_amp + x_amp_new + 0.40;
    }
    else
    {
      x_amp = x_amp + x_amp_new;
    }

    y_amp = y_amp + y_amp_new - 0.40;

    if (r_rot_temp > 0.0)
    {
      r_rot_temp = r_rot_temp + r_rot_temp_new - 0.40;
    }
    else if (r_rot_temp < 0.0)
    {
      r_rot_temp = r_rot_temp + r_rot_temp_new + 0.40;
    }
    else
    {
      r_rot_temp = r_rot_temp + r_rot_temp_new;
    }

    if (theta_rot > 0.0)
    {
      theta_rot = theta_rot + theta_rot_new - 0.005;
    }
    else if (theta_rot < 0.0)
    {
      theta_rot = theta_rot + theta_rot_new + 0.005;
    }
    else
    {
      theta_rot = theta_rot + theta_rot_new;
    }

    if (x_amp > 30.0)
    {
      x_amp = 30.0;
    }
    else if (x_amp < -30.0)
    {
      x_amp = -30.0;
    }

    if (y_amp < 0.0)
    {
      y_amp = 0.0;
    }
    else if (y_amp >= 50.0)
    {
      y_amp = 50.0;
    }
    if (r_rot_temp > 40.0)
    {
      r_rot_temp = 40.0;
    }
    else if (r_rot_temp < -40.0)
    {
      r_rot_temp = -40.0;
    }
    if (theta_rot > 0.7)
    {
      theta_rot = 0.7;
    }
    else if (theta_rot < -0.7)
    {
      theta_rot = -0.7;
    }

    trn = trn_new;
    step_time = step_time_new;

    theta_rot_new = 0.0;
    x_amp_new = 0.0;
    y_amp_new = 0.0;
    r_rot_temp_new = 0.0;

    freq = 1 / (step_time);
    t += 0.005 * freq;
    i_step = (t) / (1.5 * step_time);

    ROS_INFO("DXL_num: %d   Loop: %2.3f   Elapsed Time: %3.3f || xamp: %2.1f   yamp: %2.1f  trn: %1.1f   rot: %2.1f  theta_rot: %2.1f  step time: %1.3f   theta_p: %2.2f", DXL_num, t_loop, t_elapsed, x_amp, y_amp, trn, r_rot_temp, theta_rot, step_time, theta_p);

    switch (DXL_num / 2)
    {

    case 1:
      LEG1(DXL_ID, DXL_num, t, R, theta_p, x_amp, y_amp, trn, r_rot_temp, theta_rot);
      break;

    case 2:
      LEG2(DXL_ID, DXL_num, t, R, theta_p, x_amp, y_amp, trn, r_rot_temp, theta_rot);
      break;

    case 3:
      LEG3(DXL_ID, DXL_num, t, R, theta_p, x_amp, y_amp, trn, r_rot_temp, theta_rot);
      break;

    case 4:
      LEG4(DXL_ID, DXL_num, t, R, theta_p, x_amp, y_amp, trn, r_rot_temp, theta_rot);
      break;

    case 5:
      LEG5(DXL_ID, DXL_num, t, R, theta_p, x_amp, y_amp, trn, r_rot_temp, theta_rot);
      break;

    case 6:
      LEG6(DXL_ID, DXL_num, t, R, theta_p, x_amp, y_amp, trn, r_rot_temp, theta_rot);
      break;
    }

    // Detecting leg detatchment when there is a communication error
    com_err += DXL_Read(DXL_ID, DXL_num, DXL_pos);
    if (com_err > 10)
    {
      dxl_scan(DXL_ID_temp);
      DXL_num_new = 0;
      for (int i = 0; i < 12; i++)
      {
        if (DXL_ID_temp[i] != 0)
        {
          DXL_num_new++;
        }
      }
      com_err = 0;
    }
    else
    {
      DXL_num_new = DXL_num;
    }
    if (DXL_num > DXL_num_new)
    {
      printf("Leg out\n");
      Leg_minus(DXL_LEG_temp, DXL_ID_temp, DXL_num_new, DXL_LEG, DXL_ID);
      DXL_num = DXL_num_new;
    }

    // Check the configuration change every (500 tick or there is no dynamixel), and moving variables are close to zero (not moving)

    if ((tick % 100 == 0 || DXL_num == 0) && ((fabs(y_amp) < 0.1) && (fabs(x_amp) < 0.1) && (fabs(r_rot_temp) < 0.1)))
    {
      tick = 0;
      dxl_scan(DXL_ID_temp);
      DXL_num_new = 0;
      for (int i = 0; i < 12; i++)
      {
        if (DXL_ID_temp[i] != 0)
        {
          DXL_num_new++;
        }
      }
      if (DXL_num_new != DXL_num)
      printf("num of dxl: %d  -->   %d  \n", DXL_num, DXL_num_new);

      if (DXL_num_new > DXL_num)
      {
        Leg_plus(DXL_LEG_temp, DXL_ID_temp, DXL_num_new, DXL_LEG, DXL_ID);
        DXL_num = DXL_num_new;
      }
    }

    loop_rate.sleep();
    ros::spinOnce();

    // Measure Loop time
    end_t = ros::WallTime::now();
    t_elapsed = (end_t - t0).toNSec() * 1e-9;
    t_loop = (end_t - start_t).toNSec() * 1e-6;
  }

  close_port(DXL_ID, DXL_num);
}