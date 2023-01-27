#pragma once
#include <string_view>
template <typename T>
constexpr auto full_type_name() noexcept
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
	prefix = "auto __cdecl full_type_name<";
	suffix = ">(void) noexcept";
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());
	return name;
}

constexpr std::string_view type_name_without_ns(std::string_view full_name) noexcept
{
	auto final_part_pos = full_name.rfind("::");
	if (final_part_pos == std::string_view::npos)
	{
		return full_name;
	}
	else
	{
		return full_name.substr(final_part_pos + 2);
	}
}
template <typename T>
constexpr auto type_name() noexcept
{
	auto full_name = full_type_name<T>();
	return type_name_without_ns(full_name);
}

namespace spiritsaway::entity_component_event
{


	template <typename B>
	class base_type_hash
	{
		static std::size_t last_used_id;
	public:
		template <typename T>
		static std::enable_if_t<std::is_base_of_v<B, T>, std::size_t> hash()
		{
			if constexpr (std::is_same_v<B, T>)
			{
				// zero is reserved for self
				return 0;
			}
			else
			{
				static const std::size_t id = last_used_id++;
				return id;
			}
			
			
		}
		static std::size_t max_used()
		{
			return last_used_id;
		}
	};

	template <typename B>
	std::size_t base_type_hash<B>::last_used_id = 1;

	template <typename T>
	class inherit_mapper
	{
		
		
		static std::unordered_map<std::size_t, std::size_t>& parent_map()
		{
			static std::unordered_map<std::size_t, std::size_t> m_parent_map;
			return m_parent_map;
		}
		static std::vector<std::size_t>& parent_vec()
		{
			static std::vector<std::size_t> m_parent_vec;
			return m_parent_vec;
		}
		static void flat_map_to_vec()
		{
			auto& cur_parent_vec = parent_vec();
			auto& cur_parent_map = parent_map();
			cur_parent_vec.resize(base_type_hash<T>::max_used(), 0);
			for (const auto& one_pair : cur_parent_map)
			{
				cur_parent_vec[one_pair.first] = one_pair.second;
			}
			
		}
	public:
		template <typename A, typename P>
		static std::enable_if_t<std::is_base_of_v<T, A>&& std::is_base_of_v<T, P>&& std::is_base_of_v<P, A> && !std::is_same_v<A, P>, void> record_sub_class()
		{
			assert(parent_vec().empty());
			parent_map()[base_type_hash<T>::template hash<A>()] = base_type_hash<T>::template hash<P>();
		}
		static bool is_sub_class(std::size_t s_type_id, std::size_t p_type_id)
		{
			auto& cur_parent_vec = parent_vec();
			if (cur_parent_vec.empty())
			{
				flat_map_to_vec();
			}
			if (p_type_id == 0)
			{
				return true;
			}
			if (s_type_id >= cur_parent_vec.size() || p_type_id >= cur_parent_vec.size())
			{
				return false;
			}
			do
			{
				if (s_type_id == p_type_id)
				{
					return true;
				}
				s_type_id = cur_parent_vec[s_type_id];
			} while (s_type_id);
			return false;
		}
	};

}
