#include "vke_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace vke {

// *************** Descriptor Set Layout Builder *********************

VkeDescriptorSetLayout::Builder &VkeDescriptorSetLayout::Builder::add_binding(
    uint32_t binding,
    VkDescriptorType descriptor_type,
    VkShaderStageFlags stage_flags,
    uint32_t count) 
{
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorType = descriptor_type;
    layout_binding.descriptorCount = count;
    layout_binding.stageFlags = stage_flags;
    bindings[binding] = layout_binding;
    
    return *this;
}

std::unique_ptr<VkeDescriptorSetLayout> VkeDescriptorSetLayout::Builder::build() const {
  return std::make_unique<VkeDescriptorSetLayout>(vke_device, bindings);
}

// *************** Descriptor Set Layout *********************

VkeDescriptorSetLayout::VkeDescriptorSetLayout(
    VkeDevice &vke_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : vke_device{vke_device}, bindings{bindings} {
  std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
  for (auto kv : bindings) {
    set_layout_bindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
  descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
  descriptor_set_layout_info.pBindings = set_layout_bindings.data();

  if (vkCreateDescriptorSetLayout(
          vke_device.device(),
          &descriptor_set_layout_info,
          nullptr,
          &descriptor_set_layout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

VkeDescriptorSetLayout::~VkeDescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(vke_device.device(), descriptor_set_layout, nullptr);
}

// *************** Descriptor Pool Builder *********************

VkeDescriptorPool::Builder &VkeDescriptorPool::Builder::add_pool_size(
    VkDescriptorType descriptor_type, uint32_t count) {
  pool_sizes.push_back({descriptor_type, count});
  return *this;
}

VkeDescriptorPool::Builder &VkeDescriptorPool::Builder::set_pool_flags(
    VkDescriptorPoolCreateFlags flags) {
  pool_flags = flags;
  return *this;
}
VkeDescriptorPool::Builder &VkeDescriptorPool::Builder::set_max_sets(uint32_t count) {
  max_sets = count;
  return *this;
}

std::unique_ptr<VkeDescriptorPool> VkeDescriptorPool::Builder::build() const {
  return std::make_unique<VkeDescriptorPool>(vke_device, max_sets, pool_flags, pool_sizes);
}

// *************** Descriptor Pool *********************

VkeDescriptorPool::VkeDescriptorPool(
    VkeDevice &vke_device,
    uint32_t max_sets,
    VkDescriptorPoolCreateFlags pool_flags,
    const std::vector<VkDescriptorPoolSize> &pool_sizes)
    : vke_device{vke_device} {
  VkDescriptorPoolCreateInfo descriptor_pool_info{};
  descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  descriptor_pool_info.pPoolSizes = pool_sizes.data();
  descriptor_pool_info.maxSets = max_sets;
  descriptor_pool_info.flags = pool_flags;

  if (vkCreateDescriptorPool(vke_device.device(), &descriptor_pool_info, nullptr, &descriptor_pool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

VkeDescriptorPool::~VkeDescriptorPool() {
  vkDestroyDescriptorPool(vke_device.device(), descriptor_pool, nullptr);
}

bool VkeDescriptorPool::allocate_descriptor(
    const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet &descriptor) const {
  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = descriptor_pool;
  alloc_info.pSetLayouts = &descriptor_set_layout;
  alloc_info.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
  // a new pool whenever an old pool fills up. But this is beyond our current scope
  if (vkAllocateDescriptorSets(vke_device.device(), &alloc_info, &descriptor) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void VkeDescriptorPool::free_descriptors(std::vector<VkDescriptorSet> &descriptors) const {
  vkFreeDescriptorSets(
      vke_device.device(),
      descriptor_pool,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
}

void VkeDescriptorPool::reset_pool() {
  vkResetDescriptorPool(vke_device.device(), descriptor_pool, 0);
}

// *************** Descriptor Writer *********************

VkeDescriptorWriter::VkeDescriptorWriter(VkeDescriptorSetLayout &set_layout, VkeDescriptorPool &pool)
    : set_layout{set_layout}, pool{pool} {}

VkeDescriptorWriter &VkeDescriptorWriter::write_buffer(
    uint32_t binding, VkDescriptorBufferInfo *buffer_info) {
  assert(set_layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &binding_description = set_layout.bindings[binding];

  assert(
      binding_description.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = binding_description.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = buffer_info;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

VkeDescriptorWriter &VkeDescriptorWriter::write_image(
    uint32_t binding, VkDescriptorImageInfo *image_info) {
  assert(set_layout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &binding_description = set_layout.bindings[binding];

  assert(
      binding_description.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = binding_description.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = image_info;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

bool VkeDescriptorWriter::build(VkDescriptorSet &set) {
  bool success = pool.allocate_descriptor(set_layout.get_descriptor_set_layout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void VkeDescriptorWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(pool.vke_device.device(), writes.size(), writes.data(), 0, nullptr);
}

}

