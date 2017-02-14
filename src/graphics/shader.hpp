#pragma once

namespace graphics 
{

	class shader
	{
	public :
		shader(GLenum shader_type, const char* shader_source);
		shader(const shader&) = delete;
		~shader();

		shader& operator=(const shader&) = delete;

	public :
		void attach(GLuint program_handle) const;
		void detach(GLuint program_handle) const;

		void destroy();

	private :
		GLuint handle_{0};
	};

	template <typename T> class uniform;

	class shader_program
	{
	public :
		shader_program(const shader_program&) = delete;

		shader_program(	const shader& vertex_shader, 
						const shader& fragment_shader);

		~shader_program();

		shader_program& operator=(const shader_program&) = delete;

	public :
		void bind();
		void unbind(); 

	public :
		template <typename T>
		uniform<T> get_uniform(const char* uniform_name) const 
		{
			const GLint location = ::glGetUniformLocation(handle_, uniform_name);
			assert(location != -1);
			return uniform<T>(location);
		}

	private :
		static GLuint link(	const shader& vertex_shader, 
							const shader& fragment_shader);

		GLuint handle_{0};
	};

	template <typename T>
	class uniform
	{
	private :
		friend class shader_program;

		uniform(GLuint location)
			: location_(location)
		{ }

	public :
		void set(const T& value);

	private :
		GLuint location_;
	};


	template <>
	inline void uniform<float>::set(const float& value)
	{
		::glUniform1f(location_, value);
	}

	template <>
	inline void uniform<glm::vec3>::set(const glm::vec3& value)
	{
		::glUniform3f(location_, value[0], value[1], value[2]); 
	}

	template <>
	inline void uniform<glm::vec4>::set(const glm::vec4& value)
	{
		// glUniform4fv better?
		::glUniform4f(location_, value[0], value[1], value[2], value[3]);
	}

	void set_uniform(GLuint location, const glm::mat4& value);

	template <>
	inline void uniform<glm::mat4>::set(const glm::mat4& value)
	{
		//::glUniformMatrix4fv(location_, 1, GL_FALSE, glm::value_ptr(value));
		set_uniform(location_, value);
	}

} 
