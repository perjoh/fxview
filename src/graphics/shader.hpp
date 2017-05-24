#pragma once
#include "my_glm.hpp"
#include <memory>

namespace kvant {
namespace graphics {

    class Shader {
        friend class Shader_program;

    public:
        enum Shader_type { vertex_shader,
                           fragment_shader };

        Shader(Shader_type shader_type,
               const char* shader_source,
               int shader_source_len = 0);

        Shader(Shader&&) noexcept;
        Shader(const Shader&) = delete;
        ~Shader();

        Shader& operator=(const Shader&) = delete;

    public:
        void attach(unsigned int program_handle) const;
        void detach(unsigned int program_handle) const;

        void destroy();

    private:
        unsigned int handle_{0};
    };

    template <typename T>
    class Shader_uniform;

    class Shader_program {
		friend class Shader_scope;

    public:
        Shader_program() = default;
        Shader_program(const Shader_program&) = delete;

        Shader_program(const Shader& vertex_shader,
                       const Shader& fragment_shader);

        ~Shader_program();

    public:
        Shader_program& operator=(const Shader_program&) = delete;
        void swap(Shader_program&);

		explicit operator bool() const;

    public:
        template <typename T>
        Shader_uniform<T> get_uniform(const char* uniform_name) const
        {
            return Shader_uniform<T>(get_uniform_location(uniform_name));
        } 

    private:
        void bind();
        void unbind();

    private:
		int get_uniform_location(const char* name) const;	

        unsigned int handle_{0};
    };

	class Shader_scope {
	public :
		Shader_scope() = delete;
		Shader_scope(const Shader_scope&) = delete;
		Shader_scope(const std::weak_ptr<Shader_program>& ptr)
			: program_(ptr.lock())
		{ 
			program_->bind();
		}

		~Shader_scope()
		{ 
			program_->unbind();
		}

		explicit operator bool() const
		{
			return !!program_; 
		}

	private :
		std::shared_ptr<Shader_program> program_;

	};

    template <typename T>
    class Shader_uniform {
    private:
        friend class Shader_program;

        Shader_uniform(int location);

    public:
		Shader_uniform() = default;

        void set(const T&);

		bool is_valid() const;

    private:
		int location_{ -1 };
    };

} // namespace graphics
} // namespace kvant