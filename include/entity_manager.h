#pragma once

#include "entity.h"

namespace spiritsaway::entity_component_event
{

	class entity_manager
	{

		std::unordered_map<std::string, shr_ptr_t<base_entity>> m_entities;
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
		
		static std::unordered_set<std::size_t>& all_class_ids()
		{
			static std::unordered_set<std::size_t> data;
			return data;
		}
		using entity_construc_func = std::function<std::shared_ptr<base_entity>(const std::string&)>;
		static std::unordered_map<std::string_view, entity_construc_func>& all_named_constructors()
		{
			static std::unordered_map<std::string_view, entity_construc_func> data;
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
			if (all_class_ids().count(cur_type_id) == 0)
			{
				assert(false);
				return {};
			}

			auto result = std::make_shared<D>(entity_construct_key(cur_type_id), str_id);
			instance().add_entity(result);
			return result;
		}
		static std::shared_ptr<base_entity> make(const std::string_view& class_name, const std::string& str_id)
		{
			if (instance().has_entity(str_id))
			{
				assert(false);
				return {};
			}
			auto constructor_iter = all_named_constructors().find(class_name);

			if (constructor_iter == all_named_constructors().end())
			{
				return {};
			}
			else
			{
				return constructor_iter->second(str_id);
			}
		}

		template <class T, class B = base_entity>
		struct sub_class : public B
		{
			friend T;

			static bool trigger()
			{
				// static_assert(std::is_final_v<T>, "sub class should be final");
				auto cur_type_name = type_name<T>();
				auto cur_type_id = base_type_hash<base_entity>::template hash<T>();
				auto& register_names = entity_manager::all_named_constructors();
				auto& register_ids = entity_manager::all_class_ids();
				inherit_mapper<base_entity>::record_sub_class<T, B>();
				assert(register_names.find(cur_type_name) == register_names.end());
				assert(register_ids.find(cur_type_id) == register_ids.end());
				register_ids.insert(cur_type_id);
				register_names.emplace(cur_type_name, [=](const std::string& str_id)
					{
						return std::make_shared<T>(entity_construct_key(cur_type_id), str_id);
					});

				return true;
			}
			static bool registered;

		private:
			sub_class(entity_construct_key access_key, const std::string& str_id)
				: B(access_key, str_id)
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
			if (m_entities.find(cur_id) != m_entities.end())
			{
				return false;
			}
			m_entities[cur_id] = cur_entity;
			return true;
		}
		std::shared_ptr<base_entity> get_entity(const std::string& id)
		{
			auto cur_iter = m_entities.find(id);
			if (cur_iter == m_entities.end())
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
			return m_entities.find(id) != m_entities.end();
		}
		template <typename T>
		std::shared_ptr<T> get_entity(const std::string& id) const
		{
			auto cur_iter = m_entities.find(id);
			if (cur_iter == m_entities.end())
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
		std::vector<std::shared_ptr<T>> get_all_entity_exact_type() const
		{
			std::vector<std::shared_ptr<T>> result;
			for (auto& one_entity : m_entities)
			{
				if (one_entity.second->is_exact_type<T>())
				{
					result.push_back(std::dynamic_pointer_cast<T>(one_entity.second));
				}
			}
			return result;
		}

		template <typename T>
		std::vector<std::shared_ptr<T>> get_all_entity_sub_type() const
		{
			std::vector<std::shared_ptr<T>> result;
			for (auto& one_entity : m_entities)
			{
				if (one_entity.second->is_sub_type<T>())
				{
					result.push_back(std::dynamic_pointer_cast<T>(one_entity.second));
				}
			}
			return result;
		}

		template <typename T>
		void iterate_all(T func) const
		{
			for (auto& one_entity : m_entities)
			{
				func(*(one_entity.second));
			}
		}
		template <typename T, typename B>
		std::size_t iterate_type(T func) const
		{
			std::size_t count = 0;
			for (auto& one_entity : m_entities)
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
			return m_entities.size();
		}
		template <typename T>
		std::size_t type_count() const
		{
			std::size_t count = 0;
			for (auto& one_entity : m_entities)
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
			auto cur_iter = m_entities.find(id);
			if (cur_iter == m_entities.end())
			{
				return false;
			}
			cur_iter->second->destroy();
			m_entities.erase(cur_iter);
			return true;
		}
	};

	template <typename T, typename B>
	bool entity_manager::template sub_class<T, B>::registered = entity_manager::template sub_class<T, B>::trigger();


}