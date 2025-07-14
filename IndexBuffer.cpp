#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"


namespace bind
{
	IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
		:
		count(UINT(indices.size()))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bid = {};
		bid.Usage = D3D11_USAGE_DEFAULT;
		bid.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bid.CPUAccessFlags = 0u;
		bid.MiscFlags = 0u;
		bid.ByteWidth = UINT(count * sizeof(unsigned short));
		bid.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA sid = {};
		sid.pSysMem = indices.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bid, &sid, &pIndexBuffer));
	}

	void IndexBuffer::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count;
	}
}