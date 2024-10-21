#pragma once
#include <functional>
#include <list>

/**
 * Multicast delegate base class. Should not be used directly!
 * @tparam T Must be of type \c std::function
 */
template <class T>
struct multicastDelegate {
public:

	// Prevent assignment operator and copy/move constructor
	multicastDelegate& operator=(const multicastDelegate<T> &) = delete;
	multicastDelegate(const multicastDelegate<T> &) = delete;
	multicastDelegate(multicastDelegate<T> &&) = delete;
	multicastDelegate() = default;

	/**
	 * Bind a new function to this delegate. You can also use += operator, like \code{.cpp} delegate += function \endcode
	 * @param function The function to be added.
	 */
	void bind(T& function) { this += function; }
	void operator+=(T& function)
	{
		callbacks.push_back(&function);
	}

	/**
	 * Unbind a function from this delegate. You can also use -= operator, like \code{.cpp} delegate -= function \endcode
	 * @param function
	 */
	void unbind(T& function) { this -= function; }
	void operator-=(T& function)
	{
		callbacks.remove(&function);
	}

	/**
	 * Remove all functions bound to this delegate.
	 */
	void clearAllCallbacks()
	{
		callbacks.clear();
	}

protected:

	std::list<T*> callbacks;
};

struct multicastDelegate_ZeroParam : public multicastDelegate<std::function<void()>> {
	/**
	 * Call all functions bound to this delegate.
	 */
	void execute()
	{
		for(auto* callback : this->callbacks)
			callback->operator()();
	}
};

template<class A>
struct multicastDelegate_OneParam : public multicastDelegate<std::function<void(A)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam)
	{
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam);
	}
};

template<class A, class B>
struct multicastDelegate_TwoParams : public multicastDelegate<std::function<void(A, B)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam)
	{
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam);
	}
};

template<class A, class B, class C>
struct multicastDelegate_ThreeParams : public multicastDelegate<std::function<void(A, B, C)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam, const C& thirdParam)
	{
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam, thirdParam);
	}
};

template<class A, class B, class C, class D>
struct multicastDelegate_FourParams : public multicastDelegate<std::function<void(A, B, C, D)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam, const C& thirdParam, const D& fourthParam)
	{
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam, thirdParam, fourthParam);
	}
};

template<class A, class B, class C, class D, class E>
struct multicastDelegate_fiveParams : public multicastDelegate<std::function<void(A, B, C, D, E)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam, const C& thirdParam, const D& fourthParam, const E& fifthParam)
	{
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam, thirdParam, fourthParam, fifthParam);
	}
};
