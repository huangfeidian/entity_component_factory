#pragma once
#include "factory.h"
#include "component.h"
#include "dispatcher.h"
#include "entity_event.h"

namespace spiritsaway::entity_component_event
{
	class base_entity
	{
	protected:
		std::string entity_id;
		const std::uint32_t entity_type_id = 0;
	protected:
		dispatcher<entity_events, std::string> _dispatcher;
	public:
		base_entity(const std::string& in_entity_id, std::uint32_t in_entity_type_id)
			: entity_id(in_entity_id)
			, entity_type_id(in_entity_type_id)
		{

		}
		template <typename T>
		bool has_type()
		{
			auto dest_entity_type_id = base_type_hash<base_entity>::hash<T>();
			if (dest_entity_type_id != entity_type_id)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		template <typename... Args>
		auto DispatchEvent(entity_events event, const Args&...  args)
		{
			_dispatcher.dispatch(event, args...);
		}
		template<typename... Args>
		void DispatchEvent(const std::string& event, const Args&... args)
		{
			_dispatcher.dispatch(event, args...);
		}
		template <typename V>
		listen_handler<entity_events>  AddEventListener(entity_events cur_event, std::function<void(const entity_events&, const V&)> cur_callback)
		{
			return _dispatcher.add_listener<entity_events, V>(cur_event, cur_callback);
		}
		template <typename V>
		listen_handler<std::string>  AddEventListener(const std::string& cur_event, std::function<void(const std::string&, const V&)> cur_callback)
		{
			return _dispatcher.add_listener<std::string, V>(cur_event, cur_callback);
		}
		bool RemoveEventListener(listen_handler<entity_events> cur_handler)
		{
			return _dispatcher.remove_listener(cur_handler);
		}
		bool RemoveEventListener(listen_handler<std::string> cur_handler)
		{
			return _dispatcher.remove_listener(cur_handler);
		}
	protected:
		void ClearListeners()
		{
			_dispatcher.clear();
		}

	public:
		virtual  ~base_entity()
		{

			ClearListeners();

		}
	};

	template <typename Component>
	class component_entity: public base_entity
	{
	private:
		std::vector<Component*> components;
	public:
		component_entity(const std::string& entity_id, std::uint32_t entity_type_id)
			: base_entity(entity_id, entity_type_id)
		{
			components = std::vector<Component*>(base_type_hash<Component>::max_used(), nullptr);
		}


	public:
		template<typename C>
		C* GetComponent()
		{
			auto cur_hash_id = base_type_hash<Component>::template hash<C>();
			if(cur_hash_id >= components.size())
			{
				return {};
			}
			auto cur_val = components[cur_hash_id];
			if(!cur_val)
			{
				return {};
			}
			return dynamic_cast<C*>(cur_val);
		}

		template <typename C>
		bool AddComponent(C* comp)
		{
			auto cur_hash_id = base_type_hash<Component>::template hash<C>();
			if (cur_hash_id >= components.size())
			{
				return false;
			}
			auto cur_val = components[cur_hash_id];
			if (cur_val)
			{
				return false;
			}
			components[cur_hash_id] = dynamic_cast<Component*>(comp);
			return true;
		}
		template <typename C>
		bool RemoveComponent()
		{
			auto cur_hash_id = base_type_hash<Component>::template hash<C>();
			if(cur_hash_id >= components.size())
			{
				return false;
			}
			auto cur_val = components[cur_hash_id];
			if(!cur_val)
			{
				return false;

			}
			C* real_val = dynamic_cast<C*>(cur_val);
			real_val->Destroy();
			delete real_val;
			components[cur_hash_id] = nullptr;
			return true;
		}
		
		void ClearComponents()
		{
			for(auto& one_component: components)
			{
				if(!one_component)
				{
					delete one_component;
					one_component = nullptr;
				}
			}
			components.clear();
		}
	public:
		virtual void Destroy()
		{
			ClearComponents();
		}
		~component_entity()
		{
			Destroy();
		}
	};
}