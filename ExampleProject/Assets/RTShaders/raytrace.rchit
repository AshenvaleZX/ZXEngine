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
    vec3 Bitangent;
};

struct SimpleMaterial
{
    uint textureID;
};

struct RendererDataReference
{
    uint64_t indexAddress;          // Address of the index buffer
    uint64_t vertexAddress;         // Address of the Vertex buffer
    uint64_t materialAddress;       // Address of the material buffer
};

struct RayTracingPipelineConstants
{
    mat4 VP;
    mat4 V_Inv;
    mat4 P_Inv;
    vec3 lightPos;
};

layout(location = 0) rayPayloadInEXT HitPayload _RayPayload;
layout(location = 1) rayPayloadEXT bool isShadowed;

layout(buffer_reference, scalar) buffer IndicesBuffer { ivec3 index[]; }; // Triangle indices
layout(buffer_reference, scalar) buffer VerticesBuffer { Vertex vertex[]; };
layout(buffer_reference, scalar) buffer MaterialBuffer { SimpleMaterial material; }; // Array of all materials on an object

layout(set = 0, binding = 0) uniform accelerationStructureEXT _TLAS;
layout(set = 1, binding = 0) uniform sampler2D textureSamplers[];
layout(set = 1, binding = 1, scalar) buffer DataReferenceBuffer { RendererDataReference dataReference[]; } _ReferenceBuffer;

layout(push_constant) uniform _PushConstant { RayTracingPipelineConstants _PC; };

void main()
{
    // Object data
    RendererDataReference reference = _ReferenceBuffer.dataReference[gl_InstanceCustomIndexEXT];
    IndicesBuffer indices = IndicesBuffer(reference.indexAddress);
    VerticesBuffer vertices = VerticesBuffer(reference.vertexAddress);
    MaterialBuffer materials = MaterialBuffer(reference.materialAddress);

    // IndicesBuffer of the triangle
    ivec3 ind = indices.index[gl_PrimitiveID];

    // Vertex of the triangle
    Vertex v0 = vertices.vertex[ind.x];
    Vertex v1 = vertices.vertex[ind.y];
    Vertex v2 = vertices.vertex[ind.z];

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    // Computing the coordinates of the hit position
    const vec3 pos      = v0.Position * barycentrics.x + v1.Position * barycentrics.y + v2.Position * barycentrics.z;
    const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));  // Transforming the position to world space

    // Computing the normal at hit position
    const vec3 normal      = v0.Normal * barycentrics.x + v1.Normal * barycentrics.y + v2.Normal * barycentrics.z;
    const vec3 wNormal = normalize(vec3(normal * gl_WorldToObjectEXT));  // Transforming the normal to world space

    // Point light
    vec3 lDir = _PC.lightPos - worldPos;
    float lightDistance = length(lDir);
    vec3 L = normalize(lDir);

    // MaterialBuffer of the object
    SimpleMaterial mat = materials.material;

    // Diffuse
    float dotNL = max(dot(wNormal, L), 0.0);
    vec2 texCoord = v0.TexCoords * barycentrics.x + v1.TexCoords * barycentrics.y + v2.TexCoords * barycentrics.z;
    vec3 texColor = texture(textureSamplers[nonuniformEXT(mat.textureID)], texCoord).xyz;
    vec3 diffuse = texColor * dotNL;

    vec3  specular = vec3(0);
    float attenuation = 1;

    // Tracing shadow ray only if the light is visible from the surface
    if(dot(wNormal, L) > 0)
    {
        float tMin   = 0.001;
        float tMax   = lightDistance;
        vec3  origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
        vec3  rayDir = L;
        uint  flags  = gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT;
        isShadowed   = true;
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