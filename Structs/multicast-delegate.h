#pragma once
#include <functional>
#include <list>
#include <mutex>

/**
 * Multicast delegate base class. Should not be used directly!
 * @tparam T Must be of type \c std::function
 */
template <class T>
struct multicastDelegate {

	// Prevent assignment operator and copy/move constructor
	// TODO: does it really prevent it ? :thinking:
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
		mut.lock();
		callbacks.push_back(&function);
		mut.unlock();
	}

	/**
	 * Unbind a function from this delegate. You can also use -= operator, like \code{.cpp} delegate -= function \endcode
	 * @param function
	 */
	void unbind(T& function) { this -= function; }
	void operator-=(T& function)
	{
		mut.lock();
		callbacks.remove(&function);
		mut.unlock();
	}

	/**
	 * Remove all functions bound to this delegate.
	 */
	void clearAllCallbacks()
	{
		mut.lock();
		callbacks.clear();
		mut.unlock();
	}

protected:
	std::mutex mut;
	std::list<T*> callbacks;
};

struct multicastDelegate_ZeroParam : public multicastDelegate<std::function<void()>> {
	/**
	 * Call all functions bound to this delegate.
	 */
	void execute()
	{
		this->mut.lock();
		for(auto* callback : this->callbacks)
			callback->operator()();
		this->mut.unlock();
	}
};

template<class A>
struct multicastDelegate_OneParam : public multicastDelegate<std::function<void(A)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam)
	{
		this->mut.lock();
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam);
		this->mut.unlock();
	}
};

template<class A, class B>
struct multicastDelegate_TwoParams : public multicastDelegate<std::function<void(A, B)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam)
	{
		this->mut.lock();
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam);
		this->mut.unlock();
	}
};

template<class A, class B, class C>
struct multicastDelegate_ThreeParams : public multicastDelegate<std::function<void(A, B, C)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam, const C& thirdParam)
	{
		this->mut.lock();
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam, thirdParam);
		this->mut.unlock();
	}
};

template<class A, class B, class C, class D>
struct multicastDelegate_FourParams : public multicastDelegate<std::function<void(A, B, C, D)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam, const C& thirdParam, const D& fourthParam)
	{
		this->mut.lock();
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam, thirdParam, fourthParam);
		this->mut.unlock();
	}
};

template<class A, class B, class C, class D, class E>
struct multicastDelegate_fiveParams : public multicastDelegate<std::function<void(A, B, C, D, E)>> {
	/**
	 * Call all functions bound to this delegate with the parameters given.
	 */
	void execute(const A& firstParam, const B& secondParam, const C& thirdParam, const D& fourthParam, const E& fifthParam)
	{
		this->mut.lock();
		for(auto* callback : this->callbacks)
			callback->operator()(firstParam, secondParam, thirdParam, fourthParam, fifthParam);
		this->mut.unlock();
	}
};
