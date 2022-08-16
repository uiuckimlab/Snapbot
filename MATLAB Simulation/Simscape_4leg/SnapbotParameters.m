% Inertial Parameters

density_ABS = 1.07; % g/cm^3;
mass_xm430 = 82; % g

% Contact Parameters

Sphere_R_1 = 0.007;
Sphere_R_2 = 0.001;
Plane_x = 5;
Plane_y = 2;
Plane_d = 0.001;

Contact_Stiff_1 = 1e4;
Contact_Damp_1 = 1e2;

Contact_Stiff_2 = 1e4;
Contact_Damp_2 = 1e2;

Coff_KF_1 = 0.8;
Coff_SF_1 = 0.9;
Thres_Vel_1 = 0.001;

Coff_KF_2 = 0.5;
Coff_SF_2 = 0.7;
Thres_Vel_2 = 0.001;

% Motion Parameters

Motor_1_amp = 0.4;
Motor_1_freq = pi;
Motor_1_phase = pi;

Motor_2_amp = 0.4;
Motor_2_freq = pi;
Motor_2_phase = pi;
Motor_2_bias = pi;

Upper_limit = 5;
Lower_limit = pi;

