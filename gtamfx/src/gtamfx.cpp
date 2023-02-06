#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <gtamfx.hpp>

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
}

namespace {
  struct GlError { const char *name, *description; };
  GlError getGlError_(GLenum code) {
    switch(code) {
    case GL_NO_ERROR:
      return {"No Error", "No error has been recorded. The value of this symbolic constant is guaranteed to be 0."};
    case GL_INVALID_ENUM:
      return {"Invalid Enum", "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other sideeffect than to set the error flag."};
    case GL_INVALID_VALUE:
      return {"Invalid Value", "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag."};
    case GL_INVALID_OPERATION:
      return {"Invalid Operation", "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag."};
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return {"Invalid Framebuffer Operation", "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag."};
    case GL_OUT_OF_MEMORY:
      return {"Out Of Memory", "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."};
    case GL_STACK_UNDERFLOW:
      return {"Stack Underflow", "An attempt has been made to perform an operation that would cause an internal stack to underflow."};
    case GL_STACK_OVERFLOW:
      return {"Stack Overflow", "An attempt has been made to perform an operation that would cause an internal stack to overflow."};
    }
    return {"?", "Unknown"};
  }

  void reportGlErrors_() {
    GLenum code = glGetError();
    if(code == GL_NO_ERROR) return;
    GlError error = getGlError_(code);
    fprintf(stderr, "OpenGL Error: %s (#%u): %s\n", error.name, code, error.description);
  }

  std::string getGlfwError_() {
    const char *description;
    glfwGetError(&description);
    return description;
  }

  glm::mat4 computeTransformMatrix(const gtamfx::Sprite *sprite, const gtamfx::Camera *camera) {
    glm::mat4 model = glm::mat4(1.0f);
    model *= glm::mat4_cast(sprite->rotation);
    model *= glm::translate(glm::mat4(1.0f), sprite->position);
    model *= glm::scale(glm::mat4(1.0f), sprite->scale);

    glm::mat4 view = glm::mat4(1.0f);
    view *= glm::mat4_cast(glm::conjugate(camera->rotation));
    view *= glm::translate(glm::mat4(1.0f), -camera->position);

    glm::mat4 projection;
    switch(camera->type) {
    case gtamfx::CameraType::Orthographic:
      projection = glm::ortho(
        -camera->orthographic.size.x / 2,
        +camera->orthographic.size.x / 2,
        -camera->orthographic.size.y / 2,
        +camera->orthographic.size.y / 2
      );
      break;
    case gtamfx::CameraType::Perspective:
      projection = glm::perspective(
        camera->perspective.fov,
        camera->perspective.aspect,
        camera->perspective.zNear,
        camera->perspective.zFar
      );
      break;
    default:
      projection = glm::mat4(1.0f);
    }

    return projection * view * model;
  }
}

namespace gtamfx {
  struct WindowImpl_ {
    std::vector<std::unique_ptr<Texture>> textures;
    std::vector<std::unique_ptr<Sprite>> sprites;
    std::vector<std::unique_ptr<Shader>> shaders;
    std::vector<std::unique_ptr<Camera>> cameras;
    Camera *activeCamera = nullptr;
    GLFWwindow *window = nullptr;
    GLuint vao = 0;

    bool didReportNoActiveCamera = false;
  };

  void Window::init() {
    impl_ = new WindowImpl_;

    if(!glfwInit()) throw Exception{ ExceptionType::GlfwFailedInit, getGlfwError_() };
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    impl_->window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
    if(!impl_->window) throw Exception{ ExceptionType::GlfwFailedCreateWindow, getGlfwError_() };
    
    glfwMakeContextCurrent(impl_->window);

    // does nothing that stops us from calling this function multiple times
    int res = gl3wInit2(&glfwGetProcAddress);
    
    if(res == GL3W_ERROR_INIT) throw Exception{ ExceptionType::Gl3wFailedInit, "?" };

    struct { GLint major, minor; } version;
    glGetIntegerv(GL_MAJOR_VERSION, &version.major);
    glGetIntegerv(GL_MINOR_VERSION, &version.minor);

    if(res == GL3W_ERROR_OPENGL_VERSION)
      throw Exception{
        ExceptionType::Gl3wFailedInit,
        std::to_string(version.major) + "." + std::to_string(version.minor)
      };

    glGenVertexArrays(1, &impl_->vao);
    glBindVertexArray(impl_->vao);
  }

  void Window::deinit() {
    for(auto &shader : impl_->shaders) {
      glDeleteProgram(shader->id);
      shader->id = 0;
    }

    for(auto &texture : impl_->textures) {
      glDeleteTextures(1, &texture->id);
      texture->id = 0;
    }

    glDeleteVertexArrays(1, &impl_->vao);

    glfwDestroyWindow(impl_->window);
    impl_->window = nullptr;
    glfwTerminate();
  }

  bool Window::shouldClose() const { return glfwWindowShouldClose(impl_->window); }
  void Window::close() { return glfwSetWindowShouldClose(impl_->window, true); }
  void Window::unclose() { return glfwSetWindowShouldClose(impl_->window, false); }

  void Window::update(bool depth) {
    glfwPollEvents();
    if(!getActiveCamera()) {
      if(!impl_->didReportNoActiveCamera) {
        fputs("No active camera!\n", stderr);
        impl_->didReportNoActiveCamera = true;
      }
      return;
    }

    impl_->didReportNoActiveCamera = false;

    glClearColor(0.1415f, 0.05f, 0.13f, 1.0f);

    if(depth) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | (depth ? GL_DEPTH_BUFFER_BIT : 0));

    if(!impl_->sprites.size()) return;
    GLuint lastProgram = 0, lastTexture = 0;
    for(const auto &sprite : impl_->sprites) {
      if(sprite->shader->id != lastProgram || lastProgram == 0) {
        glUseProgram(lastProgram = sprite->shader->id);
        // fprintf(stderr, "Using program #%u\n", lastProgram);
      }

      if(sprite->texture.source->id != lastTexture || lastTexture == 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lastTexture = sprite->texture.source->id);
        // fprintf(stderr, "Binding texture #%u\n", lastTexture);
      }

      if(sprite->shader->uniforms.transform != -1) {
        glm::mat4 transform = computeTransformMatrix(sprite.get(), getActiveCamera());
        glUniformMatrix4fv(sprite->shader->uniforms.transform, 1, GL_FALSE, glm::value_ptr(transform));
      }
      
      if(sprite->shader->uniforms.texture != -1) {
        glUniform1i(sprite->shader->uniforms.texture, 0);
      }
      
      if(sprite->shader->uniforms.textureView != -1) {
        glUniform4f(
          sprite->shader->uniforms.textureView,
          sprite->texture.position.x,
          sprite->texture.position.y,
          sprite->texture.scale.x,
          sprite->texture.scale.y
        );
      }
      
      // fprintf(stderr, "Drawing %zu vertex(es), %zu element(s)\n",
      //   sprite->shader->vertexCount,
      //   sprite->shader->vertexCount < 2 ? 0 : sprite->shader->vertexCount - 2
      // );

      glDrawArrays(GL_TRIANGLE_STRIP, 0, sprite->shader->vertexCount);
      reportGlErrors_();
    }
    glfwSwapBuffers(impl_->window);
  }

  Camera *Window::getActiveCamera() const { return impl_->activeCamera; }
  void Window::setActiveCamera(Camera *camera) { impl_->activeCamera = camera; }

  Texture *Window::newTexture(const char *path) {
    int width, height, channelCount;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &channelCount, 4);
    if(!data) throw Exception{ ExceptionType::TextureLoadFail, stbi_failure_reason() };

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    impl_->textures.push_back(std::make_unique<Texture>());
    Texture *texture = impl_->textures.back().get();
    texture->id = tex;
    texture->size = {width, height};
    return texture;
  }

  void Window::delTexture(Texture *texture) {
    if(texture == NULL) return;
    size_t index;
    for(index = 0; index <= impl_->textures.size(); ++index) {
      if(impl_->textures.size() == index) return;
      if(impl_->textures[index].get() == texture) break;
    }

    glDeleteTextures(1, &impl_->textures[index]->id);
    impl_->textures.erase(impl_->textures.begin() + index);
  }

  Sprite *Window::newSprite(Texture *texture, Shader *shader) {
    impl_->sprites.push_back(std::make_unique<Sprite>());
    Sprite *sprite = impl_->sprites.back().get();
    sprite->texture.source = texture;
    sprite->texture.position = {0, 0};
    sprite->texture.scale = {1, 1};
    sprite->position = {0, 0, 0};
    sprite->scale = {texture->size.x, texture->size.y, 1};
    sprite->rotation = glm::identity<glm::quat>();
    sprite->shader = shader;
    return sprite;
  }

  void Window::delSprite(Sprite *sprite) {
    if(sprite == NULL) return;
    size_t index;
    for(index = 0; index <= impl_->sprites.size(); ++index) {
      if(impl_->sprites.size() == index) return;
      if(impl_->sprites[index].get() == sprite) break;
    }

    impl_->sprites.erase(impl_->sprites.begin() + index);
  }
  
  Shader *Window::newShader(const char *vertex_source, const char *fragment_source, size_t vertexCount) {
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertex_source, NULL);
    glCompileShader(vshader);

    GLint vertex_compiled;
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &vertex_compiled);
    if(vertex_compiled != GL_TRUE) {
      GLchar message[1024];
      glGetShaderInfoLog(vshader, 1024, NULL, message);
      throw Exception{
        ExceptionType::ShaderLoadFail,
        "[vertex shader] " + std::string(message)
      };
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragment_source, NULL);
    glCompileShader(fshader);

    GLint fragment_compiled;
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &fragment_compiled);
    if(fragment_compiled != GL_TRUE) {
      GLchar message[1024];
      glGetShaderInfoLog(fshader, 1024, NULL, message);
      throw Exception{
        ExceptionType::ShaderLoadFail,
        "[fragment shader] " + std::string(message)
      };
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    GLint program_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    if(program_linked != GL_TRUE) {
      GLchar message[1024];
      glGetProgramInfoLog(program, 1024, NULL, message);
      throw Exception{
        ExceptionType::ShaderLoadFail,
        "[program] " + std::string(message)
      };
    }

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    impl_->shaders.push_back(std::make_unique<Shader>());
    Shader *shader = impl_->shaders.back().get();
    shader->id = program;
    shader->vertexCount = vertexCount;
    shader->uniforms.transform = glGetUniformLocation(program, "uTransform");
    shader->uniforms.texture = glGetUniformLocation(program, "uTexture");
    shader->uniforms.textureView = glGetUniformLocation(program, "uTextureView");
    return shader;
  }

  void Window::delShader(Shader *shader) {
    if(shader == NULL) return;
    size_t index;
    for(index = 0; index <= impl_->shaders.size(); ++index) {
      if(impl_->shaders.size() == index) return;
      if(impl_->shaders[index].get() == shader) break;
    }

    glDeleteProgram(impl_->shaders[index]->id);
    impl_->shaders.erase(impl_->shaders.begin() + index);
  }

  Camera *Window::newCamera(CameraType type) {
    impl_->cameras.push_back(std::make_unique<Camera>());
    Camera *camera = impl_->cameras.back().get();
    camera->type = type;
    camera->position = {0, 0, 0};
    camera->rotation = glm::identity<glm::quat>();
    switch(type) {
    case CameraType::Orthographic:
      camera->orthographic.size = getFramebufferSize();
      break;
    case CameraType::Perspective:
      camera->perspective.aspect = getAspectRatio();
      camera->perspective.fov = 60.0f;
      camera->perspective.zNear = 0.01f;
      camera->perspective.zFar = 100.0f;
      break;
    }
    return camera;
  }

  void Window::delCamera(Camera *camera) {
    if(camera == NULL) return;
    size_t index;
    for(index = 0; index <= impl_->cameras.size(); ++index) {
      if(impl_->cameras.size() == index) return;
      if(impl_->cameras[index].get() == camera) break;
    }

    impl_->cameras.erase(impl_->cameras.begin() + index);
  }

  glm::vec2 Window::getFramebufferSize() const {
    glm::ivec2 size;
    glfwGetFramebufferSize(impl_->window, &size.x, &size.y);
    return size;
  }

  float Window::getAspectRatio() const {
    glm::vec2 framebufferSize = getFramebufferSize();
    return framebufferSize.y / framebufferSize.x;
  }

  float Window::getTime() { return glfwGetTime(); }

  bool Window::isKeyDown(KeyCode key) { return glfwGetKey(impl_->window, (int)key); }
  bool Window::isMouseDown(int button) { return glfwGetMouseButton(impl_->window, button); }

  glm::vec2 Window::getMousePosition() {
    glm::dvec2 pos;
    glfwGetCursorPos(impl_->window, &pos.x, &pos.y);
    return pos;
  }
}
