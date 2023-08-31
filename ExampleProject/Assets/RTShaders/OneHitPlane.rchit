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
};

struct Vertex
{
    vec3 Position;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec4 Weights;
    ivec4 BoneIDs;
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
    uint frameCount;
};

layout(location = 0) rayPayloadInEXT HitPayload _RayPayload;
layout(location = 1) rayPayloadEXT bool isShadowed;

layout(buffer_reference, scalar) buffer IndicesBuffer { ivec3 index[]; };
layout(buffer_reference, scalar) buffer VerticesBuffer { Vertex vertex[]; };

layout(set = 0, binding = 0) uniform accelerationStructureEXT _TLAS;
layout(set = 1, binding = 0, scalar) buffer DataReferenceBuffer { RendererDataReference dataReference[]; } _ReferenceBuffer;
layout(set = 1, binding = 1) uniform sampler2D textureSamplers[];

layout(push_constant) uniform _PushConstant { RayTracingPipelineConstants _PC; };

void main()
{
    // 取出当前渲染对象的Buffer引用地址，获取到对应的Buffer数据
    RendererDataReference reference = _ReferenceBuffer.dataReference[gl_InstanceCustomIndexEXT];
    IndicesBuffer indices = IndicesBuffer(reference.indexAddress);
    VerticesBuffer vertices = VerticesBuffer(reference.vertexAddress);

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
    const vec3 normal      = v0.Normal * barycentrics.x + v1.Normal * barycentrics.y + v2.Normal * barycentrics.z;
    // 变换到世界空间
    const vec3 wNormal = normalize(vec3(normal * gl_WorldToObjectEXT));

    // 点光源信息
    vec3 lDir = _PC.lightPos - worldPos;
    float lightDistance = length(lDir);
    vec3 L = normalize(lDir);

    // 计算当前交点的采样坐标，计算棋盘图案
    vec2 texCoord = v0.TexCoords * barycentrics.x + v1.TexCoords * barycentrics.y + v2.TexCoords * barycentrics.z;
    vec2 uv = fract(texCoord * 5);
    vec3 texColor = vec3(0.6, 0.6, 0.6);
    if (uv.x > 0.5 && uv.y > 0.5 || uv.x < 0.5 && uv.y < 0.5)
    {
        texColor = vec3(0.8, 0.8, 0.8);
    }

    // Diffuse
    float dotNL = max(dot(wNormal, L), 0.0);
    vec3 diffuse = texColor * dotNL;

    vec3  specular = vec3(0);
    float attenuation = 1;

    // 判断一下是否在光源正面，在正面才计算Specular和阴影
    if(dot(wNormal, L) > 0)
    {
        float tMin   = 0.001;
        float tMax   = lightDistance;
        // 计算当前交点在世界空间下的位置
        // gl_WorldRayOriginEXT 表示光线最初的起点位置，即使光线多次反射，这个值不会变
        // gl_WorldRayDirectionEXT 表示光线最初的发射方向，同样不会变
        // gl_HitTEXT 可以理解为射线起点到相交点的比例系数，有点距离的意思，用下面这个公式可以推算出当前交点位置
        // 由于我们只Hit了一次，所以可以这样算，如果光线不停反射，这样算是不行的，应该得手动记录每次相交的位置和方向数据
        vec3  origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
        vec3  rayDir = L;
        uint  flags  = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
        isShadowed   = true;
        // 发射一条光线检测是否在阴影中
        traceRayEXT(_TLAS,  // acceleration structure
                    flags,  // rayFlags
                    0xFF,   // cullMask
                    0,      // sbtRecordOffset
                    0,      // sbtRecordStride
                    1,      // missIndex
                    origin, // ray origin
                    tMin,   // ray min range
                    rayDir, // ray direction
                    tMax,   // ray max range
                    1       // payload (location = 1)
        );

        if(isShadowed)
        {
            attenuation = 0.3;
        }
        else
        {
            // Specular
            const float kPi = 3.14159265;
            const float kShininess = 2;

            const float kEnergyConservation = (2.0 + kShininess) / (2.0 * kPi);
            vec3 V = normalize(-gl_WorldRayDirectionEXT);
            vec3 R = reflect(-L, wNormal);
            float S = kEnergyConservation * pow(max(dot(V, R), 0.0), kShininess);
            specular = vec3(S);
        }
    }

    _RayPayload.hitValue = vec3(attenuation * (diffuse + specular));
}