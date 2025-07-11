#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template <class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !StaticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("Use IndexBuffer to Bind Index Buffer (static)" && typeid(*bind) != typeid(IndexBuffer));
		StaticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> iBuf) noexcept(!IS_DEBUG)
	{
		assert("you can't add index buffer again (static)" && pIndexBuffer == nullptr);
		pIndexBuffer = iBuf.get();
		StaticBinds.push_back( std::move( iBuf ) );
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("you can't add index buffer again (static)" && pIndexBuffer == nullptr);
		for (const auto& b : StaticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("can't find index buffer in static binds" && pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return StaticBinds;
	}
private:
	static std::vector<std::unique_ptr<Bindable>> StaticBinds;
};

template <class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::StaticBinds;