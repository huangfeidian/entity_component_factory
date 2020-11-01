#pragma once

#include "entity.h"

namespace spiritsaway::entity_component_event
{
	class entity_factory
	{
		using create_func_T = base_creator_func<shr_ptr_t, base_entity, std::size_t, const std::string&>;
		using CreateMapT = creator_idx_map<base_entity, create_func_T>;
	public:
		template <class D>
		static typename create_func_T::template return_type<D> make(const std::string& str_id)
		{
			return CreateMapT::template make<D>(base_type_hash<base_entity>::template hash<D>(), str_id);
		}
		template <class T, class B = base_entity>
		struct sub_class : public B
		{
			friend T;

			static bool trigger()
			{
				return type_registration<CreateMapT>::template register_derived<T>();
			}
			static bool registered;

		private:
			sub_class(std::size_t int_type_id, const std::string& str_id) 
				: B(int_type_id, str_id)
				, access_key()
			{
				(void) registered;
			}
			class key
			{
				key()
				{

				}
				template <class T, class B> friend struct sub_class;
			};
		private:
			key access_key;
		};

		friend base_entity;

	private:
		using FuncType = shr_ptr_t<base_entity>(*)(const std::string&);
		entity_factory() = default;
	};
	template <typename T, typename B>
	bool entity_factory::template sub_class<T, B>::registered = entity_factory::template sub_class<T, B>::trigger();
}