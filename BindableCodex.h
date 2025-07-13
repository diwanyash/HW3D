#pragma once
#include "Bindable.h"
#include "ConditionalNoexcept.h"
#include "BindableCodex.h"
#include <unordered_map>
#include <memory>

namespace bind
{
	class Codex
	{
	public:
		template < class T, typename...Params>
		static std::shared_ptr<Bindable> Resolve( Graphics& gfx, Params&&...p ) noxnd
		{
			return Get().Resolve_<T>( gfx, std::forward<Params>( p )...);
		}
		// static void Store( std::shared_ptr<Bindable> bind )
		// {
		// 	Get().Store_( std::move( bind ) );
		// }
	private:
		template < class T, typename...Params>
		std::shared_ptr<Bindable> Resolve_( Graphics& gfx, Params&&...p ) const noxnd
		{
			const auto key = T::GenerateUID( std::forward<Params>( p )... );
			const auto i = binds.find( key )
			auto i = binds.find(key);
			if ( i == binds.end() )
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				binds[key] = bind;
				return bind;
			}
			else
			{
				return i->second;
			}
		}
		// void Store_( std::shared_ptr<Bindable> bind )
		// {
		// 	binds[bind->GetUID()] = std::move( bind );
		// }
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
	};
}