#include <cassert>
#include <cstring>
 
#include "vke_buffer.hpp"

namespace vke {
 
/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
VkDeviceSize VkeBuffer::get_alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment) {
  if (min_offset_alignment > 0) {
    return (instance_size + min_offset_alignment - 1) & ~(min_offset_alignment - 1);
  }
  return instance_size;
}
 
VkeBuffer::VkeBuffer(
    VkeDevice &device,
    VkDeviceSize instance_size,
    uint32_t instance_count,
    VkBufferUsageFlags usage_flags,
    VkMemoryPropertyFlags memory_property_flags,
    VkDeviceSize min_offset_alignment)
    : vke_device{device},
      instance_size{instance_size},
      instance_count{instance_count},
      usage_flags{usage_flags},
      memory_property_flags{memory_property_flags} 
{
  alignment_size = get_alignment(instance_size, min_offset_alignment);
  buffer_size = alignment_size * instance_count;
  device.createBuffer(buffer_size, usage_flags, memory_property_flags, buffer, memory);
}
 
VkeBuffer::~VkeBuffer() {
  unmap();
  vkDestroyBuffer(vke_device.device(), buffer, nullptr);
  vkFreeMemory(vke_device.device(), memory, nullptr);
}


VkResult VkeBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
  assert(buffer && memory && "Called map on buffer before create");
  return vkMapMemory(vke_device.device(), memory, offset, size, 0, &mapped);
}
 

void VkeBuffer::unmap() {
  if (mapped) {
    vkUnmapMemory(vke_device.device(), memory);
    mapped = nullptr;
  }
}
 

void VkeBuffer::write_to_buffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
  assert(mapped && "Cannot copy to unmapped buffer");
 
  if (size == VK_WHOLE_SIZE) {
    memcpy(mapped, data, buffer_size);
  } else {
    char *mem_offset = (char *)mapped;
    mem_offset += offset;
    memcpy(mem_offset, data, size);
  }
}

VkResult VkeBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mapped_range = {};
  mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mapped_range.memory = memory;
  mapped_range.offset = offset;
  mapped_range.size = size;
  return vkFlushMappedMemoryRanges(vke_device.device(), 1, &mapped_range);
}

VkResult VkeBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
  VkMappedMemoryRange mapped_range = {};
  mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  mapped_range.memory = memory;
  mapped_range.offset = offset;
  mapped_range.size = size;
  return vkInvalidateMappedMemoryRanges(vke_device.device(), 1, &mapped_range);
}

VkDescriptorBufferInfo VkeBuffer::descriptor_info(VkDeviceSize size, VkDeviceSize offset) {
  return VkDescriptorBufferInfo{
      buffer,
      offset,
      size,
  };
}
 
void VkeBuffer::write_to_index(void *data, int index) {
  write_to_buffer(data, instance_size, index * alignment_size);
}
 

VkResult VkeBuffer::flush_index(int index) { return flush(alignment_size, index * alignment_size); }
 

VkDescriptorBufferInfo VkeBuffer::descriptor_info_for_index(int index) {
  return descriptor_info(alignment_size, index * alignment_size);
}
 

VkResult VkeBuffer::invalidate_index(int index) {
  return invalidate(alignment_size, index * alignment_size);
}
 
}
