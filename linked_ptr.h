/**
 * linked_ptr.h - A smart pointer implemented as a doubly linked list.
 * Based on boost's linked_ptr and smart_ptr.
 * 
 * Copyright (C) 2009 Thomas J. Gritzan. All rights reserved.
 * This source code is provided "as is" without warranty.
 *
 * This smart pointer stores three pointers and builds a doubly linked list
 * to track all references to the pointee. Once the last reference
 * (i.e. smart pointer) is gone, the pointee is deleted.
 * The interface is based on boost/TR1 shared_ptr's interface.
 * 
 * The class supports {static,const,dynamic}_pointer_cast.
 *
 * You can specialize linked_ptr_deleter to set a deleter for a class:
 *    template <>
 *    struct linked_ptr_deleter<SomeClass> {
 *       void operator()(SomeClass* ptr) const { ptr->release(); }
 *    };
 *
 * You can also specify a deleter for a base and all its subclasses
 * using a special macro:
 *    DECLARE_LINKED_PTR_BASE_DELETER(Base, linked_ptr_deleter<Base>);
 *
 * This smart pointer also works with array types:
 *     linked_ptr<int[]> p(new int[2]);
 *     p[0] = 42;
 *     
 * For an array smart pointer, it supports _aliasing_. You can add an offset
 * to the smart pointer, or use ++, --, +=, -= to modify it. On destruction
 * of the last pointer, it always deletes the original pointer. 
 * 
 * The implementation is not thread safe. Don't copy, destroy, assign
 * or reset a linked_ptr from more than one thread at a time.
 **/

#ifndef LINKED_PTR_H_INC_
#define LINKED_PTR_H_INC_

#include <functional>   // std::less
#include <memory>       // std::auto_ptr
#include <cassert>     // assert
#include <cstddef>

/**
 * doubly linked list implementation
 **/

template <typename X>
class linked_ptr;

namespace detail
{
	class linked_ptr_node
	{
	private:
		template <typename X> friend class ::linked_ptr;

		// construction
		linked_ptr_node() throw() { prev = next = this; }
		linked_ptr_node(linked_ptr_node & other) throw()
			: prev(&other), next(other.next)
		{
			next->prev = other.next = this;
		}

		~linked_ptr_node() throw()
		{
			if (!unique())
			{
				prev->next = next;
				next->prev = prev;
			}
		}

		// re-assignment
		void reset() throw()
		{
			prev->next = next;
			next->prev = prev;
			prev = next = this;
		}
		void reset(linked_ptr_node & other) throw()
		{
			reset();          // makes it self assignment safe
			prev = &other;
			next = other.next;
			next->prev = other.next = this;
		}

		// accessors
		bool unique() const throw()	{ return prev == this; };

	private:
		linked_ptr_node* prev;
		linked_ptr_node* next;
	};
} //namespace detail

/**
 * SFINAE helper functions
 **/

namespace detail
{
	struct true_type  { char dummy[1]; enum { value = true }; };
	struct false_type { char dummy[2]; enum { value = false }; };

	// enable_if<Condition>, enable_if_not<Condition>
	template <bool, typename T = void>
	struct enable_if_c { typedef T type; };

	template <typename T>
	struct enable_if_c<false, T> {};

	template <class Cond, typename T = void>
	struct enable_if : enable_if_c<Cond::value, T> {};

	template <class Cond, typename T = void>
	struct enable_if_not : enable_if_c<!Cond::value, T> {};

	// is_same_type<T, U>
	template <typename T, typename U>
	struct is_same_type : false_type {};

	template <typename T>
	struct is_same_type<T, T> : true_type {};

	// is_void<T>
	template <typename T> struct is_void : false_type {};
	template <> struct is_void<void> : true_type {};
	template <> struct is_void<const void> : true_type {};
	template <> struct is_void<volatile void> : true_type {};
	template <> struct is_void<const volatile void> : true_type {};

	// is_void_pointer<T>
	template <typename T, class Enable = void> struct is_void_pointer : false_type {};
	template <typename T> struct is_void_pointer<T*, typename enable_if< is_void<T> >::type> : true_type {};

	// is_convertible<From, To>
	template <typename From, typename To, class Enable = void>
	struct is_convertible : false_type {};

	template <typename From, typename To>
	class is_convertible<From, To, typename enable_if_not< is_void<From> >::type>
	{
		static From Make();	
		static false_type Test(...);
		static true_type Test(To);

	public:
		enum { value = sizeof(Test(Make())) == sizeof(true_type) };
	};

	// is_base_of<Base, Derived>
	template <typename Base, typename Derived, class Enable = void>
	struct is_base_of : false_type {};

	template <typename Base, typename Derived>
	struct is_base_of<Base, Derived, typename enable_if_not< is_void<Base> >::type>
		: is_convertible<const volatile Derived*, const volatile Base*> {};

} //namespace detail

/**
 * smart pointer policies
 * can be used for customization
 **/

template <typename X, class En = void>
struct linked_ptr_traits;   // typedefs for the class and policies

template <typename X, class En = void>
struct linked_ptr_deleter;  // provides the deleter function

template <typename X, class En = void>
struct linked_ptr_policy;   // public accessor functions and
                            // the actual pointee storage

// declares a deleter for a base class and its subclasses
#define DECLARE_LINKED_PTR_BASE_DELETER(Base, Deleter) \
template <typename X> \
struct linked_ptr_deleter<X, typename ::detail::enable_if< ::detail::is_base_of<Base, X> >::type> \
	: Deleter {};

/**
 * the smart pointer class
 **/

template <typename X>
class linked_ptr : public linked_ptr_policy<X>
{
public:
	typedef typename linked_ptr_traits<X>::element_type element_type;
	typedef typename linked_ptr_traits<X>::reference_type reference_type;
	typedef typename linked_ptr_traits<X>::pointer_type pointer_type;

	// construction
	linked_ptr() throw();
	template <typename Y> explicit linked_ptr(Y* ptr) throw();
	template <typename Y> linked_ptr(std::unique_ptr<Y> other) throw();
	linked_ptr(linked_ptr const& other) throw();
	template <typename Y> linked_ptr(linked_ptr<Y> const& other) throw();
	template <typename Y, typename Z> linked_ptr(linked_ptr<Y> const& other, Z* ptr) throw();	// aliasing constructor
	~linked_ptr();

	// (re-)assignment
	template <typename Y> linked_ptr& operator=(std::unique_ptr<Y> other);
	linked_ptr& operator=(linked_ptr const& other);
	template <typename Y> linked_ptr& operator=(linked_ptr<Y> const& other);

	void reset();
	template <typename Y> void reset(Y* ptr);
	template <typename Y> void reset(std::unique_ptr<Y> other);
	template <typename Y> void reset(linked_ptr<Y> const& other);
	template <typename Y, typename Z> void reset(linked_ptr<Y> const& other, Z* ptr);

	void swap(linked_ptr & other) throw();

	// accessors
	typedef bool (linked_ptr::*unspecified_bool_type)() const;

	/**
	 * provided by policy:
	 *     reference_type operator*() const throw();
	 *     pointer_type operator->() const throw();
	 *     pointer_type get() const throw();
	 *  - if template parameter is an array type T[]:
	 *     reference_type operator[](size_t idx) const throw();
	 *     pointer_type get(size_t idx = 0) const throw();
	 **/

	pointer_type release() throw();                             // unique() == true is asserted
	bool unique() const throw()		{ return link.unique(); };  // unique if only pointer in doubly linked list
	operator unspecified_bool_type() const throw();             // this is to allow "if (ptr)" constructs

	// we must be friends
	template <typename Y> friend class linked_ptr;

private:
	// pointee is stored in linked_ptr_policy<X>
	mutable ::detail::linked_ptr_node link;       // circular doubly linked list
};

/**
 * member function implementation
 **/

template<typename X>
inline linked_ptr<X>::linked_ptr() throw()
{}

template<typename X>
template<typename Y>
inline linked_ptr<X>::linked_ptr(Y* ptr) throw() : linked_ptr_policy<X>(ptr)
{}

template<typename X>
template<typename Y>
inline linked_ptr<X>::linked_ptr(std::unique_ptr<Y> other) throw()
	: linked_ptr_policy<X>( other.release() )
{}

template<typename X>
inline linked_ptr<X>::linked_ptr(linked_ptr const& other) throw()
	: linked_ptr_policy<X>(other), link(other.link)
{}

template<typename X>
template<typename Y>
inline linked_ptr<X>::linked_ptr(linked_ptr<Y> const& other) throw()
	: linked_ptr_policy<X>(other, other.pointee), link(other.link)
{}

template<typename X>
template<typename Y, typename Z>
inline linked_ptr<X>::linked_ptr(linked_ptr<Y> const& other, Z* ptr) throw()
	: linked_ptr_policy<X>(other, ptr), link(other.link)
{}

template<typename X>
inline linked_ptr<X>::~linked_ptr()
{
	if (unique() && this->get_delete())
	{
		// release the pointee if unique()
		linked_ptr_deleter<X>()( this->get_delete() );
	}
}

template<typename X>
inline linked_ptr<X>& linked_ptr<X>::operator=(linked_ptr const& other)
{
	reset(other);
	return *this;
}

template<typename X>
template<typename Y>
inline linked_ptr<X>& linked_ptr<X>::operator=(linked_ptr<Y> const& other)
{
	reset(other);
	return *this;
}

template<typename X>
template<typename Y>
inline linked_ptr<X>& linked_ptr<X>::operator=(std::unique_ptr<Y> other)
{
	reset(other);
	return *this;
}

template<typename X>
inline void linked_ptr<X>::reset()
{
	reset( pointer_type() );
}

template<typename X>
template<typename Y>
inline void linked_ptr<X>::reset(Y* ptr)
{
	linked_ptr<X> tmp(*this);   // release pointee in destructor if not shared

	link.reset();               // remove this smart pointer from the list
	this->set_pointee(ptr);     // reset the pointee
}

template<typename X>
template<typename Y>
inline void linked_ptr<X>::reset(std::unique_ptr<Y> other)
{
	reset( other.release() );
}

template<typename X>
template<typename Y>
inline void linked_ptr<X>::reset(linked_ptr<Y> const& other)
{
	reset( other, other.pointee );
}

template<typename X>
template<typename Y, typename Z>
inline void linked_ptr<X>::reset(linked_ptr<Y> const& other, Z* ptr)
{
	if (&link != &other.link)
	{
		linked_ptr<X> tmp(*this);       // release pointee in destructor if unique()

		link.reset(other.link);         // move this smart pointer to the new list
		this->set_pointee(other, ptr);  // reset the pointee
	}
	else
		this->set_pointee(other, ptr);  // reset the pointee
}

template<typename X>
inline void linked_ptr<X>::swap(linked_ptr<X> & other) throw()
{
	linked_ptr<X> tmp(other);
	other.reset(*this);
	this->reset(tmp);
}

template<typename X>
inline typename linked_ptr<X>::pointer_type linked_ptr<X>::release() throw()
{
	assert(unique() == true);

	pointer_type ptr = this->get();
	this->set_pointee( pointer_type() );
	return ptr;
}

template<typename X>
inline linked_ptr<X>::operator typename linked_ptr<X>::unspecified_bool_type() const throw()
{
	return this->pointee != 0
		? &linked_ptr::unique       // true (non-null), if non-empty
		: unspecified_bool_type();  // false otherwise
}

/**
 * freestanding functions
 **/

template<class T>
typename linked_ptr<T>::pointer_type get_pointer(linked_ptr<T> const& other)
{
	return other.get();
}

template<class T, class U>
linked_ptr<T> static_pointer_cast(linked_ptr<U> const& other)
{
	linked_ptr<T> ptr(other, static_cast<typename linked_ptr<T>::pointer_type>(other.get()));
	return ptr;
}

template<class T, class U>
linked_ptr<T> const_pointer_cast(linked_ptr<U> const& other)
{
	linked_ptr<T> ptr(other, const_cast<typename linked_ptr<T>::pointer_type>(other.get()));
	return ptr;
}

template<class T, class U>
linked_ptr<T> dynamic_pointer_cast(linked_ptr<U> const& other)
{
	linked_ptr<T> ptr(other, dynamic_cast<typename linked_ptr<T>::pointer_type>(other.get()));
	return ptr;
}

template<typename X>
inline void swap(linked_ptr<X> & a, linked_ptr<X> & b) throw()
{
	a.swap(b);
}

template<typename X>
inline bool operator==(linked_ptr<X> const& a, linked_ptr<X> const& b) throw()
{
	return a.get() == b.get();
}

template<typename X>
inline bool operator!=(linked_ptr<X> const& a, linked_ptr<X> const& b) throw()
{
	return a.get() != b.get();
}

template<typename X>
inline bool operator<(linked_ptr<X> const& a, linked_ptr<X> const& b) throw()
{
	typedef typename linked_ptr<X>::pointer_type pointer_type;

	return std::less<pointer_type>()(a.get(), b.get());
}

/**
 * policy types for linked_ptr
 **/

template <typename X, class En>
struct linked_ptr_traits
{
	typedef X element_type;
	typedef X& reference_type;
	typedef X* pointer_type;
};

template <typename X, class En>
struct linked_ptr_deleter
{
	typedef typename linked_ptr_traits<X>::pointer_type pointer_type;

	void operator()(pointer_type ptr) const { delete ptr; }
};

template <typename X, class En>
struct linked_ptr_policy
{
public:
	typedef typename linked_ptr_traits<X>::reference_type reference_type;
	typedef typename linked_ptr_traits<X>::pointer_type pointer_type;

	reference_type operator*() const throw()	{ assert(pointee != 0); return *pointee; };
	pointer_type operator->() const throw()		{ assert(pointee != 0); return pointee; };
	pointer_type get() const throw()			{ assert(pointee != 0); return pointee; };

protected:
	// we must be friends
	template <typename Y, class YEn> friend struct linked_ptr_policy;

	// preserve these constructors and resetters when specializing the policy class
	linked_ptr_policy() throw() : pointee() {}
	linked_ptr_policy(pointer_type ptr) throw() : pointee(ptr) {}
	linked_ptr_policy(linked_ptr_policy const& other) throw() : pointee(other.pointee) {}
	template <typename Y> linked_ptr_policy(linked_ptr_policy<Y> const& /*other*/, pointer_type ptr) throw() : pointee(ptr) {}

	void set_pointee(pointer_type ptr) { pointee = ptr; }
	void set_pointee(linked_ptr_policy const& other) { pointee = other.pointee; }
	template <typename Y> void set_pointee(linked_ptr_policy<Y> const& /*other*/, pointer_type ptr) { pointee = ptr; }

	// called when the host will release the pointer
	inline pointer_type get_delete() const { return pointee; }

private:
	// disabled. array pointers and non-array pointers are not compatible
	template <typename Y> linked_ptr_policy(linked_ptr_policy<Y[]> const& other, pointer_type ptr) throw();
	template <typename Y> void set_pointee(linked_ptr_policy<Y[]> const& other, pointer_type ptr);

protected:
	pointer_type pointee;
};

/**
 * policy overloads for arrays
 **/

template <typename X>
struct linked_ptr_traits<X[]> : linked_ptr_traits<X>
{
	// re-use non-array typedefs
};

template <typename X>
struct linked_ptr_deleter<X[]> : linked_ptr_deleter<X>
{
	typedef typename linked_ptr_traits<X>::pointer_type pointer_type;

	void operator()(pointer_type ptr) const { delete[] ptr; }
};

template <typename X>
struct linked_ptr_policy<X[]> : linked_ptr_policy<X>
{
	typedef typename linked_ptr_traits<X>::reference_type reference_type;
	typedef typename linked_ptr_traits<X>::pointer_type pointer_type;

	using linked_ptr_policy<X>::get;
	pointer_type get(size_t idx) const throw()          { assert(this->pointee != 0); return this->pointee + idx; };
	reference_type operator[](size_t idx) const throw() { return *get(idx); };

	void inc(ptrdiff_t off) throw() { this->pointee += off; };

protected:
	// preserve these constructors and resetters when specializing the policy class
	linked_ptr_policy() throw() : linked_ptr_policy<X>(), original_pointee() {}
	linked_ptr_policy(pointer_type ptr) throw()
		: linked_ptr_policy<X>(ptr), original_pointee(ptr) {}
	linked_ptr_policy(linked_ptr_policy const& other) throw()
		: linked_ptr_policy<X>(other.pointee), original_pointee(other.original_pointee) {}
	linked_ptr_policy(linked_ptr_policy const& other, pointer_type ptr) throw()
		: linked_ptr_policy<X>(ptr), original_pointee(other.original_pointee) {}

	void set_pointee(pointer_type ptr)
	{ linked_ptr_policy<X>::set_pointee(ptr); original_pointee = ptr; }
	void set_pointee(linked_ptr_policy const& other)
	{ original_pointee = other.original_pointee; linked_ptr_policy<X>::set_pointee(other.pointee); }
	void set_pointee(linked_ptr_policy const& other, pointer_type ptr)
	{ original_pointee = other.original_pointee; linked_ptr_policy<X>::set_pointee(ptr); }

	// called when the host will release the pointer
	pointer_type get_delete() const { return original_pointee; }

protected:
	pointer_type original_pointee;

private:
	// disabled. derived[] to base[] conversions are not safe.
	template <typename Y> linked_ptr_policy(Y* ptr) throw();
	template <typename Y, typename Z>
	linked_ptr_policy(linked_ptr_policy<Y> const& other, Z* ptr) throw();

	template <typename Y> void set_pointee(Y* ptr) throw();
	template <typename Y, typename Z>
	void set_pointee(linked_ptr_policy<Y> const& other, Z* ptr);
};

/**
 * freestanding operators for array pointer arithmetic
 **/

template<typename X>
inline linked_ptr<X[]>& operator+=(linked_ptr<X[]> & other, ptrdiff_t off) throw()
{
	other.inc(off);
	return other;
}

template<typename X>
inline linked_ptr<X[]>& operator-=(linked_ptr<X[]> & other, ptrdiff_t off) throw() { return other += -off; }
template<typename X>
inline linked_ptr<X[]>& operator++(linked_ptr<X[]> & other) throw() { return other += 1; }
template<typename X>
inline linked_ptr<X[]>& operator--(linked_ptr<X[]> & other) throw() { return other -= 1; }

template<typename X>
inline linked_ptr<X[]> operator+(linked_ptr<X[]> const& other, ptrdiff_t off) throw()
{
	linked_ptr<X[]> ptr(other, other.get() + off);
	return ptr;
}

template<typename X>
inline linked_ptr<X[]> operator-(linked_ptr<X[]> const& other, ptrdiff_t off) throw()
{
	linked_ptr<X[]> ptr(other, other.get() - off);
	return ptr;
}

#endif //LINKED_PTR_H_INC_
