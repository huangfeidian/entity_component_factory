#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <cstdlib>
#include "type_util.h"

namespace spiritsaway::entity_component_event
{
	template <typename T>
	using raw_ptr_t = T * ;
	template <typename T>
	using shr_ptr_t = std::shared_ptr<T>;

	template <typename T>
	using unq_ptr_t = std::unique_ptr<T>;

	template<template<typename...> class ptr_t, typename T, class... Args>
	struct base_creator_func
	{
		using raw_return_type = ptr_t<T>;
		template <typename D>
		using return_type = ptr_t<D>;

	private:
		using func_type = raw_return_type(*)(Args...);

	public:
		base_creator_func(func_type func_ptr = nullptr) : m_func_ptr(func_ptr)
		{
		}

		template <class T>
		static base_creator_func instance()
		{
			func_type temp = [](Args... args) -> raw_return_type
			{
				return return_type(nullptr);
			};
			return base_creator_func(temp);
		}

		template <typename D>
		typename return_type<D> create(Args &&... ts)
		{
			return return_type<D>(m_func_ptr(std::forward<Ts>(ts)...));
		}

	private:
		func_type m_func_ptr;
	};
	template <class T, class... Args>
	struct base_creator_func<unq_ptr_t, T, Args...>
	{
		using raw_return_type = unq_ptr_t<T>;
		template <typename D>
		using return_type = unq_ptr_t<D>;

	private:
		using func_type = raw_return_type(*)(Args...);

	public:
		base_creator_func(func_type func_ptr = nullptr) : m_func_ptr(func_ptr)
		{
		}

		template <class T>
		static base_creator_func instance()
		{
			func_type temp = [](Args... args) -> raw_return_type
			{
				return std::make_unique<T>(std::forward<Args>(args)...);
			};
			return base_creator_func(temp);
		}

		template <typename D>
		return_type<D> create(Args &&... ts)
		{
			auto temp = m_func_ptr(std::forward<Args>(ts)...);
			return return_type<D>{dynamic_cast<D*>(temp.release())};
		}

	private:
		func_type m_func_ptr;
	};

	template <class T, class... Args>
	struct base_creator_func<shr_ptr_t, T, Args...>
	{
		using raw_return_type = shr_ptr_t<T>;
		template <typename D>
		using return_type = shr_ptr_t<D>;

	private:
		using func_type = raw_return_type(*)(Args...);

	public:
		base_creator_func(func_type func_ptr = nullptr) : m_func_ptr(func_ptr)
		{
		}

		template <class T>
		static base_creator_func instance()
		{
			func_type temp = [](Args... args) -> raw_return_type
			{
				return std::make_shared<T>(std::forward<Args>(args)...);
			};
			return base_creator_func(temp);
		}

		template <typename D>
		typename return_type<D> create(Args &&... ts)
		{
			auto temp = m_func_ptr(std::forward<Args>(ts)...);
			return std::dynamic_pointer_cast<D>(temp);
		}

	private:
		func_type m_func_ptr;
	};

	template <class T, class... Args>
	struct base_creator_func<raw_ptr_t, T, Args...>
	{
		using raw_return_type = raw_ptr_t<T>;
		template <typename D>
		using return_type = raw_ptr_t<D>;
		

	private:
		using func_type = raw_return_type(*)(Args...);

	public:
		base_creator_func(func_type func_ptr=nullptr) : m_func_ptr(func_ptr)
		{
		}

		template <class T>
		static base_creator_func instance()
		{
			func_type temp = [](Args... args) -> raw_return_type
			{
				return new T(std::forward<Args>(args)...);
			};
			return base_creator_func(temp);
		}

		template <typename D, class... Ts>
		typename return_type<D> create(Ts &&... ts)
		{
			return dynamic_cast<return_type<D>>(m_func_ptr(std::forward<Ts>(ts)...));
		}

	private:
		func_type m_func_ptr;
	};

	
	template <typename Base, typename CreatorFunc>
	struct creator_by_typeid
	{
	private:
		static std::unordered_map<std::size_t, CreatorFunc> data;
	public:
		using FuncT = CreatorFunc;
		template <typename T>
		static bool add(CreatorFunc f)
		{
			static_assert(std::is_base_of_v<Base, T>, "T should be derivate of Base");
			auto cur_hash = base_type_hash<Base>::hash<T>();
			auto& data = GetData();

			bool result = data.find(cur_hash) != data.end();
			data[cur_hash] = f;
			return result;
		}
		template <typename T, class... Ts>
		static auto make(Ts &&... args)
		{
			static_assert(std::is_base_of_v<Base, T>, "T should be derivate of Base");
			auto cur_hash = base_type_hash<Base>::hash<T>();
			auto& data = GetData();

			auto cur_iter = data.find(cur_hash);
			if (cur_iter == data.end())
			{
				assert(false);
				return FuncT::template return_type<T>();
			}
			else
			{
				return cur_iter->second.template create<T>(std::forward<Ts>(args)...);
			}
		}
		static std::unordered_map<std::size_t, CreatorFunc>& GetData()
		{
			static std::unordered_map<std::size_t, CreatorFunc> data;
			return data;
		}
	};

	template <typename Base, typename CreatorFunc>
	struct creator_by_typename
	{
	private:
	public:
		using FuncT = CreatorFunc;
		template <typename T>
		static bool add(CreatorFunc f)
		{
			static_assert(std::is_base_of_v<Base, T>, "T should be derivate of Base");
			auto cur_hash = T::class_name();
			auto& data = GetData();
			bool result = data.find(cur_hash) != data.end();
			data[cur_hash] = f;
			return result;
		}
		template <class... Ts>
		static auto make_by_name(std::string_view cur_name, Ts &&... args)
		{
			auto& data = GetData();
			auto cur_iter = data.find(cur_name);
			if (cur_iter == data.end())
			{
				assert(false);

				return FuncT::template return_type<Base>();
			}
			else
			{
				return cur_iter->second.template create<Base>(std::forward<Ts>(args)...);
			}
		}
		static std::unordered_map<std::string_view, CreatorFunc>& GetData()
		{
			static std::unordered_map<std::string_view, CreatorFunc> data;
			return data;
		}
	};

	
	template <typename CreateMapT>
	struct type_registration
	{
		template <class derived>
		static bool register_derived()
		{
			auto factory = CreateMapT::FuncT::template instance<derived>();
			CreateMapT::add<derived>(factory);
			return true;
		}
	};


	template <template<typename ...> class ptr_t, 
			  class Base, class... Args>
	class basic_poly_factory
	{
		using create_func_T = base_creator_func<ptr_t, Base, Args...>;
	public:
		template <class D, class... T>
		static typename create_func_T::template return_type<D> make(T &&... args)
		{
			return creator_by_typeid<Base, create_func_T>::template make<D>(std::forward<T>(args)...);
		}
		template <class... T>
		static typename create_func_T::template return_type<Base> make_by_name(const std::string_view name, T&&... args)
		{
			return creator_by_typename<Base, create_func_T>::template make_by_name(name, std::forward<T>(args)...);
		}

		template <class T, class B = Base>
		struct sub_class : public B
		{
			friend T;

			static bool trigger()
			{
				static_assert(std::is_final_v<T>, "sub class should have final specified");
				type_registration<creator_by_typeid<Base, create_func_T>>::template register_derived<T>();
				type_registration<creator_by_typename<Base, create_func_T>>::template register_derived<T>();
				return true;
			}
			static bool registered;

		private:
			sub_class(Args... args) : B(std::forward<Args>(args)...)
			{
				(void)registered;
			}
		};

		friend Base;

	private:
		using FuncType = ptr_t<Base> (*)(Args...);
		basic_poly_factory() = default;
	};

	template <template<typename ...>class ptr_t, 
			  class Base, class... Args>
	template <class T, class B>
	bool basic_poly_factory< ptr_t, Base,
							Args...>::template sub_class<T, B>::registered =
		basic_poly_factory<ptr_t, Base,
						   Args...>::template sub_class<T, B>::trigger();


} // namespace spiritsaway::entity_mesh