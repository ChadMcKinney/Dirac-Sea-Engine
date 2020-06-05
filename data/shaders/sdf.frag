// Copyright (C) Chad McKinney - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential

// SDF Fragment Shader

#version 450

////////////////////////////////////////////
// Inputs
layout(set=0, binding=0) uniform sampler2D u_Texture;
layout(set=0, binding=1) uniform u_UniformBuffer
{
    mat4 u_ViewMatrix;
};

layout(location = 0) in vec2 v_Texcoord;

////////////////////////////////////////////
// Outputs
layout(location = 0) out vec4 o_Color;

////////////////////////////////////////////
// Constants

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

////////////////////////////////////////////
// SDF scene

float sphereSDF(vec3 samplePoint)
{
    return length(samplePoint) - 1.0f;
}

float sceneSDF(vec3 samplePoint)
{
    return sphereSDF(samplePoint);
}

float shortestDistanceToSurface(vec3 eye, vec3 marchingDirection, float start, float end)
{
    float depth = start;
    float dist = 0;
    for (int i = 0; i < MAX_MARCHING_STEPS; ++i)
    {
        dist = sceneSDF(eye + depth * marchingDirection);
        if (dist < EPSILON)
        {
            return depth;
        }

        depth += dist;
        if (depth >= end)
        {
            return end;
        }
    }

    return end;
}

vec3 rayDirection(float fieldOfView, vec2 size)
{
    vec2 xy = vec2(gl_FragCoord.x, gl_FragCoord.y) - (size / 2.0);
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

////////////////////////////////////////////
// main
void main()
{
    vec2 size = vec2(1024.0, 768.0);
    vec3 dir = rayDirection(45.0, size);
    vec3 eye = vec3(0.0, 0.0, 8.0);
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    if (dist > MAX_DIST - EPSILON)
    {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }
    else
    {
        o_Color = vec4(1.0, 0.0, 0.0, 1.0);
    }
}
