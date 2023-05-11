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

		indices[24] = 20; indices[25] = 22; indices[26] = 21;
		indices[27] = 20; indices[28] = 23; indices[29] = 22;

		return new StaticMesh(vertices, indices);
	}

	StaticMesh* GeometryGenerator::CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount)
	{
		vector<Vertex> vertices;
		vector<uint32_t> indices;

		// ��������
		Vertex topVertex = { .Position = { 0.0f, +radius, 0.0f }, .TexCoords = { 0.0f, 0.0f }, .Normal = { 0.0f, +1.0f, 0.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } };
		// �ײ�����
		Vertex bottomVertex = { .Position = { 0.0f, -radius, 0.0f }, .TexCoords = { 0.0f, 1.0f }, .Normal = { 0.0f, -1.0f, 0.0f }, .Tangent = { 1.0f, 0.0f, 0.0f } };

		float phiStep = Math::PI / stackCount;
		float thetaStep = 2.0f * Math::PI / sliceCount;

		vertices.push_back(topVertex);

		// �������ÿһ��
		for (uint32_t i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i * phiStep;

			// ���㵱ǰ��һ��ԲȦ�Ķ���
			for (uint32_t j = 0; j <= sliceCount; ++j)
			{
				float theta = j * thetaStep;

				Vertex v;

				// ��������ϵת���ѿ�������ϵ
				v.Position.x = radius * sinf(phi) * cosf(theta);
				v.Position.y = radius * cosf(phi);
				v.Position.z = radius * sinf(phi) * sinf(theta);

				// ��Ϊ����������������ϵԭ�㣬��������ֱ��Normalize���Ƿ���
				v.Normal = v.Position.Normalize();

				// Partial derivative of P with respect to theta
				v.Tangent.x = -radius * sinf(phi) * sinf(theta);
				v.Tangent.y = 0.0f;
				v.Tangent.z = +radius * sinf(phi) * cosf(theta);
				v.Tangent = v.Tangent.Normalize();

				v.TexCoords.x = theta / Math::PI;
				v.TexCoords.y = phi / Math::PI;

				vertices.push_back(v);
			}
		}

		vertices.push_back(bottomVertex);

		// �����һȦ���㣬��һȦ�ǹ̶��Ͷ�������������
		for (uint32_t i = 1; i <= sliceCount; ++i)
		{
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}

		// �������м�ĸ�Ȧ����
		uint32_t baseIndex = 1; // ������һ������(��������)
		uint32_t ringVertexCount = sliceCount + 1; // ÿһȦ�Ķ�����
		for (uint32_t i = 0; i < stackCount - 2; ++i)
		{
			for (uint32_t j = 0; j < sliceCount; ++j)
			{
				indices.push_back(baseIndex + i * ringVertexCount + j);
				indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indices.push_back(baseIndex + i * ringVertexCount + j + 1);

				indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			}
		}

		// �ײ����㣬Ҳ�������һ��
		uint32_t bottomIndex = static_cast<uint32_t>(vertices.size()) - 1;
		// �������һȦ������������
		baseIndex = bottomIndex - ringVertexCount;
		// ���һȦ����͵ײ���������
		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			indices.push_back(bottomIndex);
			indices.push_back(baseIndex + i + 1);
			indices.push_back(baseIndex + i);
		}

		return new StaticMesh(vertices, indices);
	}
}