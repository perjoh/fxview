#pragma once
#include <cassert>

// Based on: http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates

namespace base 
{
    template <typename Return_type, typename... Args>
    class Fast_delegate
    {
    public :
        Fast_delegate()
            : this_(nullptr)
            , f_(nullptr)
        { }

    public :
        template <typename Obj, Return_type (Obj::*method)(Args...)>
        static Fast_delegate construct(Obj* that)
        {
            return Fast_delegate(reinterpret_cast<void*>(that), &stub_func<Obj, method>);
        }

    public :
        Return_type operator()(Args... a)
        {
            assert(this_);
            assert(f_);
            return (*f_)(this_, a...);
        }

    private :
        template <typename Obj, Return_type (Obj::*method)(Args...)>
        static Return_type stub_func(void* that, Args... a)
        {
            Obj* this_ = reinterpret_cast<Obj*>(that);
            return (this_->*method)(a...); 
        }

    private : 
		using Stub_type = Return_type (*)(void*, Args...);
        void* this_;
        Stub_type f_;

        Fast_delegate(void* this__, Stub_type f)
            : this_(this__)
            , f_(f)
        { }
    };


	template <typename Return_type>
	class Fast_delegate<Return_type, void> {
	public :
        Fast_delegate()
            : this_(nullptr)
            , f_(nullptr)
        { }

    public :
        template <typename Obj, Return_type (Obj::*method)()>
        static Fast_delegate construct(Obj* that)
        {
            return Fast_delegate(reinterpret_cast<void*>(that), &stub_func<Obj, method>);
        }

    public :
        Return_type operator()()
        {
            assert(this_);
            assert(f_);
            return (*f_)(this_);
        }

    private :
        template <typename Obj, Return_type (Obj::*method)()>
        static Return_type stub_func(void* that)
        {
            Obj* this_ = reinterpret_cast<Obj*>(that);
            return (this_->*method)(); 
        }

    private : 
		using Stub_type = Return_type (*)(void*);
        void* this_;
        Stub_type f_;

        Fast_delegate(void* this__, Stub_type f)
            : this_(this__)
            , f_(f)
        { }
	};

}
