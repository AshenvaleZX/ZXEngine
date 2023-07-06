#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct HitPayload
{
    vec3 hitValue;
    uint seed;
    uint depth;
    vec3 rayOrigin;
    vec3 rayDirection;
    vec3 weight;
};

layout(location = 0) rayPayloadInEXT HitPayload _RayPayload;

layout(set = 1, binding = 2) uniform samplerCube cubeMaps[];

void main()
{
    // 采样天空盒，天空盒默认为cubeMaps中的第一个
    _RayPayload.hitValue = texture(cubeMaps[0], gl_WorldRayDirectionEXT).rgb;
    // 已经Ray Miss了，直接结束光线追踪
    _RayPayload.depth = 100;
}
