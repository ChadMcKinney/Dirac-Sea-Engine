// Copyright (C) Chad McKinney - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential

// Test Vertex Shader

#version 450

////////////////////////////////////////////
// Inputs
layout(set=0, binding=1) uniform u_UniformBuffer
{
    mat4 u_Matrix;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec2 i_Texcoord;

////////////////////////////////////////////
// Outputs
out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

////////////////////////////////////////////
// main
void main()
{
    gl_Position = u_Matrix * i_Position;
    v_Texcoord = i_Texcoord;
}
