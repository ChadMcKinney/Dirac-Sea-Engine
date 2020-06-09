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
    mat4 u_UnusedMatrix; // TODO; remove/repurpose this
};

layout(push_constant) uniform PushConstants
{
    mat4 u_ViewMatrix;
    float u_TimeSecs;
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
// SDF helpers
//

float intersectSDF(float distA, float distB)
{
    return max(distA, distB);
}

float unionSDF(float distA, float distB)
{
    return min(distA, distB);
}


float differenceSDF(float distA, float distB)
{
    return max(distA, -distB);
}
////////////////////////////////////////////
// SDF scene

float sphereSDF(vec3 samplePos, vec3 spherePos, float radius)
{
    return length(samplePos - spherePos) - radius;
}

float cubeSDF(vec3 samplePos, vec3 cubePos, vec3 halfExtents)
{
    // if d.x < 0 then -1 < p.x < 1, same for p.y, p.z
    // so if all components of d are negative, then p is inside the unit cube
    vec3 d = abs(samplePos - cubePos) - halfExtents;
    float insideDistance = min(max(d.x, max(d.y, d.z)), 0.0);

    // Assuming p is inside the cube, how far is it from the surface?
    // Result will be negative or zero
    float outsideDistance = length(max(d, 0.0));
    return insideDistance + outsideDistance;
}

float wrap(float f)
{
    const float w = 8;
    return mod(f + w, w * 2) - 2;
}


float sceneSDF(vec3 pos)
{
    /* return sphereSDF(pos, vec3(0.0, 0.0, 0.0), 1.0); */
    /* pos = vec3(mod(pos.x, 8), mod(pos.y, 8), mod(pos.z, 8)); */
    /* pos = vec3(mod(-pos.x, 8), mod(-pos.y, 8), mod(-pos.z, 8)); */
    /* pos = vec3(wrap(pos.x), wrap(pos.y), wrap(pos.z)); */
    return unionSDF(
        sphereSDF(pos, vec3(1.0, 1.0, -1.0), 0.5),
        unionSDF(cubeSDF(pos, vec3(1.0, 0.0, 1.0), vec3(0.5, 0.5, 0.5)),
        unionSDF(cubeSDF(pos, vec3(-1.0, 0.0, 1.0), vec3(0.5, 0.5, 0.5)),
        cubeSDF(pos, vec3(0.0, 0.0, 0.0), vec3(0.5, 0.5, 0.5)))));
    /*
    vec3 spherePos = vec3(sin(u_TimeSecs * 0.5) * 0.25, cos(u_TimeSecs * 2) * 0.125, 0.0);
    float sphereRadius = sin(u_TimeSecs * 4) * 0.75 + 1.5;
    vec3 cubePos = vec3(cos(u_TimeSecs * 5) * 0.125, sin(u_TimeSecs) * 0.25, 0.0);
    vec3 cubeHalfExtents = vec3(0.5, 0.5, 0.5) * (sin(u_TimeSecs * 5) * 0.5 + 1);
    float sphereDist = sphereSDF(pos, spherePos, sphereRadius);
    float cubeDist = cubeSDF(pos, cubePos, cubeHalfExtents);
    float d1 = intersectSDF(cubeDist, sphereDist);
    float d2 = intersectSDF(d1, differenceSDF(-sphereDist * 0.25, -cubeDist * 0.25));
    return unionSDF(d2, intersectSDF((d2 - d1) * 0.25, ((d1 - d2) * 0.125)) + sphereSDF(pos, spherePos, 0.8));
    */
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

// Use the gradient of the SDF to estimate the normal on the surface at point p
vec3 estimateNormal(vec3 p)
{
    return normalize(
        vec3(
            sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
            sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
            sceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
        ));
}

/*
 * Lighting contribution of a single point light source via Phong illumination
 *
 * The vec3 returned is the RGB color of the light's contribution
 *
 * k_a: Ambient color
 * k_d: Diffuse colo
 * k_s: specular
 * alpha: shininess coefficient
 * p: position of point being lit
 * eye: position of the camera
 * lightPos: the position of the light
 * lightIntensity color/intensity of the light
 */

vec3 phongContributionForLight(
    vec3 k_d,
    vec3 k_s,
    float alpha,
    vec3 p,
    vec3 eye,
    vec3 lightPos,
    vec3 lightIntensity)
{
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));

    float dotLN = dot(L, N);
    float dotRV = dot(R, N);

    // facing away from light
    if (dotLN < 0.0)
    {
        return vec3(0.0, 0.0, 0.0);
    }

    // facing away from view, apply only diffuse
    if (dotRV < 0.0)
    {
        lightIntensity * (k_d * dotLN);
    }

    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

/*
 * Lighting via Phong illumination
 * 
 * The vec3 return is the RGB color of that point after lighting is applied
 * k_a: Ambient color
 * k_d: Diffuse colo
 * k_s: specular
 * alpha: shininess coefficient
 * p: position of point being lit
 * eye: position of the camera
 */
vec3 phongIllumination(
    vec3 k_a,
    vec3 k_d,
    vec3 k_s,
    float alpha,
    vec3 p,
    vec3 eye)
{
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 light1Pos = vec3(4.0 * sin(u_TimeSecs * 1.5), 2.0 * sin(u_TimeSecs * 0.25), 4.0 * cos(u_TimeSecs * 1.5));
    vec3 light1Intensity = vec3(0.6, 0.4, 0.4) + (vec3(0.125) * sin(u_TimeSecs * 16.0));

    color += phongContributionForLight(
        k_d,
        k_s,
        alpha,
        p,
        eye,
        light1Pos,
        light1Intensity);

    vec3 light2Pos = vec3(4.0 * sin(u_TimeSecs * 0.333), 4.0 * cos(u_TimeSecs * 0.5), 2.0) + eye;
    vec3 light2Intensity = vec3(0.4, 0.4, 0.6) + (vec3(0.125) * sin(u_TimeSecs * 8.0));

    color += phongContributionForLight(
        k_d,
        k_s,
        alpha,
        p,
        eye,
        light2Pos,
        light2Intensity);

    return color;
}

mat3 lookAtMat3(vec3 eye, vec3 lookAt, vec3 up)
{
    vec3 forward = normalize(lookAt - eye);
    vec3 right = normalize(cross(up, forward));
    vec3 fixedUp = normalize(cross(forward, right));
    return mat3(
        right,
        fixedUp,
        -forward
    );
}

mat4 lookAtMat4(vec3 eye, vec3 lookAt, vec3 up)
{
    vec3 forward = normalize(lookAt - eye);
    vec3 right = normalize(cross(up, forward));
    vec3 fixedUp = normalize(cross(forward, right));
    return mat4(
        vec4(right, 0.0),
        vec4(fixedUp, 0.0),
        vec4(-forward, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

mat3 RotationY(float radians)
{
    float sinTheta = sin(radians);
    float cosTheta = cos(radians);
    return mat3(
        vec3(cosTheta, 0.0, -sinTheta),
        vec3(0.0, 1.0, 0.0),
        vec3(sinTheta, 0.0, cosTheta));
}

mat3 RotationZ(float radians)
{
    float sinTheta = sin(radians);
    float cosTheta = cos(radians);
    return mat3(
        vec3(cosTheta, sinTheta, 0.0),
        vec3(-sinTheta, cosTheta, 0.0),
        vec3(0.0, 0.0, 1.0));
}

////////////////////////////////////////////
// main
void main()
{
    vec2 size = vec2(1920, 1080); // TODO: pass in as uniform
    vec3 viewDir = rayDirection(45.0, size);
    vec3 eye = u_ViewMatrix[3].xyz;
    vec3 dir = mat3(u_ViewMatrix[0].xyz, u_ViewMatrix[1].xyz, u_ViewMatrix[2].xyz) * viewDir;
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    if (dist > MAX_DIST - EPSILON)
    {
        o_Color = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // The closest point on the surface to the eyepoint along the view ray
    vec3 p = eye + dist * dir;

    vec3 k_a = vec3(0.2, 0.2, 0.2);
    vec3 k_d = vec3(0.2, 0.2, 0.4);
    vec3 k_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    vec3 color = phongIllumination(k_a, k_d, k_s, shininess, p, eye);
    o_Color = vec4(color, 1.0);
}
