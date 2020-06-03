// Copyright (C) Chad McKinney - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential


// Test Fragment Shader

#version 450

////////////////////////////////////////////
// Samplers
layout(set=0, binding=0) uniform sampler2D u_Texture;

////////////////////////////////////////////
// Inputs
layout(location = 0) in vec2 v_Texcoord;

////////////////////////////////////////////
// Outputs
layout(location = 0) out vec4 o_Color;

////////////////////////////////////////////
// main
void main()
{
  o_Color = texture(u_Texture, v_Texcoord);
}
