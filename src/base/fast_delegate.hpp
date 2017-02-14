#pragma once
#include <cassert>

// From: http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates

namespace base 
{

    template <typename return_type, typename argument_type>
    class fast_delegate
    {
    public :
        fast_delegate()
            : this_(nullptr)
            , f_(nullptr)
        { }

    public :
        /*template <typename obj_type, unsigned (obj_type::*method)(argument_type)>
        static fast_delegate construct(obj_type* that)
        {
            return fast_delegate(that, &stub_func<obj_type, method>);
        }*/

        template <typename obj_type, unsigned (obj_type::*method)(argument_type)>
        static fast_delegate construct(obj_type* that)
        {
            return fast_delegate(reinterpret_cast<void*>(that), &stub_func<obj_type, method>);
        }

    public :
        return_type operator()(argument_type a)
        {
            assert(this_);
            assert(f_);
            return (*f_)(this_, a);
        }

    private :
        template <typename obj_type, return_type (obj_type::*method)(argument_type)>
        static return_type stub_func(void* that, argument_type a)
        {
            obj_type* this_ = reinterpret_cast<obj_type*>(that);
            return (this_->*method)(a); 
        }

    private : 
        typedef return_type (*stub_type)(void*, argument_type);
        void* this_;
        stub_type f_;

        fast_delegate(void* this__, stub_type f)
            : this_(this__)
            , f_(f)
        { }
    };

}
