/*
 * Copyright (C) 2016 Open Source Robotics Foundation
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

#include "ignition/gui/Iface.hh"
#include "ignition/gui/VariablePill.hh"
#include "ignition/gui/VariablePillContainer.hh"

using namespace ignition;
using namespace gui;

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

  // add variable to another variable - verify that containers can hold
  // multi-variables and report correct variable count
  var02->AddVariablePill(var03);
  EXPECT_EQ(3u, container01->VariablePillCount());
  EXPECT_EQ(container01, var03->Container());
  EXPECT_EQ(var02, var03->Parent());

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
