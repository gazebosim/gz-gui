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
#include <string>

#include <ignition/common/Console.hh>

#include "ignition/gui/Iface.hh"
#include "ignition/gui/VariablePill.hh"
#include "ignition/gui/VariablePillContainer.hh"

using namespace ignition;
using namespace gui;

/////////////////////////////////////////////////
TEST(VariablePillContainerTest, VariablePillBasicOperations)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // create container
  VariablePillContainer *container01 = new VariablePillContainer(nullptr);
  ASSERT_NE(nullptr, container01);
  EXPECT_EQ(0u, container01->VariablePillCount());

  // create variable pills
  VariablePill *var01 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var01);
  var01->SetText("pill0");
  EXPECT_EQ(0u, var01->VariablePillCount());
  EXPECT_EQ(0u, var01->Id());
  EXPECT_EQ("variable0", var01->Name());
  EXPECT_EQ("pill0", var01->Text());
  EXPECT_FALSE(var01->IsSelected());
  var01->SetSelected(true);
  EXPECT_TRUE(var01->IsSelected());
  var01->SetSelected(false);
  EXPECT_FALSE(var01->IsSelected());

  VariablePill *var02 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var02);
  var02->SetText("pill1");
  EXPECT_EQ(0u, var02->VariablePillCount());
  EXPECT_EQ(1u, var02->Id());
  EXPECT_EQ("variable1", var02->Name());
  EXPECT_EQ("pill1", var02->Text());
  var02->SetName("newVariable1");
  EXPECT_EQ("newVariable1", var02->Name());

  var02->SetParent(var01);
  EXPECT_EQ(nullptr, var01->Parent());
  EXPECT_EQ(var01, var02->Parent());

  var01->SetContainer(container01);
  EXPECT_EQ(container01, var01->Container());

  EXPECT_TRUE(var01->ContainsPoint(ignition::math::Vector2i()));
  EXPECT_FALSE(var01->ContainsPoint(ignition::math::Vector2i(1000, 1000)));

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(VariablePillContainerTest, VariablePillEvents)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // create container
  VariablePillContainer *container01 = new VariablePillContainer(nullptr);
  ASSERT_NE(nullptr, container01);
  EXPECT_EQ(0u, container01->VariablePillCount());

  // create variable pills
  VariablePill *var01 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var01);

  VariablePill *var02 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var02);

  container01->show();

  // First, we have a container with two variables. We're going to simulate
  // dragging one of the variables (var01) into the other (var02). At that
  // point we'll have a single multi-variable.
  container01->AddVariablePill(var01);
  container01->AddVariablePill(var02);
  EXPECT_EQ(2u, container01->VariablePillCount());
  EXPECT_EQ(0u, var01->VariablePillCount());
  EXPECT_EQ(0u, var02->VariablePillCount());

  QPoint var01Center(var01->width() * 0.5, var01->height() * 0.5);
  QPoint var02Center(var02->width() * 0.5, var02->height() * 0.5);

  // Click.
  auto mousePressEvent = new QMouseEvent(QEvent::MouseButtonPress,
    var01Center, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::postEvent(var01, mousePressEvent);
  QCoreApplication::processEvents();

  auto var01Global = var01->mapToGlobal(var01Center);
  auto var02Global = var02->mapToGlobal(var02Center);
  unsigned int diff = var02Global.x() - var01Global.x();

  // Drag.
  int created = 0;
  int triggered = 0;
  for (unsigned int i = 0; i < diff; ++i)
  {
    created++;
    QTimer::singleShot(300, [i, diff, var02Center, &triggered, &var01, &var02]
    {
      if (i == diff - 1)
      {
        // Release.
        QTimer::singleShot(50, [var02Center, &var02] {
          auto mouseReleaseEvent = new QMouseEvent(QEvent::MouseButtonRelease,
            var02Center, var02->mapToGlobal(var02Center), Qt::LeftButton,
            Qt::NoButton, Qt::NoModifier);
          QCoreApplication::postEvent(var02, mouseReleaseEvent);
          QCoreApplication::processEvents();
        });
      }

      // Compute the next x pos to move the mouse cursor to.
      QPoint center((var01->width() * 0.5) + i, var01->height() * 0.5);
      auto mouseMoveEvent = new QMouseEvent(QEvent::MouseMove,
        center, var01->mapToGlobal(center), Qt::NoButton, Qt::LeftButton,
        Qt::NoModifier);
      QCoreApplication::postEvent(var01, mouseMoveEvent);
      QCoreApplication::processEvents();
      triggered++;
    });
  }

  while (triggered < created)
  {
    QCoreApplication::processEvents();
  }
  EXPECT_EQ(triggered, created);

  // Then, a container with one multi-variable pill.
  EXPECT_EQ(2u, container01->VariablePillCount());
  EXPECT_EQ(0u, var01->VariablePillCount());
  EXPECT_EQ(1u, var02->VariablePillCount());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(VariablePillContainerTest, VariablePillContainerBasicOperations)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // create container
  VariablePillContainer *container01 = new VariablePillContainer(nullptr);
  ASSERT_NE(nullptr, container01);
  EXPECT_EQ(0u, container01->VariablePillCount());
  container01->SetText("aContainer");
  EXPECT_EQ("aContainer", container01->Text());

  auto id0 = container01->AddVariablePill("var0");
  container01->SetVariablePillLabel(id0, "label0");
  auto var0 = container01->VariablePill(id0);
  ASSERT_NE(nullptr, var0);
  EXPECT_EQ("label0", var0->Text());

  auto id1 = container01->AddVariablePill("var1");
  auto var1 = container01->VariablePill(id1);
  ASSERT_NE(nullptr, var1);

  EXPECT_FALSE(var0->IsSelected());
  EXPECT_FALSE(var1->IsSelected());
  container01->SetSelected(nullptr);
  EXPECT_FALSE(var0->IsSelected());
  EXPECT_FALSE(var1->IsSelected());

  container01->SetSelected(var0);
  EXPECT_TRUE(var0->IsSelected());
  EXPECT_FALSE(var1->IsSelected());
  container01->SetSelected(var1);
  EXPECT_FALSE(var0->IsSelected());
  EXPECT_TRUE(var1->IsSelected());

  EXPECT_EQ(2u, container01->VariablePillCount());
  container01->RemoveVariablePill(id0);
  container01->RemoveVariablePill(id1);
  EXPECT_EQ(0u, container01->VariablePillCount());

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(VariablePillContainerTest, AddRemoveVariable)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // create container
  VariablePillContainer *container01 = new VariablePillContainer(nullptr);
  ASSERT_NE(nullptr, container01);
  EXPECT_EQ(0u, container01->VariablePillCount());

  // create variable pills
  VariablePill *var01 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var01);
  EXPECT_EQ(0u, var01->VariablePillCount());

  VariablePill *var02 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var02);
  EXPECT_EQ(0u, var02->VariablePillCount());

  VariablePill *var03 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var03);
  EXPECT_EQ(0u, var03->VariablePillCount());

  VariablePill *var04 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var04);
  EXPECT_EQ(0u, var04->VariablePillCount());

  VariablePill *var05 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var05);
  EXPECT_EQ(0u, var05->VariablePillCount());
  var05->SetName("var05");

  // add variable to container
  container01->AddVariablePill(var01);
  EXPECT_EQ(1u, container01->VariablePillCount());
  EXPECT_EQ(container01, var01->Container());
  EXPECT_EQ(nullptr, var01->Parent());

  // add another variable to container
  container01->AddVariablePill(var02);
  EXPECT_EQ(2u, container01->VariablePillCount());
  EXPECT_EQ(container01, var02->Container());
  EXPECT_EQ(nullptr, var02->Parent());

  // add an invalid variable pill
  var02->AddVariablePill(nullptr);
  EXPECT_EQ(2u, container01->VariablePillCount());

  // add variable to another variable - verify that containers can hold
  // multi-variables and report correct variable count
  var02->AddVariablePill(var03);
  EXPECT_EQ(3u, container01->VariablePillCount());
  EXPECT_EQ(container01, var03->Container());
  EXPECT_EQ(var02, var03->Parent());

  EXPECT_EQ(1u, var02->VariablePills().size());

  // make another multi-variable
  var01->AddVariablePill(var04);
  EXPECT_EQ(4u, container01->VariablePillCount());
  EXPECT_EQ(container01, var04->Container());
  EXPECT_EQ(var01, var04->Parent());

  // add variable to a variable which is within a multi-variable
  var04->AddVariablePill(var05);
  EXPECT_EQ(5u, container01->VariablePillCount());
  EXPECT_EQ(container01, var05->Container());
  EXPECT_EQ(var01, var05->Parent());

  EXPECT_EQ(2u, var01->VariablePills().size());

  EXPECT_EQ(nullptr, var04->VariablePillByName("___wrong_name___"));
  EXPECT_EQ(nullptr, var04->VariablePillByName("var05"));
  EXPECT_EQ(var05, var01->VariablePillByName("var05"));
  EXPECT_EQ(nullptr, var02->VariablePillByName("var05"));
  EXPECT_EQ(var05, var05->VariablePillByName("var05"));

  // remove variable
  container01->RemoveVariablePill(var01);
  EXPECT_EQ(4u, container01->VariablePillCount());
  EXPECT_EQ(nullptr, var01->Parent());

  // remove already removed variable
  container01->RemoveVariablePill(var01);
  EXPECT_EQ(4u, container01->VariablePillCount());

  // remove child of a multi-variable - check that the parent is not affected
  container01->RemoveVariablePill(var03);
  EXPECT_EQ(3u, container01->VariablePillCount());
  EXPECT_EQ(container01, var02->Container());
  EXPECT_EQ(nullptr, var02->Parent());
  EXPECT_EQ(nullptr, var03->Parent());

  // remove multi-variable - check that it doesn't remove the child variable
  container01->RemoveVariablePill(var04);
  EXPECT_EQ(2u, container01->VariablePillCount());
  ASSERT_EQ(nullptr, var05->Parent());
  EXPECT_EQ(container01, var05->Container());
  EXPECT_EQ(nullptr, var04->Parent());
  EXPECT_EQ(nullptr, var05->Parent());

  // remove remaining variables
  container01->RemoveVariablePill(var02);
  EXPECT_EQ(1u, container01->VariablePillCount());
  EXPECT_EQ(nullptr, var02->Parent());

  container01->RemoveVariablePill(var05);
  EXPECT_EQ(0u, container01->VariablePillCount());
  EXPECT_EQ(nullptr, var05->Parent());

  delete var05;
  delete var04;
  delete var03;
  delete var02;
  delete var01;
  delete container01;

  EXPECT_TRUE(stop());
}

/////////////////////////////////////////////////
TEST(VariablePillContainerTest, MaxSize)
{
  setVerbosity(4);
  EXPECT_TRUE(initApp());

  // create new container
  VariablePillContainer *container =  new VariablePillContainer(nullptr);
  ASSERT_NE(nullptr, container);

  // set text
  std::string containerName = "container_test";
  container->SetText(containerName);
  EXPECT_EQ(containerName, container->Text());

  // set max size
  EXPECT_EQ(container->MaxSize(), -1);
  int maxSize = 1;
  container->SetMaxSize(maxSize);
  EXPECT_EQ(maxSize, container->MaxSize());

  // create variable pills
  VariablePill *var01 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var01);
  EXPECT_EQ(0u, var01->VariablePillCount());

  VariablePill *var02 = new VariablePill(nullptr);
  ASSERT_NE(nullptr, var02);
  EXPECT_EQ(0u, var02->VariablePillCount());

  // add variable to container - max size reached
  container->AddVariablePill(var01);
  EXPECT_EQ(1u, container->VariablePillCount());
  EXPECT_EQ(container, var01->Container());
  EXPECT_EQ(nullptr, var01->Parent());

  // verify no more variables can be added
  container->AddVariablePill(var02);
  EXPECT_EQ(1u, container->VariablePillCount());
  ASSERT_EQ(nullptr, var02->Container());
  EXPECT_EQ(nullptr, var02->Parent());

  var01->AddVariablePill(var02);
  EXPECT_EQ(1u, container->VariablePillCount());
  EXPECT_EQ(nullptr, var02->Container());
  EXPECT_EQ(nullptr, var02->Parent());

  // remove variable and verify we can add a different variable now
  container->RemoveVariablePill(var01);
  EXPECT_EQ(0u, container->VariablePillCount());
  EXPECT_EQ(nullptr, var01->Container());
  EXPECT_EQ(nullptr, var01->Parent());

  container->AddVariablePill(var02);
  EXPECT_EQ(1u, container->VariablePillCount());
  EXPECT_EQ(container, var02->Container());
  EXPECT_EQ(nullptr, var02->Parent());

  delete var02;
  delete var01;
  delete container;

  EXPECT_TRUE(stop());
}
