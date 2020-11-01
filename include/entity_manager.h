#pragma once

#include "entity.h"

namespace spiritsaway::entity_component_event
{
	class entity_manager
	{
		std::unordered_map<std::string, shr_ptr_t<base_entity>> _entities;
		entity_manager()
		{

		}
		entity_manager(const entity_manager& other) = delete;
	public:
		static entity_manager& instance()
		{
			static entity_manager the_one;
			return the_one;
		}
	private:
		using create_func_T = base_creator_func<shr_ptr_t, base_entity, std::size_t, const std::string&>;
		using CreateMapT = creator_idx_map<base_entity, create_func_T>;
		using FuncType = shr_ptr_t<base_entity>(*)(const std::string&);

	public:
		template <class D>
		static typename create_func_T::template return_type<D> make(const std::string& str_id)
		{
			if (instance().has_entity(str_id))
			{
				return {};
			}
			auto result = CreateMapT::template make<D>(base_type_hash<base_entity>::template hash<D>(), str_id);
			if (result)
			{
				instance().add_entity(result);
			}
			return result;
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


	public:
		bool add_entity(std::shared_ptr<base_entity> cur_entity)
		{
			if (!cur_entity)
			{
				return false;
			}
			auto cur_id = cur_entity->entity_id();
			if (_entities.find(cur_id) != _entities.end())
			{
				return false;
			}
			_entities[cur_id] = cur_entity;
			return true;
		}
		std::shared_ptr<base_entity> get_entity(const std::string& id)
		{
			auto cur_iter = _entities.find(id);
			if (cur_iter == _entities.end())
			{
				return {};
			}
			else
			{
				return cur_iter->second;
			}
		}
		bool has_entity(const std::string& id) const
		{
			return _entities.find(id) != _entities.end();
		}
		template <typename T>
		std::shared_ptr<T> get_entity(const std::string& id) const
		{
			auto cur_iter = _entities.find(id);
			if (cur_iter == _entities.end())
			{
				return {};
			}
			else
			{
				if (!cur_iter->second->has_type<T>())
				{
					return {};
				}
				else
				{
					std::dynamic_pointer_cast<T>(cur_iter->second);
				}
			}
		}
		template <typename T>
		std::vector<std::shared_ptr<T>> get_all_entity() const
		{
			std::vector<std::shared_ptr> result;
			for (auto& one_entity : _entities)
			{
				if (one_entity.second->has_type<T>())
				{
					result.push_back(one_entity.second);
				}
			}
			return result;
		}

		template <typename T>
		void iterate_all(T func) const
		{
			for (auto& one_entity : _entities)
			{
				func(*(one_entity.second));
			}
		}
		template <typename T, typename B>
		std::size_t iterate_type(T func) const
		{
			std::size_t count = 0;
			for (auto& one_entity : _entities)
			{
				if (!one_entity.second->has_type<B>())
				{
					continue;
				}
				func(std::dynamic_pointer_cast<B>(one_entity.second));
				count++;
			}
			return count;
		}
		std::size_t total_count() const
		{
			return _entities.size();
		}
		template <typename T>
		std::size_t type_count() const
		{
			std::size_t count = 0;
			for (auto& one_entity : _entities)
			{
				if (!one_entity.second->has_type<B>())
				{
					continue;
				}
				count++;
			}
			return count;
		}
		bool destroy_entity(const std::string& id)
		{
			auto cur_iter = _entities.find(id);
			if (cur_iter == _entities.end())
			{
				return false;
			}
			cur_iter->second->destroy();
			_entities.erase(cur_iter);
			return true;
		}
	};

	template <typename T, typename B>
	bool entity_manager::template sub_class<T, B>::registered = entity_manager::template sub_class<T, B>::trigger();


}