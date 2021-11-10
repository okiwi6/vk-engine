#pragma once

#include "vke_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vke {

class VkeDescriptorSetLayout {
    public:
        class Builder {
            public:
                Builder(VkeDevice &vke_device) : vke_device{vke_device} {}

    Builder &add_binding(
        uint32_t binding,
        VkDescriptorType descriptor_type,
        VkShaderStageFlags stage_flags,
        uint32_t count = 1);
    std::unique_ptr<VkeDescriptorSetLayout> build() const;

   private:
    VkeDevice &vke_device;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  VkeDescriptorSetLayout(
      VkeDevice &vke_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~VkeDescriptorSetLayout();
  VkeDescriptorSetLayout(const VkeDescriptorSetLayout &) = delete;
  VkeDescriptorSetLayout &operator=(const VkeDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout get_descriptor_set_layout() const { return descriptor_set_layout; }

 private:
  VkeDevice &vke_device;
  VkDescriptorSetLayout descriptor_set_layout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class VkeDescriptorWriter;
};

class VkeDescriptorPool {
 public:
  class Builder {
    public:
        Builder(VkeDevice &vke_device) : vke_device{vke_device} {}

        Builder &add_pool_size(VkDescriptorType descriptor_type, uint32_t count);
        Builder &set_pool_flags(VkDescriptorPoolCreateFlags flags);
        Builder &set_max_sets(uint32_t count);
        std::unique_ptr<VkeDescriptorPool> build() const;

    private:
        VkeDevice &vke_device;
        std::vector<VkDescriptorPoolSize> pool_sizes{};
        uint32_t max_sets = 1000;
        VkDescriptorPoolCreateFlags pool_flags = 0;
  };

  VkeDescriptorPool(
      VkeDevice &vke_device,
      uint32_t max_sets,
      VkDescriptorPoolCreateFlags pool_flags,
      const std::vector<VkDescriptorPoolSize> &pool_sizes);
  ~VkeDescriptorPool();
  VkeDescriptorPool(const VkeDescriptorPool &) = delete;
  VkeDescriptorPool &operator=(const VkeDescriptorPool &) = delete;

  bool allocate_descriptor(
      const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet &descriptor) const;

  void free_descriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void reset_pool();

 private:
  VkeDevice &vke_device;
  VkDescriptorPool descriptor_pool;

  friend class VkeDescriptorWriter;
};

class VkeDescriptorWriter {
 public:
  VkeDescriptorWriter(VkeDescriptorSetLayout &set_layout, VkeDescriptorPool &pool);

  VkeDescriptorWriter &write_buffer(uint32_t binding, VkDescriptorBufferInfo *buffer_info);
  VkeDescriptorWriter &write_image(uint32_t binding, VkDescriptorImageInfo *image_info);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  VkeDescriptorSetLayout &set_layout;
  VkeDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}