#include "shader.hpp"
#include <cassert>
#include <vector>
#include <stdexcept>
#include "check_opengl_error.hpp"

namespace graphics {
namespace render {

	GLuint create_shader_from_source(GLenum shader_type,
									 const char* source)
	{
		// Compile source code
		GLuint shader = glCreateShader(shader_type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);

		// Get status of compilation
		GLint status = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (GL_FALSE == status)
		{
			GLint infoLogLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::vector<GLchar> infoLog(infoLogLength + 1);
			glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);

			throw std::runtime_error(&infoLog[0]);
		}

		return shader;
	}


	Shader::Shader(GLenum shader_type, const char* shader_source)
		: handle_{ create_shader_from_source(shader_type, shader_source) }
	{
	}


	Shader::~Shader()
	{
		destroy();
	}


	void Shader::attach(GLuint program_handle) const
	{
		assert(handle_ != 0);
		::glAttachShader(program_handle, handle_);
	}


	void Shader::detach(GLuint program_handle) const
	{
		assert(handle_ != 0);
		::glDetachShader(program_handle, handle_);
	}


	void Shader::destroy()
	{
		if (handle_ != 0)
		{
			::glDeleteShader(handle_);
			handle_ = 0;
		}
	}



	Shader_program::Shader_program(const Shader& vertex_shader, const Shader& fragment_shader)
		: handle_{ link(vertex_shader, fragment_shader) }
	{
	}


	Shader_program::~Shader_program()
	{
		if (handle_ != 0)
		{
			::glDeleteProgram(handle_);
		}
	}


	void Shader_program::bind()
	{
		assert(handle_ != 0);
		::glUseProgram(handle_);
		check_opengl_error();
	}


	void Shader_program::unbind()
	{
		::glUseProgram(0);
	}


	GLuint Shader_program::link(const Shader& vertex_shader, const Shader& fragment_shader)
	{
		GLuint program_handle = ::glCreateProgram();

		vertex_shader.attach(program_handle);
		fragment_shader.attach(program_handle);

		::glLinkProgram(program_handle);

		// Get linking status
		GLint status = GL_FALSE;
		::glGetProgramiv(program_handle,
						 GL_LINK_STATUS,
						 &status);

		if (status == GL_FALSE)
		{
			GLint log_length = 0;
			::glGetProgramiv(program_handle,
							 GL_INFO_LOG_LENGTH,
							 &log_length);

			std::vector<char> infoLog(log_length);

			::glGetProgramInfoLog(program_handle,
								  log_length,
								  nullptr,
								  &infoLog[0]);

			throw std::runtime_error(&infoLog[0]);
		}

		fragment_shader.detach(program_handle);
		vertex_shader.detach(program_handle);

		return program_handle;
	}

	void set_uniform(GLuint location,
					 const glm::mat3& value)
	{
		::glUniformMatrix3fv(location,
							 1,
							 GL_FALSE,
							 glm::value_ptr(value));
	}

	void set_uniform(GLuint location,
					 const glm::mat4& value)
	{
		::glUniformMatrix4fv(location,
							 1,
							 GL_FALSE,
							 glm::value_ptr(value));
	}
} }
