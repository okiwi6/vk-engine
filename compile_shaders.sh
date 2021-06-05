#!/bin/sh

GLSLC_PATH="/usr/bin/glslc"
echo "using glslc at $GLSLC_PATH"

$GLSLC_PATH shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
$GLSLC_PATH shaders/simple_shader.frag -o shaders/simple_shader.frag.spv