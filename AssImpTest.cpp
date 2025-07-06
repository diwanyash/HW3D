#include "AssImpTest.h"
#include "BaseObject.h"
#include "BindableBase.h"
#include "DirectXMath.h"
#include "GraphicsThrowMacros.h"
#include "Surface.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "IndexedTriangleList.h"

AssImpTest::AssImpTest(Graphics& gfx, std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material,
	float scale)
	:
	BaseObject( gfx, rng, adist, ddist, odist, rdist )
	{
		namespace dx = DirectX;

		if (!IsStaticInitialized())
		{
			struct Vertex
			{
				dx::XMFLOAT3 pos;
				dx::XMFLOAT3 n;
			};

			Assimp::Importer imp;
			const auto model = imp.ReadFile("E:/chilli game dev/HW3D/model/bunny.obj", 0);
				//aiProcess_JoinIdenticalVertices);
			
			const auto pMesh = model->mMeshes[0];

			std::vector<Vertex>vertices;
			vertices.reserve(pMesh->mNumVertices);
			//
			// SOME FUN
			// 

			IndexedTriangleList<Vertex> normmodel;

			//

			for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
			{
				vertices.push_back({
					{ pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale },
					/**/DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f }
					});
			}
			/// *reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]) add insted or norm or 0

			std::vector<unsigned short> indices;
			indices.reserve( pMesh->mNumFaces * 3);

			for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
			{
				const auto& face = pMesh->mFaces[i];
				assert( face.mNumIndices == 3 );
				indices.push_back( face.mIndices[0]);
				indices.push_back( face.mIndices[1]);
				indices.push_back( face.mIndices[2]);
			}

			/*
			FUN CONTINUES
			*/

			normmodel.vertices = vertices;
			normmodel.indices = indices;
			
			normmodel.SetNormalsIndependentFlat();

			for( unsigned int i = 0; i < vertices.size(); i++)
			{
				vertices[i].n.x = normmodel.vertices[i].n.x;
				vertices[i].n.y = normmodel.vertices[i].n.y;
				vertices[i].n.z = normmodel.vertices[i].n.z;
			}
			/////////////

			AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
			AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx,indices));

			auto pvs = std::make_unique<VertexShader>(gfx, L"Shaderm\\PhongVS.cso");
			auto pvsbc = pvs->GetByteCode();
			AddStaticBind(std::move(pvs));

			AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shaderm\\PhongPS.cso"));


			const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
			{
				{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
				{"Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			};

			AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			struct PSMaterialConstant
			{
				DirectX::XMFLOAT3 color;
				float specularIntensity = 0.6f;
				float specularPower = 30.0f;
				float padding[3];
			} colorConst;
			colorConst.color = material;

			AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, colorConst, 1u));
		}
		else
		{
			SetIndexFromStatic();
		}

		AddBind(std::make_unique<TransformCBuff>(gfx, *this));

	}







