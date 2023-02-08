#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <string>
#include <vector>

// everything here is slow, cache unfriendly, dumb
// but i don't really care that much tbh...

namespace gtamfx {
enum class ExceptionType {
  None = 0,
  GlfwFailedInit = 1,
  GlfwFailedCreateWindow = 2,
  Gl3wFailedInit = 3,
  Gl3wBadVersion = 4,
  TextureLoadFail = 5,
  ShaderLoadFail = 6
};

struct Exception {
  ExceptionType type;
  std::string message;
};

struct Texture {
  GLuint id;
  glm::vec2 size;
};

struct Shader {
  GLuint id;
  size_t vertexCount;
  struct {
    GLint transform;
    GLint texture;
    GLint textureView;
  } uniforms;
};

struct TextureView {
  glm::vec2 position, scale;
  Texture *source;
};

struct Sprite {
  TextureView texture;
  Shader *shader;
  glm::vec4 color;
  glm::vec3 position;
  glm::vec3 scale;
  glm::quat rotation;
};

enum class CameraType : int { Orthographic = 0, Perspective = 1 };

struct Camera {
  CameraType type;
  glm::vec3 position;
  glm::quat rotation;
  union {
    struct {
      float fov;
      float aspect;
      float zNear;
      float zFar;
    } perspective;

    struct {
      glm::vec2 size;
      float left, right, bottom, top;
    } orthographic;
  };
};

enum class KeyCode;

class Window {
public:
  Window(glm::ivec2 size, const char *title) : size(size), title(title) {}

  void init();
  bool shouldClose() const;
  void close();
  void unclose();
  void update(bool depth = false);
  void deinit();

  float getTime();

  bool isKeyDown(KeyCode key);
  bool isMouseDown(int button);
  glm::vec2 getMousePosition();

  Texture *newTexture(const char *path);
  void delTexture(Texture *texture);

  Sprite *newSprite(Texture *texture, Shader *shader);
  void delSprite(Sprite *sprite);

  Shader *newShader(const char *vertex, const char *fragment,
                    size_t vertexCount);
  void delShader(Shader *shader);

  Camera *newCamera(CameraType type);
  void delCamera(Camera *camera);

  Camera *getActiveCamera() const;
  void setActiveCamera(Camera *camera);

  glm::vec2 getFramebufferSize() const;
  float getAspectRatio() const;

private:
  glm::vec2 size;
  std::string title;
  struct WindowImpl_ *impl_;
};

enum class KeyCode {
  Unknown = -1,
  Space = 32,
  Apostrophe = 39,
  Comma = 44,
  Minus = 45,
  Period = 46,
  Slash = 47,
  Alpha0 = 48,
  Alpha1 = 49,
  Alpha2 = 50,
  Alpha3 = 51,
  Alpha4 = 52,
  Alpha5 = 53,
  Alpha6 = 54,
  Alpha7 = 55,
  Alpha8 = 56,
  Alpha9 = 57,
  SemiColon = 59,
  Equal = 61,
  A = 65,
  B = 66,
  C = 67,
  D = 68,
  E = 69,
  F = 70,
  G = 71,
  H = 72,
  I = 73,
  J = 74,
  K = 75,
  L = 76,
  M = 77,
  N = 78,
  O = 79,
  P = 80,
  Q = 81,
  R = 82,
  S = 83,
  T = 84,
  U = 85,
  V = 86,
  W = 87,
  X = 88,
  Y = 89,
  Z = 90,
  LeftBracket = 91,
  Backslash = 92,
  RightBracket = 93,
  GraveAccent = 96,
  World1 = 161,
  World2 = 162,
  Escape = 256,
  Enter = 257,
  Tab = 258,
  Backspace = 259,
  Insert = 260,
  Delete = 261,
  Right = 262,
  Left = 263,
  Down = 264,
  Up = 265,
  PageUp = 266,
  PageDown = 267,
  Home = 268,
  End = 269,
  CapsLock = 280,
  ScrollLock = 281,
  NumLock = 282,
  PrintScreen = 283,
  Pause = 284,
  F1 = 290,
  F2 = 291,
  F3 = 292,
  F4 = 293,
  F5 = 294,
  F6 = 295,
  F7 = 296,
  F8 = 297,
  F9 = 298,
  F10 = 299,
  F11 = 300,
  F12 = 301,
  F13 = 302,
  F14 = 303,
  F15 = 304,
  F16 = 305,
  F17 = 306,
  F18 = 307,
  F19 = 308,
  F20 = 309,
  F21 = 310,
  F22 = 311,
  F23 = 312,
  F24 = 313,
  F25 = 314,
  Numpad0 = 320,
  Numpad1 = 321,
  Numpad2 = 322,
  Numpad3 = 323,
  Numpad4 = 324,
  Numpad5 = 325,
  Numpad6 = 326,
  Numpad7 = 327,
  Numpad8 = 328,
  Numpad9 = 329,
  NumpadDecimal = 330,
  NumpadDivide = 331,
  NumpadMultiply = 332,
  NumpadSubtract = 333,
  NumpadAdd = 334,
  NumpadEnter = 335,
  NumpadEqual = 336,
  LeftShift = 340,
  LeftControl = 341,
  LeftAlt = 342,
  LeftSuper = 343,
  RightShift = 344,
  RightControl = 345,
  RightAlt = 346,
  RightSuper = 347,
  Menu = 348
};
} // namespace gtamfx
