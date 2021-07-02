#!/bin/sh

GLSLC_PATH="/usr/bin/glslc"
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

echo "using glslc at $GLSLC_PATH"


$GLSLC_PATH "$SCRIPT_DIR/shaders/simple_shader.vert" -o "$SCRIPT_DIR/shaders/simple_shader.vert.spv"
$GLSLC_PATH "$SCRIPT_DIR/shaders/simple_shader.frag" -o "$SCRIPT_DIR/shaders/simple_shader.frag.spv"