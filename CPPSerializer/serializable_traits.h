#ifndef SERIALIZABLE_TRAITS_H
#define SERIALIZABLE_TRAITS_H

#include <type_traits>
#include <memory>
#include "Serializable.h"

namespace Serializer
{

//namespace for all implementation structures, meant to be used internally only.
namespace impl
{

	template<typename T>
	struct remove_inner_const { typedef T type; };

	template<typename F, typename S>
	struct remove_inner_const<std::pair<const F, S>> { typedef std::pair<F, S> type;  };

	template<typename F, typename S>
	struct remove_inner_const<std::pair<F, const S>> { typedef std::pair<F, S> type; };

	template<typename F, typename S>
	struct remove_inner_const<std::pair<const F, const S>> { typedef std::pair<F, S> type; };

	template<typename T>
	struct remove_inner_const<std::shared_ptr<const T>> { typedef std::shared_ptr<T> type; };

	template<typename T>
	struct remove_inner_const<const T*> { typedef T* type; };



	template<typename T>
	struct construct
	{

		template<typename A>
		struct make
		{
			static A get_instance() { return A(); };
		};

		template<typename A>
		struct make<A*>
		{
			static A* get_instance() { return new A; }
		};

		template<typename A>
		struct make<std::shared_ptr<A>>
		{
			static std::shared_ptr<A> get_instance() { return std::make_shared<A>(); }
		};

		static T get_instance() { return make<T>::get_instance(); }
	};

	template<class T>
	struct is_serializable
	{

		template<class S>
		struct is_base_serializable
		{
			static const bool value = std::is_base_of<Serializable, S>::value || std::is_arithmetic<S>::value;

		};

		template<class S>
		struct serializable
		{
			static const bool value = is_base_serializable<S>::value;
		};

		template<class S>
		struct serializable<S*>
		{
			static const bool value = is_base_serializable<S>::value;
		};

		template<class S>
		struct serializable<std::shared_ptr<S>>
		{
			static const bool value = is_base_serializable<S>::value;
		};

		template<class S1, class S2>
		struct serializable<std::pair<S1, S2>>
		{
			static const bool value = serializable<S1>::value && serializable<S2>::value;
		};

		static const bool value = serializable<T>::value;
	};

	template<class T>
	struct is_deserializable
	{
		template<typename S>
		static constexpr bool test(S *type)
		{
			return test(type, std::is_arithmetic<S>());
		}

		template<typename S>
		static constexpr bool test(S *type, std::true_type)
		{
			return is_serializable<S>::value;
		}
		
		template<typename S>
		static constexpr bool test(S *type, std::false_type)
		{
			return is_serializable<S>::value && std::is_default_constructible<S>::value;
		}

		static const bool value = test<T>(nullptr);
	};

	template<typename T>
	struct is_valid_normal_iterator
	{
		typedef typename std::remove_const<T>::type test_type;

		template<typename Itr>
		static constexpr bool test(
			Itr *pt,
			Itr const *cpt = nullptr,
			decltype(&pt->operator*()) * = nullptr,
			decltype(&pt->operator++()) * = nullptr,
			decltype(&pt->operator=(std::declval<const Itr&>())) * = nullptr,
			decltype(pt->operator->()) * = nullptr,
			decltype((*pt)==(std::declval<Itr&>())) * = nullptr,
			decltype((*pt)!=(std::declval<Itr&>())) * = nullptr,
			typename Itr::value_type *pv = nullptr)
		{
			typedef typename Itr::value_type value_type;

			return std::is_same<decltype(pt->operator*()), value_type&>::value &&
                std::is_same<decltype((*pt)==(std::declval<Itr&>())), bool>::value &&
                std::is_same<decltype((*pt)!=(std::declval<Itr&>())), bool>::value &&
				std::is_same<decltype(pt->operator++()), Itr&>::value &&
				std::is_same<decltype(pt->operator->()), decltype(pv)>::value;
		}

		template<typename Itr>
		static constexpr bool test(...)
		{
			return false;
		}

		static const bool value = test<test_type>(nullptr);
	};

	template<typename T>
	struct is_valid_const_iterator
	{
		typedef typename std::remove_const<T>::type test_type;

		template<typename Itr>
		static constexpr bool test(
			Itr *pt,
			Itr const *cpt = nullptr,
			decltype(&pt->operator*())	* = nullptr,
			decltype(&pt->operator++()) * = nullptr,
			decltype(&pt->operator=(std::declval<Itr&>()))	* = nullptr,
			decltype(pt->operator->()) * = nullptr,
			decltype((*pt)==(std::declval<Itr&>())) * = nullptr,
			decltype((*pt)!=(std::declval<Itr&>())) * = nullptr,
			typename Itr::value_type *pv = nullptr)
		{
			typedef typename Itr::value_type value_type;
			typedef const value_type* cp_value_type;

			return std::is_same<decltype(pt->operator*()), const value_type&>::value &&
				std::is_same<decltype((*pt)==(std::declval<const Itr&>())), bool>::value &&
				std::is_same<decltype((*pt)!=(std::declval<const Itr&>())), bool>::value &&
				std::is_same<decltype(pt->operator++()), T&>::value &&
				std::is_same<decltype(pt->operator->()), cp_value_type>::value;
		}

		template<typename Itr>
		static constexpr bool test(...)
		{
			return false;
		}

		static const bool value = test<test_type>(nullptr);
	};

	template<typename T>
	struct is_writable_container
	{
		typedef typename std::remove_const<T>::type test_type;

		template<typename A>
		static constexpr bool test(
			A * pt,
			A const * cpt = nullptr,
			decltype(pt->begin()) * = nullptr,
			decltype(pt->end()) * = nullptr,
			decltype(cpt->begin()) * = nullptr,
			decltype(cpt->end()) * = nullptr,
			typename A::iterator * pi = nullptr,
			typename A::const_iterator * pci = nullptr,
			typename A::value_type * pv = nullptr)
		{

			typedef typename A::iterator iterator;

			typedef typename A::const_iterator const_iterator;
			typedef typename A::value_type value_type;
			return  std::is_same<decltype(pt->begin()), iterator>::value &&
				std::is_same<decltype(pt->end()), iterator>::value &&
				std::is_same<decltype(cpt->begin()), const_iterator>::value &&
				std::is_same<decltype(cpt->end()), const_iterator>::value &&
				(std::is_same<decltype(**pi), value_type &>::value || std::is_same<decltype(**pi), value_type const &>::value) &&
				std::is_same<decltype(**pci), value_type const &>::value &&
				(is_valid_normal_iterator<iterator>::value || is_valid_const_iterator<iterator>::value)&&
				is_valid_const_iterator<const_iterator>::value &&
				is_serializable<value_type>::value;

		}

		template<typename A>
		static constexpr bool test(...) {
			return false;
		}

		static const bool value = test<test_type>(nullptr);
	};

	template<typename T>
	struct is_readable_seq
	{
		typedef typename std::remove_const<T>::type test_type;

		template<typename A>
		static constexpr bool test(
			A * pt,
			A const * cpt = nullptr,
			decltype(pt->begin()) * = nullptr,
			decltype(pt->end()) * = nullptr,
			decltype(cpt->begin()) * = nullptr,
			decltype(cpt->end()) * = nullptr,
			decltype(pt->insert(std::declval<typename A::iterator>(), std::declval<typename A::value_type>())) * = nullptr,
			decltype(pt->insert(std::declval<typename A::const_iterator>(), std::declval<typename A::value_type>())) * = nullptr,
			typename A::iterator * pi = nullptr,
			typename A::const_iterator * pci = nullptr,
			typename A::value_type * pv = nullptr)
		{

			typedef typename A::iterator iterator;
			typedef typename A::const_iterator const_iterator;
			typedef typename A::value_type value_type;
			return  std::is_same<decltype(pt->begin()), iterator>::value &&
				std::is_same<decltype(pt->end()), iterator>::value &&
				std::is_same<decltype(cpt->begin()), const_iterator>::value &&
				std::is_same<decltype(cpt->end()), const_iterator>::value &&
				std::is_same<decltype(pt->insert(std::declval<typename A::iterator>(), std::declval<typename A::value_type>())), iterator>::value &&
				std::is_same<decltype(pt->insert(std::declval<typename A::const_iterator>(), std::declval<typename A::value_type>())), iterator>::value &&
				std::is_same<decltype(**pi), value_type &>::value &&
				std::is_same<decltype(**pci), value_type const &>::value &&
				is_valid_normal_iterator<iterator>::value &&
				is_valid_const_iterator<const_iterator>::value &&
				is_deserializable<value_type>::value;
		}

		template<typename A>
		static constexpr bool test(...) {
			return false;
		}

		static const bool value = test<test_type>(nullptr);

	};

	template<typename T>
	struct is_readable_ass
	{
		typedef typename std::remove_const<T>::type test_type;

		template<typename A>
		static constexpr bool test(
			A * pt,
			A const * cpt = nullptr,
			decltype(pt->begin()) * = nullptr,
			decltype(pt->end()) * = nullptr,
			decltype(cpt->begin()) * = nullptr,
			decltype(cpt->end()) * = nullptr,
			decltype(pt->insert(std::declval<typename A::value_type>())) * = nullptr,
			typename A::iterator * pi = nullptr,
			typename A::const_iterator * pci = nullptr,
			typename A::value_type * pv = nullptr)
		{
			typedef typename A::iterator iterator;
			typedef typename A::const_iterator const_iterator;
			typedef typename A::value_type value_type;
			return  std::is_same<decltype(pt->begin()), iterator>::value &&
				std::is_same<decltype(pt->end()), iterator>::value &&
				std::is_same<decltype(cpt->begin()), const_iterator>::value &&
				std::is_same<decltype(cpt->end()), const_iterator>::value &&
				(std::is_same<decltype(pt->insert(std::declval<typename A::value_type>())), std::pair<iterator, bool>>::value ||
					std::is_same<decltype(pt->insert(std::declval<typename A::value_type>())), iterator>::value) &&
					(std::is_same<decltype(**pi), value_type &>::value || std::is_same<decltype(**pi), value_type const &>::value) &&
				std::is_same<decltype(**pci), value_type const &>::value &&
				(is_valid_normal_iterator<iterator>::value || is_valid_const_iterator<iterator>::value) &&
				is_valid_const_iterator<const_iterator>::value &&
				is_deserializable<value_type>::value;
		}



		template<typename A>
		static constexpr bool test(...) {
			return false;
		}

		static const bool value = test<test_type>(nullptr);

	};

	
}

template<class T>
struct remove_inner_const { typedef typename impl::remove_inner_const<T>::type type; };

template<class T>
struct default_construct
{
	static T get_instance() { return impl::construct<T>::get_instance(); }
};

template<class T>
struct is_serializable : std::bool_constant<impl::is_serializable<T>::value> {};

template<class T>
struct is_deserializable : std::bool_constant<impl::is_deserializable<T>::value> {};

template<class Itr>
struct is_valid_iterator : std::bool_constant<impl::is_valid_normal_iterator<Itr>::value> {};

template<class Itr>
struct is_valid_const_iterator : std::bool_constant<impl::is_valid_const_iterator<Itr>::value> {};

template<typename C>
struct is_writable_container : std::bool_constant<impl::is_writable_container<C>::value> {};

template<typename C>
struct is_readable_sequential : std::bool_constant<impl::is_readable_seq<C>::value> {};

template<typename C>
struct is_readable_associative : std::bool_constant<impl::is_readable_ass<C>::value> {};

template<typename C>
struct is_readable_container : std::bool_constant<impl::is_readable_ass<C>::value || impl::is_readable_seq<C>::value> {};

}
#endif
