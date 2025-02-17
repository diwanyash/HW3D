#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <random>
#include <memory>

class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual ~Drawable() = default;
	virtual void Update( float dt )noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	void AddBind( std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexeBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};