#pragma once
#include "factory.h"
#include "dispatcher.h"
#include "entity_event.h"

namespace spiritsaway::entity_component_event
{
	template <typename BaseEntity, typename... Args>
	class base_component
	{

	public:
		using component_factory = basic_poly_factory<raw_ptr_t, base_component, Args...>;
		base_component(const typename component_factory::construct_key& in_key, Args... args)
			: m_type_id(in_key.m_type_id)
		{

		}
	protected:
		BaseEntity* _owner = nullptr;
		bool is_destroyed = false;
	private:
		std::vector<listen_handler<entity_events>> listen_handlers_for_events;
		std::vector<listen_handler<std::string>> listen_handlers_for_strs;
	public:
		const std::size_t m_type_id;
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

