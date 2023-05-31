#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct HitPayload
{
    vec3 hitValue;
};

layout(location = 0) rayPayloadInEXT HitPayload _RayPayload;

void main()
{
    _RayPayload.hitValue = vec3(0.8);
}
