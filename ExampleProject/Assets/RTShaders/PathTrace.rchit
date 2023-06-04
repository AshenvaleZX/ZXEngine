#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

hitAttributeEXT vec2 attribs;

struct HitPayload
{
    vec3 hitValue;
    uint seed;
    uint depth;
    vec3 rayOrigin;
    vec3 rayDirection;
    vec3 weight;
};

struct Vertex
{
    vec3 Position;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
};

struct Material
{
    uint _TextureID;
    vec3 _Emittance;
};

struct RendererDataReference
{
    uint64_t indexAddress;    // Address of the index buffer
    uint64_t vertexAddress;   // Address of the Vertex buffer
    uint64_t materialAddress; // Address of the material buffer
};

struct RayTracingPipelineConstants
{
    mat4 VP;
    mat4 V_Inv;
    mat4 P_Inv;
    vec3 lightPos;
    uint frame;
};

layout(location = 0) rayPayloadInEXT HitPayload _RayPayload;
layout(location = 1) rayPayloadEXT bool isShadowed;

layout(buffer_reference, scalar) buffer IndicesBuffer { ivec3 index[]; };
layout(buffer_reference, scalar) buffer VerticesBuffer { Vertex vertex[]; };
layout(buffer_reference, scalar) buffer MaterialBuffer { Material material; };

layout(set = 0, binding = 0) uniform accelerationStructureEXT _TLAS;
layout(set = 1, binding = 0) uniform sampler2D textureSamplers[];
layout(set = 1, binding = 1, scalar) buffer DataReferenceBuffer { RendererDataReference dataReference[]; } _ReferenceBuffer;

layout(push_constant) uniform _PushConstant { RayTracingPipelineConstants _PC; };

// Generate a random unsigned int in [0, 2^24) given the previous RNG state
// using the Numerical Recipes linear congruential generator
uint LinearCongruentialGenerator(inout uint prev)
{
    uint LCG_A = 1664525u;
    uint LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

// Generate a random float in [0, 1) given the previous RNG state
float RandomFloat(inout uint prev)
{
  return (float(LinearCongruentialGenerator(prev)) / float(0x01000000));
}

// Randomly samples from a cosine-weighted hemisphere oriented in the `z` direction.
// From Ray Tracing Gems section 16.6.1, "Cosine-Weighted Hemisphere Oriented to the Z-Axis"
vec3 SamplingHemisphere(inout uint seed, in vec3 x, in vec3 y, in vec3 z)
{
    float M_PI = 3.14159265;
    float r1 = RandomFloat(seed);
    float r2 = RandomFloat(seed);
    float sq = sqrt(r1);

    vec3 direction = vec3(cos(2 * M_PI * r2) * sq, sin(2 * M_PI * r2) * sq, sqrt(1. - r1));
    direction = direction.x * x + direction.y * y + direction.z * z;

    return direction;
}

// Return the tangent and binormal from the incoming normal
void CreateCoordinateSystem(in vec3 N, out vec3 Nt, out vec3 Nb)
{
    if(abs(N.x) > abs(N.y))
        Nt = vec3(N.z, 0, -N.x) / sqrt(N.x * N.x + N.z * N.z);
    else
        Nt = vec3(0, -N.z, N.y) / sqrt(N.y * N.y + N.z * N.z);
    Nb = cross(N, Nt);
}

void main()
{
    // 取出当前渲染对象的Buffer引用地址，获取到对应的Buffer数据
    RendererDataReference reference = _ReferenceBuffer.dataReference[gl_InstanceCustomIndexEXT];
    IndicesBuffer indices = IndicesBuffer(reference.indexAddress);
    VerticesBuffer vertices = VerticesBuffer(reference.vertexAddress);
    MaterialBuffer materials = MaterialBuffer(reference.materialAddress);

    // 获取当前三角形的顶点索引
    ivec3 ind = indices.index[gl_PrimitiveID];

    // 获取当前三角形的顶点
    Vertex v0 = vertices.vertex[ind.x];
    Vertex v1 = vertices.vertex[ind.y];
    Vertex v2 = vertices.vertex[ind.z];

    // 获取当前交点在三角形上的偏移信息
    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    // 计算当前交点的坐标
    const vec3 pos = v0.Position * barycentrics.x + v1.Position * barycentrics.y + v2.Position * barycentrics.z;
    // 变换到世界空间
    const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));

    // 计算当前交点的法线
    const vec3 normal = v0.Normal * barycentrics.x + v1.Normal * barycentrics.y + v2.Normal * barycentrics.z;
    // 变换到世界空间
    const vec3 wNormal = normalize(vec3(normal * gl_WorldToObjectEXT));

    // 获取材质数据
    Material mat = materials.material;
    vec3 emittance = mat._Emittance;

    // 计算当前交点的采样坐标，并采样纹理
    vec2 texCoord = v0.TexCoords * barycentrics.x + v1.TexCoords * barycentrics.y + v2.TexCoords * barycentrics.z;
    vec3 texColor = texture(textureSamplers[nonuniformEXT(mat._TextureID)], texCoord).xyz;

    // 从当前交点的法线半球内随机一个方向，作为下一条光线的方向
    vec3 tangent, bitangent;
    CreateCoordinateSystem(wNormal, tangent, bitangent);
    vec3 rayDirection = SamplingHemisphere(_RayPayload.seed, tangent, bitangent, wNormal);
    // 当前交点作为下一条光线起点
    vec3 rayOrigin = worldPos;

    float M_PI = 3.14159265;
    const float cos_theta = dot(rayDirection, wNormal);
    // 采样半球选择这个射线方向的概率密度
    const float p = cos_theta / M_PI;

    // 计算这条光线的BRDF，这里只用了最简单的Lambertian模型
    vec3 albedo = texColor;
    vec3 BRDF = albedo / M_PI;

    _RayPayload.rayOrigin    = rayOrigin;
    _RayPayload.rayDirection = rayDirection;
    _RayPayload.hitValue     = emittance;
    _RayPayload.weight       = BRDF * cos_theta / p;
    
    return;
}