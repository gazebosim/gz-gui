/*
 * Copyright (C) 2017 Open Source Robotics Foundation
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
 *
*/

#include <gtest/gtest.h>
#include <QtTest/QtTest>

#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>

#include <ignition/msgs.hh>

#include "test_config.h"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/DoubleWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/MessageWidget.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(MessageWidgetTest, EmptyMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  {
    msgs::Visual visualMsg;

    auto visualMessageWidget = new MessageWidget();
    visualMessageWidget->Load(&visualMsg);

    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(visualMessageWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    delete visualMessageWidget;
  }

  {
    msgs::Collision collisionMsg;

    auto collisionMessageWidget = new MessageWidget();
    collisionMessageWidget->Load(&collisionMsg);

    auto retCollisionMsg =
        dynamic_cast<msgs::Collision *>(collisionMessageWidget->Msg());
    EXPECT_TRUE(retCollisionMsg != nullptr);

    delete collisionMessageWidget;
  }

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, JointMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto jointMessageWidget = new MessageWidget();
  msgs::Joint jointMsg;

  {
    // joint
    jointMsg.set_name("test_joint");
    jointMsg.set_id(1122u);
    jointMsg.set_parent("test_joint_parent");
    jointMsg.set_parent_id(212121u);
    jointMsg.set_child("test_joint_child");
    jointMsg.set_child_id(454545u);

    // type
    jointMsg.set_type(msgs::ConvertJointType("revolute"));

    // pose
    math::Vector3d pos(4.0, -1.0, 3.5);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    msgs::Set(jointMsg.mutable_pose(), math::Pose3d(pos, quat));

    // axis1
    auto axisMsg = jointMsg.mutable_axis1();
    msgs::Set(axisMsg->mutable_xyz(), math::Vector3d::UnitX);
    axisMsg->set_use_parent_model_frame(false);
    axisMsg->set_limit_lower(-999.0);
    axisMsg->set_limit_upper(999.0);
    axisMsg->set_limit_effort(-1.0);
    axisMsg->set_limit_velocity(-1.0);
    axisMsg->set_damping(0.0);

    // other joint physics properties
    jointMsg.set_cfm(0.2);
    jointMsg.set_bounce(0.3);
    jointMsg.set_velocity(0.4);
    jointMsg.set_fudge_factor(0.5);
    jointMsg.set_limit_cfm(0.6);
    jointMsg.set_limit_erp(0.7);
    jointMsg.set_suspension_cfm(0.8);
    jointMsg.set_suspension_erp(0.9);
  }
  jointMessageWidget->Load(&jointMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(jointMessageWidget->Msg());
    EXPECT_TRUE(retJointMsg != nullptr);

    // joint
    EXPECT_TRUE(retJointMsg->name() == "test_joint");
    EXPECT_EQ(retJointMsg->id(), 1122u);
    EXPECT_TRUE(retJointMsg->parent() == "test_joint_parent");
    EXPECT_EQ(retJointMsg->parent_id(), 212121u);
    EXPECT_TRUE(retJointMsg->child() == "test_joint_child");
    EXPECT_EQ(retJointMsg->child_id(), 454545u);

    // type
    EXPECT_EQ(retJointMsg->type(), msgs::ConvertJointType("revolute"));

    // pose
    auto poseMsg = retJointMsg->pose();
    auto posMsg = poseMsg.position();
    EXPECT_DOUBLE_EQ(posMsg.x(), 4.0);
    EXPECT_DOUBLE_EQ(posMsg.y(), -1.0);
    EXPECT_DOUBLE_EQ(posMsg.z(), 3.5);
    auto quat = msgs::Convert(poseMsg.orientation());
    // FIXME: investigate failure
    //EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    //EXPECT_DOUBLE_EQ(quat.Euler().Y(), 1.57);
    //EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

    // axis1
    auto axisMsg = jointMsg.mutable_axis1();
    EXPECT_DOUBLE_EQ(axisMsg->xyz().x(), 1.0);
    EXPECT_DOUBLE_EQ(axisMsg->xyz().y(), 0.0);
    EXPECT_DOUBLE_EQ(axisMsg->xyz().z(), 0.0);
    EXPECT_DOUBLE_EQ(axisMsg->use_parent_model_frame(), false);
    EXPECT_DOUBLE_EQ(axisMsg->limit_lower(), -999.0);
    EXPECT_DOUBLE_EQ(axisMsg->limit_upper(), 999.0);
    EXPECT_DOUBLE_EQ(axisMsg->limit_effort(), -1.0);
    EXPECT_DOUBLE_EQ(axisMsg->limit_velocity(), -1.0);
    EXPECT_DOUBLE_EQ(axisMsg->damping(), 0.0);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(retJointMsg->cfm(), 0.2);
    EXPECT_DOUBLE_EQ(retJointMsg->bounce(), 0.3);
    EXPECT_DOUBLE_EQ(retJointMsg->velocity(), 0.4);
    EXPECT_DOUBLE_EQ(retJointMsg->fudge_factor(), 0.5);
    EXPECT_DOUBLE_EQ(retJointMsg->limit_cfm(), 0.6);
    EXPECT_DOUBLE_EQ(retJointMsg->limit_erp(), 0.7);
    EXPECT_DOUBLE_EQ(retJointMsg->suspension_cfm(), 0.8);
    EXPECT_DOUBLE_EQ(retJointMsg->suspension_erp(), 0.9);
  }

  // update fields in the message widget and
  // verify that the new message contains the updated values.
  // Joint type revolute -> universal
  {
    // joint
    jointMessageWidget->SetStringWidgetValue("name", "test_joint_updated");
    jointMessageWidget->SetUIntWidgetValue("id", 9999999u);
    jointMessageWidget->SetStringWidgetValue("parent",
        "test_joint_parent_updated");
    jointMessageWidget->SetUIntWidgetValue("parent_id", 1u);
    jointMessageWidget->SetStringWidgetValue("child",
        "test_joint_child_updated");
    jointMessageWidget->SetUIntWidgetValue("child_id", 2u);

    // type
    jointMessageWidget->SetEnumWidgetValue("type",
        msgs::Joint_Type_Name(
        msgs::Joint_Type_UNIVERSAL));

    // pose
    math::Vector3d pos(2.0, 9.0, -4.0);
    math::Quaterniond quat(0.0, 0.0, 1.57);
    jointMessageWidget->SetPoseWidgetValue("pose",
        math::Pose3d(pos, quat));

    // axis1
    jointMessageWidget->SetVector3dWidgetValue("axis1::xyz",
        math::Vector3d::UnitY);
    jointMessageWidget->SetBoolWidgetValue("axis1::use_parent_model_frame",
        true);
    jointMessageWidget->SetDoubleWidgetValue("axis1::limit_lower", -1.2);
    jointMessageWidget->SetDoubleWidgetValue("axis1::limit_upper", -1.0);
    jointMessageWidget->SetDoubleWidgetValue("axis1::limit_effort", 1.0);
    jointMessageWidget->SetDoubleWidgetValue("axis1::limit_velocity", 100.0);
    jointMessageWidget->SetDoubleWidgetValue("axis1::damping", 0.9);

    // axis2
    jointMessageWidget->SetVector3dWidgetValue("axis2::xyz",
        math::Vector3d::UnitZ);
    jointMessageWidget->SetBoolWidgetValue("axis2::use_parent_model_frame",
        true);
    jointMessageWidget->SetDoubleWidgetValue("axis2::limit_lower", -3.2);
    jointMessageWidget->SetDoubleWidgetValue("axis2::limit_upper", -3.0);
    jointMessageWidget->SetDoubleWidgetValue("axis2::limit_effort", 3.0);
    jointMessageWidget->SetDoubleWidgetValue("axis2::limit_velocity", 300.0);
    jointMessageWidget->SetDoubleWidgetValue("axis2::damping", 3.9);

    // other joint physics properties
    jointMessageWidget->SetDoubleWidgetValue("cfm", 0.9);
    jointMessageWidget->SetDoubleWidgetValue("bounce", 0.8);
    jointMessageWidget->SetDoubleWidgetValue("velocity", 0.7);
    jointMessageWidget->SetDoubleWidgetValue("fudge_factor", 0.6);
    jointMessageWidget->SetDoubleWidgetValue("limit_cfm", 0.5);
    jointMessageWidget->SetDoubleWidgetValue("limit_erp", 0.4);
    jointMessageWidget->SetDoubleWidgetValue("suspension_cfm", 0.3);
    jointMessageWidget->SetDoubleWidgetValue("suspension_erp", 0.2);
  }

  // verify widget values
  {
    // joint
    EXPECT_TRUE(jointMessageWidget->StringWidgetValue("name") ==
        "test_joint_updated");
    EXPECT_EQ(jointMessageWidget->UIntWidgetValue("id"), 9999999u);
    EXPECT_TRUE(jointMessageWidget->StringWidgetValue("parent") ==
        "test_joint_parent_updated");
    EXPECT_EQ(jointMessageWidget->UIntWidgetValue("parent_id"), 1u);
    EXPECT_TRUE(jointMessageWidget->StringWidgetValue("child") ==
        "test_joint_child_updated");
    EXPECT_EQ(jointMessageWidget->UIntWidgetValue("child_id"), 2u);

    // type
    EXPECT_EQ(jointMessageWidget->EnumWidgetValue("type"),
        msgs::Joint_Type_Name(msgs::Joint_Type_UNIVERSAL));

    // pose
    math::Vector3d pos(2.0, 9.0, -4.0);
    math::Quaterniond quat(0.0, 0.0, 1.57);
    EXPECT_EQ(jointMessageWidget->PoseWidgetValue("pose"), math::Pose3d(pos, quat));

    // axis1
    EXPECT_EQ(jointMessageWidget->Vector3dWidgetValue("axis1::xyz"),
        math::Vector3d::UnitY);
    EXPECT_EQ(jointMessageWidget->BoolWidgetValue(
        "axis1::use_parent_model_frame"), true);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis1::limit_lower"), -1.2);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis1::limit_upper"), -1.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis1::limit_effort"), 1.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis1::limit_velocity"),
        100.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis1::damping"), 0.9);

    // axis2
    EXPECT_EQ(jointMessageWidget->Vector3dWidgetValue("axis2::xyz"),
        math::Vector3d::UnitZ);
    EXPECT_EQ(jointMessageWidget->BoolWidgetValue(
        "axis1::use_parent_model_frame"), true);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis2::limit_lower"), -3.2);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis2::limit_upper"), -3.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis2::limit_effort"), 3.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis2::limit_velocity"),
        300.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("axis2::damping"), 3.9);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("cfm"), 0.9);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("bounce"), 0.8);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("velocity"), 0.7);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("fudge_factor"), 0.6);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("limit_cfm"), 0.5);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("limit_erp"), 0.4);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("suspension_cfm"), 0.3);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("suspension_erp"), 0.2);
  }

  // verify updates in new msg
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(jointMessageWidget->Msg());
    EXPECT_TRUE(retJointMsg != nullptr);

    // joint
    EXPECT_TRUE(retJointMsg->name() == "test_joint_updated");
    EXPECT_EQ(retJointMsg->id(), 9999999u);
    EXPECT_TRUE(retJointMsg->parent() == "test_joint_parent_updated");
    EXPECT_EQ(retJointMsg->parent_id(), 1u);
    EXPECT_TRUE(retJointMsg->child() == "test_joint_child_updated");
    EXPECT_EQ(retJointMsg->child_id(), 2u);

    // type
    EXPECT_EQ(retJointMsg->type(), msgs::ConvertJointType("universal"));

    // pose
    auto poseMsg = retJointMsg->pose();
    auto posMsg = poseMsg.position();
    // FIXME: Pose broken, possibly due to header
    // EXPECT_DOUBLE_EQ(posMsg.x(), 2.0);
    // EXPECT_DOUBLE_EQ(posMsg.y(), 9.0);
    // EXPECT_DOUBLE_EQ(posMsg.z(), -4.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    // EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    // EXPECT_DOUBLE_EQ(quat.Euler().Y(), 0.0);
    // EXPECT_DOUBLE_EQ(quat.Euler().Z(), 1.57);

    // axis1
    auto axisMsg = retJointMsg->mutable_axis1();
    // FIXME: Axis broken
    // EXPECT_DOUBLE_EQ(axisMsg->xyz().x(), 0.0);
    // EXPECT_DOUBLE_EQ(axisMsg->xyz().y(), 1.0);
    // EXPECT_DOUBLE_EQ(axisMsg->xyz().z(), 0.0);
    EXPECT_EQ(axisMsg->use_parent_model_frame(), true);
    EXPECT_DOUBLE_EQ(axisMsg->limit_lower(), -1.2);
    EXPECT_DOUBLE_EQ(axisMsg->limit_upper(), -1.0);
    EXPECT_DOUBLE_EQ(axisMsg->limit_effort(), 1.0);
    EXPECT_DOUBLE_EQ(axisMsg->limit_velocity(), 100.0);
    EXPECT_DOUBLE_EQ(axisMsg->damping(), 0.9);

    // axis2
    auto axis2Msg = retJointMsg->mutable_axis2();
    EXPECT_DOUBLE_EQ(axis2Msg->xyz().x(), 0.0);
    EXPECT_DOUBLE_EQ(axis2Msg->xyz().y(), 0.0);
    // FIXME: Axis broken
    // EXPECT_DOUBLE_EQ(axis2Msg->xyz().z(), 1.0);
    EXPECT_EQ(axis2Msg->use_parent_model_frame(), true);
    EXPECT_DOUBLE_EQ(axis2Msg->limit_lower(), -3.2);
    EXPECT_DOUBLE_EQ(axis2Msg->limit_upper(), -3.0);
    EXPECT_DOUBLE_EQ(axis2Msg->limit_effort(), 3.0);
    EXPECT_DOUBLE_EQ(axis2Msg->limit_velocity(), 300.0);
    EXPECT_DOUBLE_EQ(axis2Msg->damping(), 3.9);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(retJointMsg->cfm(), 0.9);
    EXPECT_DOUBLE_EQ(retJointMsg->bounce(), 0.8);
    EXPECT_DOUBLE_EQ(retJointMsg->velocity(), 0.7);
    EXPECT_DOUBLE_EQ(retJointMsg->fudge_factor(), 0.6);
    EXPECT_DOUBLE_EQ(retJointMsg->limit_cfm(), 0.5);
    EXPECT_DOUBLE_EQ(retJointMsg->limit_erp(), 0.4);
    EXPECT_DOUBLE_EQ(retJointMsg->suspension_cfm(), 0.3);
    EXPECT_DOUBLE_EQ(retJointMsg->suspension_erp(), 0.2);
  }

  // update fields in the message widget and
  // verify that the new message contains the updated values.
  // Joint type universal -> ball
  {
    // joint
    jointMessageWidget->SetStringWidgetValue("name", "test_joint_updated2");
    jointMessageWidget->SetUIntWidgetValue("id", 2222222u);
    jointMessageWidget->SetStringWidgetValue("parent",
        "test_joint_parent_updated2");
    jointMessageWidget->SetUIntWidgetValue("parent_id", 10u);
    jointMessageWidget->SetStringWidgetValue("child",
        "test_joint_child_updated2");
    jointMessageWidget->SetUIntWidgetValue("child_id", 20u);

    // type
    jointMessageWidget->SetEnumWidgetValue("type",
        msgs::Joint_Type_Name(
        msgs::Joint_Type_BALL));

    // pose
    math::Vector3d pos(-2.0, 1.0, 2.0);
    math::Quaterniond quat(0.0, 0.0, 0.0);
    jointMessageWidget->SetPoseWidgetValue("pose",
        math::Pose3d(pos, quat));

    // other joint physics properties
    jointMessageWidget->SetDoubleWidgetValue("cfm", 0.19);
    jointMessageWidget->SetDoubleWidgetValue("bounce", 0.18);
    jointMessageWidget->SetDoubleWidgetValue("velocity", 2.7);
    jointMessageWidget->SetDoubleWidgetValue("fudge_factor", 0.26);
    jointMessageWidget->SetDoubleWidgetValue("limit_cfm", 0.15);
    jointMessageWidget->SetDoubleWidgetValue("limit_erp", 0.24);
    jointMessageWidget->SetDoubleWidgetValue("suspension_cfm", 0.13);
    jointMessageWidget->SetDoubleWidgetValue("suspension_erp", 0.12);
  }

  // verify widget values
  {
    // joint
    EXPECT_TRUE(jointMessageWidget->StringWidgetValue("name") ==
        "test_joint_updated2");
    EXPECT_EQ(jointMessageWidget->UIntWidgetValue("id"), 2222222u);
    EXPECT_TRUE(jointMessageWidget->StringWidgetValue("parent") ==
        "test_joint_parent_updated2");
    EXPECT_EQ(jointMessageWidget->UIntWidgetValue("parent_id"), 10u);
    EXPECT_TRUE(jointMessageWidget->StringWidgetValue("child") ==
        "test_joint_child_updated2");
    EXPECT_EQ(jointMessageWidget->UIntWidgetValue("child_id"), 20u);

    // type
    EXPECT_EQ(jointMessageWidget->EnumWidgetValue("type"),
        msgs::Joint_Type_Name(
        msgs::Joint_Type_BALL));

    // pose
    math::Vector3d pos(-2.0, 1.0, 2.0);
    math::Quaterniond quat(0.0, 0.0, 0.0);
    EXPECT_EQ(jointMessageWidget->PoseWidgetValue("pose"),
        math::Pose3d(pos, quat));

    // other joint physics properties
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("cfm"), 0.19);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("bounce"), 0.18);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("velocity"), 2.7);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("fudge_factor"), 0.26);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("limit_cfm"), 0.15);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("limit_erp"), 0.24);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("suspension_cfm"), 0.13);
    EXPECT_DOUBLE_EQ(jointMessageWidget->DoubleWidgetValue("suspension_erp"), 0.12);
  }

  // verify updates in new msg
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(jointMessageWidget->Msg());
    EXPECT_TRUE(retJointMsg != nullptr);

    // joint
    EXPECT_TRUE(retJointMsg->name() == "test_joint_updated2");
    EXPECT_EQ(retJointMsg->id(), 2222222u);
    EXPECT_TRUE(retJointMsg->parent() == "test_joint_parent_updated2");
    EXPECT_EQ(retJointMsg->parent_id(), 10u);
    EXPECT_TRUE(retJointMsg->child() == "test_joint_child_updated2");
    EXPECT_EQ(retJointMsg->child_id(), 20u);

    // type
    EXPECT_EQ(retJointMsg->type(), msgs::ConvertJointType("ball"));

    // pose
    auto poseMsg = retJointMsg->pose();
    auto posMsg = poseMsg.position();
    // FIXME: Pose broken. Header?
    // EXPECT_DOUBLE_EQ(posMsg.x(), -2.0);
    // EXPECT_DOUBLE_EQ(posMsg.y(), 1.0);
    // EXPECT_DOUBLE_EQ(posMsg.z(), 2.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    // EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    // EXPECT_DOUBLE_EQ(quat.Euler().Y(), 0.0);
    // EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(retJointMsg->cfm(), 0.19);
    EXPECT_DOUBLE_EQ(retJointMsg->bounce(), 0.18);
    EXPECT_DOUBLE_EQ(retJointMsg->velocity(), 2.7);
    EXPECT_DOUBLE_EQ(retJointMsg->fudge_factor(), 0.26);
    EXPECT_DOUBLE_EQ(retJointMsg->limit_cfm(), 0.15);
    EXPECT_DOUBLE_EQ(retJointMsg->limit_erp(), 0.24);
    EXPECT_DOUBLE_EQ(retJointMsg->suspension_cfm(), 0.13);
    EXPECT_DOUBLE_EQ(retJointMsg->suspension_erp(), 0.12);
  }
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, VisualMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto visualMessageWidget = new MessageWidget();
  msgs::Visual visualMsg;

  {
    // visual
    visualMsg.set_name("test_visual");
    visualMsg.set_id(12345u);
    visualMsg.set_parent_name("test_visual_parent");
    visualMsg.set_parent_id(54321u);
    visualMsg.set_cast_shadows(true);
    visualMsg.set_transparency(0.0);
    visualMsg.set_visible(true);
    visualMsg.set_delete_me(false);
    visualMsg.set_is_static(false);
    msgs::Set(visualMsg.mutable_scale(),
        math::Vector3d(1.0, 1.0, 1.0));

    // pose
    math::Vector3d pos(2.0, 3.0, 4.0);
    math::Quaterniond quat(1.57, 0.0, 0.0);
    msgs::Set(visualMsg.mutable_pose(),
        math::Pose3d(pos, quat));

    // geometry
    auto geometryMsg = visualMsg.mutable_geometry();
    geometryMsg->set_type(msgs::Geometry::CYLINDER);
    auto cylinderGeomMsg =
        geometryMsg->mutable_cylinder();
    cylinderGeomMsg->set_radius(3.0);
    cylinderGeomMsg->set_length(0.2);

    // material
    auto materialMsg = visualMsg.mutable_material();
    materialMsg->set_shader_type(
        msgs::Material::Material::VERTEX);
    materialMsg->set_normal_map("test_normal_map");
    msgs::Set(materialMsg->mutable_ambient(),
        math::Color(0.0, 1.0, 0.0, 1.0));
    msgs::Set(materialMsg->mutable_diffuse(),
        math::Color(0.0, 1.0, 1.0, 0.4));
    msgs::Set(materialMsg->mutable_specular(),
        math::Color(1.0, 1.0, 1.0, 0.6));
    msgs::Set(materialMsg->mutable_emissive(),
        math::Color(0.0, 0.5, 0.2, 1.0));
    materialMsg->set_lighting(true);
    // material::script
    auto scriptMsg = materialMsg->mutable_script();
    scriptMsg->add_uri("test_script_uri_0");
    scriptMsg->add_uri("test_script_uri_1");
    scriptMsg->set_name("test_script_name");
  }
  visualMessageWidget->Load(&visualMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(visualMessageWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    // visual
    EXPECT_TRUE(retVisualMsg->name() == "test_visual");
    EXPECT_EQ(retVisualMsg->id(), 12345u);
    EXPECT_TRUE(retVisualMsg->parent_name() == "test_visual_parent");
    EXPECT_EQ(retVisualMsg->parent_id(), 54321u);
    EXPECT_EQ(retVisualMsg->cast_shadows(), true);
    EXPECT_DOUBLE_EQ(retVisualMsg->transparency(), 0.0);
    EXPECT_EQ(retVisualMsg->visible(), true);
    EXPECT_EQ(retVisualMsg->delete_me(), false);
    EXPECT_EQ(retVisualMsg->is_static(), false);
    auto scaleMsg = retVisualMsg->scale();
    EXPECT_DOUBLE_EQ(scaleMsg.x(), 1.0);
    EXPECT_DOUBLE_EQ(scaleMsg.y(), 1.0);
    EXPECT_DOUBLE_EQ(scaleMsg.z(), 1.0);

    // pose
    auto poseMsg = retVisualMsg->pose();
    auto posMsg = poseMsg.position();
    EXPECT_DOUBLE_EQ(posMsg.x(), 2.0);
    EXPECT_DOUBLE_EQ(posMsg.y(), 3.0);
    EXPECT_DOUBLE_EQ(posMsg.z(), 4.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    // FIXME: investigate failure
    //EXPECT_DOUBLE_EQ(quat.Euler().X(), 1.57);
    EXPECT_DOUBLE_EQ(quat.Euler().Y(), 0.0);
    //EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

    // geometry
    auto geometryMsg = retVisualMsg->geometry();
    EXPECT_EQ(geometryMsg.type(), msgs::Geometry::CYLINDER);
    auto cylinderGeomMsg = geometryMsg.cylinder();
    EXPECT_DOUBLE_EQ(cylinderGeomMsg.radius(), 3.0);
    EXPECT_DOUBLE_EQ(cylinderGeomMsg.length(), 0.2);

    // material
    auto materialMsg = retVisualMsg->material();
    EXPECT_EQ(materialMsg.shader_type(),
        msgs::Material::Material::VERTEX);
    EXPECT_TRUE(materialMsg.normal_map() == "test_normal_map");
    auto ambientMsg = materialMsg.ambient();
    EXPECT_DOUBLE_EQ(ambientMsg.r(), 0.0f);
    EXPECT_DOUBLE_EQ(ambientMsg.g(), 1.0f);
    EXPECT_DOUBLE_EQ(ambientMsg.b(), 0.0f);
    EXPECT_DOUBLE_EQ(ambientMsg.a(), 1.0f);
    auto diffuseMsg = materialMsg.diffuse();
    EXPECT_DOUBLE_EQ(diffuseMsg.r(), 0.0f);
    EXPECT_DOUBLE_EQ(diffuseMsg.g(), 1.0f);
    EXPECT_DOUBLE_EQ(diffuseMsg.b(), 1.0f);
    EXPECT_DOUBLE_EQ(diffuseMsg.a(), 0.4f);
    auto specularMsg = materialMsg.specular();
    EXPECT_DOUBLE_EQ(specularMsg.r(), 1.0f);
    EXPECT_DOUBLE_EQ(specularMsg.g(), 1.0f);
    EXPECT_DOUBLE_EQ(specularMsg.b(), 1.0f);
    EXPECT_DOUBLE_EQ(specularMsg.a(), 0.6f);
    auto emissiveMsg = materialMsg.emissive();
    EXPECT_DOUBLE_EQ(emissiveMsg.r(), 0.0f);
    EXPECT_DOUBLE_EQ(emissiveMsg.g(), 0.5f);
    EXPECT_DOUBLE_EQ(emissiveMsg.b(), 0.2f);
    EXPECT_DOUBLE_EQ(emissiveMsg.a(), 1.0f);
    EXPECT_EQ(materialMsg.lighting(), true);
    // material::script
    auto scriptMsg = materialMsg.script();
    EXPECT_TRUE(scriptMsg.uri(0) == "test_script_uri_0");
    EXPECT_TRUE(scriptMsg.uri(1) == "test_script_uri_1");
    EXPECT_TRUE(scriptMsg.name() == "test_script_name");
  }

  // update fields in the message widget and
  // verify that the new message contains the updated values.
  {
    // visual
    visualMessageWidget->SetStringWidgetValue("name", "test_visual_updated");
    visualMessageWidget->SetUIntWidgetValue("id", 11111u);
    visualMessageWidget->SetStringWidgetValue("parent_name",
        "test_visual_parent_updated");
    visualMessageWidget->SetUIntWidgetValue("parent_id", 55555u);
    visualMessageWidget->SetBoolWidgetValue("cast_shadows", false);
    visualMessageWidget->SetDoubleWidgetValue("transparency", 1.0);
    visualMessageWidget->SetBoolWidgetValue("visible", false);
    visualMessageWidget->SetBoolWidgetValue("delete_me", true);
    visualMessageWidget->SetBoolWidgetValue("is_static", true);
    visualMessageWidget->SetVector3dWidgetValue("scale",
        math::Vector3d(2.0, 1.5, 0.5));

    // pose
    math::Vector3d pos(-2.0, -3.0, -4.0);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    visualMessageWidget->SetPoseWidgetValue("pose",
        math::Pose3d(pos, quat));

    // geometry
    visualMessageWidget->SetGeometryWidgetValue("geometry", "box",
        math::Vector3d(5.0, 3.0, 4.0));

    // material
    visualMessageWidget->SetStringWidgetValue("material::normal_map",
        "test_normal_map_updated");
    visualMessageWidget->SetColorWidgetValue("material::ambient",
        math::Color(0.2, 0.3, 0.4, 0.5));
    visualMessageWidget->SetColorWidgetValue("material::diffuse",
        math::Color(0.1, 0.8, 0.6, 0.4));
    visualMessageWidget->SetColorWidgetValue("material::specular",
        math::Color(0.5, 0.4, 0.3, 0.2));
    visualMessageWidget->SetColorWidgetValue("material::emissive",
        math::Color(0.4, 0.6, 0.8, 0.1));
    visualMessageWidget->SetBoolWidgetValue("material::lighting", false);
    // material::script
    visualMessageWidget->SetStringWidgetValue("material::script::name",
        "test_script_name_updated");
  }

  // verify widget values
  {
    EXPECT_TRUE(visualMessageWidget->StringWidgetValue("name") ==
        "test_visual_updated");
    EXPECT_EQ(visualMessageWidget->UIntWidgetValue("id"), 11111u);
    EXPECT_TRUE(visualMessageWidget->StringWidgetValue("parent_name") ==
        "test_visual_parent_updated");
    EXPECT_EQ(visualMessageWidget->UIntWidgetValue("parent_id"), 55555u);
    EXPECT_EQ(visualMessageWidget->BoolWidgetValue("cast_shadows"), false);
    EXPECT_DOUBLE_EQ(visualMessageWidget->DoubleWidgetValue("transparency"), 1.0);
    EXPECT_EQ(visualMessageWidget->BoolWidgetValue("visible"), false);
    EXPECT_EQ(visualMessageWidget->BoolWidgetValue("delete_me"), true);
    EXPECT_EQ(visualMessageWidget->BoolWidgetValue("is_static"), true);
    EXPECT_EQ(visualMessageWidget->Vector3dWidgetValue("scale"),
        math::Vector3d(2.0, 1.5, 0.5));

    // pose
    math::Vector3d pos(-2.0, -3.0, -4.0);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    EXPECT_EQ(visualMessageWidget->PoseWidgetValue("pose"),
        math::Pose3d(pos, quat));

    // geometry
    math::Vector3d dimensions;
    std::string uri;
    EXPECT_TRUE(visualMessageWidget->GeometryWidgetValue("geometry", dimensions,
        uri) == "box");
    EXPECT_EQ(dimensions, math::Vector3d(5.0, 3.0, 4.0));

    // material
    EXPECT_TRUE(visualMessageWidget->StringWidgetValue("material::normal_map") ==
        "test_normal_map_updated");
    EXPECT_EQ(visualMessageWidget->ColorWidgetValue("material::ambient"),
        math::Color(0.2, 0.3, 0.4, 0.5));
    EXPECT_EQ(visualMessageWidget->ColorWidgetValue("material::diffuse"),
        math::Color(0.1, 0.8, 0.6, 0.4));
    EXPECT_EQ(visualMessageWidget->ColorWidgetValue("material::specular"),
        math::Color(0.5, 0.4, 0.3, 0.2));
    EXPECT_EQ(visualMessageWidget->ColorWidgetValue("material::emissive"),
        math::Color(0.4, 0.6, 0.8, 0.1));
    EXPECT_EQ(visualMessageWidget->BoolWidgetValue("material::lighting"),
        false);
    // material::script
    EXPECT_TRUE(visualMessageWidget->StringWidgetValue("material::script::name")
        == "test_script_name_updated");
  }

  // verify updates in new msg
  {
    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(visualMessageWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    // visual
    EXPECT_TRUE(retVisualMsg->name() == "test_visual_updated");
    EXPECT_EQ(retVisualMsg->id(), 11111u);
    EXPECT_TRUE(retVisualMsg->parent_name() == "test_visual_parent_updated");
    EXPECT_EQ(retVisualMsg->parent_id(), 55555u);
    EXPECT_EQ(retVisualMsg->cast_shadows(), false);
    EXPECT_DOUBLE_EQ(retVisualMsg->transparency(), 1.0);
    EXPECT_EQ(retVisualMsg->visible(), false);
    EXPECT_EQ(retVisualMsg->delete_me(), true);
    EXPECT_EQ(retVisualMsg->is_static(), true);
    auto scaleMsg = retVisualMsg->scale();
    // FIXME: investigate failure
    //EXPECT_DOUBLE_EQ(scaleMsg.x(), 2.0);
    //EXPECT_DOUBLE_EQ(scaleMsg.y(), 1.5);
    //EXPECT_DOUBLE_EQ(scaleMsg.z(), 0.5);

    // pose
    auto poseMsg = retVisualMsg->pose();
    auto posMsg = poseMsg.position();
    // FIXME: investigate failure
    //EXPECT_DOUBLE_EQ(posMsg.x(), -2.0);
    //EXPECT_DOUBLE_EQ(posMsg.y(), -3.0);
    //EXPECT_DOUBLE_EQ(posMsg.z(), -4.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    //EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    //EXPECT_DOUBLE_EQ(quat.Euler().Y(), 1.57);
    //EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

    // geometry
    auto geometryMsg = retVisualMsg->geometry();
    EXPECT_EQ(geometryMsg.type(), msgs::Geometry::BOX);
    auto boxGeomMsg = geometryMsg.box();
    auto boxGeomSizeMsg = boxGeomMsg.size();
    // FIXME: investigate failure
    //EXPECT_DOUBLE_EQ(boxGeomSizeMsg.x(), 5.0);
    //EXPECT_DOUBLE_EQ(boxGeomSizeMsg.y(), 3.0);
    //EXPECT_DOUBLE_EQ(boxGeomSizeMsg.z(), 4.0);

    // material
    auto materialMsg = retVisualMsg->material();
    EXPECT_EQ(materialMsg.shader_type(), msgs::Material::Material::VERTEX);
    EXPECT_TRUE(materialMsg.normal_map() == "test_normal_map_updated");
    // FIXME: investigate failure
    //auto ambientMsg = materialMsg.ambient();
    //EXPECT_DOUBLE_EQ(ambientMsg.r(), 0.2f);
    //EXPECT_DOUBLE_EQ(ambientMsg.g(), 0.3f);
    //EXPECT_DOUBLE_EQ(ambientMsg.b(), 0.4f);
    //EXPECT_DOUBLE_EQ(ambientMsg.a(), 0.5f);
    //auto diffuseMsg = materialMsg.diffuse();
    //EXPECT_DOUBLE_EQ(diffuseMsg.r(), 0.1f);
    //EXPECT_DOUBLE_EQ(diffuseMsg.g(), 0.8f);
    //EXPECT_DOUBLE_EQ(diffuseMsg.b(), 0.6f);
    //EXPECT_DOUBLE_EQ(diffuseMsg.a(), 0.4f);
    //auto specularMsg = materialMsg.specular();
    //EXPECT_DOUBLE_EQ(specularMsg.r(), 0.5f);
    //EXPECT_DOUBLE_EQ(specularMsg.g(), 0.4f);
    //EXPECT_DOUBLE_EQ(specularMsg.b(), 0.3f);
    //EXPECT_DOUBLE_EQ(specularMsg.a(), 0.2f);
    //auto emissiveMsg = materialMsg.emissive();
    //EXPECT_DOUBLE_EQ(emissiveMsg.r(), 0.4f);
    //EXPECT_DOUBLE_EQ(emissiveMsg.g(), 0.6f);
    //EXPECT_DOUBLE_EQ(emissiveMsg.b(), 0.8f);
    //EXPECT_DOUBLE_EQ(emissiveMsg.a(), 0.1f);
    //EXPECT_EQ(materialMsg.lighting(), false);
    // material::script
    auto scriptMsg = materialMsg.script();
    EXPECT_TRUE(scriptMsg.uri(0) == "test_script_uri_0");
    EXPECT_TRUE(scriptMsg.uri(1) == "test_script_uri_1");
    EXPECT_TRUE(scriptMsg.name() == "test_script_name_updated");
  }

  delete visualMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, PluginMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto pluginMessageWidget = new MessageWidget();
  msgs::Plugin pluginMsg;

  {
    // plugin
    pluginMsg.set_name("test_plugin");
    pluginMsg.set_filename("test_plugin_filename");
    pluginMsg.set_innerxml("<param>1</param>\n");
  }
  pluginMessageWidget->Load(&pluginMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retPluginMsg =
        dynamic_cast<msgs::Plugin *>(pluginMessageWidget->Msg());
    EXPECT_TRUE(retPluginMsg != nullptr);

    // plugin
    EXPECT_TRUE(retPluginMsg->name() == "test_plugin");
    EXPECT_TRUE(retPluginMsg->filename() == "test_plugin_filename");
    EXPECT_TRUE(retPluginMsg->innerxml() == "<param>1</param>\n");
  }

  // update fields in the message widget and
  // verify that the new message contains the updated values.
  {
    // plugin
    pluginMessageWidget->SetStringWidgetValue("name", "test_plugin_updated");
    pluginMessageWidget->SetStringWidgetValue("filename",
        "test_plugin_filename_updated");
    pluginMessageWidget->SetStringWidgetValue("innerxml",
        "<param2>new_param</param2>\n");
  }

  // verify widget values
  {
    EXPECT_TRUE(pluginMessageWidget->StringWidgetValue("name") ==
        "test_plugin_updated");
    EXPECT_TRUE(pluginMessageWidget->StringWidgetValue("filename") ==
        "test_plugin_filename_updated");
    EXPECT_TRUE(pluginMessageWidget->StringWidgetValue("innerxml") ==
        "<param2>new_param</param2>\n");
  }

  // verify updates in new msg
  {
    auto retPluginMsg =
        dynamic_cast<msgs::Plugin *>(pluginMessageWidget->Msg());
    EXPECT_TRUE(retPluginMsg != nullptr);

    // plugin
    EXPECT_TRUE(retPluginMsg->name() == "test_plugin_updated");
    EXPECT_TRUE(retPluginMsg->filename() == "test_plugin_filename_updated");
    EXPECT_TRUE(retPluginMsg->innerxml() == "<param2>new_param</param2>\n");
  }

  delete pluginMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, MessageWidgetVisible)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto visualMessageWidget = new MessageWidget();
  msgs::Visual visualMsg;

  {
    // visual
    visualMsg.set_id(12345u);

    // pose
    math::Vector3d pos(2.0, 3.0, 4.0);
    math::Quaterniond quat(1.57, 0.0, 0.0);
    msgs::Set(visualMsg.mutable_pose(), math::Pose3d(pos, quat));

    // geometry
    auto geometryMsg = visualMsg.mutable_geometry();
    geometryMsg->set_type(msgs::Geometry::CYLINDER);
    auto cylinderGeomMsg = geometryMsg->mutable_cylinder();
    cylinderGeomMsg->set_radius(3.0);
    cylinderGeomMsg->set_length(0.2);

    // material
    auto materialMsg = visualMsg.mutable_material();
    msgs::Set(materialMsg->mutable_ambient(), math::Color(0.0, 1.0, 0.0, 1.0));
    msgs::Set(materialMsg->mutable_diffuse(), math::Color(0.0, 1.0, 1.0, 0.4));

    // material::script
    auto scriptMsg = materialMsg->mutable_script();
    scriptMsg->set_name("test_script_name");
  }
  visualMessageWidget->Load(&visualMsg);
  visualMessageWidget->show();

  // set different types of widgets to be not visibile
  {
    // primitive widget
    visualMessageWidget->SetWidgetVisible("id", false);
    // custom pose message widget
    visualMessageWidget->SetWidgetVisible("pose", false);
    // custom geometry message widget
    visualMessageWidget->SetWidgetVisible("geometry", false);
    // widget inside a group widget
    visualMessageWidget->SetWidgetVisible("material::diffuse", false);
    // widget two levels deep
    visualMessageWidget->SetWidgetVisible("material::script::name", false);
    // group widget
    visualMessageWidget->SetWidgetVisible("material", false);

    EXPECT_EQ(visualMessageWidget->WidgetVisible("id"), false);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("pose"), false);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("geometry"), false);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material::diffuse"), false);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material::script::name"),
        false);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material"), false);
  }

  // set visible back to true
  {
    visualMessageWidget->SetWidgetVisible("id", true);
    visualMessageWidget->SetWidgetVisible("pose", true);
    visualMessageWidget->SetWidgetVisible("geometry", true);
    visualMessageWidget->SetWidgetVisible("material::diffuse", true);
    visualMessageWidget->SetWidgetVisible("material::script::name", true);
    visualMessageWidget->SetWidgetVisible("material", true);

    EXPECT_EQ(visualMessageWidget->WidgetVisible("id"), true);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("pose"), true);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("geometry"), true);
    // FIXME: investigate failure
    //EXPECT_EQ(visualMessageWidget->WidgetVisible("material::diffuse"), true);
    //EXPECT_EQ(visualMessageWidget->WidgetVisible("material::script::name"),
    //    true);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material"), true);
  }

  delete visualMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, CustomMessageWidgetReadOnly)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();
  auto messageLayout = new QVBoxLayout();

  // Create a child widget
  {
    auto vecWidget = messageWidget->CreateVector3dWidget("vector3d", 0);
    auto stringWidget = messageWidget->CreateStringWidget("string", 0);

    EXPECT_TRUE(messageWidget->AddPropertyWidget("vector3d", vecWidget));
    EXPECT_TRUE(messageWidget->AddPropertyWidget("string", stringWidget));

    auto childLayout = new QVBoxLayout();
    childLayout->addWidget(vecWidget);
    childLayout->addWidget(stringWidget);

    auto childWidget = new PropertyWidget();
    childWidget->setLayout(childLayout);

    auto groupWidget = messageWidget->CreateCollapsibleWidget("group",
        childWidget, 0);

    messageLayout->addWidget(groupWidget);
  }

  // Create a custom child widget
  {
    auto customLabel = new QLabel("custom label");
    customLabel->setObjectName("label");
    auto custom1 = new QToolButton();
    auto custom2 = new QDoubleSpinBox();
    auto custom3 = new QLineEdit();

    auto customLayout = new QGridLayout();
    customLayout->addWidget(customLabel, 0, 0);
    customLayout->addWidget(custom1, 0, 1);
    customLayout->addWidget(custom2, 0, 2);
    customLayout->addWidget(custom3, 0, 3);

    auto customLayout2 = new QVBoxLayout();
    customLayout2->addLayout(customLayout);

    auto customWidget = new PropertyWidget();
    customWidget->setLayout(customLayout2);

    EXPECT_TRUE(messageWidget->AddPropertyWidget("custom", customWidget));

    auto customCollapsibleWidget = messageWidget->CreateCollapsibleWidget("custom group",
        customWidget, 0);

    messageLayout->addWidget(customCollapsibleWidget);
  }
  messageWidget->setLayout(messageLayout);

  // set to be read-only
  {
    messageWidget->SetWidgetReadOnly("vector3d", true);
    messageWidget->SetWidgetReadOnly("string", true);
    messageWidget->SetWidgetReadOnly("custom", true);

    EXPECT_EQ(messageWidget->WidgetReadOnly("vector3d"), true);
    EXPECT_EQ(messageWidget->WidgetReadOnly("string"), true);
    EXPECT_EQ(messageWidget->WidgetReadOnly("custom"), true);
    {
      auto childWidgets =
          messageWidget->PropertyWidgetByName("custom")->
          findChildren<QWidget *>();
      EXPECT_EQ(childWidgets.size(), 5);
      for (auto it : childWidgets)
        EXPECT_EQ(it->isEnabled(), false);
    }
  }

  // set read-only back to false
  {
    messageWidget->SetWidgetReadOnly("vector3d", false);
    messageWidget->SetWidgetReadOnly("string", false);
    messageWidget->SetWidgetReadOnly("custom", false);

    EXPECT_EQ(messageWidget->WidgetReadOnly("vector3d"), false);
    EXPECT_EQ(messageWidget->WidgetReadOnly("string"), false);
    EXPECT_EQ(messageWidget->WidgetReadOnly("custom"), false);
    {
      auto childWidgets = messageWidget->PropertyWidgetByName("custom")->
          findChildren<QWidget *>();
      EXPECT_EQ(childWidgets.size(), 5);
      for (auto it : childWidgets)
        EXPECT_EQ(it->isEnabled(), true);
    }
  }

  // set read-only back to true
  {
    messageWidget->SetWidgetReadOnly("vector3d", true);
    messageWidget->SetWidgetReadOnly("string", true);
    messageWidget->SetWidgetReadOnly("custom", true);

    EXPECT_EQ(messageWidget->WidgetReadOnly("vector3d"), true);
    EXPECT_EQ(messageWidget->WidgetReadOnly("string"), true);
    EXPECT_EQ(messageWidget->WidgetReadOnly("custom"), true);
    {
      auto childWidgets = messageWidget->PropertyWidgetByName("custom")->
          findChildren<QWidget *>();
      EXPECT_EQ(childWidgets.size(), 5);
      for (auto it : childWidgets)
        EXPECT_EQ(it->isEnabled(), false);
    }
  }

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, MessageWidgetReadOnly)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto visualMessageWidget = new MessageWidget();
  msgs::Visual visualMsg;

  {
    // visual
    visualMsg.set_id(12345u);

    // pose
    math::Vector3d pos(2.0, 3.0, 4.0);
    math::Quaterniond quat(1.57, 0.0, 0.0);
    msgs::Set(visualMsg.mutable_pose(), math::Pose3d(pos, quat));

    // geometry
    auto geometryMsg = visualMsg.mutable_geometry();
    geometryMsg->set_type(msgs::Geometry::CYLINDER);
    auto cylinderGeomMsg = geometryMsg->mutable_cylinder();
    cylinderGeomMsg->set_radius(3.0);
    cylinderGeomMsg->set_length(0.2);

    // material
    auto materialMsg = visualMsg.mutable_material();
    msgs::Set(materialMsg->mutable_ambient(), math::Color(0.0, 1.0, 0.0, 1.0));
    msgs::Set(materialMsg->mutable_diffuse(), math::Color(0.0, 1.0, 1.0, 0.4));

    // material::script
    auto scriptMsg = materialMsg->mutable_script();
    scriptMsg->set_name("test_script_name");
  }
  visualMessageWidget->Load(&visualMsg);

  // set different types of widgets to be read-only
  {
    // primitive widget
    visualMessageWidget->SetWidgetReadOnly("id", true);
    // custom pose message widget
    visualMessageWidget->SetWidgetReadOnly("pose", true);
    // custom geometry message widget
    visualMessageWidget->SetWidgetReadOnly("geometry", true);
    // widget inside a group widget
    visualMessageWidget->SetWidgetReadOnly("material::diffuse", true);
    // widget two levels deep
    visualMessageWidget->SetWidgetReadOnly("material::script::name", true);
    // group widget
    visualMessageWidget->SetWidgetReadOnly("material", true);

    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("id"), true);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("pose"), true);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("geometry"), true);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("material::diffuse"), true);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("material::script::name"),
        true);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("material"), true);
  }

  // set read-only back to false
  {
    visualMessageWidget->SetWidgetReadOnly("id", false);
    visualMessageWidget->SetWidgetReadOnly("pose", false);
    visualMessageWidget->SetWidgetReadOnly("geometry", false);
    visualMessageWidget->SetWidgetReadOnly("material::diffuse", false);
    visualMessageWidget->SetWidgetReadOnly("material::script::name", false);
    visualMessageWidget->SetWidgetReadOnly("material", false);

    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("id"), false);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("pose"), false);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("geometry"), false);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("material::diffuse"), false);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("material::script::name"),
        false);
    EXPECT_EQ(visualMessageWidget->WidgetReadOnly("material"), false);
  }

  delete visualMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, CreatedExternally)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();

  // Create predefined child widgets
  auto uintWidget = messageWidget->CreateUIntWidget("uint", 0);
  auto intWidget = messageWidget->CreateIntWidget("int", 0);
  auto doubleWidget = new DoubleWidget("double", 1);
  auto stringWidget = messageWidget->CreateStringWidget("string", 1);
  auto boolWidget = new BoolWidget("bool", 2);
  auto vector3dWidget = messageWidget->CreateVector3dWidget("vector3d", 2);
  auto colorWidget = messageWidget->CreateColorWidget("color", 3);
  auto poseWidget = messageWidget->CreatePoseWidget("pose", 3);

  std::vector<std::string> enumValues;
  enumValues.push_back("value1");
  enumValues.push_back("value2");
  enumValues.push_back("value3");
  auto enumWidget = messageWidget->CreateEnumWidget("enum", enumValues, 4);

  EXPECT_TRUE(uintWidget != nullptr);
  EXPECT_TRUE(intWidget != nullptr);
  EXPECT_TRUE(doubleWidget != nullptr);
  EXPECT_TRUE(stringWidget != nullptr);
  EXPECT_TRUE(boolWidget != nullptr);
  EXPECT_TRUE(vector3dWidget != nullptr);
  EXPECT_TRUE(colorWidget != nullptr);
  EXPECT_TRUE(poseWidget != nullptr);
  EXPECT_TRUE(enumWidget != nullptr);

  // Create a custom child widget
  auto customLabel = new QLabel("custom label");
  auto customLineEdit = new QLineEdit();
  auto customLayout = new QHBoxLayout();
  customLayout->addWidget(customLabel);
  customLayout->addWidget(customLineEdit);

  auto customWidget = new PropertyWidget();
  customWidget->setLayout(customLayout);
  customWidget->widgets.push_back(customLineEdit);

  // Add child widgets to message widget
  EXPECT_EQ(messageWidget->PropertyWidgetCount(), 0u);

  EXPECT_TRUE(messageWidget->AddPropertyWidget("uint", uintWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("int", intWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("double", doubleWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("string", stringWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("bool", boolWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("vector3d", vector3dWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("color", colorWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("pose", poseWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("enum", enumWidget));
  EXPECT_TRUE(messageWidget->AddPropertyWidget("custom", customWidget));

  EXPECT_EQ(messageWidget->PropertyWidgetCount(), 10u);

  // Fail to add invalid children
  EXPECT_EQ(messageWidget->AddPropertyWidget("", uintWidget), false);
  EXPECT_EQ(messageWidget->AddPropertyWidget("validName", nullptr), false);
  EXPECT_EQ(messageWidget->AddPropertyWidget("uint", intWidget), false);

  EXPECT_EQ(messageWidget->PropertyWidgetCount(), 10u);

  // Check that checking visibility works
  EXPECT_EQ(messageWidget->WidgetVisible("uint"), uintWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("int"), intWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("double"), doubleWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("string"), stringWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("bool"), boolWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("vector3d"),
      vector3dWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("color"), colorWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("pose"), poseWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("enum"), enumWidget->isVisible());
  EXPECT_EQ(messageWidget->WidgetVisible("custom"), customWidget->isVisible());

  // Set widgets values
  unsigned int uintValue = 123;
  int intValue = -456;
  double doubleValue = 123.456;
  std::string stringValue("123");
  bool boolValue = true;
  math::Vector3d vector3dValue(1, 2, 3);
  math::Color colorValue(0.1, 0.2, 0.3, 0.4);
  math::Pose3d poseValue(1, 2, 3, 0.1, 0.2, 0.3);
  std::string enumValue("value2");
  std::string customValue("123456789");

  EXPECT_TRUE(messageWidget->SetUIntWidgetValue("uint", uintValue));
  EXPECT_TRUE(messageWidget->SetIntWidgetValue("int", intValue));
  EXPECT_TRUE(messageWidget->SetDoubleWidgetValue("double", doubleValue));
  EXPECT_TRUE(messageWidget->SetStringWidgetValue("string", stringValue));
  EXPECT_TRUE(messageWidget->SetBoolWidgetValue("bool", boolValue));
  EXPECT_TRUE(messageWidget->SetVector3dWidgetValue("vector3d",
      math::Vector3d(vector3dValue)));
  EXPECT_TRUE(messageWidget->SetColorWidgetValue("color", colorValue));
  EXPECT_TRUE(messageWidget->SetPoseWidgetValue("pose", poseValue));
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enum", enumValue));
  EXPECT_TRUE(messageWidget->SetStringWidgetValue("custom", customValue));

  // Get widgets values
  EXPECT_EQ(messageWidget->UIntWidgetValue("uint"), uintValue);
  EXPECT_EQ(messageWidget->IntWidgetValue("int"), intValue);
  EXPECT_DOUBLE_EQ(messageWidget->DoubleWidgetValue("double"), doubleValue);
  EXPECT_EQ(messageWidget->StringWidgetValue("string"), stringValue);
  EXPECT_EQ(messageWidget->BoolWidgetValue("bool"), boolValue);
  EXPECT_EQ(messageWidget->Vector3dWidgetValue("vector3d"),
      math::Vector3d(vector3dValue));
  EXPECT_EQ(messageWidget->ColorWidgetValue("color"), colorValue);
  EXPECT_EQ(messageWidget->PoseWidgetValue("pose"),
      math::Pose3d(poseValue));
  EXPECT_EQ(messageWidget->EnumWidgetValue("enum"), enumValue);
  EXPECT_EQ(messageWidget->StringWidgetValue("custom"), customValue);

  // Collapsible some widgets
  auto groupLayout = new QVBoxLayout();
  groupLayout->addWidget(uintWidget);
  groupLayout->addWidget(intWidget);
  groupLayout->addWidget(doubleWidget);

  auto groupBox = new QGroupBox();
  groupBox->setLayout(groupLayout);

  auto groupChildWidgetLayout = new QVBoxLayout();
  groupChildWidgetLayout->addWidget(groupBox);

  auto groupChildWidget = new PropertyWidget();
  groupChildWidget->setLayout(groupChildWidgetLayout);
  groupChildWidget->widgets.push_back(groupBox);

  auto groupWidget =
      messageWidget->CreateCollapsibleWidget("groupWidget", groupChildWidget, 0);
  EXPECT_TRUE(groupWidget != nullptr);
  EXPECT_TRUE(groupWidget->childWidget != nullptr);

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, EnumMessageWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Create a parent widget
  auto messageWidget = new MessageWidget();
  EXPECT_TRUE(messageWidget != nullptr);

  // Create an enum child widget
  std::vector<std::string> enumValues;
  enumValues.push_back("value1");
  enumValues.push_back("value2");
  enumValues.push_back("value3");
  auto enumWidget = messageWidget->CreateEnumWidget("Enum Label", enumValues);

  EXPECT_TRUE(enumWidget != nullptr);

  // Add it to parent
  EXPECT_TRUE(messageWidget->AddPropertyWidget("enumWidgetName", enumWidget));

  // Check that all items can be selected
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value1"));
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value2"));
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value3"));

  // Check that an inexistent item cannot be selected
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value4")
      == false);

  // Check the number of items
  auto comboBox = enumWidget->findChild<QComboBox *>();
  EXPECT_TRUE(comboBox != nullptr);
  EXPECT_EQ(comboBox->count(), 3);

  // Add an item and check count
  EXPECT_TRUE(messageWidget->AddItemEnumWidget("enumWidgetName", "value4"));
  EXPECT_EQ(comboBox->count(), 4);

  // Check that the new item can be selected
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value4"));

  // Remove an item and check count
  EXPECT_TRUE(messageWidget->RemoveItemEnumWidget("enumWidgetName", "value2"));
  EXPECT_EQ(comboBox->count(), 3);

  // Check that the removed item cannot be selected
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value2")
      == false);

  // Clear all items and check count
  EXPECT_TRUE(messageWidget->ClearEnumWidget("enumWidgetName"));
  EXPECT_EQ(comboBox->count(), 0);

  // Check that none of the previous items can be selected
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value1")
      == false);
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value2")
      == false);
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value3")
      == false);
  EXPECT_TRUE(messageWidget->SetEnumWidgetValue("enumWidgetName", "value4")
      == false);

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildUIntSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();

  // Create child uint widget
  auto uintWidget = messageWidget->CreateUIntWidget("uint");
  EXPECT_TRUE(uintWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("uint", uintWidget));

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, const QVariant _uint)
    {
      EXPECT_TRUE(_name == "uint");
      EXPECT_TRUE(_uint == 3);
      signalReceived = true;
    });

  // Check default uint
  EXPECT_TRUE(messageWidget->UIntWidgetValue("uint") == 0u);

  // Get signal emitting widgets
  auto spins = uintWidget->findChildren<QSpinBox *>();
  EXPECT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(3);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildIntSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();

  // Create child int widget
  auto intWidget = messageWidget->CreateIntWidget("int");
  EXPECT_TRUE(intWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("int", intWidget));

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, const QVariant _int)
    {
      EXPECT_TRUE(_name == "int");
      EXPECT_TRUE(_int == -2);
      signalReceived = true;
    });

  // Check default int
  EXPECT_EQ(messageWidget->IntWidgetValue("int"), 0);

  // Get signal emitting widgets
  QList<QSpinBox *> spins = intWidget->findChildren<QSpinBox *>();
  EXPECT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(-2);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildDoubleSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();

  // Create child double widget
  auto doubleWidget = new DoubleWidget("double");
  EXPECT_TRUE(doubleWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("double", doubleWidget));

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      double d = _var.toDouble();
      EXPECT_TRUE(_name == "double");
      EXPECT_TRUE(fabs(d - 1.5) < 0.00001);
      signalReceived = true;
    });

  // Check default double
  EXPECT_DOUBLE_EQ(messageWidget->DoubleWidgetValue("double"), 0.0);

  // Get signal emitting widgets
  auto spins = doubleWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(1.5);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildBoolSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();

  // Create child bool widget
  auto boolWidget = new BoolWidget("bool");
  EXPECT_TRUE(boolWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("bool", boolWidget));

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      bool b = _var.toBool();
      EXPECT_TRUE(_name == "bool");
      EXPECT_TRUE(b == true);
      signalReceived = true;
    });

  // Check default bool
  EXPECT_EQ(messageWidget->BoolWidgetValue("bool"), false);

  // Get signal emitting widgets
  auto radios = boolWidget->findChildren<QRadioButton *>();
  EXPECT_EQ(radios.size(), 2);

  // Change the value and check new value at callback
  radios[0]->setChecked(true);
  radios[1]->setChecked(false);

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildStringSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  auto messageWidget = new MessageWidget();

  // Create child string widget
  auto stringWidget = messageWidget->CreateStringWidget("string");
  EXPECT_TRUE(stringWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("string", stringWidget));

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<std::string>();
      EXPECT_TRUE(_name == "string");
      EXPECT_TRUE(v == "new text");
      signalReceived = true;
    });

  // Check default string
  EXPECT_TRUE(messageWidget->StringWidgetValue("string") == "");

  // Get signal emitting widgets
  auto lineEdits = stringWidget->findChildren<QLineEdit *>();
  EXPECT_EQ(lineEdits.size(), 1);

  // Change the value and check new value at callback
  lineEdits[0]->setText("new text");
  lineEdits[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/*
/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildVector3dSignal)
{
  auto messageWidget = new MessageWidget();

  // Create child vector3 widget
  auto vector3Widget =
      messageWidget->CreateVector3dWidget("vector3");
  EXPECT_TRUE(vector3Widget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("vector3", vector3Widget));

  // Connect signals
  connect(messageWidget,
      SIGNAL(Vector3dValueChanged(const QString, math::Vector3d)),
      this,
      SLOT(OnVector3dValueChanged(const QString, math::Vector3d)));

  // Check default vector3
  EXPECT_TRUE(messageWidget->Vector3dWidgetValue("vector3") ==
      math::Vector3d());

  // Get axes spins
  QList<QDoubleSpinBox *> spins =
      vector3Widget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 3);

  // Get preset combo
  auto combos = vector3Widget->findChildren<QComboBox *>();
  EXPECT_EQ(combos.size(), 1);

  // Change the X value and check new value at callback
  EXPECT_TRUE(g_vector3SignalCount == 0);
  spins[0]->setValue(2.5);
  QTest::keyClick(spins[0], Qt::Key_Enter);
  EXPECT_TRUE(g_vector3SignalCount == 1);

  // Change the preset value and check new value at callback
  combos[0]->setCurrentIndex(4);
  QTest::keyClick(combos[0], Qt::Key_Enter);
  EXPECT_TRUE(g_vector3SignalCount == 2);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
//TEST(MessageWidgetTest, OnVector3dValueChanged(const QString &_name,
//    const math::Vector3d &_vector3)
//{
//  EXPECT_TRUE(_name == "vector3");
//
//  // From spins
//  if (g_vector3SignalCount == 0)
//  {
//    EXPECT_TRUE(_vector3 == math::Vector3d(2.5, 0, 0));
//    g_vector3SignalCount++;
//  }
//  // From preset combo
//  else if (g_vector3SignalCount == 1)
//  {
//    EXPECT_TRUE(_vector3 == math::Vector3d(0, -1, 0));
//    g_vector3SignalCount++;
//  }
//}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildColorSignal)
{
  auto messageWidget = new MessageWidget();

  // Create child color widget
  auto colorWidget =
      messageWidget->CreateColorWidget("color");
  EXPECT_TRUE(colorWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("color", colorWidget));

  // Connect signals
  connect(messageWidget,
      SIGNAL(ColorValueChanged(const QString, const math::Color)),
      this,
      SLOT(OnColorValueChanged(const QString, const math::Color)));

  // Check default color
  EXPECT_TRUE(messageWidget->ColorWidgetValue("color") ==
      math::Color());

  // Get signal emitting widgets
  QList<QDoubleSpinBox *> spins =
      colorWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 4);

  // Change the X value and check new value at callback
  spins[0]->setValue(0.5);
  QTest::keyClick(spins[0], Qt::Key_Enter);

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
//TEST(MessageWidgetTest, OnColorValueChanged(const QString &_name,
//    const math::Color &_color)
//{
//  EXPECT_TRUE(_name == "color");
//  EXPECT_TRUE(_color == math::Color(0.5, 0.0, 0.0, 0.0));
//  g_colorSignalReceived = true;
//}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildPoseSignal)
{
  auto messageWidget = new MessageWidget();

  // Create child pose widget
  auto poseWidget =
      messageWidget->CreatePoseWidget("pose");
  EXPECT_TRUE(poseWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("pose", poseWidget));

  // Connect signals
  connect(messageWidget,
      SIGNAL(PoseValueChanged(const QString, const math::Pose3d)),
      this,
      SLOT(OnPoseValueChanged(const QString, const math::Pose3d)));

  // Check default pose
  EXPECT_TRUE(messageWidget->PoseWidgetValue("pose") == math::Pose3d());

  // Get signal emitting widgets
  QList<QDoubleSpinBox *> spins = poseWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 6);

  // Change the X value and check new value at callback
  spins[0]->setValue(1.0);
  QTest::keyClick(spins[0], Qt::Key_Enter);

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
}

/////////////////////////////////////////////////
//TEST(MessageWidgetTest, OnPoseValueChanged(const QString &_name,
//    const math::Pose3d &_value)
//{
//  EXPECT_TRUE(_name == "pose");
//  EXPECT_TRUE(_value == math::Pose3d(1, 0, 0, 0, 0, 0));
//  g_poseSignalReceived = true;
//}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildGeometrySignal)
{
  auto messageWidget = new MessageWidget();

  // Create child widget
  auto geometryWidget =
      messageWidget->CreateGeometryWidget("geometry");
  EXPECT_TRUE(geometryWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("geometry", geometryWidget));

  // Connect signals
  connect(messageWidget,
      SIGNAL(GeometryValueChanged(const std::string &, const std::string &,
      const math::Vector3d &, const std::string &)),
      this,
      SLOT(OnGeometryValueChanged(const std::string &, const std::string &,
      const math::Vector3d &, const std::string &)));

  // Check default
  math::Vector3d dimensions;
  std::string uri;
  std::string value = messageWidget->GeometryWidgetValue("geometry",
      dimensions, uri);
  EXPECT_TRUE(value == "box");
  EXPECT_TRUE(dimensions == math::Vector3d(1, 1, 1));
  EXPECT_TRUE(uri == "");

  // Get signal emitting widgets
  QList<QDoubleSpinBox *> spins =
      geometryWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 5);

  // Change the value and check new value at callback
  spins[2]->setValue(2.0);
  QTest::keyClick(spins[2], Qt::Key_Enter);

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
}

/////////////////////////////////////////////////
//TEST(MessageWidgetTest, OnGeometryValueChanged(const std::string &_name,
//    const std::string &_value, const math::Vector3d &_dimensions,
//    const std::string &_uri)
//{
//  EXPECT_TRUE(_name == "geometry");
//  EXPECT_TRUE(_value == "box");
//  EXPECT_TRUE(_dimensions == math::Vector3d(2, 1, 1));
//  EXPECT_TRUE(_uri == "");
//  g_geometrySignalReceived = true;
//}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildEnumSignal)
{
  auto messageWidget = new MessageWidget();

  // Create child pose widget
  std::vector<std::string> enumValues;
  enumValues.push_back("value1");
  enumValues.push_back("value2");
  enumValues.push_back("value3");
  auto enumWidget =
      messageWidget->CreateEnumWidget("enum", enumValues);
  EXPECT_TRUE(enumWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("enum", enumWidget));

  // Connect signals
  connect(messageWidget,
      SIGNAL(EnumValueChanged(const QString, const QString)),
      this,
      SLOT(OnEnumValueChanged(const QString, const QString)));

  // Check default pose
  EXPECT_TRUE(messageWidget->EnumWidgetValue("enum") == "value1");

  // Get signal emitting widgets
  QList<QComboBox *> comboBoxes = enumWidget->findChildren<QComboBox *>();
  EXPECT_EQ(comboBoxes.size(), 1);

  // Change the value and check new value at callback
  comboBoxes[0]->setCurrentIndex(2);
  QTest::keyClick(comboBoxes[0], Qt::Key_Enter);

  // Check callback was called
  EXPECT_TRUE(signalReceived == true);

  delete messageWidget;
}

/////////////////////////////////////////////////
//TEST(MessageWidgetTest, OnEnumValueChanged(const QString &_name,
//    const QString &_value)
//{
//  EXPECT_TRUE(_name == "enum");
//  EXPECT_TRUE(_value == "value3");
//  g_enumSignalReceived = true;
//}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, GetChildWidgetByName)
{
  // Create message widget and check it has no children
  auto messageWidget = new MessageWidget();
  EXPECT_TRUE(messageWidget != nullptr);
  EXPECT_EQ(messageWidget->PropertyWidgetCount(), 0u);

  // Try to get a child widget by name
  auto widget =
      messageWidget->PropertyWidgetByName("child_widget");
  EXPECT_TRUE(widget == nullptr);

  widget = messageWidget->PropertyWidgetByName("");
  EXPECT_TRUE(widget == nullptr);

  // Create child widget
  auto childWidget =
      messageWidget->CreateBoolWidget("child_widget");
  EXPECT_TRUE(childWidget != nullptr);

  // Add to message widget
  EXPECT_TRUE(messageWidget->AddPropertyWidget("child_widget", childWidget));
  EXPECT_EQ(messageWidget->PropertyWidgetCount(), 1u);

  // Get the widget by name
  widget = messageWidget->PropertyWidgetByName("child_widget");
  EXPECT_TRUE(widget != nullptr);

  // Check that a bad name returns nullptr
  widget = messageWidget->PropertyWidgetByName("bad_name");
  EXPECT_TRUE(widget == nullptr);

  delete messageWidget;
}
*/
