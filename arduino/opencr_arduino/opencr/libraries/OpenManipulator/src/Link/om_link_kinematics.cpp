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

/* Authors: Hye-Jong KIM, Darby Lim */

#include "../../include/Link/om_link_kinematics.h"

using namespace ROBOTIS_MANIPULATOR;
using namespace OM_KINEMATICS;

void Link::updatePassiveJointValue(Manipulator *manipulator)
{
  std::vector<double> joint_angle;
  joint_angle = manipulator->getAllActiveJointValue();

  joint_angle.push_back(joint_angle[1] - joint_angle[2]);
  joint_angle.push_back(-M_PI - (joint_angle[1] - joint_angle[2]));
  joint_angle.push_back(-M_PI - (joint_angle[1] - joint_angle[2]));
  joint_angle.push_back(-M_PI - joint_angle[2]);
  joint_angle.push_back(joint_angle[1]);
  joint_angle.push_back(-(15 * DEG2RAD) - joint_angle[1]);
  joint_angle.push_back(joint_angle[2] - (195 * DEG2RAD));
  joint_angle.push_back((90 * DEG2RAD) - joint_angle[2]);

  manipulator->setAllJointValue(joint_angle);
}

Eigen::MatrixXd Link::jacobian(Manipulator *manipulator, Name tool_name)
{

}

void Link::forward(Manipulator *manipulator)
{
  Pose pose_to_wolrd;
  Pose link_relative_pose;
  Eigen::Matrix3d rodrigues_rotation_matrix;
  Pose result_pose;

  //Base Pose Set (from world)
  pose_to_wolrd = manipulator->getWorldPose();
  link_relative_pose = manipulator->getComponentRelativePoseToParent(manipulator->getWorldChildName());
  rodrigues_rotation_matrix = RM_MATH::rodriguesRotationMatrix(manipulator->getJointAxis(manipulator->getWorldChildName()), manipulator->getJointValue(manipulator->getWorldChildName()));

  result_pose.position = pose_to_wolrd.position + pose_to_wolrd.orientation * link_relative_pose.position;
  result_pose.orientation = pose_to_wolrd.orientation * link_relative_pose.orientation * rodrigues_rotation_matrix;
  manipulator->setComponentPoseToWorld(manipulator->getWorldChildName(), result_pose);

  //Next Component Pose Set
  for (int i = 0; i < manipulator->getComponentChildName(manipulator->getWorldChildName()).size(); i++)
  {
    solveKinematicsSinglePoint(manipulator, manipulator->getComponentChildName(manipulator->getWorldChildName()).at(i));
  }
}

void Link::forward(Manipulator *manipulator, Name component_name)
{
  forward(manipulator);
}

std::vector<double> Link::inverse(Manipulator *manipulator, Name tool_name, Pose target_pose)
{
  return geometricInverse(manipulator, tool_name, target_pose);
}


void Link::solveKinematicsSinglePoint(Manipulator *manipulator, Name component_name)
{
  Pose parent_pose;
  Pose link_relative_pose;
  Eigen::Matrix3d rodrigues_rotation_matrix;
  Pose result_pose;

  parent_pose = manipulator->getComponentPoseToWorld(manipulator->getComponentParentName(component_name));
  link_relative_pose = manipulator->getComponentRelativePoseToParent(component_name);
  rodrigues_rotation_matrix = RM_MATH::rodriguesRotationMatrix(manipulator->getJointAxis(component_name), manipulator->getJointValue(component_name));

  result_pose.position = parent_pose.position + parent_pose.orientation * link_relative_pose.position;
  result_pose.orientation = parent_pose.orientation * link_relative_pose.orientation * rodrigues_rotation_matrix;

  manipulator->setComponentPoseToWorld(component_name, result_pose);
  for (int i = 0; i < manipulator->getComponentChildName(component_name).size(); i++)
  {
    solveKinematicsSinglePoint(manipulator, manipulator->getComponentChildName(component_name).at(i));
  }
}

std::vector<double> Link::geometricInverse(Manipulator *manipulator, Name tool_name, Pose target_pose)
{
  std::vector<double> target_angle_vector;
  Eigen::Vector3d control_position; //joint6-joint1
  Eigen::Vector3d tool_relative_position = manipulator->getComponentRelativePositionToParent(tool_name);
  Eigen::Vector3d base_position = manipulator->getComponentPositionToWorld(manipulator->getWorldChildName());
  Eigen::Vector3d temp_vector;

  double target_angle[3];
  double link[3];
  double temp_x;
  double temp_y;

  temp_x = target_pose.position(0) - base_position(0);
  temp_y = target_pose.position(1) - base_position(1);
  target_angle[0] = atan2(temp_y, temp_x);

  control_position(0) = target_pose.position(0) - tool_relative_position(0) * cos(target_angle[0]);
  control_position(1) = target_pose.position(1) - tool_relative_position(0) * sin(target_angle[0]);
  control_position(2) = target_pose.position(2) - tool_relative_position(2);

  // temp_vector = omlink.link_[0].getRelativeJointPosition(1,0);
  temp_vector = manipulator->getComponentRelativePositionToParent(manipulator->getComponentParentName(manipulator->getComponentParentName(manipulator->getComponentParentName(tool_name))));
  link[0] = temp_vector(2);
  // temp_vector = omlink.link_[1].getRelativeJointPosition(5,1);
  temp_vector = manipulator->getComponentRelativePositionToParent(manipulator->getComponentParentName(manipulator->getComponentParentName(tool_name)));
  link[1] = temp_vector(0);
  // temp_vector = omlink.link_[4].getRelativeJointPosition(6,5);
  temp_vector = manipulator->getComponentRelativePositionToParent(manipulator->getComponentParentName(tool_name));
  link[2] = temp_vector(0);

  temp_y = control_position(2) - base_position(2) - link[0];
  temp_x = (control_position(0) - base_position(0)) / cos(target_angle[0]);

  target_angle[1] = acos(((temp_x * temp_x + temp_y * temp_y + link[1] * link[1] - link[2] * link[2])) / (2 * link[1] * sqrt(temp_x * temp_x + temp_y * temp_y))) + atan2(temp_y, temp_x);
  target_angle[2] = acos((link[1] * link[1] + link[2] * link[2] - (temp_x * temp_x + temp_y * temp_y)) / (2 * link[1] * link[2])) + target_angle[1];

  target_angle_vector.push_back(target_angle[0]);
  target_angle_vector.push_back(-target_angle[1]);
  target_angle_vector.push_back(-target_angle[2]);

  return target_angle_vector;
} //for basic model)















