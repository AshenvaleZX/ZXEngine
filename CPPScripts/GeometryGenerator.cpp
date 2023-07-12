#include "GeometryGenerator.h"
#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh* GeometryGenerator::CreateBox(float xLength, float yLength, float zLength)
	{
		float hx = 0.5f * xLength;
		float hy = 0.5f * yLength;
		float hz = 0.5f * zLength;

		vector<Vertex> vertices(24);
		vector<uint32_t> indices(36);

		// -Z, Front
		vertices[0]  = { .Position = { -hx, -hy, -hz }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };
		vertices[1]  = { .Position = { -hx,  hy, -hz }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };
		vertices[2]  = { .Position = {  hx,  hy, -hz }, .TexCoords = { 1.0f, 0.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };
		vertices[3]  = { .Position = {  hx, -hy, -hz }, .TexCoords = { 1.0f, 1.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };

		indices[0] = 0; indices[1] = 2; indices[2] = 1;
		indices[3] = 0; indices[4] = 3; indices[5] = 2;

		// +Z, Back
		vertices[4]  = { .Position = { -hx, -hy,  hz }, .TexCoords = { 1.0f, 1.0f }, .Normal = { 0.0f, 0.0f,  1.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };
		vertices[5]  = { .Position = {  hx, -hy,  hz }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f, 0.0f,  1.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };
		vertices[6]  = { .Position = {  hx,  hy,  hz }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f, 0.0f,  1.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };
		vertices[7]  = { .Position = { -hx,  hy,  hz }, .TexCoords = { 1.0f, 0.0f }, .Normal = { 0.0f, 0.0f,  1.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };

		indices[6] = 4; indices[7]  = 6; indices[8]  = 5;
		indices[9] = 4; indices[10] = 7; indices[11] = 6;

		// -X, Left
		vertices[8]  = { .Position = { -hx, -hy,  hz }, .TexCoords = { 0.0f, 1.0f }, .Normal = { -1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f, -1.0f } };
		vertices[9]  = { .Position = { -hx,  hy,  hz }, .TexCoords = { 0.0f, 0.0f }, .Normal = { -1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f, -1.0f } };
		vertices[10] = { .Position = { -hx,  hy, -hz }, .TexCoords = { 1.0f, 0.0f }, .Normal = { -1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f, -1.0f } };
		vertices[11] = { .Position = { -hx, -hy, -hz }, .TexCoords = { 1.0f, 1.0f }, .Normal = { -1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f, -1.0f } };

		indices[12] = 8; indices[13] = 10; indices[14] = 9;
		indices[15] = 8; indices[16] = 11; indices[17] = 10;

		// +X, Right
		vertices[12] = { .Position = {  hx, -hy, -hz }, .TexCoords = { 0.0f, 1.0f }, .Normal = {  1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f,  1.0f } };
		vertices[13] = { .Position = {  hx,  hy, -hz }, .TexCoords = { 0.0f, 0.0f }, .Normal = {  1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f,  1.0f } };
		vertices[14] = { .Position = {  hx,  hy,  hz }, .TexCoords = { 1.0f, 0.0f }, .Normal = {  1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f,  1.0f } };
		vertices[15] = { .Position = {  hx, -hy,  hz }, .TexCoords = { 1.0f, 1.0f }, .Normal = {  1.0f, 0.0f, 0.0f }, .Tangent = { 0.0f, 0.0f,  1.0f } };

		indices[18] = 12; indices[19] = 14; indices[20] = 13;
		indices[21] = 12; indices[22] = 15; indices[23] = 14;

		// -Y, Bottom
		vertices[16] = { .Position = { -hx, -hy, -hz }, .TexCoords = { 1.0f, 1.0f }, .Normal = { 0.0f, -1.0f, 0.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };
		vertices[17] = { .Position = {  hx, -hy, -hz }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f, -1.0f, 0.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };
		vertices[18] = { .Position = {  hx, -hy,  hz }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f, -1.0f, 0.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };
		vertices[19] = { .Position = { -hx, -hy,  hz }, .TexCoords = { 1.0f, 0.0f }, .Normal = { 0.0f, -1.0f, 0.0f }, .Tangent = { -1.0f, 0.0f, 0.0f } };

		indices[24] = 16; indices[25] = 18; indices[26] = 17;
		indices[27] = 16; indices[28] = 19; indices[29] = 18;

		// +Y, Top
		vertices[20] = { .Position = { -hx,  hy, -hz }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f,  1.0f, 0.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };
		vertices[21] = { .Position = { -hx,  hy,  hz }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f,  1.0f, 0.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };
		vertices[22] = { .Position = {  hx,  hy,  hz }, .TexCoords = { 1.0f, 0.0f }, .Normal = { 0.0f,  1.0f, 0.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };
		vertices[23] = { .Position = {  hx,  hy, -hz }, .TexCoords = { 1.0f, 1.0f }, .Normal = { 0.0f,  1.0f, 0.0f }, .Tangent = {  1.0f, 0.0f, 0.0f } };

		indices[30] = 20; indices[31] = 22; indices[32] = 21;
		indices[33] = 20; indices[34] = 23; indices[35] = 22;

		return new StaticMesh(vertices, indices);
	}

	StaticMesh* GeometryGenerator::CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount)
	{
		vector<Vertex> vertices;
		vector<uint32_t> indices;

		// 顶部顶点
		Vertex topVertex = { .Position = { 0.0f, +radius, 0.0f }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f, +1.0f, 0.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } };
		// 底部顶点
		Vertex bottomVertex = { .Position = { 0.0f, -radius, 0.0f }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f, -1.0f, 0.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } };

		float phiStep = Math::PI / stackCount;
		float thetaStep = 2.0f * Math::PI / sliceCount;

		vertices.push_back(topVertex);

		// 遍历球的每一层
		for (uint32_t i = 1; i < stackCount; i++)
		{
			float phi = i * phiStep;

			// 计算当前这一层圆圈的顶点
			for (uint32_t j = 0; j <= sliceCount; j++)
			{
				float theta = j * thetaStep;

				Vertex v;

				// 从球坐标系转到笛卡尔坐标系
				v.Position.x = radius * sinf(phi) * cosf(theta);
				v.Position.y = radius * cosf(phi);
				v.Position.z = radius * sinf(phi) * sinf(theta);

				// 因为是球，球心又在坐标系原点，所以坐标直接Normalize就是法线
				v.Normal = v.Position.GetNormalized();

				// 重新推导切线
				v.Tangent.x = -radius * sinf(phi) * sinf(theta);
				v.Tangent.y = 0.0f;
				v.Tangent.z = +radius * sinf(phi) * cosf(theta);
				v.Tangent.Normalize();

				v.TexCoords.x = theta / Math::PI;
				v.TexCoords.y = phi / Math::PI;

				vertices.push_back(v);
			}
		}

		vertices.push_back(bottomVertex);

		// 计算第一圈顶点，第一圈是固定和顶部顶点相连的
		for (uint32_t i = 1; i <= sliceCount; i++)
		{
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		// 计算在中间的各圈顶点
		uint32_t baseIndex = 1; // 跳过第一个顶点(顶部顶点)
		uint32_t ringVertexCount = sliceCount + 1; // 每一圈的顶点数
		for (uint32_t i = 0; i < stackCount - 2; i++)
		{
			for (uint32_t j = 0; j < sliceCount; j++)
			{
				indices.push_back(baseIndex + i * ringVertexCount + j);
				indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indices.push_back(baseIndex + i * ringVertexCount + j + 1);

				indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			}
		}

		// 底部顶点，也就是最后一个
		uint32_t bottomIndex = static_cast<uint32_t>(vertices.size()) - 1;
		// 计算最后一圈顶点的起点索引
		baseIndex = bottomIndex - ringVertexCount;
		// 最后一圈顶点和底部顶点相连
		for (uint32_t i = 0; i < sliceCount; i++)
		{
			indices.push_back(bottomIndex);
			indices.push_back(baseIndex + i + 1);
			indices.push_back(baseIndex + i);
		}

		return new StaticMesh(vertices, indices);
	}

	StaticMesh* GeometryGenerator::CreateSphereTessellation(float radius, uint32_t subdivisionNum)
	{
		// 先手写一个正20面体
		const float X = 0.525731f;
		const float Z = 0.850651f;

		vector<Vertex> vertices = 
		{
			{ .Position = { -X, 0.0f,  Z } },
			{ .Position = {  X, 0.0f,  Z } },
			{ .Position = { -X, 0.0f, -Z } },
			{ .Position = {  X, 0.0f, -Z } },

			{ .Position = { 0.0f,  Z,  X } },
			{ .Position = { 0.0f,  Z, -X } },
			{ .Position = { 0.0f, -Z,  X } },
			{ .Position = { 0.0f, -Z, -X } },

			{ .Position = {  Z,  X, 0.0f } },
			{ .Position = { -Z,  X, 0.0f } },
			{ .Position = {  Z, -X, 0.0f } },
			{ .Position = { -Z, -X, 0.0f } },
		};

		vector<uint32_t> indices = 
		{
			1,0,4,   4,0,9,   4,9,5,   8,4,5,   1,4,8,
			1,8,10,  10,8,3,  8,5,3,   3,5,2,   3,2,7,
			3,7,10,  10,7,6,  6,7,11,  6,11,0,  6,0,1,
			10,6,1,  11,9,0,  2,9,11,  5,9,2,   11,7,2
		};

		// 细分N次
		for (uint32_t i = 0; i < subdivisionNum; i++)
			Subdivide(vertices, indices);

		// 计算重新细分后的顶点属性
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			// 调整法线
			Vector3 n = vertices[i].Position.GetNormalized();
			// 根据法线和半径重新计算位置
			Vector3 p = radius * n;

			vertices[i].Normal = n;
			vertices[i].Position = p;

			// 把位置转换回角度
			float theta = atan2f(vertices[i].Position.z, vertices[i].Position.x);
			// 限制到0到2 PI之间
			if (theta < 0.0f)
				theta += Math::PIx2;
			float phi = acosf(vertices[i].Position.y / radius);

			// 根据角度计算UV
			vertices[i].TexCoords.x = theta / Math::PIx2;
			vertices[i].TexCoords.y = phi / Math::PI;

			// 重新推导切线
			vertices[i].Tangent.x = -radius * sinf(phi) * sinf(theta);
			vertices[i].Tangent.y = 0.0f;
			vertices[i].Tangent.z = +radius * sinf(phi) * cosf(theta);
			vertices[i].Tangent.Normalize();
		}

		return new StaticMesh(vertices, indices);
	}

	StaticMesh* GeometryGenerator::CreateCylinder(float topRadius, float bottomRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		vector<Vertex> vertices;
		vector<uint32_t> indices;

		// 圆柱上的顶点圈数
		uint32_t ringCount = stackCount + 1;
		// 圆柱上每一层的高度
		float stackHeight = height / stackCount;
		// 每一层的半径增量
		float radiusStep = (topRadius - bottomRadius) / stackCount;
		// XZ平面上每一圈的弧度增量
		float dTheta = Math::PIx2 / sliceCount;

		// 从下到上计算每一圈的顶点
		for (uint32_t i = 0; i < ringCount; i++)
		{
			float y = -0.5f * height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;

			// 计算当前这一圈的顶点
			for (uint32_t j = 0; j <= sliceCount; j++)
			{
				Vertex vertex;

				// 根据三角函数计算各种顶点信息，文字不太好描述就不解释了
				float c = cosf(j * dTheta);
				float s = sinf(j * dTheta);

				vertex.Position = { r * c, y, r * s };

				vertex.TexCoords.x = (float)j / sliceCount;
				vertex.TexCoords.y = 1.0f - (float)i / stackCount;

				vertex.Tangent = { -s, 0.0f, c };

				float dr = bottomRadius - topRadius;
				vertex.Bitangent = { dr * c, -height, dr * s };

				vertex.Normal = Math::Cross(vertex.Tangent, vertex.Bitangent);

				vertices.push_back(vertex);
			}
		}

		// 每一圈的顶点数量(起点和中点在同一个位置，但是UV不同，所以是2个点，这里要+1)
		uint32_t ringVertexCount = sliceCount + 1;

		// 为刚刚那些顶点计算索引，构成三角形
		for (uint32_t i = 0; i < stackCount; i++)
		{
			for (uint32_t j = 0; j < sliceCount; j++)
			{
				indices.push_back(i * ringVertexCount + j);
				indices.push_back((i + 1) * ringVertexCount + j + 1);
				indices.push_back((i + 1) * ringVertexCount + j);

				indices.push_back(i * ringVertexCount + j);
				indices.push_back(i * ringVertexCount + j + 1);
				indices.push_back((i + 1) * ringVertexCount + j + 1);
			}
		}

		// 开始生成圆柱顶部的圆
		// 记录当前索引，作为顶部圆的起始索引
		uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
		// 虽然最顶部一圈顶点刚刚已经算过了，但是为了顶部圆圈的UV正确，这里需要重复生成一圈UV不同的顶点
		float topY = 0.5f * height;
		for (uint32_t i = 0; i <= sliceCount; i++)
		{
			float c = cosf(i * dTheta);
			float s = sinf(i * dTheta);

			float x = topRadius * c;
			float z = topRadius * s;

			float u = 0.5f + c * 0.5f;
			float v = 0.5f + s * 0.5f;

			vertices.push_back({
				.Position = { x, topY, z },
				.TexCoords = { u, v },
				.Normal = { 0.0f, 1.0f, 0.0f },
				.Tangent = { 1.0f, 0.0f, 0.0f }
			});
		}

		// 顶部中心顶点
		vertices.push_back({
			.Position = { 0.0f, topY, 0.0f },
			.TexCoords = { 0.5f, 0.5f },
			.Normal = { 0.0f, 1.0f, 0.0f },
			.Tangent = { 1.0f, 0.0f, 0.0f }
		});

		// 顶部中心顶点索引
		uint32_t topCenterIndex = static_cast<uint32_t>(vertices.size()) - 1;

		for (uint32_t i = 0; i < sliceCount; i++)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(baseIndex + i);
			indices.push_back(baseIndex + i + 1);
		}

		// 开始生成圆柱底部的圆，步骤和生成顶部圆一致
		baseIndex = static_cast<uint32_t>(vertices.size());

		float bottomY = -0.5f * height;
		for (uint32_t i = 0; i <= sliceCount; i++)
		{
			float c = cosf(i * dTheta);
			float s = sinf(i * dTheta);

			float x = bottomRadius * c;
			float z = bottomRadius * s;

			float u = 0.5f + c * 0.5f;
			float v = 0.5f + s * 0.5f;

			vertices.push_back({
				.Position = { x, bottomY, z },
				.TexCoords = { u, v },
				.Normal = { 0.0f, -1.0f, 0.0f },
				.Tangent = { 1.0f, 0.0f, 0.0f }
			});
		}

		vertices.push_back({
			.Position = { 0.0f, bottomY, 0.0f },
			.TexCoords = { 0.5f, 0.5f },
			.Normal = { 0.0f, -1.0f, 0.0f },
			.Tangent = { 1.0f, 0.0f, 0.0f }
		});

		uint32_t bottomCenterIndex = static_cast<uint32_t>(vertices.size()) - 1;

		for (uint32_t i = 0; i < sliceCount; i++)
		{
			indices.push_back(bottomCenterIndex);
			indices.push_back(baseIndex + i + 1);
			indices.push_back(baseIndex + i);
		}

		return new StaticMesh(vertices, indices);
	}

	StaticMesh* GeometryGenerator::CreatePlane(float xLength, float zLength, uint32_t xSplit, uint32_t zSplit)
	{
		vector<Vertex> vertices;
		vector<uint32_t> indices;

		uint32_t vertexCount = xSplit * zSplit;
		uint32_t faceCount = (xSplit - 1) * (zSplit - 1) * 2;

		float halfWidth = 0.5f * xLength;
		float halfDepth = 0.5f * zLength;

		float dx = xLength / (xSplit - 1);
		float dz = zLength / (zSplit - 1);

		float du = 1.0f / (xSplit - 1);
		float dv = 1.0f / (zSplit - 1);

		vertices.resize(vertexCount);
		for (size_t i = 0; i < zSplit; i++)
		{
			float z = halfDepth - i * dz;
			for (size_t j = 0; j < xSplit; j++)
			{
				float x = -halfWidth + j * dx;

				vertices[i * xSplit + j].Position = { x, 0.0f, z };
				vertices[i * xSplit + j].Normal = { 0.0f, 1.0f, 0.0f };
				vertices[i * xSplit + j].Tangent = { 1.0f, 0.0f, 0.0f };

				vertices[i * xSplit + j].TexCoords.x = j * du;
				vertices[i * xSplit + j].TexCoords.y = i * dv;
			}
		}

		indices.resize(static_cast<size_t>(faceCount) * 3);

		size_t k = 0;
		for (uint32_t i = 0; i < zSplit - 1; i++)
		{
			for (uint32_t j = 0; j < xSplit - 1; j++)
			{
				indices[k] = i * xSplit + j;
				indices[k + 1] = (i + 1) * xSplit + j;
				indices[k + 2] = i * xSplit + j + 1;

				indices[k + 3] = (i + 1) * xSplit + j;
				indices[k + 4] = (i + 1) * xSplit + j + 1;
				indices[k + 5] = i * xSplit + j + 1;

				// 移到下个四边形
				k += 6;
			}
		}

		return new StaticMesh(vertices, indices);
	}

	StaticMesh* GeometryGenerator::CreateQuad(float xLength, float yLength)
	{
		float halfX = 0.5f * xLength;
		float halfY = 0.5f * yLength;

		vector<Vertex> vertices =
		{
			{ .Position = {  halfX,  halfY, 0.0f }, .TexCoords = { 1.0f, 1.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } },
			{ .Position = {  halfX, -halfY, 0.0f }, .TexCoords = { 1.0f, 0.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } },
			{ .Position = { -halfX,  halfY, 0.0f }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } },
			{ .Position = { -halfX, -halfY, 0.0f }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f, 0.0f, -1.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } },
		};

		vector<uint32_t> indices = 
		{
			1, 0, 2,
			1, 2, 3
		};

		return new StaticMesh(vertices, indices);
	}

	Vertex GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
	{
		auto& p0 = v0.Position;
		auto& p1 = v1.Position;

		auto& c0 = v0.TexCoords;
		auto& c1 = v1.TexCoords;

		auto& n0 = v0.Normal;
		auto& n1 = v1.Normal;

		auto& t0 = v0.Tangent;
		auto& t1 = v1.Tangent;

		auto& b0 = v0.Bitangent;
		auto& b1 = v1.Bitangent;

		Vertex v = {};
		v.Position  =  0.5f * (p0 + p1);
		v.TexCoords =  0.5f * (c0 + c1);
		v.Normal    = (0.5f * (n0 + n1)).GetNormalized();
		v.Tangent   = (0.5f * (t0 + t1)).GetNormalized();
		v.Bitangent = (0.5f * (b0 + b1)).GetNormalized();

		return v;
	}

	void GeometryGenerator::Subdivide(vector<Vertex>& vertices, vector<uint32_t>& indices)
	{
		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2
		// 
		// 这个函数只会按这种方式把三角形细分，但是细分完还是在原来的面上
		// 如果需要将平面变形，需要按不同几何模型结构，在调用这个函数后自行计算

		vector<Vertex> verticesCopy = vertices;
		vector<uint32_t> indicesCopy = indices;

		vertices.clear();
		indices.clear();

		uint32_t triangleNum = static_cast<uint32_t>(indicesCopy.size()) / 3;
		for (uint32_t i = 0; i < triangleNum; i++)
		{
			Vertex v0 = verticesCopy[indicesCopy[i * 3 + 0]];
			Vertex v1 = verticesCopy[indicesCopy[i * 3 + 1]];
			Vertex v2 = verticesCopy[indicesCopy[i * 3 + 2]];

			Vertex m0 = MidPoint(v0, v1);
			Vertex m1 = MidPoint(v1, v2);
			Vertex m2 = MidPoint(v0, v2);

			vertices.push_back(v0);
			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(m0);
			vertices.push_back(m1);
			vertices.push_back(m2);

			indices.push_back(i * 6 + 0);
			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 5);

			indices.push_back(i * 6 + 5);
			indices.push_back(i * 6 + 4);
			indices.push_back(i * 6 + 2);

			indices.push_back(i * 6 + 3);
			indices.push_back(i * 6 + 1);
			indices.push_back(i * 6 + 4);
		}
	}
}