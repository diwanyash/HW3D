#include "AssImpTest.h"
#include "BaseObject.h"
#include "BindableBase.h"
#include "DirectXMath.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

AssImpTest::AssImpTest(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material,
	float scale)
	:
	BaseObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{

		using Dvtx::VertexLayout;
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		Assimp::Importer imp;
		const auto model = imp.ReadFile("E:/chilli game dev/HW3D/model/suzanne.obj",
			aiProcess_JoinIdenticalVertices);

		const auto pMesh = model->mMeshes[0];

		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3{ pMesh->mVertices[i].x * scale, pMesh->mVertices[i].y * scale, pMesh->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i])
			);
		}


		std::vector<unsigned short> indices;
		indices.reserve(pMesh->mNumFaces * 3);

		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vbuf));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"Shaderm\\PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"Shaderm\\PhongPS.cso"));


		AddStaticBind(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

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







