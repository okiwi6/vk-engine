#ifndef vke_buffer_
    #define vke_buffer_

#include "vke_device.hpp"
#include <vulkan/vulkan_core.h>

namespace vke {

class VkeBuffer {
    public:
    VkeBuffer(
        VkeDevice& device,
        VkDeviceSize instance_size,
        uint32_t instance_count,
        VkBufferUsageFlags usage_flags,
        VkMemoryPropertyFlags memory_property_flags,
        VkDeviceSize min_offset_alignment = 1
    );
    ~VkeBuffer();

    VkeBuffer(const VkeBuffer&) = delete;
    VkeBuffer& operator=(const VkeBuffer&) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void write_to_buffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    
    void write_to_index(void* data, int index);
    VkResult flush_index(int index);
    VkDescriptorBufferInfo descriptor_info_for_index(int index);
    VkResult invalidate_index(int index);
    
    VkBuffer get_buffer() const { return buffer; }
    void* get_mapped_memory() const { return mapped; }
    uint32_t get_instance_count() const { return instance_count; }
    VkDeviceSize get_instance_size() const { return instance_size; }
    VkDeviceSize get_alignment_size() const { return instance_size; }
    VkBufferUsageFlags get_usage_flags() const { return usage_flags; }
    VkMemoryPropertyFlags get_memory_property_flags() const { return memory_property_flags; }
    VkDeviceSize get_buffer_size() const { return buffer_size; }
    
    private:
    static VkDeviceSize get_alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);
    
    VkeDevice& vke_device;
    void* mapped = nullptr;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    
    VkDeviceSize buffer_size;
    uint32_t instance_count;
    VkDeviceSize instance_size;
    VkDeviceSize alignment_size;
    VkBufferUsageFlags usage_flags;
    VkMemoryPropertyFlags memory_property_flags;
};
}

#endif
