#!/bin/sh

GLSLC_PATH="/home/ole/Documents/Programming/vulkan/1.2.176.1/x86_64/bin/glslc"
echo "using glslc at $GLSLC_PATH"

$GLSLC_PATH shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
$GLSLC_PATH shaders/simple_shader.frag -o shaders/simple_shader.frag.spv