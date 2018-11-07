﻿/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef OM_CHAIN_KINEMATICS_H_
#define OM_CHAIN_KINEMATICS_H_

#include <Eigen.h>
#include <Eigen/LU>
#include <Eigen/QR>

#include <math.h>
#include <vector>
#include <map>
#include <stdio.h>
#include <robotis_manipulator_manager.h>
#include <robotis_manipulator.h>


using namespace ROBOTIS_MANIPULATOR;

namespace OM_KINEMATICS
{
class Link : public ROBOTIS_MANIPULATOR::Kinematics
{
public:
  Link(){}
  virtual ~Link(){}

  virtual void updatePassiveJointValue(Manipulator *manipulator);
  virtual Eigen::MatrixXd jacobian(Manipulator *manipulator, Name tool_name);

  virtual void forward(Manipulator *manipulator);
  virtual void forward(Manipulator *manipulator, Name component_name);
  virtual std::vector<double> inverse(Manipulator *manipulator, Name tool_name, Pose target_pose);

  void solveKinematicsSinglePoint(Manipulator *manipulator, Name component_name);
  std::vector<double> geometricInverse(Manipulator *manipulator, Name tool_name, Pose target_pose); //for basic model);
};

} // namespace OM_CHAIN_KINEMATICS

#endif // OM_CHAIN_KINEMATICS_H_