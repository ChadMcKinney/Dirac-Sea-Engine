// Copyright (C) Chad McKinney - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential

// SDF Fragment Shader

#version 450

////////////////////////////////////////////
// Inputs
layout(set=0, binding=1) uniform u_UniformBuffer
{
    mat4 u_ViewMatrix;
};

layout(location = 0) in vec2 v_Texcoord;

////////////////////////////////////////////
// Outputs
layout(location = 0) out vec4 o_Color;

////////////////////////////////////////////
// main
void main()
{
  o_Color = vec4(1.0, 0.0, 0.0, 1.0);
}
