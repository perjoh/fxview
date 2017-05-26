#include "render.hpp"
#include "../base/file_io.hpp"
#include "check_opengl_error.hpp"
#include <vector>
#include <SDL.h>
#include <memory>
#include <cassert>
#include <map>

namespace kvant {
namespace graphics {

    class Opengl_mesh {
    public:
        Opengl_mesh() = default;
        Opengl_mesh(Opengl_mesh&& other) noexcept;
        ~Opengl_mesh();

        Opengl_mesh(const Opengl_mesh&) = delete;
        Opengl_mesh& operator=(const Opengl_mesh&) = delete;

    public:
        void destroy();

        bool is_valid() const;

        void set_draw_mode(GLenum draw_mode);

        //typedef graphics::mesh::Vertex Vertex;
        void setup_vertex_buffer_object(const Vertex* vertices, size_t vertex_count);
        void setup_element_buffer_object(const GLuint* indices, size_t index_count);
        void setup_vertex_array_object();

        void render();

    private:
        GLuint vertex_buffer_object_{0};
        GLuint vertex_array_object_{0};
        GLuint element_buffer_object_{0};

        GLuint render_count_{0};

        GLenum draw_mode_{GL_TRIANGLES};
    };

    Opengl_mesh::Opengl_mesh(Opengl_mesh&& other) noexcept
        : vertex_buffer_object_{other.vertex_buffer_object_}
        , vertex_array_object_{other.vertex_array_object_}
        , element_buffer_object_{other.element_buffer_object_}
        , render_count_{other.render_count_}
    {
        other.vertex_buffer_object_ = 0;
        other.vertex_array_object_ = 0;
        other.element_buffer_object_ = 0;
        other.render_count_ = 0;
    }

    Opengl_mesh::~Opengl_mesh()
    {
        destroy();
    }

    void Opengl_mesh::destroy()
    {
        if (element_buffer_object_ != 0)
        {
            ::glDeleteBuffers(1, &element_buffer_object_);
            element_buffer_object_ = 0;
        }

        if (vertex_buffer_object_ != 0)
        {
            ::glDeleteBuffers(1, &vertex_buffer_object_);
            vertex_buffer_object_ = 0;
        }

        if (vertex_array_object_ != 0)
        {
            ::glDeleteVertexArrays(1, &vertex_array_object_);
            vertex_array_object_ = 0;
        }
    }

    bool Opengl_mesh::is_valid() const
    {
        return vertex_array_object_ != 0;
    }

    void Opengl_mesh::set_draw_mode(GLenum draw_mode)
    {
        draw_mode_ = draw_mode;
    }

    void Opengl_mesh::setup_vertex_buffer_object(const Vertex* vertices,
                                                 size_t vertex_count)
    {
        ::glGenBuffers(1, &vertex_buffer_object_);

        ::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

        const GLsizeiptr dataSize = sizeof(Vertex) * vertex_count;

        ::glBufferData(GL_ARRAY_BUFFER,
                       dataSize,
                       &vertices[0],
                       GL_STATIC_DRAW);

        ::glBindBuffer(GL_ARRAY_BUFFER, 0);

        check_opengl_error();

        render_count_ = vertex_count;
    }

    void Opengl_mesh::setup_element_buffer_object(const GLuint* indices,
                                                  size_t index_count)
    {
        assert(indices);

        ::glGenBuffers(1, &element_buffer_object_);
        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);

        const size_t index_buffer_size = sizeof(GLuint) * index_count;
        ::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       index_buffer_size,
                       &indices[0],
                       GL_STATIC_DRAW);

        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        render_count_ = index_count; // ???

        check_opengl_error();
    }

    void Opengl_mesh::setup_vertex_array_object()
    {
        assert(vertex_array_object_ == 0);
        assert(vertex_buffer_object_ != 0);

        ::glGenVertexArrays(1, &vertex_array_object_);
        ::glBindVertexArray(vertex_array_object_);

        check_opengl_error();

        // Vertex Buffer Object
        ::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

        check_opengl_error();

        // Element Buffer Object
        if (element_buffer_object_ != 0)
        {
            ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
        }

        const unsigned NUM_ARRAYS = 3;
        for (unsigned index = 0, offset = 0;
             index < NUM_ARRAYS;
             ++index)
        {
            ::glEnableVertexAttribArray(index);

            const GLboolean normalize = GL_FALSE;
            ::glVertexAttribPointer(index,
                                    3,
                                    GL_FLOAT,
                                    normalize,
                                    sizeof(Vertex),
                                    reinterpret_cast<const GLvoid*>(offset));

            offset += sizeof(glm::vec3);

            check_opengl_error();
        }

        ::glBindVertexArray(0);
    }

    void Opengl_mesh::render()
    {
        ::glBindVertexArray(vertex_array_object_);

        if (0 == element_buffer_object_)
        {
            // Individual triangles
            ::glDrawArrays(draw_mode_, 0, render_count_);
        }
        else
        {
            // Indexed triangles
            ::glDrawElements(GL_TRIANGLES, render_count_, GL_UNSIGNED_INT, 0);
        }

        check_opengl_error();

        ::glBindVertexArray(0);
    }

    class Opengl_renderer final : public Renderer {
    public:
        Opengl_renderer() = default;

    private:
        SDL_Window* window_{nullptr};
        SDL_GLContext context_{nullptr};

        void create_windowed(unsigned window_width,
                             unsigned window_height,
                             const char* window_title) override
        {
            assert(window_ == nullptr);

            window_ = ::SDL_CreateWindow(window_title,
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         window_width,
                                         window_height,
                                         SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

            if (nullptr == window_)
            {
                throw std::runtime_error("Failed to create window.");
            }

            context_ = ::SDL_GL_CreateContext(window_);
            if (nullptr == context_)
            {
                throw std::runtime_error("Failed to create OpenGL context.");
            }

            GLenum error = ::glewInit();
            if (error != GLEW_OK)
            {
                throw std::runtime_error("glewInit failed.");
            }

            ::glEnable(GL_DEPTH_TEST);
            ::glEnable(GL_CULL_FACE);
            ::glFrontFace(GL_CW);
            ::glCullFace(GL_BACK);

            // Enable v-sync.
            ::SDL_GL_SetSwapInterval(1);
        }

        void destroy() override
        {
            if (context_)
            {
                ::SDL_GL_DeleteContext(context_);
                context_ = nullptr;
            }

            if (window_)
            {
                ::SDL_DestroyWindow(window_);
                window_ = nullptr;
            }
        }

        void clear_buffers()
        {
            glClearColor(0.0, 1.0, 0.0, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

    private:
        base::Delegate_list<Render_callback> render_callbacks_;

        void register_render_callback(Render_callback callback) override
        {
            render_callbacks_.push_back(callback);
        }

        void begin_render() override
        {
            clear_buffers();
            render_callbacks_();
        }

        void present() override
        {
            ::SDL_GL_SwapWindow(window_);
        }

    private:
        std::vector<Opengl_mesh> allocated_meshes_;
        unsigned num_free_{0}; // Stupid, remove.

        Mesh_id allocate_mesh(const Triangle_mesh<>& tri_mesh) override
        {
            unsigned mesh_id = invalid_mesh_id;

            if (num_free_ > 0)
            {
                for (unsigned i = 0; i < allocated_meshes_.size(); ++i)
                {
                    if (!allocated_meshes_[i].is_valid())
                    {
                        --num_free_;
                        mesh_id = i;
                        break;
                    }
                }
            }
            else
            {
                allocated_meshes_.emplace_back();
                mesh_id = allocated_meshes_.size() - 1;
            }

            Opengl_mesh& m = allocated_meshes_[mesh_id];

            m.setup_vertex_buffer_object(&tri_mesh.vertices[0], tri_mesh.vertices.size());

            if (!tri_mesh.triangles.empty())
            {
                m.setup_element_buffer_object(&tri_mesh.triangles[0].v0, tri_mesh.triangles.size() * 3);
            }

            m.setup_vertex_array_object();

            return mesh_id;
        }

        void deallocate_mesh(Mesh_id mesh_id) override
        {
            allocated_meshes_[mesh_id].destroy();
            ++num_free_;
        }

        void render_mesh(Mesh_id mesh_id) override
        {
            allocated_meshes_[mesh_id].render();
        }

    private:
        using Shader_map = std::map<std::string, std::shared_ptr<Shader_program>>;
        Shader_map shader_map_;

        static std::string read_shader_source(const char* pattern,
                                              Shader::Shader_type shader_type)
        {
            const std::string resource_path("resources/shaders/");
            const char* suffix = (shader_type == Shader::vertex_shader) ? "_vs.glsl"
                                                                        : "_fs.glsl";
            return base::read_textfile((resource_path + pattern + suffix).c_str());
        }

        std::weak_ptr<Shader_program> allocate_shader_program(const char* vs_name,
                                                              const char* fs_name) override
        {
            if (nullptr == fs_name)
            {
                fs_name = vs_name;
            }

            const std::string shader_source_vs = read_shader_source(vs_name, Shader::vertex_shader);
            if (shader_source_vs.length() > 0)
            {
                Shader vertex_shader(Shader::vertex_shader,
                                     shader_source_vs.c_str(),
                                     shader_source_vs.length());

                const std::string shader_source_fs = read_shader_source(fs_name, Shader::fragment_shader);
                if (shader_source_fs.length())
                {
                    Shader fragment_shader(Shader::fragment_shader,
                                           shader_source_fs.c_str(),
                                           shader_source_fs.length());

                    Shader_program program(vertex_shader, fragment_shader);
                    if (program)
                    {
                        const std::string key(std::string(vs_name) + fs_name);
                        std::shared_ptr<Shader_program>& program_ref = shader_map_[key];
						program_ref = std::make_shared<Shader_program>();
                        program_ref->swap(program);
                        return program_ref;
                    }
                }
            }

            return std::shared_ptr<Shader_program>();
        }

    private:
    };

    Renderer& Renderer::instance()
    {
        static Opengl_renderer renderer;
        return renderer;
    }

} // namespace graphics
} // namespace kvant
