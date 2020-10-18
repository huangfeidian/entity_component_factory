#pragma once
#include <string_view>
namespace spiritsaway::entity_component_event
{
	template <typename T>
	constexpr auto type_name() noexcept
	{
		std::string_view name, prefix, suffix;
	#ifdef __clang__
		name = __PRETTY_FUNCTION__;
		prefix = "auto type_name() [T = ";
		suffix = "]";
	#elif defined(__GNUC__)
		name = __PRETTY_FUNCTION__;
		prefix = "constexpr auto type_name() [with T = ";
		suffix = "]";
	#elif defined(_MSC_VER)
		name = __FUNCSIG__;
		prefix = "auto __cdecl type_name<";
		suffix = ">(void) noexcept";
	#endif
		name.remove_prefix(prefix.size());
		name.remove_suffix(suffix.size());
		return name;
	}

	template <typename B>
	class base_type_hash
	{
		static std::size_t last_used_id;
	public:
		template <typename T>
		static std::enable_if_t<std::is_base_of_v<B, T>, std::size_t> hash()
		{
			static const std::size_t id = last_used_id++;
			return id;
		}
	};
	template <typename B>
	std::size_t base_type_hash<B>::last_used_id = 0;
}
