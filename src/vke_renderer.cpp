#include "vke_renderer.hpp"
#include <GLFW/glfw3.h>
#include <iostream>


// std
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <array>

namespace vke {

    VkeRenderer::VkeRenderer(VkeWindow &window, VkeDevice &device) :
        vke_window(window),
        vke_device(device)
    {
        recreate_swap_chain();
        create_command_buffers();
    }

    VkeRenderer::~VkeRenderer() {
        free_command_buffers();
    }

    void VkeRenderer::create_command_buffers() {
        // one command buffer per frame buffer
        command_buffer.resize(VkeSwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = vke_device.getCommandPool();
        alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffer.size());

        if (vkAllocateCommandBuffers(vke_device.device(), &alloc_info, command_buffer.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffer");
        }
  
    }

    void VkeRenderer::free_command_buffers() {
        vkFreeCommandBuffers(vke_device.device(), vke_device.getCommandPool(), static_cast<uint32_t>(command_buffer.size()), command_buffer.data());
        command_buffer.clear();
    }

    void VkeRenderer::recreate_swap_chain() {
        auto extent = vke_window.getExtent();
        while(extent.width == 0 || extent.height == 0) {
            extent = vke_window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vke_device.device());

        if (vke_swap_chain == nullptr) {
            vke_swap_chain = std::make_unique<VkeSwapChain>(vke_device, extent);
        } else {
            std::shared_ptr<VkeSwapChain> old_swap_chain = std::move(vke_swap_chain);
            vke_swap_chain = std::make_unique<VkeSwapChain>(vke_device, extent, std::move(vke_swap_chain));
            
            if(!old_swap_chain -> compareSwapChainFormats(*vke_swap_chain.get())) {
                throw std::runtime_error("swap chain image format has changed");
            }        
        }
    }

    VkCommandBuffer VkeRenderer::begin_frame() { 
        assert(!is_frame_started && "Cannot call begin_frame because it is already in progress");
        
        auto result = vke_swap_chain -> acquireNextImage(&current_image_index);

        if(result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreate_swap_chain();
            return nullptr;
        }
        
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to aquire swap chain image");
        }

        is_frame_started = true;

        auto command_buffer = get_current_command_buffer();

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType  =VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        return command_buffer;
    }

    void VkeRenderer::end_frame() { 
        assert(is_frame_started && "Cannot end frame because there is no frame started");
        auto command_buffer = get_current_command_buffer();

        if(vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }

        auto result = vke_swap_chain -> submitCommandBuffers(&command_buffer, &current_image_index);
        
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vke_window.was_window_resized()) {
            vke_window.reset_window_resized_flag();
            recreate_swap_chain();
        }

        if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image");
        }

        is_frame_started = false;
        current_frame_index = (current_frame_index + 1) % VkeSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void VkeRenderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer) {
        assert(is_frame_started && "Cannot begin swap_chain_render_pass if frame hasn't started");
        assert(command_buffer == get_current_command_buffer() && "Cannot begin render pass on command buffer from different frame");

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = vke_swap_chain -> getRenderPass();
        render_pass_info.framebuffer = vke_swap_chain -> getFrameBuffer(current_image_index);

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = vke_swap_chain -> getSwapChainExtent();

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clear_values[1].depthStencil = {1.0f, 0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vke_swap_chain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vke_swap_chain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, vke_swap_chain->getSwapChainExtent()};
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    }

    void VkeRenderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer) {
        assert(is_frame_started && "Cannot begin end_chain_render_pass if frame hasn't started");
        assert(command_buffer == get_current_command_buffer() && "Cannot end render pass on command buffer from different frame");

        vkCmdEndRenderPass(command_buffer);
    }
}
