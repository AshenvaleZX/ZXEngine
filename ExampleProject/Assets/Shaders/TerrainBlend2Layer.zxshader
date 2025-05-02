Setting 
{
    Blend SrcAlpha OneMinusSrcAlpha
    BlendOp Add
    Cull Back
    ZTest Less
    ZWrite On
    RenderQueue Opaque
}

Vertex
{
    Input
    {
        0 vec3 aPos       : POSITION
        1 vec2 aTexCoords : TEXCOORD
        2 vec3 aNormal    : NORMAL
        3 vec3 aTangent   : TANGENT
    }

    Output
    {
        0 vec3 WorldPos  : TEXCOORD0
        1 vec3 Normal    : TEXCOORD1
        2 vec4 uv1       : TEXCOORD2
        3 vec2 uvControl : TEXCOORD3
        4 vec3 tSpace0   : TEXCOORD4
        5 vec3 tSpace1   : TEXCOORD5
        6 vec3 tSpace2   : TEXCOORD6
    }

    Properties
    {
        using ENGINE_Model
        using ENGINE_View
        using ENGINE_Projection
        using ENGINE_Model_Inv

        float _Tiling
    }

    Program
    {
        void main()
        {
            WorldPos = to_vec3(mul(ENGINE_Model * vec4(aPos, 1.0)));
            Normal = mul(transpose(to_mat3(ENGINE_Model_Inv)) * aNormal);

            // 纹理采样放大缩小倍数
            uv1.xy = aTexCoords * _Tiling;
            uv1.zw = aTexCoords * _Tiling;
            uvControl = aTexCoords;

            mat3 worldToObject = transpose(to_mat3(ENGINE_Model_Inv));
            vec3 wNormal = mul(worldToObject * aNormal);
            vec3 wTangent = mul(worldToObject * aTangent);
            vec3 wBitangent = cross(wNormal, wTangent);

            tSpace0 = vec3(wTangent.x, wBitangent.x, wNormal.x);
            tSpace1 = vec3(wTangent.y, wBitangent.y, wNormal.y);
            tSpace2 = vec3(wTangent.z, wBitangent.z, wNormal.z);
            
            ZX_Position = mul(ENGINE_Projection * ENGINE_View * ENGINE_Model * vec4(aPos, 1.0));
        }
    }
}

Fragment
{
    Input
    {
        0 vec3 WorldPos  : TEXCOORD0
        1 vec3 Normal    : TEXCOORD1
        2 vec4 uv1       : TEXCOORD2
        3 vec2 uvControl : TEXCOORD3
        4 vec3 tSpace0   : TEXCOORD4
        5 vec3 tSpace1   : TEXCOORD5
        6 vec3 tSpace2   : TEXCOORD6
    }

    Output
    {
        0 vec4 FragColor : SV_Target
    }

    Properties
    {
        using ENGINE_Light_Pos
        using ENGINE_Camera_Pos
        using ENGINE_Shadow_Cube_Map
        using ENGINE_Far_Plane

        float _Height

        sampler2D _Texture1
        sampler2D _Texture1N
        sampler2D _Texture2
        sampler2D _Control
    }

    Program
    {
        float ShadowCalculation(vec3 fragPos, vec3 lightPos, vec3 camPos, float farPlane)
        {
            // 做Soft Shadow的采样偏移
            array<vec3, 20> gridSamplingDisk = 
            {
                vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
                vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
                vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
                vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
                vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
            };

            // 光源到当前片元的方向(即采样ShadowCubeMap的方向)
            vec3 fragToLight = fragPos - lightPos;
            // 当前片元到光源的距离
            float currentDepth = length(fragToLight);
            // 阴影厚度
            float shadow = 0.0;
            // 为防止浮点数精度问题导致的Z-Fight问题增加的偏移量
            float bias = 0.15;
            // 采样次数
            int samples = 20;
            // 观察点到当前片元的距离
            float viewDistance = length(camPos - fragPos);
            // 控制阴影柔和程度的参数，越小越柔和
            float soft = 5.0;
            // Soft Shadow采样半径，观察距离更远的时候阴影更柔和，更近了就更锐利
            float diskRadius = (1.0 + (viewDistance / farPlane)) / soft;
            for(int i = 0; i < samples; ++i)
            {
                // 在原方向上增加偏移量进行采样，得到当前片元到光源的方向上，离光源最近片元的距离
                float closestDepth = texture(ENGINE_Shadow_Cube_Map, fragToLight + gridSamplingDisk[i] * diskRadius).r;
                // 采样出来的值在0-1之间，还原到世界空间下的真实距离
                closestDepth *= farPlane;
                // 如果当前距离大于最近距离，说明被其它对象遮挡了光源，在阴影内，阴影厚度+1
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;
            }
            // 阴影厚度映射到0-1之间
            shadow /= float(samples);
            return shadow;
        }

        void main()
        {
            // 对多层地表纹理采样，这里的纹理a通道代表高度
            vec4 col1 = texture(_Texture1, uv1.xy);
            vec4 col2 = texture(_Texture2, uv1.wz);

            // control纹理的rgba代表各层地表的混合比例
            // 本来这个算法是混合4层地表的，直接读取_Control获得control，rgba分别代表4个图层的比例
            // 但是这里因为只有2层，所以用一个_Control通道生成只有2个比例数据的control
            vec4 controlMap = texture(_Control, uvControl);
            vec2 control = vec2(controlMap.b, 1 - controlMap.b);
            vec2 blend = vec2(col1.a * control.x, col2.a * control.y);
            float ma = max(blend.x, blend.y);
            // 控制高度效果的参数（如果为1相当于直接使用_Control贴图混合，不使用高度优化效果）
            blend = max(blend - ma + _Height, 0) * control;
            // 保证blend的2个比例加起来总和为1
            blend = blend / (blend.x + blend.y);

            // 混合纹理1和2的颜色
            vec3 color = col1.rgb * blend.x + col2.rgb * blend.y;

            // 把法线贴图纹理转到世界空间
            vec3 normalMapN = normalize((texture(_Texture1N, uv1.xy).xyz) * 2 - 1);
            vec3 N;
            N.x = dot(tSpace0, normalMapN);
            N.y = dot(tSpace1, normalMapN);
            N.z = dot(tSpace2, normalMapN);
            N = normalize(N);
            // 原始模型法线
            vec3 originN = normalize(Normal);
            // 混合法线（沙子部分没有法线纹理，就用原始法线表示）
            vec3 normal = normalize(N * blend.x + originN * blend.y);

            // ambient
            vec3 ambient = 0.05 * color;
            // diffuse
            vec3 lightDir = normalize(ENGINE_Light_Pos - WorldPos);
            float diff = max(dot(lightDir, normal), 0.0);
            vec3 diffuse = diff * color;
            // specular
            vec3 viewDir = normalize(ENGINE_Camera_Pos - WorldPos);
            vec3 reflectDir = reflect(-lightDir, normal);
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
            vec3 specular = to_vec3(0.7) * spec; // assuming bright white light color
            // 沙子的部分不要镜反射
            specular *= blend.x;

            // 计算阴影
            float shadow = ShadowCalculation(WorldPos, ENGINE_Light_Pos, ENGINE_Camera_Pos, ENGINE_Far_Plane);
            // 对漫反射的影响参数
            float shadowD = 1.0 - shadow / 2;
            // 对镜面反射的影响参数，这个值更小，因为被挡住了光源的直射，应该几乎没有镜面反射了
            float shadowS = 1.0 - shadow;
            // 计算添加了阴影后的光照颜色
            vec3 lightColor = shadowD * diffuse + shadowS * specular;

            FragColor = vec4(ambient + lightColor, 1.0);
        }
    }
}