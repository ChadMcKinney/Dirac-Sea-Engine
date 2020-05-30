// Copyright (C) Chad McKinney - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential


// Test Fragment Shader

#version 450

////////////////////////////////////////////
// Inputs
layout(location = 0) in vec4 v_Color;

////////////////////////////////////////////
// Outputs
layout(location = 0) out vec4 o_Color;

////////////////////////////////////////////
// main
void main() {
  o_Color = v_Color;
}
