#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <memory>
namespace spiritsaway::entity_component_event
{

	class event_data_wrapper
	{
	public:
		template <typename K>
		event_data_wrapper(const K& data, std::size_t data_type_id)
			: data_type(data_type_id)
			, data_ptr(&data)
		{

		}
		std::size_t data_type;
		const void* data_ptr;
	};
	template <typename T>
	class listen_handler
	{
	public:
		listen_handler(std::size_t event_id, std::size_t data_type_id, std::size_t in_callback_id)
			:callback_idx(in_callback_id)
			, data_type_idx(data_type_id)
			, event_idx(event_id)
		{

		}
		std::size_t callback_idx;
		std::size_t data_type_idx;
		std::size_t event_idx;

		void reset()
		{
			callback_idx = 0;
			data_type_idx = 0;
			event_idx = 0;
		}
	};
	const static std::size_t max_dispatch_depth = 10;
	template <typename K>
	class dispatcher_impl
	{
	private:
		struct event_desc
		{
			std::size_t event_id = 0;
			std::size_t dispatch_depth = 0; // to stop recursive dispatch
			std::unordered_map<std::size_t, std::vector<std::size_t>> data_callbacks; // data type_id to callbacks
		};
		std::unordered_map<K, std::size_t> event_idxes;
		std::vector<event_desc> event_descs;
		std::vector< std::shared_ptr<std::function<void(const K&, const event_data_wrapper&)>>> handler_to_callbacks;
		std::vector<std::size_t> recycle_callback_idxes;
	public:
		dispatcher_impl()
		{
			handler_to_callbacks.push_back({});
			event_descs.push_back({});
		}
	private:
		std::size_t get_next_callback_idx()
		{
			if (recycle_callback_idxes.empty())
			{
				handler_to_callbacks.push_back({});
				return handler_to_callbacks.size() - 1;
			}
			auto result = recycle_callback_idxes.back();
			recycle_callback_idxes.pop_back();
			return result;
		}
		std::size_t get_event_idx(const K& event)
		{
			auto cur_iter = event_idxes.find(event);
			if (cur_iter != event_idxes.end())
			{
				return cur_iter->second;
			}
			else
			{
				auto cur_event_idx = event_descs.size();
				event_idxes[event] = cur_event_idx;
				auto cur_event_desc = event_desc();
				cur_event_desc.event_id = cur_event_idx;
				event_descs.push_back(cur_event_desc);
				return cur_event_idx;
			}
		}
		bool invoke_callback(std::size_t callback_idx, const K& event, const event_data_wrapper& event_data)
		{
			if (callback_idx == 0 || callback_idx >= handler_to_callbacks.size())
			{
				return false;
			}
			auto callback_copy = handler_to_callbacks[callback_idx];

			if (!callback_copy)
			{
				return false;
			}
			callback_copy->operator()(event, event_data);
			return true;
		}
	public:
		template <typename V>
		bool dispatch(const K& event, const V& data, std::size_t cur_data_type_id)
		{

			auto cur_event_desc_iter = event_idxes.find(event);
			if (cur_event_desc_iter == event_idxes.end())
			{
				// this event is not registered
				return false;
			}
			auto& cur_event_desc = event_descs[cur_event_desc_iter->second];
			auto& cur_event_callbacks = cur_event_desc.data_callbacks;

			auto cur_event_callback_iter = cur_event_callbacks.find(cur_data_type_id);
			if (cur_event_callback_iter == cur_event_callbacks.end())
			{
				return false;
			}
			if (cur_event_desc.dispatch_depth >= max_dispatch_depth)
			{
				return false;
			}
			cur_event_desc.dispatch_depth++;
			std::vector<std::size_t>& cur_callbacks = cur_event_callback_iter->second;
			auto cur_data_wrapper = event_data_wrapper(data, cur_data_type_id);
			for (std::size_t i = 0; i < cur_callbacks.size();)
			{
				if (invoke_callback(cur_callbacks[i], event, cur_data_wrapper))
				{
					i++;
					continue;
				}
				if (i + 1 != cur_callbacks.size())
				{
					std::swap(cur_callbacks[i], cur_callbacks.back());
				}
				recycle_callback_idxes.push_back(cur_callbacks.back());
				cur_callbacks.pop_back();

			}
			cur_event_desc.dispatch_depth--;
			return true;
		}
		template <typename V>
		listen_handler<K> add_listener(const K& event, std::function<void(const K&, const V&)> cur_callback, std::size_t cur_data_type_idx)
		{

			auto cur_callback_idx = get_next_callback_idx();
			auto temp_lambda = [=](const K& event, const event_data_wrapper& data)
			{
				if (data.data_type != cur_data_type_idx)
				{
					return;
				}
				return cur_callback(event, *reinterpret_cast<const V*>(data.data_ptr));
			};
			handler_to_callbacks[cur_callback_idx] = std::make_shared< std::function<void(const K&, const event_data_wrapper&)>>(temp_lambda);
			auto cur_event_id = get_event_idx(event);

			event_descs[cur_event_id].data_callbacks[cur_data_type_idx].push_back(cur_callback_idx);
			return listen_handler<K>{ cur_event_id, cur_data_type_idx, cur_callback_idx};
		}
		bool remove_listener(const listen_handler<K>& cur_event_handler)
		{
			auto callback_idx = cur_event_handler.callback_idx;
			auto event_idx = cur_event_handler.event_idx;

			if (callback_idx == 0 || callback_idx >= handler_to_callbacks.size())
			{
				return false;
			}
			if (event_idx == 0 || event_idx >= event_descs.size())
			{
				return false;
			}
			handler_to_callbacks[callback_idx].reset();
			return true;
		}
		void clear()
		{
			event_idxes.clear();
			event_descs.clear();
			handler_to_callbacks.clear();
			recycle_callback_idxes.clear();
			handler_to_callbacks.push_back({});
			event_descs.push_back({});
		}

	};
	template <typename... args>
	class dispatcher
	{
	private:
		std::tuple<dispatcher_impl<args>...> dispatcher_impls;
		std::size_t last_type_id = 0;
	private:
		template <typename K>
		dispatcher_impl<K>& dispatcher_for()
		{
			return std::get<dispatcher_impl<K>>(dispatcher_impls);
		}
		template <class K>
		std::size_t get_type_id()
		{
			static const std::size_t id = ++last_type_id;
			return id;
		}

	public:
		dispatcher()
		{

		}
		template <typename K, typename V>
		listen_handler<K> add_listener(const K& event, std::function<void(const K&, const V&)> cur_callback)
		{
			return dispatcher_for<K>().add_listener(event, cur_callback, get_type_id<V>());
		}
		template <typename K, typename V>
		listen_handler<K> add_listener(const K& event, void(*cur_callback)(const K&, const V&))
		{
			std::function<void(const K&, const V&)> temp_func(cur_callback);
			return dispatcher_for<K>().add_listener(event, temp_func, get_type_id<V>());
		}
		template <typename K, typename V, typename T>
		listen_handler<K> add_listener(const K& event, void(T::*cur_callback)(const K&, const V&), T* self)
		{
			auto temp_lambda = [=](const K& key, const V& data)
			{
				return (self->*cur_callback)(key, data);
			};
			std::function<void(const K&, const V&)> temp_func(temp_lambda);
			return dispatcher_for<K>().add_listener(event, temp_func, get_type_id<V>());
		}

		template <typename K>
		bool remove_listener(listen_handler<K>& handler)
		{
			auto result = dispatcher_for<K>().remove_listener(handler);
			handler.reset();
			return result;
		}
		template <typename K, typename V>
		bool dispatch(const K& event, const V& data)
		{
			auto& cur_dispatcher = std::get<dispatcher_impl<K>>(dispatcher_impls);
			return cur_dispatcher.dispatch(event, data, get_type_id<V>());
		}
		void clear()
		{
			((dispatcher_for<args>().clear()),...);
		}
	};
}
