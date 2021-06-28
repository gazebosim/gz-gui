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

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <ignition/common/Console.hh>
#include <ignition/common/MeshManager.hh>
#include <ignition/math/Pose3.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/plugin/Register.hh>

// TODO(louise) Remove these pragmas once ign-rendering and ign-msgs
// are disabling the warnings
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include <ignition/msgs.hh>

#include <ignition/rendering/Capsule.hh>
#include <ignition/rendering/RenderEngine.hh>
#include <ignition/rendering/RenderingIface.hh>
#include <ignition/rendering/Scene.hh>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <ignition/transport/Node.hh>

#include "ignition/gui/Application.hh"
#include "ignition/gui/Conversions.hh"
#include "ignition/gui/GuiEvents.hh"
#include "ignition/gui/MainWindow.hh"

#include "TransportSceneManager.hh"

/// \brief Private data class for TransportSceneManager
class ignition::gui::plugins::TransportSceneManagerPrivate
{
  /// \brief Make the scene service request and populate the scene
  public: void Request();

  /// \brief Update the scene based on pose msgs received
  public: void OnRender();

  /// \brief Initialize transport, subscribing to the necessary topics.
  /// To be called after a valid scene has been found.
  public: void InitializeTransport();

  /// \brief Callback function for the pose topic
  /// \param[in] _msg Pose vector msg
  public: void OnPoseVMsg(const msgs::Pose_V &_msg);

  /// \brief Load the scene from a scene msg
  /// \param[in] _msg Scene msg
  public: void LoadScene(const msgs::Scene &_msg);

  /// \brief Callback function for the request topic
  /// \param[in] _msg Deletion message
  public: void OnDeletionMsg(const msgs::UInt32_V &_msg);

  /// \brief Load the scene from a scene msg
  /// \param[in] _msg Scene msg
  public: void OnSceneSrvMsg(const msgs::Scene &_msg, const bool result);

  /// \brief Called when there's an entity is added to the scene
  /// \param[in] _msg Scene msg
  public: void OnSceneMsg(const msgs::Scene &_msg);

  /// \brief Load the model from a model msg
  /// \param[in] _msg Model msg
  /// \return Model visual created from the msg
  public: rendering::VisualPtr LoadModel(const msgs::Model &_msg);

  /// \brief Load a link from a link msg
  /// \param[in] _msg Link msg
  /// \return Link visual created from the msg
  public: rendering::VisualPtr LoadLink(const msgs::Link &_msg);

  /// \brief Load a visual from a visual msg
  /// \param[in] _msg Visual msg
  /// \return Visual visual created from the msg
  public: rendering::VisualPtr LoadVisual(const msgs::Visual &_msg);

  /// \brief Load a geometry from a geometry msg
  /// \param[in] _msg Geometry msg
  /// \param[out] _scale Geometry scale that will be set based on msg param
  /// \param[out] _localPose Additional local pose to be applied after the
  /// visual's pose
  /// \return Geometry object created from the msg
  public: rendering::GeometryPtr LoadGeometry(const msgs::Geometry &_msg,
      math::Vector3d &_scale, math::Pose3d &_localPose);

  /// \brief Load a material from a material msg
  /// \param[in] _msg Material msg
  /// \return Material object created from the msg
  public: rendering::MaterialPtr LoadMaterial(const msgs::Material &_msg);

  /// \brief Load a light from a light msg
  /// \param[in] _msg Light msg
  /// \return Light object created from the msg
  public: rendering::LightPtr LoadLight(const msgs::Light &_msg);

  /// \brief Delete an entity
  /// \param[in] _entity Entity to delete
  public: void DeleteEntity(const unsigned int _entity);

  //// \brief Ign-transport scene service name
  public: std::string service;

  //// \brief Ign-transport pose topic name
  public: std::string poseTopic;

  //// \brief Ign-transport deletion topic name
  public: std::string deletionTopic;

  //// \brief Ign-transport scene topic name
  public: std::string sceneTopic;

  //// \brief Pointer to the rendering scene
  public: rendering::ScenePtr scene{nullptr};

  //// \brief Mutex to protect the msgs
  public: std::mutex msgMutex;

  /// \brief Map of entity id to pose
  public: std::map<unsigned int, math::Pose3d> poses;

  /// \brief Map of entity id to initial local poses
  /// This is currently used to handle the normal vector in plane visuals. In
  /// general, this can be used to store any local transforms between the
  /// parent Visual and geometry.
  public: std::map<unsigned int, math::Pose3d> localPoses;

  /// \brief Map of visual id to visual pointers.
  public: std::map<unsigned int, rendering::VisualPtr::weak_type> visuals;

  /// \brief Map of light id to light pointers.
  public: std::map<unsigned int, rendering::LightPtr::weak_type> lights;

  /// Entities to be deleted
  public: std::vector<unsigned int> toDeleteEntities;

  /// \brief Keeps the a list of unprocessed scene messages
  public: std::vector<msgs::Scene> sceneMsgs;

  /// \brief Transport node for making service request and subscribing to
  /// pose topic
  public: ignition::transport::Node node;
};

using namespace ignition;
using namespace gui;
using namespace plugins;

/////////////////////////////////////////////////
TransportSceneManager::TransportSceneManager()
  : Plugin(), dataPtr(new TransportSceneManagerPrivate)
{
}

/////////////////////////////////////////////////
TransportSceneManager::~TransportSceneManager()
{
}

/////////////////////////////////////////////////
void TransportSceneManager::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  if (this->title.empty())
    this->title = "Transport Scene Manager";

  // Custom parameters
  if (_pluginElem)
  {
    auto elem = _pluginElem->FirstChildElement("service");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      this->dataPtr->service = elem->GetText();
    }

    elem = _pluginElem->FirstChildElement("pose_topic");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      this->dataPtr->poseTopic = elem->GetText();
    }

    elem = _pluginElem->FirstChildElement("deletion_topic");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      this->dataPtr->deletionTopic = elem->GetText();
    }

    elem = _pluginElem->FirstChildElement("scene_topic");
    if (nullptr != elem && nullptr != elem->GetText())
    {
      this->dataPtr->sceneTopic = elem->GetText();
    }
  }

  App()->findChild<MainWindow *>()->installEventFilter(this);
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::InitializeTransport()
{
  this->Request();

  if (!this->node.Subscribe(this->poseTopic,
      &TransportSceneManagerPrivate::OnPoseVMsg, this))
  {
    ignerr << "Error subscribing to pose topic: " << this->poseTopic
      << std::endl;
  }
  else
  {
    ignmsg << "Listening to pose messages on [" << this->poseTopic << "]"
           << std::endl;
  }

  if (!this->node.Subscribe(this->deletionTopic,
      &TransportSceneManagerPrivate::OnDeletionMsg, this))
  {
    ignerr << "Error subscribing to deletion topic: " << this->deletionTopic
      << std::endl;
  }
  else
  {
    ignmsg << "Listening to deletion messages on [" << this->deletionTopic
           << "]" << std::endl;
  }

  if (!this->node.Subscribe(this->sceneTopic,
      &TransportSceneManagerPrivate::OnSceneMsg, this))
  {
    ignerr << "Error subscribing to scene topic: " << this->sceneTopic
           << std::endl;
  }
  else
  {
    ignmsg << "Listening to scene messages on [" << this->sceneTopic << "]"
           << std::endl;
  }

  ignmsg << "Transport initialized." << std::endl;
}

/////////////////////////////////////////////////
bool TransportSceneManager::eventFilter(QObject *_obj, QEvent *_event)
{
  if (_event->type() == events::Render::kType)
  {
    this->dataPtr->OnRender();
  }

  // Standard event processing
  return QObject::eventFilter(_obj, _event);
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::Request()
{
  // wait for the service to be advertized
  std::vector<transport::ServicePublisher> publishers;
  const std::chrono::duration<double> sleepDuration{1.0};
  const std::size_t tries = 30;
  for (std::size_t i = 0; i < tries; ++i)
  {
    this->node.ServiceInfo(this->service, publishers);
    if (publishers.size() > 0)
      break;
    std::this_thread::sleep_for(sleepDuration);
    igndbg << "Waiting for service " << this->service << "\n";
  }

  if (publishers.empty() || !this->node.Request(this->service,
      &TransportSceneManagerPrivate::OnSceneSrvMsg, this))
  {
    ignerr << "Error making service request to " << this->service << std::endl;
  }
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::OnPoseVMsg(const msgs::Pose_V &_msg)
{
  std::lock_guard<std::mutex> lock(this->msgMutex);
  for (int i = 0; i < _msg.pose_size(); ++i)
  {
    math::Pose3d pose = msgs::Convert(_msg.pose(i));

    // apply additional local poses if available
    const auto it = this->localPoses.find(_msg.pose(i).id());
    if (it != this->localPoses.end())
    {
      pose = pose * it->second;
    }

    this->poses[_msg.pose(i).id()] = pose;
  }
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::OnDeletionMsg(const msgs::UInt32_V &_msg)
{
  std::lock_guard<std::mutex> lock(this->msgMutex);
  std::copy(_msg.data().begin(), _msg.data().end(),
            std::back_inserter(this->toDeleteEntities));
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::OnRender()
{
  if (nullptr == this->scene)
  {
    this->scene = rendering::sceneFromFirstRenderEngine();
    if (nullptr == this->scene)
      return;

    this->InitializeTransport();
  }

  std::lock_guard<std::mutex> lock(this->msgMutex);

  for (const auto &msg : this->sceneMsgs)
  {
    this->LoadScene(msg);
  }
  this->sceneMsgs.clear();

  for (const auto &entity : this->toDeleteEntities)
  {
    this->DeleteEntity(entity);
  }
  this->toDeleteEntities.clear();

  for (auto pIt = this->poses.begin(); pIt != this->poses.end();)
  {
    auto vIt = this->visuals.find(pIt->first);
    if (vIt != this->visuals.end())
    {
      auto visual = vIt->second.lock();
      if (visual)
      {
        visual->SetLocalPose(pIt->second);
      }
      else
      {
        this->visuals.erase(vIt);
      }
      this->poses.erase(pIt++);
    }
    else
    {
      auto lIt = this->lights.find(pIt->first);
      if (lIt != this->lights.end())
      {
        auto light = lIt->second.lock();
        if (light)
        {
          light->SetLocalPose(pIt->second);
        }
        else
        {
          this->lights.erase(lIt);
        }
        this->poses.erase(pIt++);
      }
      else
      {
        ++pIt;
      }
    }
  }

  // Note we are clearing the pose msgs here but later on we may need to
  // consider the case where pose msgs arrive before scene/visual msgs
  this->poses.clear();
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::OnSceneMsg(const msgs::Scene &_msg)
{
  std::lock_guard<std::mutex> lock(this->msgMutex);
  this->sceneMsgs.push_back(_msg);
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::OnSceneSrvMsg(const msgs::Scene &_msg,
    const bool result)
{
  if (!result)
  {
    ignerr << "Error making service request to " << this->service
           << std::endl;
    return;
  }

  {
    std::lock_guard<std::mutex> lock(this->msgMutex);
    this->sceneMsgs.push_back(_msg);
  }
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::LoadScene(const msgs::Scene &_msg)
{
  rendering::VisualPtr rootVis = this->scene->RootVisual();

  // load models
  for (int i = 0; i < _msg.model_size(); ++i)
  {
    // Only add if it's not already loaded
    if (this->visuals.find(_msg.model(i).id()) == this->visuals.end())
    {
      rendering::VisualPtr modelVis = this->LoadModel(_msg.model(i));
      if (modelVis)
        rootVis->AddChild(modelVis);
      else
        ignerr << "Failed to load model: " << _msg.model(i).name() << std::endl;
    }
  }

  // load lights
  for (int i = 0; i < _msg.light_size(); ++i)
  {
    if (this->lights.find(_msg.light(i).id()) == this->lights.end())
    {
      rendering::LightPtr light = this->LoadLight(_msg.light(i));
      if (light)
        rootVis->AddChild(light);
      else
        ignerr << "Failed to load light: " << _msg.light(i).name() << std::endl;
    }
  }
}

/////////////////////////////////////////////////
rendering::VisualPtr TransportSceneManagerPrivate::LoadModel(
    const msgs::Model &_msg)
{
  rendering::VisualPtr modelVis = this->scene->CreateVisual();
  if (_msg.has_pose())
    modelVis->SetLocalPose(msgs::Convert(_msg.pose()));
  this->visuals[_msg.id()] = modelVis;

  // load links
  for (int i = 0; i < _msg.link_size(); ++i)
  {
    rendering::VisualPtr linkVis = this->LoadLink(_msg.link(i));
    if (linkVis)
      modelVis->AddChild(linkVis);
    else
      ignerr << "Failed to load link: " << _msg.link(i).name() << std::endl;
  }

  // load nested models
  for (int i = 0; i < _msg.model_size(); ++i)
  {
    rendering::VisualPtr nestedModelVis = this->LoadModel(_msg.model(i));
    if (nestedModelVis)
      modelVis->AddChild(nestedModelVis);
    else
      ignerr << "Failed to load nested model: " << _msg.model(i).name()
             << std::endl;
  }

  return modelVis;
}

/////////////////////////////////////////////////
rendering::VisualPtr TransportSceneManagerPrivate::LoadLink(
    const msgs::Link &_msg)
{
  rendering::VisualPtr linkVis = this->scene->CreateVisual();
  if (_msg.has_pose())
    linkVis->SetLocalPose(msgs::Convert(_msg.pose()));
  this->visuals[_msg.id()] = linkVis;

  // load visuals
  for (int i = 0; i < _msg.visual_size(); ++i)
  {
    rendering::VisualPtr visualVis = this->LoadVisual(_msg.visual(i));
    if (visualVis)
      linkVis->AddChild(visualVis);
    else
      ignerr << "Failed to load visual: " << _msg.visual(i).name() << std::endl;
  }

  // load lights
  for (int i = 0; i < _msg.light_size(); ++i)
  {
    rendering::LightPtr light = this->LoadLight(_msg.light(i));
    if (light)
      linkVis->AddChild(light);
    else
      ignerr << "Failed to load light: " << _msg.light(i).name() << std::endl;
  }

  return linkVis;
}

/////////////////////////////////////////////////
rendering::VisualPtr TransportSceneManagerPrivate::LoadVisual(
    const msgs::Visual &_msg)
{
  if (!_msg.has_geometry())
    return rendering::VisualPtr();

  rendering::VisualPtr visualVis = this->scene->CreateVisual();
  this->visuals[_msg.id()] = visualVis;

  math::Vector3d scale = math::Vector3d::One;
  math::Pose3d localPose;
  rendering::GeometryPtr geom =
      this->LoadGeometry(_msg.geometry(), scale, localPose);

  if (_msg.has_pose())
    visualVis->SetLocalPose(msgs::Convert(_msg.pose()) * localPose);
  else
    visualVis->SetLocalPose(localPose);

  if (geom)
  {
    // store the local pose
    this->localPoses[_msg.id()] = localPose;

    visualVis->AddGeometry(geom);
    visualVis->SetLocalScale(scale);

    // set material
    rendering::MaterialPtr material{nullptr};
    if (_msg.has_material())
    {
      material = this->LoadMaterial(_msg.material());
    }
    // Don't set a default material for meshes because they
    // may have their own
    // TODO(anyone) support overriding mesh material
    else if (!_msg.geometry().has_mesh())
    {
      // create default material
      material = this->scene->Material("ign-grey");
      if (!material)
      {
        material = this->scene->CreateMaterial("ign-grey");
        material->SetAmbient(0.3, 0.3, 0.3);
        material->SetDiffuse(0.7, 0.7, 0.7);
        material->SetSpecular(1.0, 1.0, 1.0);
        material->SetRoughness(0.2f);
        material->SetMetalness(1.0f);
      }
    }
    else
    {
      // meshes created by mesh loader may have their own materials
      // update/override their properties based on input sdf element values
      auto mesh = std::dynamic_pointer_cast<rendering::Mesh>(geom);
      for (unsigned int i = 0; i < mesh->SubMeshCount(); ++i)
      {
        auto submesh = mesh->SubMeshByIndex(i);
        auto submeshMat = submesh->Material();
        if (submeshMat)
        {
          double productAlpha = (1.0-_msg.transparency()) *
              (1.0 - submeshMat->Transparency());
          submeshMat->SetTransparency(1 - productAlpha);
          submeshMat->SetCastShadows(_msg.cast_shadows());
        }
      }
    }

    if (material)
    {
      // set transparency
      material->SetTransparency(_msg.transparency());

      // cast shadows
      material->SetCastShadows(_msg.cast_shadows());

      geom->SetMaterial(material);
      // todo(anyone) SetMaterial function clones the input material.
      // but does not take ownership of it so we need to destroy it here.
      // This is not ideal. We should let ign-rendering handle the lifetime
      // of this material
      this->scene->DestroyMaterial(material);
    }
  }
  else
  {
    ignerr << "Failed to load geometry for visual: " << _msg.name()
           << std::endl;
  }

  return visualVis;
}

/////////////////////////////////////////////////
rendering::GeometryPtr TransportSceneManagerPrivate::LoadGeometry(
    const msgs::Geometry &_msg, math::Vector3d &_scale,
    math::Pose3d &_localPose)
{
  math::Vector3d scale = math::Vector3d::One;
  math::Pose3d localPose = math::Pose3d::Zero;
  rendering::GeometryPtr geom{nullptr};
  if (_msg.has_box())
  {
    geom = this->scene->CreateBox();
    if (_msg.box().has_size())
      scale = msgs::Convert(_msg.box().size());
  }
  else if (_msg.has_cylinder())
  {
    geom = this->scene->CreateCylinder();
    scale.X() = _msg.cylinder().radius() * 2;
    scale.Y() = scale.X();
    scale.Z() = _msg.cylinder().length();
  }
  else if (_msg.has_capsule())
  {
    auto capsule = this->scene->CreateCapsule();
    capsule->SetRadius(_msg.capsule().radius());
    capsule->SetLength(_msg.capsule().length());
    geom = capsule;

    scale.X() = _msg.capsule().radius() * 2;
    scale.Y() = scale.X();
    scale.Z() = _msg.capsule().length() + scale.X();
  }
  else if (_msg.has_ellipsoid())
  {
    geom = this->scene->CreateSphere();
    scale.X() = _msg.ellipsoid().radii().x() * 2;
    scale.Y() = _msg.ellipsoid().radii().y() * 2;
    scale.Z() = _msg.ellipsoid().radii().z() * 2;
  }
  else if (_msg.has_plane())
  {
    geom = this->scene->CreatePlane();

    if (_msg.plane().has_size())
    {
      scale.X() = _msg.plane().size().x();
      scale.Y() = _msg.plane().size().y();
    }

    if (_msg.plane().has_normal())
    {
      // Create a rotation for the plane mesh to account for the normal vector.
      // The rotation is the angle between the +z(0,0,1) vector and the
      // normal, which are both expressed in the local (Visual) frame.
      math::Vector3d normal = msgs::Convert(_msg.plane().normal());
      localPose.Rot().From2Axes(math::Vector3d::UnitZ, normal.Normalized());
    }
  }
  else if (_msg.has_sphere())
  {
    geom = this->scene->CreateSphere();
    scale.X() = _msg.sphere().radius() * 2;
    scale.Y() = scale.X();
    scale.Z() = scale.X();
  }
  else if (_msg.has_mesh())
  {
    if (_msg.mesh().filename().empty())
    {
      ignerr << "Mesh geometry missing filename" << std::endl;
      return geom;
    }
    rendering::MeshDescriptor descriptor;

    // Assume absolute path to mesh file
    descriptor.meshName = _msg.mesh().filename();

    ignition::common::MeshManager* meshManager =
        ignition::common::MeshManager::Instance();
    descriptor.mesh = meshManager->Load(descriptor.meshName);
    geom = this->scene->CreateMesh(descriptor);

    scale = msgs::Convert(_msg.mesh().scale());
  }
  else
  {
    ignerr << "Unsupported geometry type" << std::endl;
  }
  _scale = scale;
  _localPose = localPose;
  return geom;
}

/////////////////////////////////////////////////
rendering::MaterialPtr TransportSceneManagerPrivate::LoadMaterial(
    const msgs::Material &_msg)
{
  rendering::MaterialPtr material = this->scene->CreateMaterial();
  if (_msg.has_ambient())
  {
    material->SetAmbient(msgs::Convert(_msg.ambient()));
  }
  if (_msg.has_diffuse())
  {
    material->SetDiffuse(msgs::Convert(_msg.diffuse()));
  }
  if (_msg.has_specular())
  {
    material->SetDiffuse(msgs::Convert(_msg.specular()));
  }
  if (_msg.has_emissive())
  {
    material->SetEmissive(msgs::Convert(_msg.emissive()));
  }

  return material;
}

/////////////////////////////////////////////////
rendering::LightPtr TransportSceneManagerPrivate::LoadLight(
    const msgs::Light &_msg)
{
  rendering::LightPtr light;

  switch (_msg.type())
  {
    case msgs::Light_LightType_POINT:
      light = this->scene->CreatePointLight();
      break;
    case msgs::Light_LightType_SPOT:
    {
      light = this->scene->CreateSpotLight();
      rendering::SpotLightPtr spotLight =
          std::dynamic_pointer_cast<rendering::SpotLight>(light);
      spotLight->SetInnerAngle(_msg.spot_inner_angle());
      spotLight->SetOuterAngle(_msg.spot_outer_angle());
      spotLight->SetFalloff(_msg.spot_falloff());
      break;
    }
    case msgs::Light_LightType_DIRECTIONAL:
    {
      light = this->scene->CreateDirectionalLight();
      rendering::DirectionalLightPtr dirLight =
          std::dynamic_pointer_cast<rendering::DirectionalLight>(light);

      if (_msg.has_direction())
        dirLight->SetDirection(msgs::Convert(_msg.direction()));
      break;
    }
    default:
      ignerr << "Light type not supported" << std::endl;
      return light;
  }

  if (_msg.has_pose())
    light->SetLocalPose(msgs::Convert(_msg.pose()));

  if (_msg.has_diffuse())
    light->SetDiffuseColor(msgs::Convert(_msg.diffuse()));

  if (_msg.has_specular())
    light->SetSpecularColor(msgs::Convert(_msg.specular()));

  light->SetAttenuationConstant(_msg.attenuation_constant());
  light->SetAttenuationLinear(_msg.attenuation_linear());
  light->SetAttenuationQuadratic(_msg.attenuation_quadratic());
  light->SetAttenuationRange(_msg.range());

  light->SetCastShadows(_msg.cast_shadows());

  this->lights[_msg.id()] = light;
  return light;
}

/////////////////////////////////////////////////
void TransportSceneManagerPrivate::DeleteEntity(const unsigned int _entity)
{
  if (this->visuals.find(_entity) != this->visuals.end())
  {
    auto visual = this->visuals[_entity].lock();
    if (visual)
    {
      this->scene->DestroyVisual(visual, true);
    }
    this->visuals.erase(_entity);
  }
  else if (this->lights.find(_entity) != this->lights.end())
  {
    auto light = this->lights[_entity].lock();
    if (light)
    {
      this->scene->DestroyLight(light, true);
    }
    this->lights.erase(_entity);
  }
}

// Register this plugin
IGNITION_ADD_PLUGIN(ignition::gui::plugins::TransportSceneManager,
                    ignition::gui::Plugin)

