#ifndef SERIALIZABLE_TRAITS_H
#define SERIALIZABLE_TRAITS_H

#include <type_traits>
#include <memory>
#include "serializable.h"

namespace binbuff
{


/**
 * This file defines what types are serializable in addition to providing some simple utilities.
 *
 * To attain this functionality a substantial amount of black magic is used,
 * the impl namespace should'nt be messed with science small changes may have significant effects
 * on the the dependencies of this code especially considering different compilers implement some
 * of the features used differently.
 *
 * Unless you are modifying this library this file probably should'nt be used anyway,
 * the "rules" defined in this file are used inside static_asserts in the library to verify template input from
 * the user of the library and thus the user should never need to use these structures.
 *
 */

 /**
* This file defines both a serializable type and a deserializable type, at first this might seem weird
* why would a type be serializable and not deserializable ?
* Well it should'nt if it is possible always use a deserializable type it is the same as a serializable type
* only it has an empty constructor which is useful when initializing an instance for reading.
* The reason a distinction is made is that serializing does not require the constructor, and thus it is
* possible to have a type that is only serializable which you would write using this version of the library,
* and then read using a different version, or alternatively read the data into a different type.
* The same distinction is made for writable and readable containers.
*
* Additionally there are separate definitions for readable sequential and associative containers,
* this is done to adhere to the standard library which defines different ways of insertion into
* sequential and associative containers.
*/

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
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

		template<class S>
		struct is_primitive_serializable
		{
			static const bool value = std::is_base_of<Serializable, S>::value || std::is_arithmetic<S>::value;
		};

		template<class S>
		struct serializable
		{
			static const bool value = is_primitive_serializable<S>::value;
		};

		template<class S>
		struct serializable<S*>
		{
			static const bool value = is_primitive_serializable<S>::value;
		};

		template<class S>
		struct serializable<std::shared_ptr<S>>
		{
			static const bool value = is_primitive_serializable<S>::value;
		};

		template<class S1, class S2>
		struct serializable<std::pair<S1, S2>>
		{
			static const bool value = serializable<S1>::value && serializable<S2>::value;
		};

		static const bool value = serializable<test_type>::value;
	};

	template<class T>
	struct is_deserializable
	{
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

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

		static const bool value = test<test_type>(nullptr);
	};

	template<typename T>
	struct is_valid_normal_iterator
	{
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

		template<typename Itr>
		static constexpr bool test(
			Itr *pt,
			typename std::remove_reference<decltype(*(*pt))>::type * = nullptr,
			typename std::remove_reference<decltype(++(*pt))>::type * = nullptr,
			typename std::remove_reference<decltype((*pt)=(std::declval<const Itr&>()))>::type * = nullptr,
			typename std::remove_reference<decltype((*pt)==(std::declval<Itr&>()))>::type * = nullptr,
			typename std::remove_reference<decltype((*pt)!=(std::declval<Itr&>()))>::type * = nullptr)
		{
			typedef decltype(*(*pt)) value_type;

			return std::is_same<decltype((*pt) == (std::declval<Itr&>())), bool>::value &&
				std::is_same<decltype((*pt) != (std::declval<Itr&>())), bool>::value &&
				std::is_same<decltype(++(*pt)), Itr&>::value;
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
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

		template<typename Itr>
		static constexpr bool test(
			Itr *pt,
			typename std::remove_reference<decltype(*(*pt))>::type * = nullptr,
            typename std::remove_reference<decltype(++(*pt))>::type * = nullptr,
			typename std::remove_reference<decltype((*pt) = (std::declval<const Itr&>()))>::type * = nullptr,
			typename std::remove_reference<decltype((*pt) == (std::declval<Itr&>()))>::type * = nullptr,
			typename std::remove_reference<decltype((*pt) != (std::declval<Itr&>()))>::type * = nullptr)
		{
			typedef decltype(*(*pt)) value_type;

			return std::is_same<decltype((*pt) == (std::declval<const Itr&>())), bool>::value &&
				std::is_same<decltype((*pt) != (std::declval<const Itr&>())), bool>::value &&
				std::is_same<decltype(++(*pt)), Itr&>::value;
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
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

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

			return std::is_same<decltype(pt->begin()), iterator>::value &&
				std::is_same<decltype(pt->end()), iterator>::value &&
				std::is_same<decltype(cpt->begin()), const_iterator>::value &&
				std::is_same<decltype(cpt->end()), const_iterator>::value &&
				(std::is_same<decltype(**pi), value_type &>::value || std::is_same<decltype(**pi), value_type const &>::value) &&
				std::is_same<decltype(**pci), value_type const &>::value &&
				(is_valid_normal_iterator<iterator>::value || is_valid_const_iterator<iterator>::value) &&
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
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

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
        typedef typename std::remove_const<typename std::remove_reference<T>::type>::type test_type;

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

/**
 * Removes const of inner types.
 *
 * example: if the type shared_ptr<cont T> then we will get a type of shared_ptr<T>
 *
 * @tparam T a std::pair<K,V> T* or shared_ptr<T>
 */
template<class T>
struct remove_inner_const { typedef typename impl::remove_inner_const<T>::type type; };

/**
 * Creates an instance of the given type.
 * T must have an empty constructor otherwise the behavior is undefined.
 *
 * @tparam T some pointer, reference or a shared pointer type.
 */
template<class T>
struct default_construct
{
	static T get_instance() { return impl::construct<T>::get_instance(); }
};




/**
 * Defines a serializable type i.e type T is called serializable if and only if is_serializable<T>::value is true.
 *                                                                                                                     
 * let type T be called primitive serializable if one of the following applies:
 * *    T is a primitive type or more specifically T satisfies is_arithmetic<T>
 * *    T is a descendant of the Serializable class, more specifically T needs to satisfy is_base_of<Serializable, T>.
 *                                                                                                                     
 * let type T be serializable if it is primitive serializable or one of the following:
 * *    a pointer to a primitive serializable type.
 * *    a std::shared_ptr to a primitive serializable type.
 * *    a std::pair of two primitive serializable types.
 */
template<class T>
struct is_serializable : std::integral_constant<bool, impl::is_serializable<T>::value> {};


/**
 * Defines a deserializable type i.e type T is called deserializable if and only if is_deserializable<T>::value is true.<br>
 *
 * let type T be called deserializable if it is serializable, as defined above, and it has
 * an empty constructor, more specifically if it satisfies is_default_constructable<T>
 */
template<class T>
struct is_deserializable : std::integral_constant<bool, impl::is_deserializable<T>::value> {};

/**
* Defines an iterator over a serializable type i.e type Itr is an iterator in this context if and only if is_valid_iterator<Itr>::value is true.<br>
*
* The iterator definition is very simple and it is based on the definition in the standard library.
* Note that this definition does not require the value type of the iterator to be serializable
* that responsibility falls on the container on which the iterator operates.
* 
* let Itr be an iterator if all of the following apply:
* * Itr defines copy assignment operator. (itr and o being of type Itr the following should be valid 'itr = o')
* * Itr can be compared using equality/inequality operators.
*   (itr1 and itr2 being of type Itr the following should be valid 'itr == itr2', 'itr1 != itr2')
* * Itr can be dereferenced as an rvalue. (itr being of type Itr the following should be valid '*itr', 'itr->x')
* * Itr can be incremented using the ++ operator. (itr being of type Itr the following should be valid '++itr')
*/
template<class Itr>
struct is_valid_iterator : std::integral_constant<bool, impl::is_valid_normal_iterator<Itr>::value> {};

/**
* Defines a const iterator over a serializable type i.e type Itr is a const iterator in this context if and only if is_valid_const_iterator<Itr>::value is true.<br>
*
* The const iterator definition is the same as the other iterators with the addition that it allows to iterate over const values.
*/
template<class Itr>
struct is_valid_const_iterator : std::integral_constant<bool, impl::is_valid_const_iterator<Itr>::value> {};

/**
 * Defines a writable container type i.e type C is called a writable container if and only if is_writable_container<C>::value is true.
 *
 * A writable container is simply any container of serializable types that can be serialized as a container without manual iteration.
 * 
 * let C be a writable container if all of the following apply:
 * * C defines both a valid iterator and a valid const iterator (as defined above).
 * * C defines the following methods to access an iterator pointing to the beginning of the container - begin()
 *   and to access an iterator pointing to the end of the container - end().
 * * C defines a value_type that is of a serializable type (as defined above).
 */
template<typename C>
struct is_writable_container : std::integral_constant<bool, impl::is_writable_container<C>::value> {};

/**
 * Defines a readable container type i.e type C is called a readable container if and only if is_readable_container<C>::value is true.
 *
 * A readable container is simply any container of serializable types that can be deserialized as a container without manual iteration.
 *
 * let C be a readable container if all of the following apply:
 * * C defines either a sequential insert method or a associative insert method (as defined below).
 * * C is also a writable container (as defined above).
 *
 */
template<typename C>
struct is_readable_container : std::integral_constant<bool, impl::is_readable_ass<C>::value || impl::is_readable_seq<C>::value> {};

/**
 * Defines a sequential readable container type i.e type C is called a sequential readable container if and only if is_readable_sequential<C>::value is true.
 * 
 * A sequential readable container needs to define the insert(iterator, value_type) method so that it accepts an iterator and a value_type,
 * the iterator pointing to the position in the container where the insertion should be made
 * and the value type being the value that should be inserted.
 */
template<typename C>
struct is_readable_sequential : std::integral_constant<bool, impl::is_readable_seq<C>::value> {};

/**
* Defines an associative readable container type i.e type C is called a associative readable container if and only if is_readable_sequential<C>::value is true.
*
* A sequential readable container needs to define the insert(value_type) method so that it accepts a value_type,
* the value type being usually a key value pair representing the new mapping being added to the container.
*/
template<typename C>
struct is_readable_associative : std::integral_constant<bool, impl::is_readable_ass<C>::value> {};

}
#endif
