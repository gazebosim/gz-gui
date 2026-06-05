/*
 * Copyright (C) 2026 Open Source Robotics Foundation
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

// Regression test pinning down what works (and what doesn't) in
// QSGSimpleTextureNode + QSGVulkanTexture::fromNative -- the path used by
// MinimalScene's Vulkan RHI bridge (MinimalSceneRhiVulkan).
//
// Context: a grey-viewport failure traced during zero-copy bringup (May 2026)
// to this rendering path. Cross-device sampling of the imported image is
// correct (a compute-shader texelFetch on Qt's VkDevice reads the producer's
// pixels byte-identically), yet QSGSimpleTextureNode's draw of the
// QSGVulkanTexture::fromNative-wrapped image renders uniform on the window.
// This file isolates the variables.
//
// Test 1 (FromNativeRendersLinearPattern) -- PASSES today:
//   The simplest possible fromNative case. A LINEAR-tiling, host-visible
//   VkImage on Qt's OWN VkDevice, host-written with a four-quadrant pattern.
//   Strips away every "is the producer broken?" / cross-device / OPTIMAL
//   tiling variable. PASS locks in the empirical finding that the *simple*
//   fromNative + QSGSimpleTextureNode path is correct.
//
// Test 2 (FromNativeRendersOptimalPattern) -- PASSES today:
//   OPTIMAL tiling on Qt's own VkDevice, COLOR_ATTACHMENT | SAMPLED |
//   TRANSFER_DST usage, populated via a staging buffer +
//   vkCmdCopyBufferToImage on Qt's own queue, then transitioned to
//   SHADER_READ_ONLY_OPTIMAL. Matches the usage flags of a cross-device-exported
//   image but on a single device. The PASS isolates the production bug to the
//   *cross-device* dimension: OPTIMAL+COLOR_ATTACHMENT+fromNative is fine
//   on its own; only when the VkImage was created on (and exported by) a
//   SECOND VkDevice does the QSGSimpleTextureNode draw render uniform.
//
// Test 3 (cross-device, FromNativeRendersImportedFdPattern) -- FOLLOW-UP,
// not yet implemented:
//   The decisive bug-reproducer. Stand up a second private VkInstance/
//   VkDevice, create an OPTIMAL R8G8B8A8_UNORM VkImage with
//   VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT, a dedicated allocation,
//   populate it via a staging upload there, vkGetMemoryFdKHR, then import
//   that FD onto Qt's VkDevice with VkMemoryDedicatedAllocateInfo, wrap via
//   fromNative, and assert the pattern. Expected: FAIL today -- this is the
//   focused harness against which to debug QSGSimpleTextureNode.
//   The cross-device import helpers can be inlined here when needed.

#include <gtest/gtest.h>

#include <QtGlobal>
// qtgui-config.h defines QT_FEATURE_vulkan, which QT_CONFIG(vulkan) consults.
// Without it the QT_CONFIG macro divides by an undefined token below.
#include <QtGui/qtguiglobal.h>

#include <gz/utils/ExtraTestMacros.hh>

// Compiles on every platform. The test body is guarded by QT_CONFIG(vulkan) so
// builds where Qt was configured without Vulkan support (or with a Qt < 6.0
// that lacks the QSGVulkanTexture native interface) emit a GTEST_SKIP() rather
// than a build failure. Mirrors MinimalScene's GZ_GUI_HAVE_VULKAN gating
// (src/plugins/minimal_scene/MinimalSceneRhi.hh).
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_CONFIG(vulkan)
#define GZ_GUI_TEST_HAVE_VULKAN 1
#else
#define GZ_GUI_TEST_HAVE_VULKAN 0
#endif

#if GZ_GUI_TEST_HAVE_VULKAN
// Vulkan MUST be included before any Qt header. Qt's Vulkan integration
// defines VK_NO_PROTOTYPES (it dispatches through QVulkanFunctions); if Qt
// headers come first, every later <vulkan/vulkan.h> sees prototypes off and
// our direct vkCreateImage / vkAllocateMemory / vkMapMemory calls fail to
// resolve. Including Vulkan first means we get real prototypes, and Qt's
// subsequent VK_NO_PROTOTYPES only affects its own internal dispatch. Linking
// against the system Vulkan loader (Vulkan::Vulkan in test/regression/
// CMakeLists.txt) resolves the prototype symbols at link time.
#include <vulkan/vulkan.h>

#include <QGuiApplication>
#include <QImage>
#include <QQuickItem>
#include <QQuickView>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QSize>
#include <QtTest/QtTest>

#include <cstdlib>  // std::abs for the pattern-colour tolerance check
#include <set>
#include <utility>
#include <vector>
#endif  // GZ_GUI_TEST_HAVE_VULKAN

#if GZ_GUI_TEST_HAVE_VULKAN
namespace
{
// Pattern dimensions. Small to keep the test fast; the test checks unique-colour
// count, not exact pixel positions, so resolution-independence is preserved.
constexpr int kPatternW = 64;
constexpr int kPatternH = 64;

// Four-quadrant solid pattern: red / green / blue / yellow. Choosing four
// highly distinct primaries (no two within a small RGB distance of each other)
// so the "did the texture content arrive on screen?" assertion below is
// unambiguous.
//
// IMPORTANT: these are stored as little-endian uint32_t, so the byte layout
// in memory (for a R8G8B8A8_UNORM image) is reversed from the literal: the
// constant 0xAABBGGRR ends up as bytes [RR, GG, BB, AA] = [R, G, B, A]. So
// the literal for "R=0xFF, G=0x00, B=0x00, A=0xFF" (red, opaque) is
// 0xFF0000FF. A first version of this test mistakenly used the literal in
// big-endian RGBA order, which on little-endian x86 wrote magenta/yellow
// where it wanted green/blue -- and the resulting test failure (faithfully
// showing the broken pattern!) was the experiment that *eliminated* the
// hypothesis "fromNative + QSGSimpleTextureNode is broken at the simplest
// level". Lesson recorded so the byte-order trap is visible to future readers.
constexpr uint32_t kPatternRGBA[4] = {
    0xFF0000FFu,  // red:    bytes [FF, 00, 00, FF]
    0xFF00FF00u,  // green:  bytes [00, FF, 00, FF]
    0xFFFF0000u,  // blue:   bytes [00, 00, FF, FF]
    0xFF00FFFFu,  // yellow: bytes [FF, FF, 00, FF]
};

uint32_t QuadrantColour(int _x, int _y)
{
  const int qx = (_x < kPatternW / 2) ? 0 : 1;
  const int qy = (_y < kPatternH / 2) ? 0 : 1;
  return kPatternRGBA[qy * 2 + qx];
}

// RAII holder for the Vulkan objects the test creates on Qt's QRhi device.
struct PatternImage
{
  VkInstance instance = VK_NULL_HANDLE;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;
  VkImage image = VK_NULL_HANDLE;
  VkDeviceMemory memory = VK_NULL_HANDLE;

  ~PatternImage()
  {
    if (this->device != VK_NULL_HANDLE)
    {
      if (this->image != VK_NULL_HANDLE)
        vkDestroyImage(this->device, this->image, nullptr);
      if (this->memory != VK_NULL_HANDLE)
        vkFreeMemory(this->device, this->memory, nullptr);
    }
  }
};

// Obtain Qt's QRhi Vulkan handles from a shown QQuickWindow. Mirrors what
// MinimalSceneRhiVulkan does to inject Qt's VkDevice into the engine -- if this
// returns false the harness, not the bug, is at fault.
bool GetQtVulkanHandles(QQuickWindow *_window, PatternImage *_out)
{
  QSGRendererInterface *rif = _window->rendererInterface();
  if (rif == nullptr ||
      rif->graphicsApi() != QSGRendererInterface::Vulkan)
  {
    return false;
  }
  _out->instance = *static_cast<VkInstance *>(rif->getResource(
      _window, QSGRendererInterface::VulkanInstanceResource));
  _out->physicalDevice = *static_cast<VkPhysicalDevice *>(rif->getResource(
      _window, QSGRendererInterface::PhysicalDeviceResource));
  _out->device = *static_cast<VkDevice *>(rif->getResource(
      _window, QSGRendererInterface::DeviceResource));
  return _out->instance != VK_NULL_HANDLE &&
      _out->physicalDevice != VK_NULL_HANDLE &&
      _out->device != VK_NULL_HANDLE;
}

// Create a LINEAR, host-visible, R8G8B8A8_UNORM VkImage and host-fill it with
// the four-quadrant pattern. Transitions to SHADER_READ_ONLY_OPTIMAL so
// QSGVulkanTexture::fromNative can wrap it directly. The simplest possible
// "the texture has content" setup: no cross-device, no tiling-swizzle, no FD
// import -- isolates the bug to Qt's draw of fromNative.
bool CreatePatternImage(PatternImage *_io)
{
  VkImageCreateInfo imgInfo{};
  imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imgInfo.imageType = VK_IMAGE_TYPE_2D;
  imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imgInfo.extent = {kPatternW, kPatternH, 1u};
  imgInfo.mipLevels = 1u;
  imgInfo.arrayLayers = 1u;
  imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imgInfo.tiling = VK_IMAGE_TILING_LINEAR;
  imgInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
  imgInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
  imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (vkCreateImage(_io->device, &imgInfo, nullptr, &_io->image) != VK_SUCCESS)
    return false;

  VkMemoryRequirements req{};
  vkGetImageMemoryRequirements(_io->device, _io->image, &req);
  VkPhysicalDeviceMemoryProperties mp{};
  vkGetPhysicalDeviceMemoryProperties(_io->physicalDevice, &mp);
  uint32_t typeIdx = UINT32_MAX;
  const VkMemoryPropertyFlags want =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  for (uint32_t i = 0u; i < mp.memoryTypeCount; ++i)
  {
    if ((req.memoryTypeBits & (1u << i)) &&
        (mp.memoryTypes[i].propertyFlags & want) == want)
    {
      typeIdx = i;
      break;
    }
  }
  if (typeIdx == UINT32_MAX)
    return false;

  VkMemoryAllocateInfo alloc{};
  alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc.allocationSize = req.size;
  alloc.memoryTypeIndex = typeIdx;
  if (vkAllocateMemory(_io->device, &alloc, nullptr, &_io->memory) != VK_SUCCESS)
    return false;
  if (vkBindImageMemory(_io->device, _io->image, _io->memory, 0u) != VK_SUCCESS)
    return false;

  // Fill via the LINEAR subresource layout (rowPitch may exceed width*4).
  VkImageSubresource sub{};
  sub.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  VkSubresourceLayout layout{};
  vkGetImageSubresourceLayout(_io->device, _io->image, &sub, &layout);
  void *mapped = nullptr;
  if (vkMapMemory(_io->device, _io->memory, 0u, req.size, 0u, &mapped)
      != VK_SUCCESS)
  {
    return false;
  }
  auto *base = static_cast<uint8_t *>(mapped) + layout.offset;
  for (int y = 0; y < kPatternH; ++y)
  {
    auto *row = reinterpret_cast<uint32_t *>(base + y * layout.rowPitch);
    for (int x = 0; x < kPatternW; ++x)
      row[x] = QuadrantColour(x, y);
  }
  vkUnmapMemory(_io->device, _io->memory);
  // PREINITIALIZED + HOST_COHERENT is observable by the sampler without a
  // pipeline barrier on LINEAR tiling per the Vulkan spec, so no transition
  // command-buffer is needed. fromNative is told the layout below.
  return true;
}

// Find a memory type that satisfies _typeBits + every flag in _wantProps.
int FindMemoryType(VkPhysicalDevice _phys, uint32_t _typeBits,
    VkMemoryPropertyFlags _wantProps)
{
  VkPhysicalDeviceMemoryProperties mp{};
  vkGetPhysicalDeviceMemoryProperties(_phys, &mp);
  for (uint32_t i = 0u; i < mp.memoryTypeCount; ++i)
  {
    if ((_typeBits & (1u << i)) &&
        (mp.memoryTypes[i].propertyFlags & _wantProps) == _wantProps)
      return static_cast<int>(i);
  }
  return -1;
}

// Create an OPTIMAL-tiling VkImage on Qt's device with the same usage flags
// an external-engine-exported image has (COLOR_ATTACHMENT | SAMPLED |
// TRANSFER_DST), populate it via a staging buffer + vkCmdCopyBufferToImage,
// then transition to SHADER_READ_ONLY_OPTIMAL. This matches a production
// engine-exported image's image/layout lifecycle (modulo cross-device sharing),
// so a failure here vs Test 1's LINEAR PASS isolates the bug to OPTIMAL
// tiling on its own, even without a second VkDevice in the picture.
//
// Uses Qt's own VkQueue (acquired via the QSGRendererInterface
// CommandQueueResource) for the staging upload submit; vkQueueWaitIdle
// before returning so Qt's render thread can resume safely.
bool CreateOptimalPatternImage(VkQueue _queue, uint32_t _queueFamily,
    PatternImage *_io)
{
  // 1) OPTIMAL image, COLOR_ATTACHMENT | SAMPLED | TRANSFER_DST usage.
  VkImageCreateInfo imgInfo{};
  imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imgInfo.imageType = VK_IMAGE_TYPE_2D;
  imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imgInfo.extent = {kPatternW, kPatternH, 1u};
  imgInfo.mipLevels = 1u;
  imgInfo.arrayLayers = 1u;
  imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imgInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if (vkCreateImage(_io->device, &imgInfo, nullptr, &_io->image) != VK_SUCCESS)
    return false;
  VkMemoryRequirements imgReq{};
  vkGetImageMemoryRequirements(_io->device, _io->image, &imgReq);
  const int imgType = FindMemoryType(_io->physicalDevice, imgReq.memoryTypeBits,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  if (imgType < 0)
    return false;
  VkMemoryAllocateInfo imgAlloc{};
  imgAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  imgAlloc.allocationSize = imgReq.size;
  imgAlloc.memoryTypeIndex = static_cast<uint32_t>(imgType);
  if (vkAllocateMemory(_io->device, &imgAlloc, nullptr, &_io->memory)
      != VK_SUCCESS)
    return false;
  vkBindImageMemory(_io->device, _io->image, _io->memory, 0u);

  // 2) Staging buffer (HOST_VISIBLE) carrying the pattern.
  const VkDeviceSize stagingSize =
      static_cast<VkDeviceSize>(kPatternW) * kPatternH * 4u;
  VkBufferCreateInfo bufInfo{};
  bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufInfo.size = stagingSize;
  bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VkBuffer staging = VK_NULL_HANDLE;
  if (vkCreateBuffer(_io->device, &bufInfo, nullptr, &staging) != VK_SUCCESS)
    return false;
  VkMemoryRequirements bufReq{};
  vkGetBufferMemoryRequirements(_io->device, staging, &bufReq);
  const int bufType = FindMemoryType(_io->physicalDevice, bufReq.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  if (bufType < 0)
  {
    vkDestroyBuffer(_io->device, staging, nullptr);
    return false;
  }
  VkDeviceMemory stagingMem = VK_NULL_HANDLE;
  VkMemoryAllocateInfo bufAlloc{};
  bufAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  bufAlloc.allocationSize = bufReq.size;
  bufAlloc.memoryTypeIndex = static_cast<uint32_t>(bufType);
  if (vkAllocateMemory(_io->device, &bufAlloc, nullptr, &stagingMem)
      != VK_SUCCESS)
  {
    vkDestroyBuffer(_io->device, staging, nullptr);
    return false;
  }
  vkBindBufferMemory(_io->device, staging, stagingMem, 0u);
  void *mapped = nullptr;
  vkMapMemory(_io->device, stagingMem, 0u, stagingSize, 0u, &mapped);
  auto *base = static_cast<uint32_t *>(mapped);
  for (int y = 0; y < kPatternH; ++y)
    for (int x = 0; x < kPatternW; ++x)
      base[y * kPatternW + x] = QuadrantColour(x, y);
  vkUnmapMemory(_io->device, stagingMem);

  // 3) One-shot command buffer: UNDEFINED -> TRANSFER_DST, copy, TRANSFER_DST
  //    -> SHADER_READ_ONLY_OPTIMAL. Submit + vkQueueWaitIdle.
  VkCommandPool pool = VK_NULL_HANDLE;
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = _queueFamily;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
  vkCreateCommandPool(_io->device, &poolInfo, nullptr, &pool);
  VkCommandBufferAllocateInfo cbAlloc{};
  cbAlloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cbAlloc.commandPool = pool;
  cbAlloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cbAlloc.commandBufferCount = 1u;
  VkCommandBuffer cmd = VK_NULL_HANDLE;
  vkAllocateCommandBuffers(_io->device, &cbAlloc, &cmd);
  VkCommandBufferBeginInfo begin{};
  begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(cmd, &begin);

  auto Barrier = [&](VkImageLayout _old, VkImageLayout _new,
      VkAccessFlags _srcA, VkAccessFlags _dstA,
      VkPipelineStageFlags _srcS, VkPipelineStageFlags _dstS)
  {
    VkImageMemoryBarrier b{};
    b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    b.srcAccessMask = _srcA;
    b.dstAccessMask = _dstA;
    b.oldLayout = _old;
    b.newLayout = _new;
    b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    b.image = _io->image;
    b.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u};
    vkCmdPipelineBarrier(cmd, _srcS, _dstS, 0u, 0u, nullptr, 0u, nullptr,
        1u, &b);
  };
  Barrier(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      0u, VK_ACCESS_TRANSFER_WRITE_BIT,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
  VkBufferImageCopy region{};
  region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0u, 0u, 1u};
  region.imageExtent = {kPatternW, kPatternH, 1u};
  vkCmdCopyBufferToImage(cmd, staging, _io->image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1u, &region);
  Barrier(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
      VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.commandBufferCount = 1u;
  submit.pCommandBuffers = &cmd;
  vkQueueSubmit(_queue, 1u, &submit, VK_NULL_HANDLE);
  vkQueueWaitIdle(_queue);

  vkDestroyCommandPool(_io->device, pool, nullptr);
  vkDestroyBuffer(_io->device, staging, nullptr);
  vkFreeMemory(_io->device, stagingMem, nullptr);
  return true;
}

// Decisive pattern check used by both LINEAR and OPTIMAL tests: each of the
// four quadrants of the rendered framebuffer must carry its expected pattern
// colour. A weaker "any non-uniform output" assertion would false-PASS on
// Qt's window chrome.
void AssertRenderedPattern(const QImage &_rendered, const char *_caseLabel)
{
  ASSERT_FALSE(_rendered.isNull()) << "grabWindow returned a null QImage";
  ASSERT_GE(_rendered.width(), kPatternW);
  ASSERT_GE(_rendered.height(), kPatternH);
  auto NearMatch = [](QRgb _got, uint32_t _wantRGBA, int _tol) -> bool {
    const int r = qRed(_got), g = qGreen(_got), b = qBlue(_got);
    const int wr = static_cast<int>(_wantRGBA & 0xFFu);
    const int wg = static_cast<int>((_wantRGBA >> 8) & 0xFFu);
    const int wb = static_cast<int>((_wantRGBA >> 16) & 0xFFu);
    return std::abs(r - wr) <= _tol && std::abs(g - wg) <= _tol &&
        std::abs(b - wb) <= _tol;
  };
  const int w = _rendered.width();
  const int h = _rendered.height();
  struct Probe { int x; int y; uint32_t want; const char *name; };
  const Probe probes[4] = {
      {w / 4,     h / 4,     kPatternRGBA[0], "red (top-left)"},
      {3 * w / 4, h / 4,     kPatternRGBA[1], "green (top-right)"},
      {w / 4,     3 * h / 4, kPatternRGBA[2], "blue (bottom-left)"},
      {3 * w / 4, 3 * h / 4, kPatternRGBA[3], "yellow (bottom-right)"},
  };
  for (const Probe &p : probes)
  {
    const QRgb got = _rendered.pixel(p.x, p.y);
    const uint32_t wantR = p.want & 0xFFu;
    const uint32_t wantG = (p.want >> 8) & 0xFFu;
    const uint32_t wantB = (p.want >> 16) & 0xFFu;
    const uint32_t wantRgb = (wantR << 16) | (wantG << 8) | wantB;
    EXPECT_TRUE(NearMatch(got, p.want, 16))
        << _caseLabel << ": QSGSimpleTextureNode + fromNative did not render "
           "the expected pattern colour at " << p.name
        << " (" << p.x << "," << p.y << "): got 0x"
        << QString::number(got, 16).toStdString() << " want 0x"
        << QString::number(wantRgb, 16).toStdString()
        << " (ARGB).";
  }
}

// QQuickItem mirroring MinimalSceneRhiVulkan's role: on updatePaintNode it
// creates a QSGSimpleTextureNode wrapping the externally-owned VkImage via
// QSGVulkanTexture::fromNative. This is the EXACT path under test.
class PatternItem : public QQuickItem
{
 public:
  PatternItem(QQuickItem *_parent, VkImage _image, QSize _size)
      : QQuickItem(_parent), image(_image), size(_size)
  {
    this->setFlag(ItemHasContents);
  }

  QSGNode *updatePaintNode(QSGNode *_old,
      QQuickItem::UpdatePaintNodeData *) override
  {
    auto *node = static_cast<QSGSimpleTextureNode *>(_old);
    if (node == nullptr)
    {
      node = new QSGSimpleTextureNode();
      // This is the call path being debugged. fromNative is documented to wrap
      // an externally-created VkImage in its current layout; the consumer
      // (this node) does not transition it.
      // https://doc.qt.io/qt-6/qsgvulkantexture.html
      QSGTexture *tex =
          QNativeInterface::QSGVulkanTexture::fromNative(this->image,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
              this->window(), this->size);
      node->setTexture(tex);
      node->setOwnsTexture(true);
    }
    node->setRect(this->boundingRect());
    return node;
  }

 private:
  VkImage image{VK_NULL_HANDLE};
  QSize size;
};
}  // namespace

/////////////////////////////////////////////////
TEST(QsgSimpleTextureNodeVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(FromNativeRendersLinearPattern))
{
  // Force Qt to use the Vulkan RHI BEFORE the QGuiApplication is constructed
  // (the same setup MinimalScene does when GZ_GUI_RENDER_ENGINE_GUI_API_BACKEND
  // == "vulkan"). Without this Qt picks the platform default (GL on Linux).
  QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

  int argc = 1;
  static char argv0[] = "qsg_simple_texture_node_vulkan";
  static char *argv[] = {argv0, nullptr};
  QGuiApplication app(argc, argv);

  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);
  view.resize(kPatternW, kPatternH);

  // Show + spin the event loop so QRhi initialises its VkDevice. Without this,
  // rendererInterface()->getResource(...) returns nulls.
  view.show();
  QTest::qWaitForWindowExposed(&view);
  QCoreApplication::processEvents();

  PatternImage pi;
  ASSERT_TRUE(GetQtVulkanHandles(&view, &pi))
      << "could not get Qt's QRhi VkDevice -- did Qt actually pick the Vulkan "
         "RHI? Set QSG_INFO=1 to see Qt's chosen backend.";
  ASSERT_TRUE(CreatePatternImage(&pi))
      << "could not create the LINEAR host-visible pattern VkImage";

  // Parent a PatternItem under the view's content item. We don't load any QML
  // source; the test's QQuickItem is the only content.
  auto *parent = view.contentItem();
  ASSERT_NE(parent, nullptr);
  auto *item = new PatternItem(parent, pi.image, QSize(kPatternW, kPatternH));
  item->setSize(QSizeF(kPatternW, kPatternH));
  item->setPosition(QPointF(0.0, 0.0));

  // Trigger a redraw, then grab. grabWindow() blocks the GUI thread until the
  // window is rendered; returns the QImage of the rendered framebuffer.
  view.update();
  QImage rendered = view.grabWindow();
  AssertRenderedPattern(rendered, "LINEAR/host-visible");
  view.close();
}

/////////////////////////////////////////////////
// Closer-to-production reproducer: same pattern, but the VkImage is OPTIMAL
// tiling with COLOR_ATTACHMENT | SAMPLED | TRANSFER_DST usage (matching an
// engine-exported image's flags) and is populated via a staging buffer +
// vkCmdCopyBufferToImage + layout transition to SHADER_READ_ONLY_OPTIMAL on
// Qt's own VkDevice/VkQueue. The only variable removed vs production is the
// cross-device FD import. If this test passes while the production cross-device
// path still renders uniform, the bug requires the cross-device dimension; if
// it fails, we have a single-device reproducer of the QSGSimpleTextureNode bug.
TEST(QsgSimpleTextureNodeVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(FromNativeRendersOptimalPattern))
{
  QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
  int argc = 1;
  static char argv0[] = "qsg_simple_texture_node_vulkan_optimal";
  static char *argv[] = {argv0, nullptr};
  QGuiApplication app(argc, argv);

  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);
  view.resize(kPatternW, kPatternH);
  view.show();
  QTest::qWaitForWindowExposed(&view);
  QCoreApplication::processEvents();

  PatternImage pi;
  ASSERT_TRUE(GetQtVulkanHandles(&view, &pi));

  // Acquire Qt's main queue. CommandQueueResource is a VkQueue* in Qt 6's
  // QSGRendererInterface. The queue family index isn't exposed as a resource
  // in the Qt version targeted here, so discover it: the first queue family
  // with VK_QUEUE_GRAPHICS_BIT is the one Qt asks QRhi for, and is the one
  // CommandQueueResource was vkGetDeviceQueue()'d from. (Verified in QSG_INFO
  // logs of this RHI: queue family 0 flags=0xf -- graphics+compute+transfer.)
  QSGRendererInterface *rif = view.rendererInterface();
  VkQueue queue = *static_cast<VkQueue *>(rif->getResource(&view,
      QSGRendererInterface::CommandQueueResource));
  ASSERT_NE(queue, VK_NULL_HANDLE)
      << "could not get Qt's CommandQueueResource";
  uint32_t qfCount = 0u;
  vkGetPhysicalDeviceQueueFamilyProperties(pi.physicalDevice, &qfCount,
      nullptr);
  std::vector<VkQueueFamilyProperties> qfs(qfCount);
  vkGetPhysicalDeviceQueueFamilyProperties(pi.physicalDevice, &qfCount,
      qfs.data());
  uint32_t queueFamily = UINT32_MAX;
  for (uint32_t i = 0u; i < qfCount; ++i)
  {
    if (qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      queueFamily = i;
      break;
    }
  }
  ASSERT_NE(queueFamily, UINT32_MAX)
      << "no graphics queue family on Qt's physical device";

  ASSERT_TRUE(CreateOptimalPatternImage(queue, queueFamily, &pi))
      << "could not create + populate the OPTIMAL VkImage";

  auto *parent = view.contentItem();
  ASSERT_NE(parent, nullptr);
  auto *item = new PatternItem(parent, pi.image, QSize(kPatternW, kPatternH));
  item->setSize(QSizeF(kPatternW, kPatternH));
  item->setPosition(QPointF(0.0, 0.0));

  view.update();
  QImage rendered = view.grabWindow();
  AssertRenderedPattern(rendered, "OPTIMAL/staging-uploaded");
  view.close();
}
#else  // GZ_GUI_TEST_HAVE_VULKAN
TEST(QsgSimpleTextureNodeVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(FromNativeRendersLinearPattern))
{
  GTEST_SKIP() << "Qt was built without Vulkan support; "
                  "QSGVulkanTexture::fromNative is not available.";
}
TEST(QsgSimpleTextureNodeVulkan,
    GZ_UTILS_TEST_ENABLED_ONLY_ON_LINUX(FromNativeRendersOptimalPattern))
{
  GTEST_SKIP() << "Qt was built without Vulkan support; "
                  "QSGVulkanTexture::fromNative is not available.";
}
#endif  // GZ_GUI_TEST_HAVE_VULKAN
