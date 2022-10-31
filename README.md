# SNAPBOT 2.0
<div align="center">
<img src="https://user-images.githubusercontent.com/7993458/184975618-2754c274-dcd9-45dc-908a-67227676da2a.jpg" width = "550">
</div>

-----------------
**Snapbot** is a reconfigurable legged robot to emulate configuration changes and various styles of legged locomotion. **Snapbot V2.0** has upgraded hardware to improve dynamical performance and robustness of the system for repetitive, and dynamical operations. 

Snapbot consists of a body and leg modules. The body modules includes a computer, batteries, electronic circuit boards, and a camera module. The camera module has 2-DOF, which can be used to recognize the robot's configuration or see surroundings. The leg module can be attached and detached to the body using magnetic connection, so it doesn't need any fastners but the docking structure ensures sturdy connection even during an impulsive motions. 

In this repository, we provide STL files and a BOM that is necessary to build the Snapbot V2.0 hardware, MATLAB Simscape simulation models, ROS package for the hardware, and URDF of the robot. 



## Contents
1. [3D printing STL](#3d-printing-stl)
2. [MATLAB Simulation](#matlab-simulation)
3. [ROS Package](#ros-package)
4. [Related Work](#related-work)
5. [Citation](#citation)

## 3D printing STL
STL files are provided in "3D printing STL" folder. The parts can be printed using most of 3D printing methods (FDM, SLS, SLA, etc.). The folder also contains the BOM (Bill of Materials) which lists the off-the-shelf components needed to build the Snapbot2.0 hardware. 

Currently there are two different head designs:
1. Default head
<img src="https://user-images.githubusercontent.com/7993458/199081487-12a50863-39c7-4169-9c77-fade4f5a1c8c.jpg" width = "550">

2. Babyface head
<img src="https://user-images.githubusercontent.com/7993458/199081439-8fc511b5-8348-4f07-8856-be7eedeee6ea.jpg" width = "550">

"3D printing STL/Head" folder has subfolders of two heads including the head cover and motor brackets. 

## MATLAB Simulation
<img src="https://user-images.githubusercontent.com/7993458/185044196-23c7702c-8e2a-4fc4-8d24-60f312364eda.gif" width = 400>

Simulation model for Snapbot V2 was created using MATLAB Simscape multi-body (MATLAB 2021a). There are three simulations models for 2 leg, 4 leg, and 6 leg configuration with the motion generator implemented in the [paper](#citation).



## ROS Package
ROS package used to operate Snapbot V2 is provided with URDF file and meshes. By executing launch file "snapbot.launch", the robot can be teleoperated using keyboard inputs.


## Related Work

- [**Snapbot: A reconfigurable legged robot**](https://ieeexplore.ieee.org/abstract/document/8206477)
- [**Trajectory-based Probabilistic Policy Gradient for Learning Locomotion Behaviors**](https://ieeexplore.ieee.org/abstract/document/8794207)
- [**Automated Deep Reinforcement Learning Environment for Hardware of a Modular Legged Robot**](https://ieeexplore.ieee.org/abstract/document/8442201)
 
<!-- @INPROCEEDINGS{8206477,
  author={Kim, Joohyung and Alspach, Alexander and Yamane, Katsu},
  booktitle={2017 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS)}, 
  title={Snapbot: A reconfigurable legged robot}, 
  year={2017}}
  

@INPROCEEDINGS{8442201,  
  author={Ha, Sehoon and Kim, Joohyung and Yamane, Katsu},  
  booktitle={2018 15th International Conference on Ubiquitous Robots (UR)},   
  title={Automated Deep Reinforcement Learning Environment for Hardware of a Modular Legged Robot},   
  year={2018}}

@INPROCEEDINGS{8794207,  
  author={Choi, Sungjoon and Kim, Joohyung},  
  booktitle={2019 International Conference on Robotics and Automation (ICRA)},   
  title={Trajectory-based Probabilistic Policy Gradient for Learning Locomotion Behaviors},   
  year={2019}}
 -->

## Citation
```
@INPROCEEDINGS{9341279,  
  author={Gim, Kevin G. and Kim, Joohyung},  
  booktitle={2020 IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS)},   
  title={Snapbot V2: a Reconfigurable Legged Robot with a Camera for Self Configuration Recognition},   
  year={2020}
}
```
Link: https://ieeexplore.ieee.org/abstract/document/9341279



