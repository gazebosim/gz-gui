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
TEST(MessageWidgetTest, ConstructAndUpdate)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Invalid constructor
  {
    auto widget = new MessageWidget(nullptr);
    ASSERT_NE(widget, nullptr);

    EXPECT_FALSE(widget->UpdateFromMsg(new msgs::StringMsg()));
  }

  // Valid constructor, invalid update
  {
    auto widget = new MessageWidget(new msgs::StringMsg());
    ASSERT_NE(widget, nullptr);

    EXPECT_FALSE(widget->UpdateFromMsg(nullptr));
  }

  // Update type different from constructor
  {
    auto widget = new MessageWidget(new msgs::StringMsg());
    ASSERT_NE(widget, nullptr);

    EXPECT_FALSE(widget->UpdateFromMsg(new msgs::Int32()));
  }

  // Same type as constructor
  {
    auto widget = new MessageWidget(new msgs::StringMsg());
    ASSERT_NE(widget, nullptr);

    EXPECT_TRUE(widget->UpdateFromMsg(new msgs::StringMsg()));
  }

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, EmptyMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  {
    msgs::Visual visualMsg;

    auto msgWidget = new MessageWidget(&visualMsg);

    QCoreApplication::processEvents();

    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(msgWidget->Msg());
    EXPECT_TRUE(retVisualMsg != nullptr);

    delete msgWidget;
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
  auto msgWidget = new MessageWidget(&jointMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(msgWidget->Msg());
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
    msgWidget->SetPropertyValue("name", QVariant::fromValue(
        std::string("test_joint_updated")));
    msgWidget->SetPropertyValue("id", 9999999u);
    msgWidget->SetPropertyValue("parent", QVariant::fromValue(
        std::string("test_joint_parent_updated")));
    msgWidget->SetPropertyValue("parent_id", 1u);
    msgWidget->SetPropertyValue("child", QVariant::fromValue(
        std::string("test_joint_child_updated")));
    msgWidget->SetPropertyValue("child_id", 2u);

    // type
    msgWidget->SetPropertyValue("type", QVariant::fromValue(
        msgs::Joint_Type_Name(msgs::Joint_Type_UNIVERSAL)));

    // pose
    math::Vector3d pos(2.0, 9.0, -4.0);
    math::Quaterniond quat(0.0, 0.0, 1.57);
    msgWidget->SetPropertyValue("pose", QVariant::fromValue(
        math::Pose3d(pos, quat)));

    // axis1
    msgWidget->SetPropertyValue("axis1::xyz", QVariant::fromValue(
        math::Vector3d::UnitY));
    msgWidget->SetPropertyValue("axis1::use_parent_model_frame",
        true);
    msgWidget->SetPropertyValue("axis1::limit_lower", -1.2);
    msgWidget->SetPropertyValue("axis1::limit_upper", -1.0);
    msgWidget->SetPropertyValue("axis1::limit_effort", 1.0);
    msgWidget->SetPropertyValue("axis1::limit_velocity", 100.0);
    msgWidget->SetPropertyValue("axis1::damping", 0.9);

    // axis2
    msgWidget->SetPropertyValue("axis2::xyz", QVariant::fromValue(
        math::Vector3d::UnitZ));
    msgWidget->SetPropertyValue("axis2::use_parent_model_frame",
        true);
    msgWidget->SetPropertyValue("axis2::limit_lower", -3.2);
    msgWidget->SetPropertyValue("axis2::limit_upper", -3.0);
    msgWidget->SetPropertyValue("axis2::limit_effort", 3.0);
    msgWidget->SetPropertyValue("axis2::limit_velocity", 300.0);
    msgWidget->SetPropertyValue("axis2::damping", 3.9);

    // other joint physics properties
    msgWidget->SetPropertyValue("cfm", 0.9);
    msgWidget->SetPropertyValue("bounce", 0.8);
    msgWidget->SetPropertyValue("velocity", 0.7);
    msgWidget->SetPropertyValue("fudge_factor", 0.6);
    msgWidget->SetPropertyValue("limit_cfm", 0.5);
    msgWidget->SetPropertyValue("limit_erp", 0.4);
    msgWidget->SetPropertyValue("suspension_cfm", 0.3);
    msgWidget->SetPropertyValue("suspension_erp", 0.2);
  }

  // verify widget values
  {
    // joint
    EXPECT_EQ(msgWidget->PropertyValue("name").value<std::string>(),
        "test_joint_updated");
    EXPECT_EQ(msgWidget->PropertyValue("id"), 9999999u);
    EXPECT_EQ(msgWidget->PropertyValue("parent").value<std::string>(),
        "test_joint_parent_updated");
    EXPECT_EQ(msgWidget->PropertyValue("parent_id"), 1u);
    EXPECT_EQ(msgWidget->PropertyValue("child").value<std::string>(),
        "test_joint_child_updated");
    EXPECT_EQ(msgWidget->PropertyValue("child_id"), 2u);

    // type
    msgWidget->SetPropertyValue("type", QVariant::fromValue(
        msgs::Joint_Type_Name(msgs::Joint_Type_UNIVERSAL)));

    // pose
    math::Vector3d pos(2.0, 9.0, -4.0);
    math::Quaterniond quat(0.0, 0.0, 1.57);
    EXPECT_EQ(msgWidget->PropertyValue("pose").value<math::Pose3d>(),
        math::Pose3d(pos, quat));

    // axis1
    EXPECT_EQ(msgWidget->PropertyValue(
        "axis1::xyz").value<math::Vector3d>(), math::Vector3d::UnitY);
    EXPECT_EQ(msgWidget->PropertyValue(
        "axis1::use_parent_model_frame").toBool(), true);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis1::limit_lower").toDouble(), -1.2);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis1::limit_upper").toDouble(), -1.0);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis1::limit_effort").toDouble(), 1.0);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis1::limit_velocity").toDouble(), 100.0);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis1::damping").toDouble(), 0.9);

    // axis2
    EXPECT_EQ(msgWidget->PropertyValue(
        "axis2::xyz").value<math::Vector3d>(), math::Vector3d::UnitZ);
    EXPECT_EQ(msgWidget->PropertyValue(
        "axis1::use_parent_model_frame").toBool(), true);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis2::limit_lower").toDouble(), -3.2);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis2::limit_upper").toDouble(), -3.0);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis2::limit_effort").toDouble(), 3.0);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis2::limit_velocity").toDouble(), 300.0);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "axis2::damping").toDouble(), 3.9);

    // other joint physics properties
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "cfm").toDouble(), 0.9);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "bounce").toDouble(), 0.8);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "velocity").toDouble(), 0.7);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "fudge_factor").toDouble(), 0.6);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "limit_cfm").toDouble(), 0.5);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "limit_erp").toDouble(), 0.4);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "suspension_cfm").toDouble(), 0.3);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "suspension_erp").toDouble(), 0.2);
  }

  // verify updates in new msg
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(msgWidget->Msg());
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
    msgWidget->SetPropertyValue("name", QVariant::fromValue(
        std::string("test_joint_updated2")));
    msgWidget->SetPropertyValue("id", 2222222u);
    msgWidget->SetPropertyValue("parent", QVariant::fromValue(
        std::string("test_joint_parent_updated2")));
    msgWidget->SetPropertyValue("parent_id", 10u);
    msgWidget->SetPropertyValue("child", QVariant::fromValue(
        std::string("test_joint_child_updated2")));
    msgWidget->SetPropertyValue("child_id", 20u);

    // type
    msgWidget->SetPropertyValue("type", QVariant::fromValue(
        msgs::Joint_Type_Name(msgs::Joint_Type_BALL)));

    // pose
    math::Vector3d pos(-2.0, 1.0, 2.0);
    math::Quaterniond quat(0.0, 0.0, 0.0);
    msgWidget->SetPropertyValue("pose", QVariant::fromValue(
        math::Pose3d(pos, quat)));

    // other joint physics properties
    msgWidget->SetPropertyValue("cfm", 0.19);
    msgWidget->SetPropertyValue("bounce", 0.18);
    msgWidget->SetPropertyValue("velocity", 2.7);
    msgWidget->SetPropertyValue("fudge_factor", 0.26);
    msgWidget->SetPropertyValue("limit_cfm", 0.15);
    msgWidget->SetPropertyValue("limit_erp", 0.24);
    msgWidget->SetPropertyValue("suspension_cfm", 0.13);
    msgWidget->SetPropertyValue("suspension_erp", 0.12);
  }

  // verify widget values
  {
    // joint
    EXPECT_EQ(msgWidget->PropertyValue("name").value<std::string>(),
        "test_joint_updated2");
    EXPECT_EQ(msgWidget->PropertyValue("id"), 2222222u);
    EXPECT_EQ(msgWidget->PropertyValue("parent").value<std::string>(),
        "test_joint_parent_updated2");
    EXPECT_EQ(msgWidget->PropertyValue("parent_id"), 10u);
    EXPECT_EQ(msgWidget->PropertyValue("child").value<std::string>(),
        "test_joint_child_updated2");
    EXPECT_EQ(msgWidget->PropertyValue("child_id"), 20u);

    // type
    msgWidget->SetPropertyValue("type", QVariant::fromValue(
        msgs::Joint_Type_Name(msgs::Joint_Type_BALL)));

    // pose
    math::Vector3d pos(-2.0, 1.0, 2.0);
    math::Quaterniond quat(0.0, 0.0, 0.0);
    EXPECT_EQ(msgWidget->PropertyValue("pose"), QVariant::fromValue(
        math::Pose3d(pos, quat)));

    // other joint physics properties
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "cfm").toDouble(), 0.19);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "bounce").toDouble(), 0.18);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "velocity").toDouble(), 2.7);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "fudge_factor").toDouble(), 0.26);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "limit_cfm").toDouble(), 0.15);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "limit_erp").toDouble(), 0.24);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "suspension_cfm").toDouble(), 0.13);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "suspension_erp").toDouble(), 0.12);
  }

  // verify updates in new msg
  {
    auto retJointMsg = dynamic_cast<msgs::Joint *>(msgWidget->Msg());
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
  auto msgWidget = new MessageWidget(&visualMsg);

  // retrieve the message from the message widget and
  // verify that all values have not been changed.
  {
    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(msgWidget->Msg());
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
    msgWidget->SetPropertyValue("name", QVariant::fromValue(
        std::string("test_visual_updated")));
    msgWidget->SetPropertyValue("id", 11111u);
    msgWidget->SetPropertyValue("parent_name", QVariant::fromValue(
        std::string("test_visual_parent_updated")));
    msgWidget->SetPropertyValue("parent_id", 55555u);
    msgWidget->SetPropertyValue("cast_shadows", false);
    msgWidget->SetPropertyValue("transparency", 1.0);
    msgWidget->SetPropertyValue("visible", false);
    msgWidget->SetPropertyValue("delete_me", true);
    msgWidget->SetPropertyValue("is_static", true);
    msgWidget->SetPropertyValue("scale", QVariant::fromValue(
        math::Vector3d(2.0, 1.5, 0.5)));

    // pose
    math::Vector3d pos(-2.0, -3.0, -4.0);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    msgWidget->SetPropertyValue("pose", QVariant::fromValue(
        math::Pose3d(pos, quat)));

    // geometry
    msgs::Geometry newGeom;
    newGeom.set_type(msgs::Geometry::BOX);
    msgs::Set(newGeom.mutable_box()->mutable_size(),
              math::Vector3d(5.0, 3.0, 4.0));
    msgWidget->SetPropertyValue("geometry", QVariant::fromValue(
        newGeom));

    // material
    msgWidget->SetPropertyValue("material::normal_map", QVariant::fromValue(
        std::string("test_normal_map_updated")));
    msgWidget->SetPropertyValue("material::ambient", QVariant::fromValue(
        math::Color(0.2, 0.3, 0.4, 0.5)));
    msgWidget->SetPropertyValue("material::diffuse", QVariant::fromValue(
        math::Color(0.1, 0.8, 0.6, 0.4)));
    msgWidget->SetPropertyValue("material::specular", QVariant::fromValue(
        math::Color(0.5, 0.4, 0.3, 0.2)));
    msgWidget->SetPropertyValue("material::emissive", QVariant::fromValue(
        math::Color(0.4, 0.6, 0.8, 0.1)));
    msgWidget->SetPropertyValue("material::lighting", false);
    // material::script
    msgWidget->SetPropertyValue("material::script::name", QVariant::fromValue(
        std::string("test_script_name_updated")));
  }

  // verify widget values
  {
    EXPECT_EQ(msgWidget->PropertyValue(
        "name").value<std::string>(), "test_visual_updated");
    EXPECT_EQ(msgWidget->PropertyValue(
        "id"), 11111u);
    EXPECT_EQ(msgWidget->PropertyValue(
        "parent_name").value<std::string>(), "test_visual_parent_updated");
    EXPECT_EQ(msgWidget->PropertyValue(
        "parent_id"), 55555u);
    EXPECT_EQ(msgWidget->PropertyValue(
        "cast_shadows").toBool(), false);
    EXPECT_DOUBLE_EQ(msgWidget->PropertyValue(
        "transparency").toDouble(), 1.0);
    EXPECT_EQ(msgWidget->PropertyValue(
        "visible").toBool(), false);
    EXPECT_EQ(msgWidget->PropertyValue(
        "delete_me").toBool(), true);
    EXPECT_EQ(msgWidget->PropertyValue(
        "is_static").toBool(), true);
    EXPECT_EQ(msgWidget->PropertyValue(
        "scale").value<math::Vector3d>(), math::Vector3d(2.0, 1.5, 0.5));

    // pose
    math::Vector3d pos(-2.0, -3.0, -4.0);
    math::Quaterniond quat(0.0, 1.57, 0.0);
    EXPECT_EQ(msgWidget->PropertyValue("pose"), QVariant::fromValue(
        math::Pose3d(pos, quat)));

    // geometry
    auto geomValue =
        msgWidget->PropertyValue("geometry").value<msgs::Geometry>();
    EXPECT_EQ(msgs::ConvertGeometryType(geomValue.type()), "box");
    EXPECT_EQ(msgs::Convert(geomValue.box().size()),
              math::Vector3d(5.0, 3.0, 4.0));

    // material
    EXPECT_EQ(msgWidget->PropertyValue(
        "material::normal_map").value<std::string>(),
        "test_normal_map_updated");
    EXPECT_EQ(msgWidget->PropertyValue("material::ambient"),
        QVariant::fromValue(math::Color(0.2, 0.3, 0.4, 0.5)));
    EXPECT_EQ(msgWidget->PropertyValue("material::diffuse"),
        QVariant::fromValue(math::Color(0.1, 0.8, 0.6, 0.4)));
    EXPECT_EQ(msgWidget->PropertyValue("material::specular"),
        QVariant::fromValue(math::Color(0.5, 0.4, 0.3, 0.2)));
    EXPECT_EQ(msgWidget->PropertyValue("material::emissive"),
        QVariant::fromValue(math::Color(0.4, 0.6, 0.8, 0.1)));
    EXPECT_EQ(msgWidget->PropertyValue("material::lighting").toBool(), false);
    // material::script
    EXPECT_EQ(msgWidget->PropertyValue(
        "material::script::name").value<std::string>(),
        "test_script_name_updated");
  }

  // verify updates in new msg
  {
    auto retVisualMsg =
        dynamic_cast<msgs::Visual *>(msgWidget->Msg());
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

  delete msgWidget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test LINE and PLAIN_TEXT string fields
TEST(MessageWidgetTest, PluginMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Plugin msg;
  msg.set_name("test_plugin");
  msg.set_filename("test_plugin_filename");
  msg.set_innerxml("<param>1</param>\n");

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::Plugin *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->name(), "test_plugin");
  EXPECT_EQ(retMsg->filename(), "test_plugin_filename");
  EXPECT_EQ(retMsg->innerxml(), "<param>1</param>\n");

  // Update from message
  msg.set_name("test_plugin_new");
  msg.set_filename("test_plugin_filename_new");
  msg.set_innerxml("<param>2</param>\n");

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::Plugin *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->name(), "test_plugin_new");
  EXPECT_EQ(retMsg->filename(), "test_plugin_filename_new");
  EXPECT_EQ(retMsg->innerxml(), "<param>2</param>\n");

  // Update fields
  widget->SetPropertyValue("name", QVariant::fromValue(
      std::string("test_plugin_updated")));
  widget->SetPropertyValue("filename", QVariant::fromValue(
      std::string("test_plugin_filename_updated")));
  widget->SetPropertyValue("innerxml", QVariant::fromValue(
      std::string("<param2>new_param</param2>\n")));

  // Check fields
  EXPECT_EQ(widget->PropertyValue(
      "name").value<std::string>(), "test_plugin_updated");
  EXPECT_EQ(widget->PropertyValue(
      "filename").value<std::string>(), "test_plugin_filename_updated");
  EXPECT_EQ(widget->PropertyValue(
      "innerxml").value<std::string>(), "<param2>new_param</param2>\n");

  // Check new message
  retMsg = dynamic_cast<msgs::Plugin *>(widget->Msg());
  EXPECT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->name(), "test_plugin_updated");
  EXPECT_EQ(retMsg->filename(), "test_plugin_filename_updated");
  EXPECT_EQ(retMsg->innerxml(), "<param2>new_param</param2>\n");

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test double, uint32 and bool fields
TEST(MessageWidgetTest, SurfaceMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Surface msg;
  msg.set_kp(100.5);
  msg.set_collide_bitmask(1);
  msg.set_collide_without_contact(true);

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::Surface *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_DOUBLE_EQ(retMsg->kp(), 100.5);
  EXPECT_EQ(retMsg->collide_bitmask(), 1u);
  EXPECT_TRUE(retMsg->collide_without_contact());

  // Update from message
  msg.set_kp(888.44);
  msg.set_collide_bitmask(444);
  msg.set_collide_without_contact(false);

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::Surface *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_DOUBLE_EQ(retMsg->kp(), 888.44);
  EXPECT_EQ(retMsg->collide_bitmask(), 444u);
  EXPECT_FALSE(retMsg->collide_without_contact());

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test float fields
TEST(MessageWidgetTest, LightMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::Light msg;
  msg.set_spot_falloff(0.5);

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::Light *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_LT(fabs(retMsg->spot_falloff()- 0.5), 0.000001);

  // Update from message
  msg.set_spot_falloff(0.001);

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::Light *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_LT(fabs(retMsg->spot_falloff()- 0.001), 0.000001);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
// Test uint64 fields
TEST(MessageWidgetTest, WorldStatsMsgWidget)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  msgs::WorldStatistics msg;
  msg.set_iterations(555);

  // Create widget
  auto widget = new MessageWidget(&msg);
  ASSERT_NE(widget, nullptr);

  // Retrieve message
  auto retMsg = dynamic_cast<msgs::WorldStatistics *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->iterations(), 555u);

  // Update from message
  msg.set_iterations(99999999);

  widget->UpdateFromMsg(&msg);

  // Check new message
  retMsg = dynamic_cast<msgs::WorldStatistics *>(widget->Msg());
  ASSERT_NE(retMsg, nullptr);

  EXPECT_EQ(retMsg->iterations(), 99999999u);

  delete widget;
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
  auto msgWidget = new MessageWidget(&visualMsg);
  msgWidget->show();

  // Check that only top-level widgets are visible by default
  {
    EXPECT_TRUE(msgWidget->WidgetVisible("id"));
    EXPECT_TRUE(msgWidget->WidgetVisible("pose"));
    EXPECT_TRUE(msgWidget->WidgetVisible("geometry"));
    EXPECT_TRUE(msgWidget->WidgetVisible("material"));
    EXPECT_FALSE(msgWidget->WidgetVisible("material::diffuse"));
    EXPECT_FALSE(msgWidget->WidgetVisible("material::script::name"));
  }

  // Expand nested collapsible widgets and check they become visible
  {
    auto material = msgWidget->PropertyWidgetByName("material");
    ASSERT_NE(material, nullptr);

    auto button = material->findChild<QPushButton *>();
    ASSERT_NE(button, nullptr);

    button->click();

    EXPECT_TRUE(msgWidget->WidgetVisible("material::diffuse"));
    // Checking that `script` is visible, since `name` is an only child
    EXPECT_TRUE(msgWidget->WidgetVisible("material::script::name"));
  }

  // set different types of widgets to be not visibile
  {
    // primitive widget
    msgWidget->SetWidgetVisible("id", false);
    // custom pose message widget
    msgWidget->SetWidgetVisible("pose", false);
    // custom geometry message widget
    msgWidget->SetWidgetVisible("geometry", false);
    // widget inside a group widget
    msgWidget->SetWidgetVisible("material::diffuse", false);
    // widget two levels deep
    msgWidget->SetWidgetVisible("material::script::name", false);
    // group widget
    msgWidget->SetWidgetVisible("material", false);

    EXPECT_EQ(msgWidget->WidgetVisible("id"), false);
    EXPECT_EQ(msgWidget->WidgetVisible("pose"), false);
    EXPECT_EQ(msgWidget->WidgetVisible("geometry"), false);
    EXPECT_EQ(msgWidget->WidgetVisible("material::diffuse"), false);
    EXPECT_EQ(msgWidget->WidgetVisible("material::script::name"), false);
    EXPECT_EQ(msgWidget->WidgetVisible("material"), false);
  }

  // set visible back to true
  {
    msgWidget->SetWidgetVisible("id", true);
    msgWidget->SetWidgetVisible("pose", true);
    msgWidget->SetWidgetVisible("geometry", true);
    msgWidget->SetWidgetVisible("material::diffuse", true);
    msgWidget->SetWidgetVisible("material::script::name", true);
    msgWidget->SetWidgetVisible("material", true);

    EXPECT_EQ(msgWidget->WidgetVisible("id"), true);
    EXPECT_EQ(msgWidget->WidgetVisible("pose"), true);
    EXPECT_EQ(msgWidget->WidgetVisible("geometry"), true);
    EXPECT_EQ(msgWidget->WidgetVisible("material::diffuse"), true);
    EXPECT_EQ(msgWidget->WidgetVisible("material::script::name"), true);
    EXPECT_EQ(msgWidget->WidgetVisible("material"), true);
  }

  delete msgWidget;
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
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Check we got a string widget
  auto propWidget = widget->PropertyWidgetByName("data");
  ASSERT_NE(propWidget, nullptr);

  auto stringWidget = qobject_cast<StringWidget *>(propWidget);
  ASSERT_NE(stringWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<std::string>();
      EXPECT_EQ(_name, "data");
      EXPECT_EQ(v, "orange");
      signalReceived = true;
    });

  // Check default string
  EXPECT_EQ(widget->PropertyValue("string").value<std::string>(), "");

  // Get signal emitting widgets
  auto lineEdits = stringWidget->findChildren<QLineEdit *>();
  EXPECT_EQ(lineEdits.size(), 1);

  // Change the value and check new value at callback
  lineEdits[0]->setText("orange");
  lineEdits[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildNumberSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Double();
  msg->set_data(-1.5);

  // Create widget from message
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Check we got a number widget
  auto propWidget = widget->PropertyWidgetByName("data");
  ASSERT_NE(propWidget, nullptr);

  auto numberWidget = qobject_cast<NumberWidget *>(propWidget);
  ASSERT_NE(numberWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<double>();
      EXPECT_EQ(_name, "data");
      EXPECT_DOUBLE_EQ(v, 0.999);
      signalReceived = true;
    });

  // Get signal emitting widgets
  auto spins = widget->findChildren<QDoubleSpinBox *>();
  ASSERT_EQ(spins.size(), 1);

  // Change the value and check new value at callback
  spins[0]->setValue(0.999);
  spins[0]->editingFinished();

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(MessageWidgetTest, ChildBoolSignal)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // Message
  auto msg = new msgs::Boolean();
  msg->set_data(true);

  // Create widget from message
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Check we got a bool widget
  auto propWidget = widget->PropertyWidgetByName("data");
  ASSERT_NE(propWidget, nullptr);

  auto boolWidget = qobject_cast<BoolWidget *>(propWidget);
  ASSERT_NE(boolWidget, nullptr);

  // Connect signals
  bool signalReceived = false;
  widget->connect(widget, &MessageWidget::ValueChanged,
    [&signalReceived](const std::string &_name, QVariant _var)
    {
      auto v = _var.value<bool>();
      EXPECT_EQ(_name, "data");
      EXPECT_FALSE(v);
      signalReceived = true;
    });

  // Get signal emitting widgets
  auto radios = widget->findChildren<QRadioButton *>();
  EXPECT_EQ(radios.size(), 2);

  // Change the value and check new value at callback
  radios[0]->setChecked(false);
  radios[1]->setChecked(true);

  // Check callback was called
  EXPECT_TRUE(signalReceived);

  delete widget;
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
  auto msg = new msgs::StringMsg();

  // Create widget from message
  auto widget = new MessageWidget(msg);
  ASSERT_NE(widget, nullptr);

  // Get generated widgets by name
  for (auto name : {"header", "header::stamp", "header::stamp::sec",
      "header::stamp::nsec",  "data"})
  {
    EXPECT_NE(widget->PropertyWidgetByName(name), nullptr) << name;
  }

  // Fail with invalid names
  for (auto name : {"", "banana"})
  {
    EXPECT_EQ(widget->PropertyWidgetByName(name), nullptr) << name;
  }

  delete widget;
  EXPECT_TRUE(stop());
}

