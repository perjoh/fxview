#pragma once
#include <cassert>

// Based on: http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates

namespace kvant {
namespace base {

    template <typename Return_type, typename... Args>
    class Fast_delegate {
    public:
        Fast_delegate()
            : this_(nullptr)
            , f_(nullptr)
        {
        }

    public:
        template <typename Obj, Return_type (Obj::*method)(Args...)>
        static Fast_delegate construct(Obj* that)
        {
            return Fast_delegate(reinterpret_cast<void*>(that), &stub_func<Obj, method>);
        }

        // Optimization for plain c functions.
        template <Return_type (*method)(Args...)>
        static Fast_delegate construct()
        {
            return Fast_delegate(nullptr, &stub_func_null<method>);
        }

    public:
        Return_type operator()(Args&&... a)
        {
            return (*f_)(this_, a...);
        }

        Return_type operator()(Args&&... a) const
        {
            return (*f_)(this_, a...);
        }

    public:
        bool operator==(const Fast_delegate& rhs)
        {
            return this_ == rhs.this_ && f_ == rhs.f_;
        }

    private:
        template <typename Obj, Return_type (Obj::*method)(Args...)>
        static Return_type stub_func(void* that, Args... a)
        {
            Obj* this_ = reinterpret_cast<Obj*>(that);
            return (this_->*method)(a...);
        }

        template <Return_type (*method)(Args...)>
        static Return_type stub_func_null(void*, Args... a)
        {
            // Ignore 'this_'.
            return (*method)(a...);
        }

    private:
        using Stub_type = Return_type (*)(void*, Args...);
        void* this_;
        Stub_type f_;

        Fast_delegate(void* this__, Stub_type f)
            : this_(this__)
            , f_(f)
        {
        }
    };

	template <typename Delegate>
	class Delegate_list
	{
	public : 
		void push_back(Delegate d)
		{
			delegates_.push_back(d); 
		}

		template <typename... Args>
		void operator()(Args&&... args)
		{
			for (auto& d : delegates_)
			{
				d(args...); 
			} 
		}

	private :
		std::vector<Delegate> delegates_;
	};

} // namespace base
} // namespace kvant
