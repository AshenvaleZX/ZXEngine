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

void main()
{
    if(_RayPayload.depth == 0)
        _RayPayload.hitValue = vec3(0.8); // 光线从相机出来就直接Miss，采样天空盒，或Clear Color
    else
        _RayPayload.hitValue = vec3(0.01); // 光线从物体表面折射出来后Miss，就当作这根光线几乎没有任何贡献(也可以采样天空盒)
    _RayPayload.depth = 100; // 如果遇到Ray Miss了就直接结束光线追踪
}
