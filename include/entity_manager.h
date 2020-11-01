#pragma once

#include "entity.h"

namespace spiritsaway::entity_component_event
{
	class entity_construct_key
	{
		entity_construct_key()
		{

		}
		friend class entity_manager;
	};

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
		static std::unordered_set<std::size_t>& all_registered_class()
		{
			static std::unordered_set<std::size_t> data;
			return data;
		}
		
	public:
		
		template <class D>
		static std::shared_ptr<D> make(const std::string& str_id)
		{
			if (instance().has_entity(str_id))
			{
				assert(false);
				return {};
			}
			auto cur_type_id = base_type_hash<base_entity>::template hash<D>();
			if (all_registered_class().count(cur_type_id) == 0)
			{
				assert(false);
				return {};
			}

			auto result = std::make_shared<D>(entity_construct_key(), cur_type_id, str_id);
			instance().add_entity(result);
			return result;
		}

		template <class T, class B = base_entity>
		struct sub_class : public B
		{
			friend T;

			static bool trigger()
			{
				entity_manager::all_registered_class().insert(base_type_hash<base_entity>::template hash<T>());
				return true;
			}
			static bool registered;

		private:
			sub_class(entity_construct_key access_key, std::size_t int_type_id, const std::string& str_id)
				: B(int_type_id, str_id)
			{
				(void) registered;
			}
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