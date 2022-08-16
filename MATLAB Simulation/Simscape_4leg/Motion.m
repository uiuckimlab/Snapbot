% IK

z = 236;
x = 10;
y = 90;
L0 = 119.75;
L1 = 56.75;
L2 = 60;
L3 = 90;
L4 = 47.43;
L5 = 25;
L6 = 24.5;
theta_offset = deg2rad(14.18);
theta_0_1 = deg2rad(0);

theta_1 = atan(x/z) - theta_0_1;

r3 = sqrt(((x-L1*sin(theta_1)-L0*sin(theta_1))^2)+y^2 + (z-L1*cos(theta_1)-L0*cos(theta_0_1))^2);

theta_3 = acos((L2^2 + L3^2 - r3^2)/(2*L2*L3))-pi/2;

alpha1 = acos((L2^2 + r3^2 - L3^2)/(2*L2*r3));
alpha2 = acos(y/r3);
theta_2 = alpha1 + alpha2 - pi/2;