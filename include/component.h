#pragma once
#include "factory.h"
#include "dispatcher.h"
#include "entity_event.h"

namespace spiritsaway::entity_component_event
{
	template <typename BaseEntity, typename... Args>
	class base_component: public poly_hash_factory<base_component<BaseEntity, Args...>, raw_ptr_t, BaseEntity*, Args&&...>
	{
	public:
		base_component(Key )
		{

		}
	protected:
		BaseEntity* _owner = nullptr;
		bool is_destroyed = false;
	private:
		std::vector<listen_handler<entity_events>> listen_handlers_for_events;
		std::vector<listen_handler<std::string>> listen_handlers_for_strs;
	public:
		BaseEntity* GetOwner()
		{
			return _owner;
		}
		void SetOwner(BaseEntity* owner)
		{
			RemoveOwner();
			_owner = owner;
			OnSetOwner();
		}
		void RemoveOwner()
		{
			if(!_owner)
			{
				return;
			}
			ClearEventListener();
			OnRemoveOwner();
			_owner = nullptr;

		}
		void Destroy()
		{
			RemoveOwner();
			OnDestroy();
		}
		virtual std::string_view name() const = 0;
		virtual void OnSetOwner()
		{

		}
		virtual void OnRemoveOwner()
		{

		}
		void ClearEventListener()
		{
			for(auto one_listener: listen_handlers_for_events)
			{
				_owner->RemoveEventListener(one_listener);
			}
			listen_handlers_for_events.clear();
			for(auto one_listener: listen_handlers_for_strs)
			{
				_owner->RemoveEventListener(one_listener);
			}
			listen_handlers_for_strs.clear();
		}
		
		template <typename V>
		listen_handler<entity_events>  AddEventListener(entity_events cur_event, std::function<void(const entity_events&, const V&)> cur_callback)
		{
			auto cur_handler = _owner->template AddEventListener<V>(cur_event, cur_callback);
			listen_handlers_for_events.push_back(cur_handler);
			return cur_handler;
		}
		template <typename V>
		listen_handler<std::string>  AddEventListener(const std::string& cur_event, std::function<void(const std::string&, const V&)> cur_callback)
		{
			auto cur_handler = _owner->template AddEventListener<V>(cur_event, cur_callback);
			listen_handlers_for_strs.push_back(cur_handler);
			return cur_handler;
		}
		virtual void OnDestroy()
		{

		}
		virtual ~base_component()
		{
			Destroy();
		}
	};
}

