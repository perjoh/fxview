#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace graphics {
namespace render {

	class Shader {
	public:
		Shader(GLenum shader_type, const char* shader_source);
		Shader(const Shader&) = delete;
		~Shader();

		Shader& operator=(const Shader&) = delete;

	public:
		void attach(GLuint program_handle) const;
		void detach(GLuint program_handle) const;

		void destroy();

	private:
		GLuint handle_{ 0 };
	};

	template <typename T> class Uniform;

	class Shader_program {
	public:
		Shader_program(const Shader_program&) = delete;

		Shader_program(const Shader& vertex_shader,
					   const Shader& fragment_shader);

		~Shader_program();

		Shader_program& operator=(const Shader_program&) = delete;

	public:
		void bind();
		void unbind();

	public:
		template <typename T>
		Uniform<T> get_uniform(const char* uniform_name) const
		{
			const GLint location = ::glGetUniformLocation(handle_, uniform_name);
			assert(location != -1 && "Uniform not found.");
			return Uniform<T>(location);
		}

	private:
		static GLuint link(const Shader& vertex_shader,
						   const Shader& fragment_shader);

		GLuint handle_{ 0 };
	};

	template <typename T>
	class Uniform
	{
	private:
		friend class Shader_program;

		Uniform(GLuint location)
			: location_(location)
		{ }

	public:
		void set(const T&) { assert(false); }

	private:
		GLuint location_;
	};


	template <>
	inline void Uniform<float>::set(const float& value)
	{
		::glUniform1f(location_, value);
	}

	template <>
	inline void Uniform<glm::vec3>::set(const glm::vec3& value)
	{
		::glUniform3f(location_, value[0], value[1], value[2]);
	}

	template <>
	inline void Uniform<glm::vec4>::set(const glm::vec4& value)
	{
		// glUniform4fv better?
		::glUniform4f(location_, value[0], value[1], value[2], value[3]);
	}

	void set_uniform(GLuint location, const glm::mat3& value);
	void set_uniform(GLuint location, const glm::mat4& value);

	template <>
	inline void Uniform<glm::mat3>::set(const glm::mat3& value)
	{
		set_uniform(location_, value);
	}

	template <>
	inline void Uniform<glm::mat4>::set(const glm::mat4& value)
	{
		set_uniform(location_, value);
	}

} }
