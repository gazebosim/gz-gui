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

#include <ignition/common/Console.hh>

#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>

#include <ignition/msgs.hh>

#include "test_config.h"  // NOLINT(build/include)
#include "ignition/gui/BoolWidget.hh"
#include "ignition/gui/ColorWidget.hh"
#include "ignition/gui/CollapsibleWidget.hh"
#include "ignition/gui/EnumWidget.hh"
#include "ignition/gui/GeometryWidget.hh"
#include "ignition/gui/Iface.hh"
#include "ignition/gui/NumberWidget.hh"
#include "ignition/gui/Pose3dWidget.hh"
#include "ignition/gui/PropertyWidget.hh"
#include "ignition/gui/QtMetatypes.hh"
#include "ignition/gui/StringWidget.hh"
#include "ignition/gui/Vector3dWidget.hh"

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

    auto visualMessageWidget = new MessageWidget(&visualMsg);

    QCoreApplication::processEvents();

    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(visualMessageWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    delete visualMessageWidget;
  }

  {
    msgs::Collision collisionMsg;

    auto collisionMessageWidget = new MessageWidget(&collisionMsg);

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

  msgs::Joint jointMsg;

  QVariant variant;

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
  auto jointMessageWidget = new MessageWidget(&jointMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(jointMessageWidget->Msg());
    EXPECT_TRUE(retJointMsg != nullptr);

    // joint
    EXPECT_EQ(retJointMsg->name(), "test_joint");
    EXPECT_EQ(retJointMsg->id(), 1122u);
    EXPECT_EQ(retJointMsg->parent(), "test_joint_parent");
    EXPECT_EQ(retJointMsg->parent_id(), 212121u);
    EXPECT_EQ(retJointMsg->child(), "test_joint_child");
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
    EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    EXPECT_LT(fabs(quat.Euler().Y() - 1.57), 0.0001);
    EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

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
    variant.setValue(std::string("test_joint_updated"));
    jointMessageWidget->SetPropertyValue("name", variant);
    jointMessageWidget->SetPropertyValue("id", 9999999u);
    variant.setValue(std::string("test_joint_parent_updated"));
    jointMessageWidget->SetPropertyValue("parent", variant);
    jointMessageWidget->SetPropertyValue("parent_id", 1u);
    variant.setValue(std::string("test_joint_child_updated"));
    jointMessageWidget->SetPropertyValue("child", variant);
    jointMessageWidget->SetPropertyValue("child_id", 2u);

    // type
    variant.setValue(msgs::Joint_Type_Name(msgs::Joint_Type_UNIVERSAL));
    jointMessageWidget->SetPropertyValue("type", variant);

    // pose
    math::Vector3d pos(2.0, 9.0, -4.0);
    math::Quaterniond quat(0.0, 0.0, 1.57);
    variant.setValue(math::Pose3d(pos, quat));
    jointMessageWidget->SetPropertyValue("pose", variant);

    // axis1
    variant.setValue(math::Vector3d::UnitY);
    jointMessageWidget->SetPropertyValue("axis1::xyz", variant);
    jointMessageWidget->SetPropertyValue("axis1::use_parent_model_frame",
        true);
    jointMessageWidget->SetPropertyValue("axis1::limit_lower", -1.2);
    jointMessageWidget->SetPropertyValue("axis1::limit_upper", -1.0);
    jointMessageWidget->SetPropertyValue("axis1::limit_effort", 1.0);
    jointMessageWidget->SetPropertyValue("axis1::limit_velocity", 100.0);
    jointMessageWidget->SetPropertyValue("axis1::damping", 0.9);

    // axis2
    variant.setValue(math::Vector3d::UnitZ);
    jointMessageWidget->SetPropertyValue("axis2::xyz", variant);
    jointMessageWidget->SetPropertyValue("axis2::use_parent_model_frame",
        true);
    jointMessageWidget->SetPropertyValue("axis2::limit_lower", -3.2);
    jointMessageWidget->SetPropertyValue("axis2::limit_upper", -3.0);
    jointMessageWidget->SetPropertyValue("axis2::limit_effort", 3.0);
    jointMessageWidget->SetPropertyValue("axis2::limit_velocity", 300.0);
    jointMessageWidget->SetPropertyValue("axis2::damping", 3.9);

    // other joint physics properties
    jointMessageWidget->SetPropertyValue("cfm", 0.9);
    jointMessageWidget->SetPropertyValue("bounce", 0.8);
    jointMessageWidget->SetPropertyValue("velocity", 0.7);
    jointMessageWidget->SetPropertyValue("fudge_factor", 0.6);
    jointMessageWidget->SetPropertyValue("limit_cfm", 0.5);
    jointMessageWidget->SetPropertyValue("limit_erp", 0.4);
    jointMessageWidget->SetPropertyValue("suspension_cfm", 0.3);
    jointMessageWidget->SetPropertyValue("suspension_erp", 0.2);
  }

  // verify widget values
  {
    // joint
    EXPECT_EQ(jointMessageWidget->PropertyValue("name").value<std::string>(),
        "test_joint_updated");
    EXPECT_EQ(jointMessageWidget->PropertyValue("id"), 9999999u);
    EXPECT_EQ(jointMessageWidget->PropertyValue("parent").value<std::string>(),
        "test_joint_parent_updated");
    EXPECT_EQ(jointMessageWidget->PropertyValue("parent_id"), 1u);
    EXPECT_EQ(jointMessageWidget->PropertyValue("child").value<std::string>(),
        "test_joint_child_updated");
    EXPECT_EQ(jointMessageWidget->PropertyValue("child_id"), 2u);

    // type
    variant.setValue(msgs::Joint_Type_Name(msgs::Joint_Type_UNIVERSAL));
    jointMessageWidget->SetPropertyValue("type", variant);

    // pose
    math::Vector3d pos(2.0, 9.0, -4.0);
    math::Quaterniond quat(0.0, 0.0, 1.57);
    EXPECT_EQ(jointMessageWidget->PropertyValue("pose").value<math::Pose3d>(),
        math::Pose3d(pos, quat));

    // axis1
    EXPECT_EQ(jointMessageWidget->PropertyValue(
        "axis1::xyz").value<math::Vector3d>(), math::Vector3d::UnitY);
    EXPECT_EQ(jointMessageWidget->PropertyValue(
        "axis1::use_parent_model_frame").toBool(), true);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis1::limit_lower").toDouble(), -1.2);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis1::limit_upper").toDouble(), -1.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis1::limit_effort").toDouble(), 1.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis1::limit_velocity").toDouble(), 100.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis1::damping").toDouble(), 0.9);

    // axis2
    EXPECT_EQ(jointMessageWidget->PropertyValue(
        "axis2::xyz").value<math::Vector3d>(), math::Vector3d::UnitZ);
    EXPECT_EQ(jointMessageWidget->PropertyValue(
        "axis1::use_parent_model_frame").toBool(), true);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis2::limit_lower").toDouble(), -3.2);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis2::limit_upper").toDouble(), -3.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis2::limit_effort").toDouble(), 3.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis2::limit_velocity").toDouble(), 300.0);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "axis2::damping").toDouble(), 3.9);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "cfm").toDouble(), 0.9);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "bounce").toDouble(), 0.8);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "velocity").toDouble(), 0.7);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "fudge_factor").toDouble(), 0.6);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "limit_cfm").toDouble(), 0.5);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "limit_erp").toDouble(), 0.4);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "suspension_cfm").toDouble(), 0.3);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "suspension_erp").toDouble(), 0.2);
  }

  // verify updates in new msg
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(jointMessageWidget->Msg());
    EXPECT_TRUE(retJointMsg != nullptr);

    // joint
    EXPECT_EQ(retJointMsg->name(), "test_joint_updated");
    EXPECT_EQ(retJointMsg->id(), 9999999u);
    EXPECT_EQ(retJointMsg->parent(), "test_joint_parent_updated");
    EXPECT_EQ(retJointMsg->parent_id(), 1u);
    EXPECT_EQ(retJointMsg->child(), "test_joint_child_updated");
    EXPECT_EQ(retJointMsg->child_id(), 2u);

    // type
    EXPECT_EQ(retJointMsg->type(), msgs::ConvertJointType("universal"));

    // pose
    auto poseMsg = retJointMsg->pose();
    auto posMsg = poseMsg.position();
    EXPECT_DOUBLE_EQ(posMsg.x(), 2.0);
    EXPECT_DOUBLE_EQ(posMsg.y(), 9.0);
    EXPECT_DOUBLE_EQ(posMsg.z(), -4.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    EXPECT_DOUBLE_EQ(quat.Euler().Y(), 0.0);
    EXPECT_DOUBLE_EQ(quat.Euler().Z(), 1.57);

    // axis1
    auto axisMsg = retJointMsg->mutable_axis1();
    EXPECT_DOUBLE_EQ(axisMsg->xyz().x(), 0.0);
    EXPECT_DOUBLE_EQ(axisMsg->xyz().y(), 1.0);
    EXPECT_DOUBLE_EQ(axisMsg->xyz().z(), 0.0);
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
    EXPECT_DOUBLE_EQ(axis2Msg->xyz().z(), 1.0);
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
    variant.setValue(std::string("test_joint_updated2"));
    jointMessageWidget->SetPropertyValue("name", variant);
    jointMessageWidget->SetPropertyValue("id", 2222222u);
    variant.setValue(std::string("test_joint_parent_updated2"));
    jointMessageWidget->SetPropertyValue("parent", variant);
    jointMessageWidget->SetPropertyValue("parent_id", 10u);
    variant.setValue(std::string("test_joint_child_updated2"));
    jointMessageWidget->SetPropertyValue("child", variant);
    jointMessageWidget->SetPropertyValue("child_id", 20u);

    // type
    variant.setValue(msgs::Joint_Type_Name(msgs::Joint_Type_BALL));
    jointMessageWidget->SetPropertyValue("type", variant);

    // pose
    math::Vector3d pos(-2.0, 1.0, 2.0);
    math::Quaterniond quat(0.0, 0.0, 0.0);
    variant.setValue(math::Pose3d(pos, quat));
    jointMessageWidget->SetPropertyValue("pose", variant);

    // other joint physics properties
    jointMessageWidget->SetPropertyValue("cfm", 0.19);
    jointMessageWidget->SetPropertyValue("bounce", 0.18);
    jointMessageWidget->SetPropertyValue("velocity", 2.7);
    jointMessageWidget->SetPropertyValue("fudge_factor", 0.26);
    jointMessageWidget->SetPropertyValue("limit_cfm", 0.15);
    jointMessageWidget->SetPropertyValue("limit_erp", 0.24);
    jointMessageWidget->SetPropertyValue("suspension_cfm", 0.13);
    jointMessageWidget->SetPropertyValue("suspension_erp", 0.12);
  }

  // verify widget values
  {
    // joint
    EXPECT_EQ(jointMessageWidget->PropertyValue("name").value<std::string>(),
        "test_joint_updated2");
    EXPECT_EQ(jointMessageWidget->PropertyValue("id"), 2222222u);
    EXPECT_EQ(jointMessageWidget->PropertyValue("parent").value<std::string>(),
        "test_joint_parent_updated2");
    EXPECT_EQ(jointMessageWidget->PropertyValue("parent_id"), 10u);
    EXPECT_EQ(jointMessageWidget->PropertyValue("child").value<std::string>(),
        "test_joint_child_updated2");
    EXPECT_EQ(jointMessageWidget->PropertyValue("child_id"), 20u);

    // type
    variant.setValue(msgs::Joint_Type_Name(msgs::Joint_Type_BALL));
    jointMessageWidget->SetPropertyValue("type", variant);

    // pose
    math::Vector3d pos(-2.0, 1.0, 2.0);
    math::Quaterniond quat(0.0, 0.0, 0.0);
    variant.setValue(math::Pose3d(pos, quat));
    EXPECT_EQ(jointMessageWidget->PropertyValue("pose"), variant);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "cfm").toDouble(), 0.19);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "bounce").toDouble(), 0.18);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "velocity").toDouble(), 2.7);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "fudge_factor").toDouble(), 0.26);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "limit_cfm").toDouble(), 0.15);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "limit_erp").toDouble(), 0.24);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "suspension_cfm").toDouble(), 0.13);
    EXPECT_DOUBLE_EQ(jointMessageWidget->PropertyValue(
        "suspension_erp").toDouble(), 0.12);
  }

  // verify updates in new msg
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(jointMessageWidget->Msg());
    EXPECT_TRUE(retJointMsg != nullptr);

    // joint
    EXPECT_EQ(retJointMsg->name(), "test_joint_updated2");
    EXPECT_EQ(retJointMsg->id(), 2222222u);
    EXPECT_EQ(retJointMsg->parent(), "test_joint_parent_updated2");
    EXPECT_EQ(retJointMsg->parent_id(), 10u);
    EXPECT_EQ(retJointMsg->child(), "test_joint_child_updated2");
    EXPECT_EQ(retJointMsg->child_id(), 20u);

    // type
    EXPECT_EQ(retJointMsg->type(), msgs::ConvertJointType("ball"));

    // pose
    auto poseMsg = retJointMsg->pose();
    auto posMsg = poseMsg.position();
    EXPECT_DOUBLE_EQ(posMsg.x(), -2.0);
    EXPECT_DOUBLE_EQ(posMsg.y(), 1.0);
    EXPECT_DOUBLE_EQ(posMsg.z(), 2.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    EXPECT_DOUBLE_EQ(quat.Euler().Y(), 0.0);
    EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

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

  msgs::Visual visualMsg;

  QVariant variant;

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
    auto cylinderGeomMsg = geometryMsg->mutable_cylinder();
    cylinderGeomMsg->set_radius(3.0);
    cylinderGeomMsg->set_length(0.2);

    // material
    auto materialMsg = visualMsg.mutable_material();
    materialMsg->set_shader_type(msgs::Material::Material::VERTEX);
    materialMsg->set_normal_map("test_normal_map");
    msgs::Set(materialMsg->mutable_ambient(), math::Color(0.0, 1.0, 0.0, 1.0));
    msgs::Set(materialMsg->mutable_diffuse(), math::Color(0.0, 1.0, 1.0, 0.4));
    msgs::Set(materialMsg->mutable_specular(), math::Color(1.0, 1.0, 1.0, 0.6));
    msgs::Set(materialMsg->mutable_emissive(), math::Color(0.0, 0.5, 0.2, 1.0));
    materialMsg->set_lighting(true);
    // material::script
    auto scriptMsg = materialMsg->mutable_script();
    scriptMsg->add_uri("test_script_uri_0");
    scriptMsg->add_uri("test_script_uri_1");
    scriptMsg->set_name("test_script_name");
  }
  auto visualMessageWidget = new MessageWidget(&visualMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(visualMessageWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    // visual
    EXPECT_EQ(retVisualMsg->name(), "test_visual");
    EXPECT_EQ(retVisualMsg->id(), 12345u);
    EXPECT_EQ(retVisualMsg->parent_name(), "test_visual_parent");
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
    EXPECT_DOUBLE_EQ(quat.Euler().X(), 1.57);
    EXPECT_DOUBLE_EQ(quat.Euler().Y(), 0.0);
    EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

    // geometry
    auto geometryMsg = retVisualMsg->geometry();
    EXPECT_EQ(geometryMsg.type(), msgs::Geometry::CYLINDER);
    auto cylinderGeomMsg = geometryMsg.cylinder();
    EXPECT_DOUBLE_EQ(cylinderGeomMsg.radius(), 3.0);
    EXPECT_DOUBLE_EQ(cylinderGeomMsg.length(), 0.2);

    // material
    auto materialMsg = retVisualMsg->material();
    EXPECT_EQ(materialMsg.shader_type(), msgs::Material::Material::VERTEX);
    EXPECT_EQ(materialMsg.normal_map(), "test_normal_map");
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
    EXPECT_EQ(scriptMsg.uri(0), "test_script_uri_0");
    EXPECT_EQ(scriptMsg.uri(1), "test_script_uri_1");
    EXPECT_EQ(scriptMsg.name(), "test_script_name");
  }

  // update fields in the message widget and
  // verify that the new message contains the updated values.
  {
    // visual
    variant.setValue(std::string("test_visual_updated"));
    visualMessageWidget->SetPropertyValue("name", variant);
    visualMessageWidget->SetPropertyValue("id", 11111u);
    variant.setValue(std::string("test_visual_parent_updated"));
    visualMessageWidget->SetPropertyValue("parent_name", variant);
    visualMessageWidget->SetPropertyValue("parent_id", 55555u);
    visualMessageWidget->SetPropertyValue("cast_shadows", false);
    visualMessageWidget->SetPropertyValue("transparency", 1.0);
    visualMessageWidget->SetPropertyValue("visible", false);
    visualMessageWidget->SetPropertyValue("delete_me", true);
    visualMessageWidget->SetPropertyValue("is_static", true);
    variant.setValue(math::Vector3d(2.0, 1.5, 0.5));
    visualMessageWidget->SetPropertyValue("scale", variant);

    // pose
    math::Vector3d pos(-2.0, -3.0, -4.0);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    variant.setValue(math::Pose3d(pos, quat));
    visualMessageWidget->SetPropertyValue("pose", variant);

    // geometry
    msgs::Geometry newGeom;
    newGeom.set_type(msgs::Geometry::BOX);
    msgs::Set(newGeom.mutable_box()->mutable_size(),
              math::Vector3d(5.0, 3.0, 4.0));
    variant.setValue(newGeom);
    visualMessageWidget->SetPropertyValue("geometry", variant);

    // material
    variant.setValue(std::string("test_normal_map_updated"));
    visualMessageWidget->SetPropertyValue("material::normal_map", variant);
    variant.setValue(math::Color(0.2, 0.3, 0.4, 0.5));
    visualMessageWidget->SetPropertyValue("material::ambient", variant);
    variant.setValue(math::Color(0.1, 0.8, 0.6, 0.4));
    visualMessageWidget->SetPropertyValue("material::diffuse", variant);
    variant.setValue(math::Color(0.5, 0.4, 0.3, 0.2));
    visualMessageWidget->SetPropertyValue("material::specular", variant);
    variant.setValue(math::Color(0.4, 0.6, 0.8, 0.1));
    visualMessageWidget->SetPropertyValue("material::emissive", variant);
    visualMessageWidget->SetPropertyValue("material::lighting", false);
    // material::script
    variant.setValue(std::string("test_script_name_updated"));
    visualMessageWidget->SetPropertyValue(
        "material::script::name", variant);
  }

  // verify widget values
  {
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "name").value<std::string>(), "test_visual_updated");
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "id"), 11111u);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "parent_name").value<std::string>(), "test_visual_parent_updated");
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "parent_id"), 55555u);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "cast_shadows").toBool(), false);
    EXPECT_DOUBLE_EQ(visualMessageWidget->PropertyValue(
        "transparency").toDouble(), 1.0);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "visible").toBool(), false);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "delete_me").toBool(), true);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "is_static").toBool(), true);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "scale").value<math::Vector3d>(), math::Vector3d(2.0, 1.5, 0.5));

    // pose
    math::Vector3d pos(-2.0, -3.0, -4.0);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    variant.setValue(math::Pose3d(pos, quat));
    EXPECT_EQ(visualMessageWidget->PropertyValue("pose"), variant);

    // geometry
    auto geomValue =
        visualMessageWidget->PropertyValue("geometry").value<msgs::Geometry>();
    EXPECT_EQ(msgs::ConvertGeometryType(geomValue.type()), "box");
    EXPECT_EQ(msgs::Convert(geomValue.box().size()),
              math::Vector3d(5.0, 3.0, 4.0));

    // material
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::normal_map").value<std::string>(),
        "test_normal_map_updated");
    variant.setValue(math::Color(0.2, 0.3, 0.4, 0.5));
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::ambient"), variant);
    variant.setValue(math::Color(0.1, 0.8, 0.6, 0.4));
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::diffuse"), variant);
    variant.setValue(math::Color(0.5, 0.4, 0.3, 0.2));
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::specular"), variant);
    variant.setValue(math::Color(0.4, 0.6, 0.8, 0.1));
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::emissive"), variant);
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::lighting").toBool(), false);
    // material::script
    EXPECT_EQ(visualMessageWidget->PropertyValue(
        "material::script::name").value<std::string>(),
        "test_script_name_updated");
  }

  // verify updates in new msg
  {
    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(visualMessageWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    // visual
    EXPECT_EQ(retVisualMsg->name(), "test_visual_updated");
    EXPECT_EQ(retVisualMsg->id(), 11111u);
    EXPECT_EQ(retVisualMsg->parent_name(), "test_visual_parent_updated");
    EXPECT_EQ(retVisualMsg->parent_id(), 55555u);
    EXPECT_EQ(retVisualMsg->cast_shadows(), false);
    EXPECT_DOUBLE_EQ(retVisualMsg->transparency(), 1.0);
    EXPECT_EQ(retVisualMsg->visible(), false);
    EXPECT_EQ(retVisualMsg->delete_me(), true);
    EXPECT_EQ(retVisualMsg->is_static(), true);
    auto scaleMsg = retVisualMsg->scale();
    EXPECT_DOUBLE_EQ(scaleMsg.x(), 2.0);
    EXPECT_DOUBLE_EQ(scaleMsg.y(), 1.5);
    EXPECT_DOUBLE_EQ(scaleMsg.z(), 0.5);

    // pose
    auto poseMsg = retVisualMsg->pose();
    auto posMsg = poseMsg.position();
    EXPECT_DOUBLE_EQ(posMsg.x(), -2.0);
    EXPECT_DOUBLE_EQ(posMsg.y(), -3.0);
    EXPECT_DOUBLE_EQ(posMsg.z(), -4.0);
    auto quat = msgs::Convert(poseMsg.orientation());
    EXPECT_DOUBLE_EQ(quat.Euler().X(), 0.0);
    EXPECT_LT(fabs(quat.Euler().Y() - 1.57), 0.0001);
    EXPECT_DOUBLE_EQ(quat.Euler().Z(), 0.0);

    // geometry
    auto geometryMsg = retVisualMsg->geometry();
    EXPECT_EQ(geometryMsg.type(), msgs::Geometry::BOX);
    auto boxGeomMsg = geometryMsg.box();
    auto boxGeomSizeMsg = boxGeomMsg.size();
    EXPECT_DOUBLE_EQ(boxGeomSizeMsg.x(), 5.0);
    EXPECT_DOUBLE_EQ(boxGeomSizeMsg.y(), 3.0);
    EXPECT_DOUBLE_EQ(boxGeomSizeMsg.z(), 4.0);

    // material
    auto materialMsg = retVisualMsg->material();
    EXPECT_EQ(materialMsg.shader_type(), msgs::Material::Material::VERTEX);
    EXPECT_EQ(materialMsg.normal_map(), "test_normal_map_updated");
    auto ambientMsg = materialMsg.ambient();
    EXPECT_DOUBLE_EQ(ambientMsg.r(), 0.2f);
    EXPECT_DOUBLE_EQ(ambientMsg.g(), 0.3f);
    EXPECT_DOUBLE_EQ(ambientMsg.b(), 0.4f);
    EXPECT_DOUBLE_EQ(ambientMsg.a(), 0.5f);
    auto diffuseMsg = materialMsg.diffuse();
    EXPECT_DOUBLE_EQ(diffuseMsg.r(), 0.1f);
    EXPECT_DOUBLE_EQ(diffuseMsg.g(), 0.8f);
    EXPECT_DOUBLE_EQ(diffuseMsg.b(), 0.6f);
    EXPECT_DOUBLE_EQ(diffuseMsg.a(), 0.4f);
    auto specularMsg = materialMsg.specular();
    EXPECT_DOUBLE_EQ(specularMsg.r(), 0.5f);
    EXPECT_DOUBLE_EQ(specularMsg.g(), 0.4f);
    EXPECT_DOUBLE_EQ(specularMsg.b(), 0.3f);
    EXPECT_DOUBLE_EQ(specularMsg.a(), 0.2f);
    auto emissiveMsg = materialMsg.emissive();
    EXPECT_DOUBLE_EQ(emissiveMsg.r(), 0.4f);
    EXPECT_DOUBLE_EQ(emissiveMsg.g(), 0.6f);
    EXPECT_DOUBLE_EQ(emissiveMsg.b(), 0.8f);
    EXPECT_DOUBLE_EQ(emissiveMsg.a(), 0.1f);
    EXPECT_EQ(materialMsg.lighting(), false);
    // material::script
    auto scriptMsg = materialMsg.script();
    EXPECT_EQ(scriptMsg.uri(0), "test_script_uri_0");
    EXPECT_EQ(scriptMsg.uri(1), "test_script_uri_1");
    EXPECT_EQ(scriptMsg.name(), "test_script_name_updated");
  }

  delete visualMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, PluginMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  msgs::Plugin pluginMsg;

  {
    // plugin
    pluginMsg.set_name("test_plugin");
    pluginMsg.set_filename("test_plugin_filename");
    pluginMsg.set_innerxml("<param>1</param>\n");
  }
  auto pluginMessageWidget = new MessageWidget(&pluginMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retPluginMsg =
        dynamic_cast<msgs::Plugin *>(pluginMessageWidget->Msg());
    EXPECT_TRUE(retPluginMsg != nullptr);

    // plugin
    EXPECT_EQ(retPluginMsg->name(), "test_plugin");
    EXPECT_EQ(retPluginMsg->filename(), "test_plugin_filename");
    EXPECT_EQ(retPluginMsg->innerxml(), "<param>1</param>\n");
  }

  // update fields in the message widget and
  // verify that the new message contains the updated values.
  {
    QVariant variant;

    // plugin
    variant.setValue(std::string("test_plugin_updated"));
    pluginMessageWidget->SetPropertyValue("name", variant);
    variant.setValue(std::string("test_plugin_filename_updated"));
    pluginMessageWidget->SetPropertyValue("filename", variant);
    variant.setValue(std::string("<param2>new_param</param2>\n"));
    pluginMessageWidget->SetPropertyValue("innerxml", variant);
  }

  // verify widget values
  {
    EXPECT_EQ(pluginMessageWidget->PropertyValue(
        "name").value<std::string>(), "test_plugin_updated");
    EXPECT_EQ(pluginMessageWidget->PropertyValue(
        "filename").value<std::string>(), "test_plugin_filename_updated");
    EXPECT_EQ(pluginMessageWidget->PropertyValue(
        "innerxml").value<std::string>(), "<param2>new_param</param2>\n");
  }

  // verify updates in new msg
  {
    auto retPluginMsg =
        dynamic_cast<msgs::Plugin *>(pluginMessageWidget->Msg());
    EXPECT_TRUE(retPluginMsg != nullptr);

    // plugin
    EXPECT_EQ(retPluginMsg->name(), "test_plugin_updated");
    EXPECT_EQ(retPluginMsg->filename(), "test_plugin_filename_updated");
    EXPECT_EQ(retPluginMsg->innerxml(), "<param2>new_param</param2>\n");
  }

  delete pluginMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, MessageWidgetVisible)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

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
  auto visualMessageWidget = new MessageWidget(&visualMsg);
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
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material::diffuse"), true);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material::script::name"),
        true);
    EXPECT_EQ(visualMessageWidget->WidgetVisible("material"), true);
  }

  delete visualMessageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, MessageWidgetReadOnly)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

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
  auto visualMessageWidget = new MessageWidget(&visualMsg);

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
TEST(MessageWidgetTest, ChildUIntSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::UInt32();
  msg->set_data(42);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a uint widget
  auto propWidget = messageWidget->PropertyWidgetByName("data");
  EXPECT_NE(propWidget, nullptr);

  auto uintWidget = qobject_cast<NumberWidget *>(propWidget);
  EXPECT_NE(uintWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, const QVariant _uint)
    {
      EXPECT_EQ(_name, "data");
      EXPECT_EQ(_uint, 88888);
      signalReceived = true;
    });

  // Check default uint
  EXPECT_EQ(messageWidget->PropertyValue("data"), 42u);

  // Get signal emitting widgets
  auto spins = uintWidget->findChildren<QSpinBox *>();
  EXPECT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(88888);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildIntSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Int32();
  msg->set_data(-989);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a uint widget
  auto propWidget = messageWidget->PropertyWidgetByName("data");
  EXPECT_NE(propWidget, nullptr);

  auto intWidget = qobject_cast<NumberWidget *>(propWidget);
  EXPECT_NE(intWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, const QVariant _v)
    {
      EXPECT_EQ(_name, "data");
      EXPECT_EQ(_v, 2000);
      signalReceived = true;
    });

  // Check default int
  EXPECT_EQ(messageWidget->PropertyValue("data"), -989);

  // Get signal emitting widgets
  auto spins = intWidget->findChildren<QSpinBox *>();
  EXPECT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(2000);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildDoubleSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Double();
  msg->set_data(-0.123);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a double widget
  auto propWidget = messageWidget->PropertyWidgetByName("data");
  EXPECT_NE(propWidget, nullptr);

  auto doubleWidget = qobject_cast<NumberWidget *>(propWidget);
  EXPECT_NE(doubleWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      double d = _var.toDouble();
      EXPECT_EQ(_name, "data");
      EXPECT_TRUE(fabs(d - 1.5) < 0.00001);
      signalReceived = true;
    });

  // Check default double
  EXPECT_DOUBLE_EQ(messageWidget->PropertyValue("data").toDouble(), -0.123);

  // Get signal emitting widgets
  auto spins = doubleWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(1.5);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildBoolSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Boolean();
  msg->set_data(false);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a bool widget
  auto propWidget = messageWidget->PropertyWidgetByName("data");
  EXPECT_NE(propWidget, nullptr);

  auto boolWidget = qobject_cast<BoolWidget *>(propWidget);
  EXPECT_NE(boolWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      bool b = _var.toBool();
      EXPECT_EQ(_name, "data");
      EXPECT_EQ(b, true);
      signalReceived = true;
    });

  // Check default bool
  EXPECT_EQ(messageWidget->PropertyValue("data"), false);

  // Get signal emitting widgets
  auto radios = boolWidget->findChildren<QRadioButton *>();
  EXPECT_EQ(radios.size(), 2);

  // Change the value and check new value at callback
  radios[0]->setChecked(true);
  radios[1]->setChecked(false);

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildStringSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::StringMsg();
  msg->set_data("banana");

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a string widget
  auto propWidget = messageWidget->PropertyWidgetByName("data");
  EXPECT_NE(propWidget, nullptr);

  auto stringWidget = qobject_cast<StringWidget *>(propWidget);
  EXPECT_NE(stringWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<std::string>();
      EXPECT_EQ(_name, "data");
      EXPECT_EQ(v, "orange");
      signalReceived = true;
    });

  // Check default string
  EXPECT_EQ(messageWidget->PropertyValue("string").value<std::string>(), "");

  // Get signal emitting widgets
  auto lineEdits = stringWidget->findChildren<QLineEdit *>();
  EXPECT_EQ(lineEdits.size(), 1);

  // Change the value and check new value at callback
  lineEdits[0]->setText("orange");
  lineEdits[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildVector3dSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Vector3d();
  msg->set_x(1);
  msg->set_y(-2);
  msg->set_z(3);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a vector 3d widget
  auto propWidget = messageWidget->PropertyWidgetByName("");
  ASSERT_NE(propWidget, nullptr);

  auto vector3Widget = qobject_cast<Vector3dWidget *>(propWidget);
  ASSERT_NE(vector3Widget, nullptr);

  // Connect signals
  int vector3SignalCount = 0;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&vector3SignalCount](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<ignition::math::Vector3d>();

      EXPECT_EQ(_name, "");

      // From spins
      if (vector3SignalCount == 0)
      {
        EXPECT_EQ(v, math::Vector3d(2.5, -2, 3));
        vector3SignalCount++;
      }
      // From preset combo
      else if (vector3SignalCount == 1)
      {
        EXPECT_EQ(v, math::Vector3d(0, -1, 0));
        vector3SignalCount++;
      }
    });

  // Check default vector3
  EXPECT_EQ(messageWidget->PropertyValue("").value<math::Vector3d>(),
      math::Vector3d(1, -2, 3));

  // Get axes spins
  auto spins = vector3Widget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 3);

  // Get preset combo
  auto combos = vector3Widget->findChildren<QComboBox *>();
  EXPECT_EQ(combos.size(), 1);

  // Change the X value and check new value at callback
  EXPECT_EQ(vector3SignalCount, 0);
  spins[0]->setValue(2.5);
  spins[0]->editingFinished();
  EXPECT_EQ(vector3SignalCount, 1);

  // Change the preset value and check new value at callback
  combos[0]->setCurrentIndex(4);
  EXPECT_EQ(vector3SignalCount, 2);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildColorSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Color();
  msg->set_r(0.1);
  msg->set_g(0.2);
  msg->set_b(0.3);
  msg->set_a(0.4);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a color widget
  auto propWidget = messageWidget->PropertyWidgetByName("");
  EXPECT_NE(propWidget, nullptr);

  auto colorWidget = qobject_cast<ColorWidget *>(propWidget);
  EXPECT_NE(colorWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<math::Color>();
      EXPECT_EQ(_name, "");
      EXPECT_EQ(v, math::Color(1.0, 0.2, 0.3, 0.4));
      signalReceived = true;
    });

  // Check default color (opaque white)
  EXPECT_EQ(messageWidget->PropertyValue("").value<math::Color>(),
      math::Color(0.1, 0.2, 0.3, 0.4));

  // Get signal emitting widgets
  auto spins = colorWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 4);

  // Change the X value and check new value at callback
  spins[0]->setValue(1.0);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildPoseSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Pose();
  msg->mutable_position()->set_x(0.1);
  msg->mutable_position()->set_y(0.2);
  msg->mutable_position()->set_z(0.3);
  msgs::Set(msg->mutable_orientation(),
            math::Quaterniond(-0.4, -0.5, -0.6));

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a pose widget
  auto propWidget = messageWidget->PropertyWidgetByName("");
  EXPECT_NE(propWidget, nullptr);

  auto poseWidget = qobject_cast<Pose3dWidget *>(propWidget);
  EXPECT_NE(poseWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<math::Pose3d>();
      EXPECT_EQ(_name, "");
      EXPECT_EQ(v, math::Pose3d(1.0, 0.2, 0.3, -0.4, -0.5, -0.6));
      signalReceived = true;
    });

  // Check default pose
  EXPECT_EQ(messageWidget->PropertyValue("").value<math::Pose3d>(),
      math::Pose3d(0.1, 0.2, 0.3, -0.4, -0.5, -0.6));

  // Get signal emitting widgets
  auto spins = poseWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 6);

  // Change the X value and check new value at callback
  spins[0]->setValue(1.0);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildGeometrySignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Geometry();
  msg->set_type(msgs::Geometry::CYLINDER);
  auto cylinder = msg->mutable_cylinder();
  cylinder->set_length(10.0);
  cylinder->set_radius(0.5);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got a geometry widget
  auto propWidget = messageWidget->PropertyWidgetByName("");
  EXPECT_NE(propWidget, nullptr);

  auto geometryWidget = qobject_cast<GeometryWidget *>(propWidget);
  EXPECT_NE(geometryWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<msgs::Geometry>();
      EXPECT_EQ(_name, "");
      EXPECT_EQ(v.type(), msgs::Geometry::CYLINDER);
      EXPECT_DOUBLE_EQ(v.cylinder().radius(), 2.0);
      signalReceived = true;
    });

  // Check value
  auto value = messageWidget->PropertyValue("").value<msgs::Geometry>();
  EXPECT_EQ(value.type(), msgs::Geometry::CYLINDER);
  EXPECT_DOUBLE_EQ(value.cylinder().length(), 10.0);
  EXPECT_DOUBLE_EQ(value.cylinder().radius(), 0.5);

  // Get signal emitting widgets
  auto spins = geometryWidget->findChildren<QDoubleSpinBox *>();
  EXPECT_EQ(spins.size(), 5);

  // Change the value and check new value at callback
  spins[3]->setValue(2.0);
  spins[3]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildEnumSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Visual();
  msg->set_type(msgs::Visual::LINK);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Check we got an enum widget
  auto propWidget = messageWidget->PropertyWidgetByName("type");
  EXPECT_NE(propWidget, nullptr);

  auto enumWidget = qobject_cast<EnumWidget *>(propWidget);
  EXPECT_NE(enumWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  messageWidget->connect(messageWidget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<std::string>();
      EXPECT_EQ(_name, "type");
      EXPECT_EQ(v, "GUI");
      signalReceived = true;
    });

  // Check default value
  EXPECT_EQ(messageWidget->PropertyValue("type").value<std::string>(),
      std::string("LINK"));

  auto label = enumWidget->findChild<QLabel *>();
  EXPECT_NE(label, nullptr);
  EXPECT_EQ(label->text(), "Type");

  // Get signal emitting widgets
  auto comboBoxes = enumWidget->findChildren<QComboBox *>();
  EXPECT_EQ(comboBoxes.size(), 1);
  EXPECT_EQ(comboBoxes[0]->count(), 8);

  // Change the value and check new value at callback
  comboBoxes[0]->setCurrentIndex(6);
  comboBoxes[0]->currentIndexChanged(6);

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete messageWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, PropertyByName)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Boolean();
  msg->set_data(false);

  // Create widget from message
  auto messageWidget = new MessageWidget(msg);
  EXPECT_TRUE(messageWidget != nullptr);

  // Get generated widgets by name
  for (auto name : {"header", "header::stamp", "header::stamp::sec",
      "header::stamp::nsec", "data"})
  {
    EXPECT_NE(messageWidget->PropertyWidgetByName(name), nullptr);
  }

  // Fail with invalid names
  for (auto name : {"", "banana"})
  {
    EXPECT_EQ(messageWidget->PropertyWidgetByName(name), nullptr);
  }

  delete messageWidget;
  EXPECT_TRUE(stop());
}

