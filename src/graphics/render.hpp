#pragma once
#include "mesh.hpp"
#include "shader.hpp"
#include "../base/fast_delegate.hpp"
#include <memory>

namespace kvant {
namespace graphics {

    // Note: If inherited class is marked 'final', virtual function calls 
	// should, in this case, be optimized out.

    class Renderer {
    public:
        static Renderer& instance();

    public:
        virtual void create_windowed(unsigned window_width,
                                     unsigned window_height,
                                     const char* window_title) = 0;

        virtual void destroy() = 0;

    public:
		using Render_callback = base::Fast_delegate<void>;
        virtual void register_render_callback(Render_callback) = 0; 

        virtual void begin_render() = 0;
        virtual void present() = 0;

    public:
        static const unsigned invalid_mesh_id{~0ul};
        using Mesh_id = unsigned;

        virtual Mesh_id allocate_mesh(const Triangle_mesh<>&) = 0;
        virtual void deallocate_mesh(Mesh_id mesh_id) = 0;

        virtual void render_mesh(Mesh_id mesh_id) = 0;

    public:
        // Shader scripts must be named as 'name_vs.glsl' or 'name_fs.glsl'.
        // Examples:
        // To load basic_vs.glsl and basic_fs.glsl: Renderer::instance().allocate_shader_program("basic");
        // To load basic_vs.glsl and nice_fs.glsl: Renderer::instance().allocate_shader_program("basic", "nice");
        virtual std::weak_ptr<Shader_program> allocate_shader_program(const char* vs_name,
                                                                      const char* fs_name = nullptr) = 0;

    public:
        virtual ~Renderer() = default;
    };

	template <typename T, void (T::*Fun)()>
	void register_render_callback(T* that)
	{
		Renderer::instance().register_render_callback(Renderer::Render_callback::construct<T, Fun>(that));
	};

} // namespace graphics
} // namespace kvant
