#include "snapbot_functions.h"
using namespace dynamixel;

dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);
dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, ADDR_INDDATA_WRITE, LEN_IND_WRITE);
dynamixel::GroupSyncRead groupSyncRead(portHandler, packetHandler, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);

int dxl_comm_result = COMM_TX_FAIL; // Communication result
bool dxl_addparam_result = false;   // addParam result
bool dxl_getdata_result = false;    // GetParam result
uint8_t dxl_error = 0;
uint8_t param_indirect_data_for_write[LEN_GOAL_POSITION];
int DXL320_ID[2] = {100, 101};
int PROFILE_V = 40, PROFILE_A = 20;

int getch()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

char getKey()
{
    if (kbhit())
    {
        return getch();
    }
    return '\0';
}

int *IK(float x, float y, float theta_0)
{
    static int motor_angle[] = {2048, 2048};

    float L0 = 119.75;
    float L1 = 56.75;
    float L2 = 60.0;
    float L3 = 90.0;

    float x_1 = 0.0;
    float y_1 = 0.0;
    float theta_1 = 0.0;
    float theta_2 = 0.0;
    float a = 0.0;
    float b = 0.0;
    float c = 0.0;
    float z = 0.0;
    float r3 = 0.0;
    float alpha1 = 0.0;
    float alpha2 = 0.0;
    float p[] = {156.737, 632.694, -262.204, -3922.912,
                 -2148.520, 10019.108, 9255.935, -13572.658,
                 -16000.941, 10449.785, 14885.911, -4530.2,
                 -7871.88, 1014.50, 2301.833, -88.755,
                 -331.121, 1.652, 39.511, 11.102, -92.893};

    x_1 = x - L0 * cos(theta_0);
    y_1 = y - L0 * sin(theta_0);

    theta_1 = (atan2(y_1, x_1) - (theta_0));
    motor_angle[0] = 2048 - (int)(theta_1 * 651.088);

    a = x_1 - L1 * cos(theta_1 + theta_0);
    b = y_1 - L1 * sin(theta_1 + theta_0);
    c = (sqrt(a * a + b * b) - 47.5768) / 60.6919;

    if (c > 1.23)
    {
        c = 1.23;
    }

    //20th Polynomial fit
    z = p[0] * pow(c, 20) + p[1] * pow(c, 19) + p[2] * pow(c, 18) + p[3] * pow(c, 17) + p[4] * pow(c, 16) + p[5] * pow(c, 15) + p[6] * pow(c, 14) + p[7] * pow(c, 13) + p[8] * pow(c, 12) + p[9] * pow(c, 11) + p[10] * pow(c, 10) + p[11] * pow(c, 9) + p[12] * pow(c, 8) + p[13] * pow(c, 7) + p[14] * pow(c, 6) + p[15] * pow(c, 5) + p[16] * pow(c, 4) + p[17] * pow(c, 3) + p[18] * pow(c, 2) + p[19] * c + p[20];

    r3 = sqrt(b * b + z * z + a * a);
    if (r3 > 122.0)
    {
        r3 = 122.0;
    }
    alpha1 = acos((L2 * L2 + r3 * r3 - L3 * L3) / (2 * L2 * r3 + 0.001));
    alpha2 = PI - acos(z / r3);

    theta_2 = (alpha1 + alpha2) - PI / 2;
    motor_angle[1] = 2048 + (int)(theta_2 * 651.088);

    // printf("IK function: %f,    %f    ", theta_1*57.2958, r3);
    // printf("IK function: %d,    %d    \n", motor_angle[0], motor_angle[1]);
    return motor_angle;
}

void write1(int DXL_ID[], int DXL_num, int ADDR, int CMD)
{
    for (int i = 0; i < DXL_num; i++)
    {
        if (DXL_ID[i] != 0)
        {
            packetHandler->write1ByteTxRx(portHandler, DXL_ID[i], ADDR, CMD, &dxl_error);
        }
    }
}

void write2(int DXL_ID[], int DXL_num, int ADDR, int CMD)
{
    for (int i = 0; i < DXL_num; i++)
    {
        if (DXL_ID[i] != 0)
        {
            packetHandler->write2ByteTxRx(portHandler, DXL_ID[i], ADDR, CMD, &dxl_error);
        }
    }
}

void write4(int DXL_ID[], int DXL_num, int ADDR, int CMD)
{
    for (int i = 0; i < DXL_num; i++)
    {
        if (DXL_ID[i] != 0)
        {
            packetHandler->write4ByteTxRx(portHandler, DXL_ID[i], ADDR, CMD, &dxl_error);
        }
    }
}

void ind_addr(int DXL_ID[], int DXL_num, int ADDR_IND, int ADDR_DIR, int LEN_DATA)
{
    for (int j = 0; j < LEN_DATA; j++)
    {
        for (int i = 0; i < DXL_num; i++)
        {
            if (DXL_ID[i] != 0)
            {
                packetHandler->write2ByteTxRx(portHandler, DXL_ID[i], ADDR_IND + 2 * j, ADDR_DIR + j, &dxl_error);
            }
        }
    }
}

void addparam_read(int DXL_ID[], int DXL_num)
{
    for (int i = 0; i < DXL_num; i++)
    {
        if (DXL_ID[i] != 0)
        {
            dxl_addparam_result = groupSyncRead.addParam(DXL_ID[i]);
        }
    }
}

// void rmparam_read(int groupread_num, int DXL_ID[], int DXL_num)
// {
// 	for (int i = 0; i < DXL_num; i++)
// 	{
// 		if (DXL_ID[i] != 0)
// 		{
// 			groupSyncReadRemoveParam(groupread_num, DXL_ID[i]);
// 			// if (dxl_addparam_result != True)
// 			// {
// 			// fprintf(stderr, "[ID:%03d] groupSyncRead addparam failed", DXL_ID[i]);
// 			// // return 0;
// 			// }
// 		}
// 	}
// }

int open_port()
{
    printf("Port open\n");
    // Open port
    if (portHandler->openPort())
    {
        printf("Succeeded to open the port!\n");
    }
    else
    {
        printf("Failed to open the port!\n");
        printf("Press any key to terminate...\n");
        getch();
        return 0;
    }
    // Set port baudrate
    if (portHandler->setBaudRate(BAUDRATE))
    {
        printf("Succeeded to change the baudrate!\n");
    }
    else
    {
        printf("Failed to change the baudrate!\n");
        printf("Press any key to terminate...\n");
        getch();
        return 0;
    }
    return 1;
}
int close_port(int DXL_ID[12], int DXL_num)
{
    write1(DXL_ID, DXL_num, ADDR_TORQUE_ENABLE, TORQUE_DISABLE);
    write1(DXL320_ID, 2, ADDR_320_TORQUE, TORQUE_DISABLE);

    // Close port
    portHandler->closePort();
    return 0;
}
void DXL_setting(int DXL_ID[12], int DXL_num)
{
    printf("Number of Dxnamixels: %d\n", DXL_num);
    // printf("Press any key to continue! (or press ESC to quit!)\n");

    write1(DXL_ID, DXL_num, ADDR_TORQUE_ENABLE, TORQUE_DISABLE);
    write1(DXL_ID, DXL_num, ADDR_Oper_Mode, POS_MODE); // Position Mode
    write1(DXL320_ID, 2, ADDR_320_TORQUE, TORQUE_DISABLE);
    write1(DXL320_ID, 2, ADDR_320_MODE, POS_MODE); // Position Mode

    ind_addr(DXL_ID, DXL_num, ADDR_INDADDR_WRITE, ADDR_GOAL_POSITION, LEN_GOAL_POSITION);
    ind_addr(DXL_ID, DXL_num, ADDR_INDADDR_READ_POS, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
    ind_addr(DXL_ID, DXL_num, ADDR_INDADDR_READ_VEL, ADDR_PRESENT_VEL, LEN_PRESENT_VELOCITY);
    ind_addr(DXL_ID, DXL_num, ADDR_INDADDR_READ_CUR, ADDR_PRESENT_CURRENT, LEN_PRESENT_CURRENT);

    write1(DXL_ID, DXL_num, ADDR_TORQUE_ENABLE, TORQUE_ENABLE);
    write1(DXL320_ID, 2, ADDR_320_TORQUE, TORQUE_ENABLE);

    // Set Profile Vel & Acc
    PROFILE_V = 100; // Slow Down
    PROFILE_A = 200; // Slow Down
    write4(DXL_ID, DXL_num, ADDR_PROFILE_VEL, PROFILE_V);
    write4(DXL_ID, DXL_num, ADDR_PROFILE_ACCL, PROFILE_A);

    // PID GAIN
    write2(DXL_ID, DXL_num, ADDR_POS_P, 1500);
    write2(DXL_ID, DXL_num, ADDR_POS_D, 10);
    write2(DXL_ID, DXL_num, ADDR_POS_I, 100);
    write1(DXL320_ID, 2, ADDR_320_P, 50); // 0 ~ 254
    write1(DXL320_ID, 2, ADDR_320_I, 5);  // 0 ~ 254
    write1(DXL320_ID, 2, ADDR_320_D, 15); // 0 ~ 254

    addparam_read(DXL_ID, DXL_num);
}

void dxl_scan(int DXL_ID[12])
{
    uint8_t dxl_error = 0;     // Dynamixel error
    uint16_t dxl_model_number; // Dynamixel model number
    int dxl_comm_result = COMM_TX_FAIL;
    int initID = 25;
    for (int j = 0; j < 12; j++)
    {
        DXL_ID[j] = 0;
    }

    int i = 0;
    for (int id = 0; id < 12; id++)
    {
        dxl_comm_result = packetHandler->ping(portHandler, initID + id, &dxl_error);
        if (dxl_comm_result != COMM_SUCCESS)
        {
            packetHandler->getTxRxResult(dxl_comm_result);
        }
        else if (dxl_error != 0)
        {
            packetHandler->getRxPacketError(dxl_error);
        }
        else
        {
            DXL_ID[i] = initID + id;
            i++;
        }
    }
}

void conf_check(int DXL_ID_LEG[], int DXL_ID_input[], int DXL_num_input)
// Count the nummber and location of the legs
{
    int DXL320_ID[2] = {100, 101};
    int Yawpos_320[6] = {22, 229, 436, 643, 850, 1023}; // Storage of port location
    float head_port = 0.0;
    int Leg_num = int(DXL_num_input / 2);
    int vision_num = 0;
    int iscolor[6] = {0};
    int isport[6] = {0};
    int index_port[6] = {0};
    int index_nport[6] = {0};
    int nport_num = 0;
    int index_sum = 0;
    int index2 = 0;
    int dxl_320_goal = 0;
    int LED = 1;

    printf("num: %d  \n", DXL_num_input);
    // Slowly go to initial pose
    write1(DXL_ID_input, DXL_num_input, ADDR_TORQUE_ENABLE, 1);

    int PROFILE_V = 80;
    int PROFILE_A = 40;
    write4(DXL_ID_input, DXL_num_input, ADDR_PROFILE_VEL, PROFILE_V);
    write4(DXL_ID_input, DXL_num_input, ADDR_PROFILE_ACCL, PROFILE_A);
    write2(DXL_ID_input, DXL_num_input, ADDR_POS_P, 500);

    write1(DXL320_ID, 2, ADDR_320_TORQUE, 1);
    write1(DXL320_ID, 2, ADDR_320_P, 120); // 0 ~ 254
    write1(DXL320_ID, 2, ADDR_320_I, 5);   // 0 ~ 254
    write1(DXL320_ID, 2, ADDR_320_D, 5);   // 0 ~ 254

    write1(DXL320_ID, 2, ADDR_320_LED, 0);
    packetHandler->write2ByteTxRx(portHandler, DXL320_ID[0], 30, Yawpos_320[0], &dxl_error);
    packetHandler->write2ByteTxRx(portHandler, DXL320_ID[1], 30, 500, &dxl_error);
    usleep(5000);

    while (Leg_num != vision_num)
    {
        printf("List of scanned motors\n");
        for (int i = 0; i < Leg_num; i++)
        {
            printf("ID:%d    ID:%d  \n", DXL_ID_input[2 * i], DXL_ID_input[2 * i + 1]);
            // Leg Pose initialization (with slower s)
            packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * i], ADDR_GOAL_POSITION, 2048, &dxl_error);
            packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * i + 1], ADDR_GOAL_POSITION, 2350, &dxl_error);
        }
        usleep(5000);

        // Make the legs faster
        PROFILE_V = 800;
        PROFILE_A = 400;
        write4(DXL_ID_input, DXL_num_input, ADDR_PROFILE_VEL, PROFILE_V);
        write4(DXL_ID_input, DXL_num_input, ADDR_PROFILE_ACCL, PROFILE_A);

        vision_num = 0;
        for (int i = 0; i < 6; i++)
        {
            iscolor[i] = 0; // Reset Array
            isport[i] = 0;
            index_port[i] = 0;
        }

        /////// Image Processing Parameters ////////
        int rl = 200, rh = 254, gl = 195, gh = 255, bl = 154, bh = 238; // RGB Threshold for white
        int row_h = 125, row_l = 85, col_h = 110, col_l = 110;

        LED = 1;
        for (int ii = 0; ii < Leg_num; ii++)
        { // For loop for the number of detected Leg
            for (int i = 0; i < 6; i++)
            { //For loop for numbe of the ports
                if (iscolor[i] == 0)
                {
                    write1(DXL320_ID, 2, 25, LED);
                    packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * ii + 1], ADDR_GOAL_POSITION, 2700, &dxl_error); // Raise Foot
                    packetHandler->write2ByteTxRx(portHandler, DXL320_ID[0], 30, Yawpos_320[i], &dxl_error);                    // Rotate the camera to the next port
                    usleep(300000);
                    // if (i == 1)
                    // {
                    // 	iscolor[i] = camera(rh, rl, gh, gl, bh, bl, 0, 0.15, row_h, row_l, col_h, col_l); // Camera color check
                    // }
                    // else
                    // {
                    // 	iscolor[i] = camera(rh, rl, gh, gl, bh, bl,0, 0.15, row_h, row_l, col_h, col_l); // Camera color check
                    // }

                    iscolor[i] = 1; // When there is no camera!!!!!!!!!!!!!!!!!!!!

                    isport[i] = iscolor[i];
                    // Short pause to have enough time to rotate the camera to the next port
                    usleep(40000);
                }

                if (isport[i])
                {
                    LED++;
                    write1(DXL320_ID, 2, 25, LED);
                    index_port[ii] = i + 1;
                    DXL_ID_LEG[2 * i] = DXL_ID_input[2 * (ii)];
                    DXL_ID_LEG[2 * i + 1] = DXL_ID_input[2 * (ii) + 1];
                    packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * ii + 1], ADDR_GOAL_POSITION, 2350, &dxl_error);
                    isport[i] = 0;
                    vision_num++;
                    // printf("%d    DXL: %d    DXL: %d    LEG: %d   LEG: %d \n", vision_num, DXL_ID_input[2 * ii], DXL_ID_input[2 * ii + 1], ID[2 * i], ID[2 * i + 1]);
                    break;
                }
            } // i loop ends
        }     // ii loop ends
    }
    write1(DXL320_ID, 2, ADDR_320_LED, 4);

    PROFILE_V = 100;
    PROFILE_A = 50;
    write4(DXL_ID_input, DXL_num_input, ADDR_PROFILE_VEL, PROFILE_V);
    write4(DXL_ID_input, DXL_num_input, ADDR_PROFILE_ACCL, PROFILE_A);

    // Smoothly raise the body with the detected legs when the # of legs are more than three
    if (Leg_num > 3)
    {

        for (int i = 0; i < Leg_num; i++)
        {
            packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * i], ADDR_GOAL_POSITION, 1100, &dxl_error);
            packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * i + 1], ADDR_GOAL_POSITION, 2450, &dxl_error);
        }

        usleep(800000);

        for (int i = 0; i < Leg_num; i++)
        {
            packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * i], ADDR_GOAL_POSITION, 2048, &dxl_error);
            packetHandler->write4ByteTxRx(portHandler, DXL_ID_input[2 * i + 1], ADDR_GOAL_POSITION, 2048, &dxl_error);
        }
        usleep(500000);
    }
    write1(DXL320_ID, 2, ADDR_320_LED, 2);
}
void Leg_minus(int DXL_ID_saved[], int DXL_ID_new[], int DXL_num_new, int DXL_LEG[], int DXL_ID[])
{
    // Leg -
    int bln = 0;
    for (int i = 0; i < 12; i++)
    {
        bln = 0;
        printf("1:%d    ", DXL_ID_saved[i]);
        for (int j = 0; j < 12; j++)
        {
            if (DXL_ID_saved[i] == DXL_ID_new[j])
            {
                bln = 1;
            }
        }
        if ((bln == 1) && (DXL_ID_saved[i] != 0))
        {
            DXL_ID_saved[i] = DXL_ID_saved[i];
        }
        else
        {
            DXL_ID_saved[i] = 0;
        }
        printf("2:%d    \n", DXL_ID_saved[i]);
    }
    printf("\n");
    printf("dxl_num: %d", DXL_num_new);
    Leg_assign(DXL_LEG, DXL_ID_saved, DXL_num_new);

    int DXL_idx = 0;
    for (int i = 0; i < 12; i++)
    {
        if (DXL_LEG[i] != 0)
        {
            DXL_ID[DXL_idx] = DXL_LEG[i];
            DXL_idx++;
        }
    }
    for (int i = 0; i < 12; i++)
    {
        printf("DXL_ID:%d     DXL_ID_LEG2: %d  \n", DXL_ID[i], DXL_LEG[i]);
    }
    groupSyncRead.clearParam();
    addparam_read(DXL_ID, DXL_num_new);
    groupSyncWrite.clearParam();
}

void Leg_plus(int DXL_ID_saved[], int DXL_ID_new[], int DXL_num_new, int DXL_LEG[], int DXL_ID[])
{
    int new_port = 0;
    int bln = 0;
    int DXL_added[2] = {0};
    new_port = conf_simple(DXL_ID_saved, DXL_num_new); // When the camera is not used, this function returns the smallest ID of empty port
    printf("new_port: %d  \n", new_port);

    int ii = 0;
    for (int i = 0; i < 12; i++)
    {
        bln = 0;
        printf("Priv:%d    New:%d       \n", DXL_ID_saved[i], DXL_ID_new[i]);
        for (int j = 0; j < 12; j++)
        {
            if (DXL_ID_new[i] == DXL_ID_saved[j])
            {
                bln = 1;
            }
        }
        if ((bln == 1) && (DXL_ID_new[i] != 0))
        {
            DXL_ID_saved[i] = DXL_ID_saved[i];
        }
        else
        {
            DXL_added[ii] = DXL_ID_new[i];
            ii++;
        }
    }

    for (int i = 0; i < ii; i++)
    {
        printf("new dxl: %d\n", DXL_added[i]);
    }

    DXL_ID_saved[2 * (new_port - 1)] = DXL_added[0];
    DXL_ID_saved[2 * (new_port - 1) + 1] = DXL_added[1];

    Leg_assign(DXL_LEG, DXL_ID_saved, DXL_num_new);

    int DXL_idx = 0;
    for (int i = 0; i < 12; i++)
    {
        if (DXL_LEG[i] != 0)
        {
            DXL_ID[DXL_idx] = DXL_LEG[i];
            DXL_idx++;
        }
    }
    for (int i = 0; i < 12; i++)
    {
        printf("DXL_ID:%d     DXL_ID_LEG2: %d  \n", DXL_ID[i], DXL_LEG[i]);
    }

    ind_addr(DXL_ID, DXL_num_new, ADDR_INDADDR_WRITE, ADDR_GOAL_POSITION, LEN_GOAL_POSITION);
    ind_addr(DXL_ID, DXL_num_new, ADDR_INDADDR_READ_POS, ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
    ind_addr(DXL_ID, DXL_num_new, ADDR_INDADDR_READ_VEL, ADDR_PRESENT_VEL, LEN_PRESENT_VELOCITY);
    ind_addr(DXL_ID, DXL_num_new, ADDR_INDADDR_READ_CUR, ADDR_PRESENT_CURRENT, LEN_PRESENT_CURRENT);

    write1(DXL_ID, DXL_num_new, ADDR_TORQUE_ENABLE, TORQUE_ENABLE);

    // Set Profile Vel & Acc
    PROFILE_V = 100; // Slow Down
    PROFILE_A = 200; // Slow Down
    write4(DXL_ID, DXL_num_new, ADDR_PROFILE_VEL, PROFILE_V);
    write4(DXL_ID, DXL_num_new, ADDR_PROFILE_ACCL, PROFILE_A);

    // PID GAIN
    write2(DXL_ID, DXL_num_new, ADDR_POS_P, 1500);
    write2(DXL_ID, DXL_num_new, ADDR_POS_D, 10);
    write2(DXL_ID, DXL_num_new, ADDR_POS_I, 100);

    groupSyncRead.clearParam();
    addparam_read(DXL_ID, DXL_num_new);
    groupSyncWrite.clearParam();

    // 			// PID GAIN

    if (DXL_num_new / 2 > 3)
    {
        for (int i = 0; i < DXL_num_new / 2; i++)
        {
            packetHandler->write4ByteTxRx(portHandler, DXL_ID[2 * i], ADDR_GOAL_POSITION, 1100, &dxl_error);
            packetHandler->write4ByteTxRx(portHandler, DXL_ID[2 * i + 1], ADDR_GOAL_POSITION, 2450, &dxl_error);
        }

        usleep(800000);

        for (int i = 0; i < DXL_num_new / 2; i++)
        {
            packetHandler->write4ByteTxRx(portHandler, DXL_ID[2 * i], ADDR_GOAL_POSITION, 2048, &dxl_error);
            packetHandler->write4ByteTxRx(portHandler, DXL_ID[2 * i + 1], ADDR_GOAL_POSITION, 2058, &dxl_error);
        }
        usleep(800000);
    }
}

int conf_simple(int DXL_ID_input[], int DXL_num)
{
    // Vision Part
    int idx_port[6] = {0};
    int idx_nport[6] = {0};
    int prt_num = 0;
    int nprt_num = 0;
    int iscolor = 0;

    int rl2 = 200, rh2 = 254, gl2 = 195, gh2 = 255, bl2 = 154, bh2 = 238; // RGB Threshold for white
    int row_h = 0, row_l = 150, col_h = 130, col_l = 130;
    int Yawpos_320[6] = {22, 229, 436, 643, 850, 1023};
    int newport = 0;

    int PROFILE_V = 100;
    int PROFILE_A = 50;

    write1(DXL320_ID, 2, ADDR_320_TORQUE, 1);
    write1(DXL320_ID, 2, ADDR_320_P, 100); // 0 ~ 254
    write1(DXL320_ID, 2, ADDR_320_I, 5);   // 0 ~ 254
    write1(DXL320_ID, 2, ADDR_320_D, 15);  // 0 ~ 254

    write1(DXL320_ID, 2, ADDR_320_LED, 3);
    packetHandler->write2ByteTxRx(portHandler, DXL320_ID[1], 30, 500, &dxl_error);

    for (int i = 0; i < 6; i++)
    {
        if (DXL_ID_input[2 * i] != 0)
        {
            idx_port[prt_num] = i + 1;
            prt_num++;
        }
        else
        {
            idx_nport[nprt_num] = i + 1;
            nprt_num++;
        }
        // printf("port: %d     nport %d  ",port_num, nport_num);
        // printf("port: %d     nport %d  \n",idx_port[port_num-1], idx_nport[nport_num-1]);
    }
    while (iscolor != 1)
    {
        for (int i = 0; i < nprt_num; i++)
        { // For loop for the number of detected Leg
            iscolor = 0;
            packetHandler->write2ByteTxRx(portHandler, DXL320_ID[0], 30, Yawpos_320[idx_nport[i] - 1], &dxl_error);
            // iscolor = camera(rh2, rl2, gh2, gl2, bh2, bl2, 500, 0.15, row_h, row_l, col_h, col_l);
            ///////////////////// No camera!!! - iscolor ==1 turn off when camera is enabled
            iscolor = 1;
            usleep(40000);
            if (iscolor == 1)
            {
                newport = idx_nport[i];
                printf("newport:%d   \n", newport);
                break;
            }
        }
        usleep(20000);
    }

    return newport;
}

void Leg_assign(int DXL_LEG[], int DXL_LEG_Input[], int DXL_num_Input)
{
    printf("Leg Configuration Start\n");
    // //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // //////////////////////////////////////////////// Leg Assign  /////////////////////////////////////////////////////
    // //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // int index_port[6] = {
    //     0,
    // };
    // int index_nport[6] = {
    //     0,
    // };
    std::vector<int> index_port(6, 1);
    std::vector<int> index_nport(6, 1);
    int Leg_num = DXL_num_Input / 2;
    int index_sum = 0;
    int DXL320_ID[2] = {100, 101};
    int Yawpos_320[6] = {22, 229, 436, 643, 850, 1023};
    float head_port = 0.0;
    int dxl_320_goal = 0;
    int index2 = 0;

    int prt_num = 0;
    int nprt_num = 0;
    // Find Empty port i == port num
    for (int i = 0; i < 6; i++)
    {
        if (DXL_LEG_Input[2 * i] == 0)
        {
            index_nport[nprt_num] = i + 1;
            nprt_num++;
        }

        else
        {
            index_port[prt_num] = i + 1;
            prt_num++;
        }

        printf("%d  port: %d    nport %d  \n", DXL_LEG_Input[2 * i], index_port[prt_num - 1], index_nport[nprt_num - 1]);
    }

    for (int i = 0; i < Leg_num; i++)
    {
        index_sum = index_sum + index_port[i];
    }

    if (Leg_num % 2 == 0)
    {
        head_port = ((float)index_sum / Leg_num);
    }
    else
    {
        head_port = ceil((float)index_sum / Leg_num);
    }

    int ii = 0;
    int iii = 0;
    switch (Leg_num)
    {

        //////////////////////////////////////////////// 1 Leg  /////////////////////////////////////////////////////
    case 1:
        for (int i = 0; i < DXL_num_Input; i++)
        {
            DXL_LEG[i] = DXL_LEG_Input[i];
        }
        break;

        //////////////////////////////////////////////// 2 Leg  /////////////////////////////////////////////////////
    case 2:
        index2 = (index_port[0] * index_port[0]) + index_port[1] * index_port[1]; // This is to identify special condition

        if (index2 == 26)
        {
            head_port = 6.0;
        }
        else if (index2 == 40)
        {
            head_port = 1.0;
        }
        else if (index2 == 37)
        {
            head_port = 6.5;
        }
        break;

        //////////////////////////////////////////////// 3 Leg  /////////////////////////////////////////////////////
    case 3:

        break;

        //////////////////////////////////////////////// 4 Leg  /////////////////////////////////////////////////////
    case 4:
        head_port = (index_nport[nprt_num - 1]);
        break;

        //////////////////////////////////////////////// 5 Leg  /////////////////////////////////////////////////////
    case 5:
        head_port = (index_nport[nprt_num - 1]);
        break;

        //////////////////////////////////////////////// 6 Leg  /////////////////////////////////////////////////////
    case 6:
        head_port = 1.5;
        break;
    }

    printf("heading:%f    \n", head_port);
    for (int i = ceil(head_port); i < 6 + ceil(head_port); i++)
    {
        if (i > 6)
        {
            iii = i - 7;
        }
        else
        {
            iii = i - 1;
        }

        printf("iii:%d      ", iii);
        printf("%d:  %d    %d\n", iii, DXL_LEG_Input[2 * iii], DXL_LEG_Input[2 * iii + 1]);
        DXL_LEG[2 * ii] = DXL_LEG_Input[2 * iii];
        DXL_LEG[2 * ii + 1] = DXL_LEG_Input[2 * iii + 1];
        ii = ii + 1;
    }
    for (int i = 0; i < 12; i++)
    {
        printf("ID: %d     ", DXL_LEG[i]);
    }

    dxl_320_goal = (int)(Yawpos_320[0] + ((head_port - 1.0) * 60.0 / 0.29));
    if (dxl_320_goal > 1023)
    {
        dxl_320_goal = 1023;
    }
    else if (dxl_320_goal < 0)
    {
        dxl_320_goal = 0;
    }
    packetHandler->write2ByteTxRx(portHandler, DXL320_ID[0], 30, dxl_320_goal, &dxl_error);
    printf("\n\n");
    usleep(500000);
}

// Motion for # leg 1 ~ 6
void LEG1(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot)
{
    int motor_angle[2];
    double theta_0 = PI / 2;
    double r_rot = 0.0;
    int dir = 1;
    double x0 = 0.0;
    double y0 = 0.0;
    double spin_mag = (10.0 / PI) * 180.0;

    int motor1 = 2048;
    int motor2 = 2048;

    x0 = R * cos(theta_p);
    y0 = R * sin(theta_p);

    double slope = atan((sin(theta_p - spin_mag) - sin(theta_p + spin_mag)) / (cos(theta_p - spin_mag) - cos(theta_p + spin_mag) + 0.0001));

    if (r_rot_temp < 0)
    {
        r_rot = fabs(r_rot_temp);
        dir = -1;
    }
    else
    {
        r_rot = r_rot_temp;
        dir = 1;
    }

    // Leg 1
    motor1 = 2048 + (trn * 10) + (20.0 * (x_amp)) * sin(2.0 * PI / 6.0 * t);
    if (motor1 > 3060)
    {
        motor1 = 3060;
    }
    else if (motor1 < 1030)
    {
        motor1 = 1030;
    }
    if (x_amp != 0)
    {
        motor2 = 2050 + 60 * (y_amp)*sin(2.0 * 2.0 * PI / 6.0 * t + PI);
    }
    else
    {
        motor2 = 2050 + 30 * (y_amp)*sin(2.0 * 2.0 * PI / 8.0 * t + PI);
    }
    motor_angle[0] = motor1;
    if (motor2 < 2030)
    {
        motor_angle[1] = 2030;
    }
    else
    {
        motor_angle[1] = motor2;
    }
    for (int i = 0; i < DXL_num; i++)
    {
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(motor_angle[i]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(motor_angle[i]));

        dxl_addparam_result = groupSyncWrite.addParam(DXL_ID[i], param_indirect_data_for_write);
    }
    dxl_comm_result = groupSyncWrite.txPacket();
    groupSyncWrite.clearParam();
}

void LEG2(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot)
{
    int *motor_angle_buffer;
    static int motor_angle[4] = {0};
    double theta_0 = PI / 6;
    double r_rot = 0.0;
    int dir = 1;
    double x0 = 0.0, y0 = 0.0;
    double spin_mag = (10.0 / PI) * 180.0;

    double x1 = 0.0, x2 = 0.0;
    double y1 = 0.0, y2 = 0.0;
    double x1_r = 0.0, x2_r = 0.0;
    double y1_r = 0.0, y2_r = 0.0;

    double x_IK_1 = 0.0, x_IK_2 = 0.0;
    double y_IK_1 = 0.0, y_IK_2 = 0.0;
    double y_thresh = 0.0;

    x0 = (R + 30.0) * cos(theta_p);
    y0 = (R + 30.0) * sin(theta_p);

    double slope = atan((sin(theta_p - spin_mag) - sin(theta_p + spin_mag)) / (cos(theta_p - spin_mag) - cos(theta_p + spin_mag) + 0.0001));

    if (r_rot_temp < 0)
    {
        r_rot = fabs(r_rot_temp);
        dir = -1;
    }
    else
    {
        r_rot = r_rot_temp;
        dir = 1;
    }

    // Leg 1 Left Front
    x1 = (1.0 + trn) * (x_amp + 10.0) * sin(2 * PI * t);
    y1 = y_amp * cos(2 * PI * t);
    if (y1 < y_thresh)
    {
        y1 = y_thresh;
    }
    x1_r = dir * r_rot * sin(2 * PI * t);
    y1_r = r_rot * cos(2 * PI * t);
    if (y1_r < y_thresh)
    {
        y1_r = y_thresh;
    }
    x_IK_1 = x1 * cos(theta_rot) - y1 * sin(theta_rot) + x1_r * cos(slope) - y1_r * sin(slope) + x0;
    y_IK_1 = x1 * sin(theta_rot) + y1 * cos(theta_rot) + x1_r * sin(slope) + y1_r * cos(slope) + y0;

    motor_angle_buffer = IK(x_IK_1, y_IK_1, theta_0);
    motor_angle[0] = motor_angle_buffer[0];
    motor_angle[1] = motor_angle_buffer[1];

    // Leg 2 Right Front
    x2 = (1.0 - trn) * (x_amp + 10.0) * sin(2 * PI * t);
    y2 = -y_amp * cos(2 * PI * t);
    if (y2 > y_thresh)
    {
        y2 = y_thresh;
    }
    x2_r = -dir * r_rot * sin(2 * PI * t);
    y2_r = -r_rot * cos(2 * PI * t);
    if (y2_r > y_thresh)
    {
        y2_r = y_thresh;
    }
    x_IK_2 = x2 * cos(theta_rot) - y2 * sin(theta_rot) + x2_r * cos(-slope) - y2_r * sin(-slope) + x0;
    y_IK_2 = x2 * sin(theta_rot) + y2 * cos(theta_rot) + x2_r * sin(-slope) + y2_r * cos(-slope) - y0;
    motor_angle_buffer = IK(x_IK_2, y_IK_2, -theta_0);

    motor_angle[2] = motor_angle_buffer[0];
    motor_angle[3] = motor_angle_buffer[1];
    for (int i = 0; i < DXL_num; i++)
    {
        // GOAL POS
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(motor_angle[i]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(motor_angle[i]));

        dxl_addparam_result = groupSyncWrite.addParam(DXL_ID[i], param_indirect_data_for_write);
    }

    dxl_comm_result = groupSyncWrite.txPacket();
    groupSyncWrite.clearParam();
}

void LEG3(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot)
{

    int *motor_angle_buffer;
    static int motor_angle[6] = {0};
    double theta_0 = PI / 4; // pi/3
    double r_rot = 0.0;
    int dir = 1;
    double x0 = 0.0, y0 = 0.0;
    double L0 = 119.75;
    double L1 = 56.75;
    double spin_mag = (10.0 / PI) * 180.0;

    double x1 = 0.0, x2 = 0.0, x3 = 0.0;
    double y1 = 0.0, y2 = 0.0, y3 = 0.0;
    double x1_r = 0.0, x2_r = 0.0, x3_r = 0.0;
    double y1_r = 0.0, y2_r = 0.0, y3_r = 0.0;

    double x_IK_1 = 0.0, x_IK_2 = 0.0, x_IK_3 = 0.0;
    double y_IK_1 = 0.0, y_IK_2 = 0.0, y_IK_3 = 0.0;
    double y_thresh = 0.0;

    x0 = (R + 35.0) * cos(theta_p);
    y0 = (R + 35.0) * sin(theta_p);

    double slope = atan((sin(theta_p - spin_mag) - sin(theta_p + spin_mag)) / (cos(theta_p - spin_mag) - cos(theta_p + spin_mag) + 0.0001));
    double y00 = sqrt(pow((x0 - (L0)*sin(theta_0)), 2) + pow((y0 - (L0)*cos(theta_0)), 2)) + L0;

    if (r_rot_temp < 0)
    {
        r_rot = fabs(r_rot_temp);
        dir = -1;
    }
    else
    {
        r_rot = r_rot_temp;
        dir = 1;
    }

    // Leg 1 Middle

    x1 = x_amp * sin(2 * PI * t);
    y1 = 0 * cos(2 * PI * t);
    if (y1 > y_thresh)
    {
        y1 = y_thresh;
    }
    x1_r = -dir * r_rot * sin(2 * PI * t);
    y1_r = -r_rot * cos(2 * PI * t);
    if (y1_r > y_thresh)
    {
        y1_r = y_thresh;
    }
    x_IK_1 = x1 * cos(theta_rot) - y1 * sin(theta_rot) + x1_r + y00;
    y_IK_1 = x1 * sin(theta_rot) + y1 * cos(theta_rot) + y1_r;
    motor_angle_buffer = IK(x_IK_1, y_IK_1, 0);
    motor_angle[0] = motor_angle_buffer[0];
    motor_angle[1] = motor_angle_buffer[1];

    // Leg 2 Left
    x2 = (1.0 + trn) * x_amp * sin(2 * PI * t + PI);
    y2 = y_amp * cos(2 * PI * t + PI);
    if (y2 < y_thresh)
    {
        y2 = y_thresh;
    }
    x2_r = dir * r_rot * sin(2 * PI * t + PI);
    y2_r = r_rot * cos(2 * PI * t + PI);
    if (y2_r < y_thresh)
    {
        y2_r = y_thresh;
    }
    x_IK_2 = x2 * cos(theta_rot) - y2 * sin(theta_rot) + x2_r * cos(slope) - y2_r * sin(slope) + x0;
    y_IK_2 = x2 * sin(theta_rot) + y2 * cos(theta_rot) + x2_r * sin(slope) + y2_r * cos(slope) + y0;

    motor_angle_buffer = IK(x_IK_2, y_IK_2, theta_0);
    motor_angle[2] = motor_angle_buffer[0];
    motor_angle[3] = motor_angle_buffer[1];

    // Leg 3 Right
    x3 = (1.0 - trn) * x_amp * sin(2 * PI * t + PI);
    y3 = -y_amp * cos(2 * PI * t + PI);
    if (y3 > y_thresh)
    {
        y3 = y_thresh;
    }
    x3_r = -dir * r_rot * sin(2 * PI * t + PI);
    y3_r = -r_rot * cos(2 * PI * t + PI);
    if (y3_r > y_thresh)
    {
        y3_r = y_thresh;
    }
    x_IK_3 = x3 * cos(theta_rot) - y3 * sin(theta_rot) + x3_r * cos(-slope) - y3_r * sin(-slope) + x0;
    y_IK_3 = x3 * sin(theta_rot) + y3 * cos(theta_rot) + x3_r * sin(-slope) + y3_r * cos(-slope) - y0;
    motor_angle_buffer = IK(x_IK_3, y_IK_3, -theta_0);
    motor_angle[4] = motor_angle_buffer[0];
    motor_angle[5] = motor_angle_buffer[1];
    for (int i = 0; i < DXL_num; i++)
    {
        // GOAL POS
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(motor_angle[i]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(motor_angle[i]));

        dxl_addparam_result = groupSyncWrite.addParam(DXL_ID[i], param_indirect_data_for_write);
    }

    dxl_comm_result = groupSyncWrite.txPacket();
    groupSyncWrite.clearParam();
}

void LEG4(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot)
{

    int *motor_angle_buffer;
    static int motor_angle[8] = {0};
    double theta_0 = 1.0472; // pi/3
    double r_rot = 0.0;
    int dir = 1;
    double x0 = 0.0;
    double y0 = 0.0;
    double spin_mag = (10.0 / PI) * 180.0;

    double x1 = 0.0, x2 = 0.0, x3 = 0.0, x4 = 0.0;
    double y1 = 0.0, y2 = 0.0, y3 = 0.0, y4 = 0.0;
    double x1_r = 0.0, x2_r = 0.0, x3_r = 0.0, x4_r = 0.0;
    double y1_r = 0.0, y2_r = 0.0, y3_r = 0.0, y4_r = 0.0;

    double x_IK_1 = 0.0, x_IK_2 = 0.0, x_IK_3 = 0.0, x_IK_4 = 0.0;
    double y_IK_1 = 0.0, y_IK_2 = 0.0, y_IK_3 = 0.0, y_IK_4 = 0.0;
    double y_thresh = 0.0;

    x0 = R * cos(theta_p);
    y0 = R * sin(theta_p);

    double slope = atan((sin(theta_p - spin_mag) - sin(theta_p + spin_mag)) / (cos(theta_p - spin_mag) - cos(theta_p + spin_mag) + 0.0001));

    if (r_rot_temp < 0)
    {
        r_rot = fabs(r_rot_temp);
        dir = -1;
    }
    else
    {
        r_rot = r_rot_temp;
        dir = 1;
    }

    // Leg 1 Left Front
    x1 = (1.0 + trn) * x_amp * sin(2 * PI * t);
    y1 = y_amp * cos(2 * PI * t);
    if (y1 < y_thresh)
    {
        y1 = y_thresh;
    }
    x1_r = dir * r_rot * sin(2 * PI * t);
    y1_r = r_rot * cos(2 * PI * t);
    if (y1_r < y_thresh)
    {
        y1_r = y_thresh;
    }
    x_IK_1 = x1 * cos(theta_rot) - y1 * sin(theta_rot) + x1_r * cos(slope) - y1_r * sin(slope) + x0;
    y_IK_1 = x1 * sin(theta_rot) + y1 * cos(theta_rot) + x1_r * sin(slope) + y1_r * cos(slope) + y0;

    motor_angle_buffer = IK(x_IK_1, y_IK_1, theta_0);
    motor_angle[0] = motor_angle_buffer[0];
    motor_angle[1] = motor_angle_buffer[1];

    // Leg 2 Left Rear
    x2 = (1.0 + trn) * x_amp * sin(2 * PI * t + PI);
    y2 = y_amp * cos(2 * PI * t + PI);
    if (y2 < y_thresh)
    {
        y2 = y_thresh;
    }
    x2_r = dir * r_rot * sin(2 * PI * t + PI);
    y2_r = r_rot * cos(2 * PI * t + PI);
    if (y2_r < y_thresh)
    {
        y2_r = y_thresh;
    }
    x_IK_2 = x2 * cos(theta_rot) - y2 * sin(theta_rot) + x2_r * cos(-slope) - y2_r * sin(-slope) - x0;
    y_IK_2 = x2 * sin(theta_rot) + y2 * cos(theta_rot) + x2_r * sin(-slope) + y2_r * cos(-slope) + y0;
    motor_angle_buffer = IK(x_IK_2, y_IK_2, 2 * theta_0);

    motor_angle[2] = motor_angle_buffer[0];
    motor_angle[3] = motor_angle_buffer[1];

    //Leg 3 Right Rear
    x3 = (1.0 - trn) * x_amp * sin(2 * PI * t);
    y3 = -y_amp * cos(2 * PI * t);
    if (y3 > y_thresh)
    {
        y3 = y_thresh;
    }
    x3_r = -dir * r_rot * sin(2 * PI * t);
    y3_r = -r_rot * cos(2 * PI * t);
    if (y3_r > y_thresh)
    {
        y3_r = y_thresh;
    }
    x_IK_3 = x3 * cos(theta_rot) - y3 * sin(theta_rot) + x3_r * cos(slope) - y3_r * sin(slope) - x0;
    y_IK_3 = x3 * sin(theta_rot) + y3 * cos(theta_rot) + x3_r * sin(slope) + y3_r * cos(slope) - y0;
    motor_angle_buffer = IK(x_IK_3, y_IK_3, -2 * theta_0);
    motor_angle[4] = motor_angle_buffer[0];
    motor_angle[5] = motor_angle_buffer[1];

    //Leg 4 Right Front
    x4 = (1.0 - trn) * x_amp * sin(2 * PI * t + PI);
    y4 = -y_amp * cos(2 * PI * t + PI);
    if (y4 > y_thresh)
    {
        y4 = y_thresh;
    }
    x4_r = -dir * r_rot * sin(2 * PI * t + PI);
    y4_r = -r_rot * cos(2 * PI * t + PI);
    if (y4_r > y_thresh)
    {
        y4_r = y_thresh;
    }
    x_IK_4 = x4 * cos(theta_rot) - y4 * sin(theta_rot) + x4_r * cos(-slope) - y4_r * sin(-slope) + x0;
    y_IK_4 = x4 * sin(theta_rot) + y4 * cos(theta_rot) + x4_r * sin(-slope) + y4_r * cos(-slope) - y0;
    motor_angle_buffer = IK(x_IK_4, y_IK_4, -theta_0);
    motor_angle[6] = motor_angle_buffer[0];
    motor_angle[7] = motor_angle_buffer[1];

    for (int i = 0; i < DXL_num; i++)
    {
        // GOAL POS
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(motor_angle[i]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(motor_angle[i]));

        dxl_addparam_result = groupSyncWrite.addParam(DXL_ID[i], param_indirect_data_for_write);
    }

    dxl_comm_result = groupSyncWrite.txPacket();
    groupSyncWrite.clearParam();
}

void LEG5(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot)
{
    int *motor_angle_buffer;
    static int motor_angle[10] = {0};
    double theta_0 = 1.0472; // pi/3
    double r_rot = 0.0;
    int dir = 1;
    double x0 = 0.0;
    double y0 = 0.0;
    double spin_mag = (10.0 / PI) * 180.0;
    double L0 = 119.75;
    double L1 = 56.75;

    double x1 = 0.0, x2 = 0.0, x3 = 0.0, x4 = 0.0, x5 = 0.0;
    double y1 = 0.0, y2 = 0.0, y3 = 0.0, y4 = 0.0, y5 = 0.0;
    double x1_r = 0.0, x2_r = 0.0, x3_r = 0.0, x4_r = 0.0, x5_r = 0.0;
    double y1_r = 0.0, y2_r = 0.0, y3_r = 0.0, y4_r = 0.0, y5_r = 0.0;

    double x_IK_1 = 0.0, x_IK_2 = 0.0, x_IK_3 = 0.0, x_IK_4 = 0.0, x_IK_5 = 0.0;
    double y_IK_1 = 0.0, y_IK_2 = 0.0, y_IK_3 = 0.0, y_IK_4 = 0.0, y_IK_5 = 0.0;
    double y_thresh = 0.0;
    double y00 = sqrt(pow((x0 - (L0)*sin(theta_0)), 2) + pow((y0 - (L0)*cos(theta_0)), 2)) + L0;

    x0 = R * cos(theta_p);
    y0 = R * sin(theta_p);

    double slope = atan((sin(theta_p - spin_mag) - sin(theta_p + spin_mag)) / (cos(theta_p - spin_mag) - cos(theta_p + spin_mag) + 0.0001));

    if (r_rot_temp < 0)
    {
        r_rot = fabs(r_rot_temp);
        dir = -1;
    }
    else
    {
        r_rot = r_rot_temp;
        dir = 1;
    }

    // Leg 1 Left Front
    x1 = (1.0 + trn) * x_amp * sin(2 * PI * t);
    y1 = y_amp * cos(2 * PI * t);
    if (y1 < y_thresh)
    {
        y1 = y_thresh;
    }
    x1_r = dir * r_rot * sin(2 * PI * t);
    y1_r = r_rot * cos(2 * PI * t);
    if (y1_r < y_thresh)
    {
        y1_r = y_thresh;
    }
    x_IK_1 = x1 * cos(theta_rot) - y1 * sin(theta_rot) + x1_r * cos(slope) - y1_r * sin(slope) + x0;
    y_IK_1 = x1 * sin(theta_rot) + y1 * cos(theta_rot) + x1_r * sin(slope) + y1_r * cos(slope) + y0;

    motor_angle_buffer = IK(x_IK_1, y_IK_1, theta_0);
    motor_angle[0] = motor_angle_buffer[0];
    motor_angle[1] = motor_angle_buffer[1];

    // Leg 2 Left Rear
    x2 = (1.0 + trn) * x_amp * sin(2 * PI * t + PI);
    y2 = y_amp * cos(2 * PI * t + PI);
    if (y2 < y_thresh)
    {
        y2 = y_thresh;
    }
    x2_r = dir * r_rot * sin(2 * PI * t + PI);
    y2_r = r_rot * cos(2 * PI * t + PI);
    if (y2_r < y_thresh)
    {
        y2_r = y_thresh;
    }
    x_IK_2 = x2 * cos(theta_rot) - y2 * sin(theta_rot) + x2_r * cos(-slope) - y2_r * sin(-slope) - x0;
    y_IK_2 = x2 * sin(theta_rot) + y2 * cos(theta_rot) + x2_r * sin(-slope) + y2_r * cos(-slope) + y0;
    motor_angle_buffer = IK(x_IK_2, y_IK_2, 2 * theta_0);
    motor_angle[2] = motor_angle_buffer[0];
    motor_angle[3] = motor_angle_buffer[1];

    //Leg 3 Right Rear
    x3 = (1.0 - trn) * x_amp * sin(2 * PI * t);
    y3 = -y_amp * cos(2 * PI * t);
    if (y3 > y_thresh)
    {
        y3 = y_thresh;
    }
    x3_r = -dir * r_rot * sin(2 * PI * t);
    y3_r = -r_rot * cos(2 * PI * t);
    if (y3_r > y_thresh)
    {
        y3_r = y_thresh;
    }
    x_IK_3 = x3 * cos(theta_rot) - y3 * sin(theta_rot) + x3_r * cos(slope) - y3_r * sin(slope) - x0;
    y_IK_3 = x3 * sin(theta_rot) + y3 * cos(theta_rot) + x3_r * sin(slope) + y3_r * cos(slope) - y0;
    motor_angle_buffer = IK(x_IK_3, y_IK_3, -2 * theta_0);
    motor_angle[6] = motor_angle_buffer[0];
    motor_angle[7] = motor_angle_buffer[1];

    //Leg 4 Right Front
    x4 = (1.0 - trn) * x_amp * sin(2 * PI * t + PI);
    y4 = -y_amp * cos(2 * PI * t + PI);
    if (y4 > y_thresh)
    {
        y4 = y_thresh;
    }
    x4_r = -dir * r_rot * sin(2 * PI * t + PI);
    y4_r = -r_rot * cos(2 * PI * t + PI);
    if (y4_r > y_thresh)
    {
        y4_r = y_thresh;
    }
    x_IK_4 = x4 * cos(theta_rot) - y4 * sin(theta_rot) + x4_r * cos(-slope) - y4_r * sin(-slope) + x0;
    y_IK_4 = x4 * sin(theta_rot) + y4 * cos(theta_rot) + x4_r * sin(-slope) + y4_r * cos(-slope) - y0;
    motor_angle_buffer = IK(x_IK_4, y_IK_4, -theta_0);
    motor_angle[8] = motor_angle_buffer[0];
    motor_angle[9] = motor_angle_buffer[1];

    // Tail
    x5 = 80 + (x_amp)*0.2*sin(4.0 * PI * t);
    y5 = -2.0 * y_amp * cos(2.0 * PI * t);

    x_IK_5 = x5 * cos(theta_rot) - y5 * sin(theta_rot) + y00;
    y_IK_5 = x5 * sin(theta_rot) + y5 * cos(theta_rot);
    motor_angle_buffer = IK(x_IK_5, y_IK_5, 0);
    motor_angle[4] = motor_angle_buffer[0];
    motor_angle[5] = motor_angle_buffer[1];

    for (int i = 0; i < DXL_num; i++)
    {
        // GOAL POS
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(motor_angle[i]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(motor_angle[i]));

        dxl_addparam_result = groupSyncWrite.addParam(DXL_ID[i], param_indirect_data_for_write);
    }

    dxl_comm_result = groupSyncWrite.txPacket();
    groupSyncWrite.clearParam();
}

void LEG6(int DXL_ID[], int DXL_num, double t, double R, double theta_p, double x_amp, double y_amp, double trn, double r_rot_temp, double theta_rot)
{

    int *motor_angle_buffer;
    static int motor_angle[12] = {
        0,
    };
    double theta_0 = PI / 6; // pi/6
    double r_rot = 0.0;
    int dir = 1;
    double x0 = 0.0;
    double y0 = 0.0;
    double spin_mag = (20.0 / PI) * 180.0;
    double L0 = 119.75;
    double L1 = 56.75;

    double x1 = 0.0, x2 = 0.0, x3 = 0.0, x4 = 0.0, x5 = 0.0, x6 = 0.0;
    double y1 = 0.0, y2 = 0.0, y3 = 0.0, y4 = 0.0, y5 = 0.0, y6 = 0.0;
    double x1_r = 0.0, x2_r = 0.0, x3_r = 0.0, x4_r = 0.0, x5_r = 0.0, x6_r = 0.0;
    double y1_r = 0.0, y2_r = 0.0, y3_r = 0.0, y4_r = 0.0, y5_r = 0.0, y6_r = 0.0;

    double x_IK_1 = 0.0, x_IK_2 = 0.0, x_IK_3 = 0.0, x_IK_4 = 0.0, x_IK_5 = 0.0, x_IK_6 = 0.0;
    double y_IK_1 = 0.0, y_IK_2 = 0.0, y_IK_3 = 0.0, y_IK_4 = 0.0, y_IK_5 = 0.0, y_IK_6 = 0.0;
    double y_thresh = 0.0;

    x0 = R * cos(theta_p);
    y0 = R * sin(theta_p);

    double slope = atan((sin(theta_p - spin_mag) - sin(theta_p + spin_mag)) / (cos(theta_p - spin_mag) - cos(theta_p + spin_mag) + 0.0001));
    double y00 = sqrt(pow((x0 - (L0)*sin(theta_0)), 2) + pow((y0 - (L0)*cos(theta_0)), 2)) + L0;

    if (r_rot_temp < 0)
    {
        r_rot = fabs(r_rot_temp);
        dir = -1;
    }
    else
    {
        r_rot = r_rot_temp;
        dir = 1;
    }

    // Leg 1 Left Front
    x1 = (1.0 + trn) * x_amp * sin(2 * PI * t);
    y1 = y_amp * cos(2 * PI * t);
    if (y1 < y_thresh)
    {
        y1 = y_thresh;
    }
    x1_r = dir * r_rot * sin(2 * PI * t);
    y1_r = r_rot * cos(2 * PI * t);
    if (y1_r < y_thresh)
    {
        y1_r = y_thresh;
    }
    x_IK_1 = x1 * cos(theta_rot) - y1 * sin(theta_rot) + x1_r * cos(slope) - y1_r * sin(slope) + x0;
    y_IK_1 = x1 * sin(theta_rot) + y1 * cos(theta_rot) + x1_r * sin(slope) + y1_r * cos(slope) + y0;

    motor_angle_buffer = IK(x_IK_1, y_IK_1, theta_0);
    motor_angle[0] = motor_angle_buffer[0];
    motor_angle[1] = motor_angle_buffer[1];

    // Leg 2 Left Middle
    x2 = (1.0 + trn) * x_amp * sin(2 * PI * t + PI);
    y2 = y_amp * cos(2 * PI * t + PI);
    if (y2 < y_thresh)
    {
        y2 = y_thresh;
    }
    x2_r = dir * r_rot * sin(2 * PI * t + PI);
    y2_r = r_rot * cos(2 * PI * t + PI);
    if (y2_r < y_thresh)
    {
        y2_r = y_thresh;
    }
    x_IK_2 = x2 * cos(theta_rot) - y2 * sin(theta_rot) + x2_r;
    y_IK_2 = x2 * sin(theta_rot) + y2 * cos(theta_rot) + y2_r + y00;
    motor_angle_buffer = IK(x_IK_2, y_IK_2, PI / 2);

    motor_angle[2] = motor_angle_buffer[0];
    motor_angle[3] = motor_angle_buffer[1];

    // Leg 3 Left Rear
    x3 = (1.0 + trn) * x_amp * sin(2 * PI * t);
    y3 = y_amp * cos(2 * PI * t);
    if (y3 < y_thresh)
    {
        y3 = y_thresh;
    }
    x3_r = dir * r_rot * sin(2 * PI * t);
    y3_r = r_rot * cos(2 * PI * t);
    if (y3_r < y_thresh)
    {
        y3_r = y_thresh;
    }
    x_IK_3 = x3 * cos(theta_rot) - y3 * sin(theta_rot) + x3_r * cos(-slope) - y3_r * sin(-slope) - x0;
    y_IK_3 = x3 * sin(theta_rot) + y3 * cos(theta_rot) + x3_r * sin(-slope) + y3_r * cos(-slope) + y0;
    motor_angle_buffer = IK(x_IK_3, y_IK_3, 5 * theta_0);

    motor_angle[4] = motor_angle_buffer[0];
    motor_angle[5] = motor_angle_buffer[1];

    //Leg 4 Right Rear
    x4 = (1.0 - trn) * x_amp * sin(2 * PI * t + PI);
    y4 = -y_amp * cos(2 * PI * t + PI);
    if (y4 > y_thresh)
    {
        y4 = y_thresh;
    }
    x4_r = -dir * r_rot * sin(2 * PI * t + PI);
    y4_r = -r_rot * cos(2 * PI * t + PI);
    if (y4_r > y_thresh)
    {
        y4_r = y_thresh;
    }
    x_IK_4 = x4 * cos(theta_rot) - y4 * sin(theta_rot) + x4_r * cos(slope) - y4_r * sin(slope) - x0;
    y_IK_4 = x4 * sin(theta_rot) + y4 * cos(theta_rot) + x4_r * sin(slope) + y4_r * cos(slope) - y0;
    motor_angle_buffer = IK(x_IK_4, y_IK_4, -5 * theta_0);
    motor_angle[6] = motor_angle_buffer[0];
    motor_angle[7] = motor_angle_buffer[1];

    //Leg 5 Right Middle

    x5 = (1.0 - trn) * x_amp * sin(2 * PI * t);
    y5 = -y_amp * cos(2 * PI * t);
    if (y5 > y_thresh)
    {
        y5 = y_thresh;
    }
    x5_r = -dir * r_rot * sin(2 * PI * t);
    y5_r = -r_rot * cos(2 * PI * t);
    if (y5_r > y_thresh)
    {
        y5_r = y_thresh;
    }
    x_IK_5 = x5 * cos(theta_rot) - y5 * sin(theta_rot) + x5_r;
    y_IK_5 = x5 * sin(theta_rot) + y5 * cos(theta_rot) + y5_r - y00;
    motor_angle_buffer = IK(x_IK_5, y_IK_5, -PI / 2);
    motor_angle[8] = motor_angle_buffer[0];
    motor_angle[9] = motor_angle_buffer[1];

    //Leg 6 Right Front
    x6 = (1.0 - trn) * x_amp * sin(2 * PI * t + PI);
    y6 = -y_amp * cos(2 * PI * t + PI);
    if (y6 > y_thresh)
    {
        y6 = y_thresh;
    }

    x6_r = -dir * r_rot * sin(2 * PI * t + PI);
    y6_r = -r_rot * cos(2 * PI * t + PI);

    if (y6_r > y_thresh)
    {
        y6_r = y_thresh;
    }
    x_IK_6 = x6 * cos(theta_rot) - y6 * sin(theta_rot) + x6_r * cos(-slope) - y6_r * sin(-slope) + x0;
    y_IK_6 = x6 * sin(theta_rot) + y6 * cos(theta_rot) + x6_r * sin(-slope) + y6_r * cos(-slope) - y0;

    motor_angle_buffer = IK(x_IK_6, y_IK_6, -theta_0);
    motor_angle[10] = motor_angle_buffer[0];
    motor_angle[11] = motor_angle_buffer[1];

    for (int i = 0; i < DXL_num; i++)
    {
        // GOAL POS
        param_indirect_data_for_write[0] = DXL_LOBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[1] = DXL_HIBYTE(DXL_LOWORD(motor_angle[i]));
        param_indirect_data_for_write[2] = DXL_LOBYTE(DXL_HIWORD(motor_angle[i]));
        param_indirect_data_for_write[3] = DXL_HIBYTE(DXL_HIWORD(motor_angle[i]));

        dxl_addparam_result = groupSyncWrite.addParam(DXL_ID[i], param_indirect_data_for_write);
        if (dxl_addparam_result != true)
        {
            fprintf(stderr, "[ID:%03d] groupSyncWrite addparam failed", DXL_ID[i]);
        }
    }

    dxl_comm_result = groupSyncWrite.txPacket();
    if (dxl_comm_result != COMM_SUCCESS)
        printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
    groupSyncWrite.clearParam();
}

int DXL_Read(int DXL_ID[], int DXL_num, int DXL_pos[])
{
    int com_err = 0;
    dxl_comm_result = groupSyncRead.txRxPacket();
    if (dxl_comm_result != COMM_SUCCESS)
    {
        com_err = 1;
        // printf("comerr\n");
    }
    else
    {
        com_err = 0;
    }
    for (int i = 0; i < DXL_num; i++)
    {
        // printf("ID: %d  ", DXL_ID[i]);
        DXL_pos[i] = groupSyncRead.getData(DXL_ID[i], ADDR_PRESENT_POSITION, LEN_PRESENT_POSITION);
    }

    // printf("DXL_num: %d \n", DXL_num);

    return com_err;
}