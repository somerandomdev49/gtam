#ifndef GTAMFX_CWRAP_HEADER_
#define GTAMFX_CWRAP_HEADER_

#include <stddef.h>

#ifdef _WIN32
#define EXPORT __dllspec(dllexport)
#else
#define EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct GtamVec2 {
  float x, y;
};
struct GtamVec2i {
  int x, y;
};
struct GtamVec2d {
  double x, y;
};
struct GtamVec3 {
  float x, y, z;
};
struct GtamVec3i {
  int x, y, z;
};
struct GtamVec3d {
  double x, y, z;
};
struct GtamVec4 {
  float x, y, z, w;
};
struct GtamVec4i {
  int x, y, z, w;
};
struct GtamVec4d {
  double x, y, z, w;
};
struct GtamQuat {
  double w, x, y, z;
};

typedef struct GtamWindow_T GtamWindow;

typedef struct GtamTexture_T {
  unsigned int id;
  struct GtamVec2 size;
} GtamTexture;

typedef struct GtamShader_T {
  unsigned int id;
  size_t vertexCount;
} GtamShader;

struct GtamTextureView {
  struct GtamVec2 position, scale;
  GtamTexture *source;
};

typedef struct GtamSprite_T {
  struct GtamTextureView texture;
  GtamShader *shader;
  struct GtamVec4 color;
  struct GtamVec3 position;
  struct GtamVec3 scale;
  struct GtamQuat rotation;
} GtamSprite;

#define GTAM_CAMERA_TYPE_ORTHOGRAPHIC 0
#define GTAM_CAMERA_TYPE_PERSPECTIVE 1

typedef struct GtamCamera_T {
  int type;
  struct GtamVec3 position;
  struct GtamQuat rotation;
  union {
    struct {
      float fov;
      float aspect;
      float zNear;
      float zFar;
    } perspective;

    struct {
      struct GtamVec2 size;
      float left, right, bottom, top;
    } orthographic;
  };
} GtamCamera;

#define GTAM_ERROR_NONE 0
#define GTAM_ERROR_GLFW_FAILED_INIT 1
#define GTAM_ERROR_GLFW_FAILED_CREATE_WINDOW 2
#define GTAM_ERROR_GL3W_FAILED_INIT 3
#define GTAM_ERROR_GL3W_BAD_VERSION 4
#define GTAM_ERROR_TEXTURE_LOAD_FAIL 5
#define GTAM_ERROR_SHADER_LOAD_FAIL 6

EXPORT int gtamGetError(void);
EXPORT const char *gtamGetErrorMessage(void);

EXPORT GtamWindow *gtamCreateWindow(struct GtamVec2i size, const char *title);
EXPORT void gtamDestroyWindow(GtamWindow *window);
EXPORT void gtamInitWindow(GtamWindow *window);
EXPORT int gtamWindowShouldClose(const GtamWindow *window);
EXPORT void gtamCloseWindow(GtamWindow *window);
EXPORT void gtamUncloseWindow(GtamWindow *window);
EXPORT void gtamUpdateWindow(GtamWindow *window, int depth);
EXPORT void gtamDeinitWindow(GtamWindow *window);
EXPORT float gtamWindowGetTime(GtamWindow *window);
EXPORT int gtamWindowIsKeyDown(GtamWindow *window, int keycode);
EXPORT int gtamWindowIsMouseDown(GtamWindow *window, int button);
EXPORT void gtamWindowGetMousePosition(GtamWindow *window,
                                       struct GtamVec2 *position);
EXPORT GtamTexture *gtamWindowNewTexture(GtamWindow *window, const char *path);
EXPORT void gtamWindowDelTexture(GtamWindow *window, GtamTexture *texture);
EXPORT GtamShader *gtamWindowNewShader(GtamWindow *window, const char *vertex,
                                       const char *fragment,
                                       size_t vertexCount);
EXPORT void gtamWindowDelShader(GtamWindow *window, GtamShader *shader);
EXPORT GtamSprite *gtamWindowNewSprite(GtamWindow *window, GtamTexture *texture,
                                       GtamShader *shader);
EXPORT void gtamWindowDelSprite(GtamWindow *window, GtamSprite *sprite);
EXPORT GtamCamera *gtamWindowNewCamera(GtamWindow *window, int type);
EXPORT void gtamWindowDelCamera(GtamWindow *window, GtamCamera *camera);
EXPORT void gtamWindowSetActiveCamera(GtamWindow *window, GtamCamera *camera);
EXPORT GtamCamera *gtamWindowGetActiveCamera(const GtamWindow *window);
EXPORT void gtamWindowGetFramebufferSize(const GtamWindow *window,
                                         struct GtamVec2 *framebufferSize);
EXPORT float gtamWindowGetAspectRatio(const GtamWindow *window);

enum GtamKeyCode {
  GTAM_KEYCODE_UNKNOWN = -1,
  GTAM_KEYCODE_SPACE = 32,
  GTAM_KEYCODE_APOSTROPHE = 39,
  GTAM_KEYCODE_COMMA = 44,
  GTAM_KEYCODE_MINUS = 45,
  GTAM_KEYCODE_PERIOD = 46,
  GTAM_KEYCODE_SLASH = 47,
  GTAM_KEYCODE_ALPHA0 = 48,
  GTAM_KEYCODE_ALPHA1 = 49,
  GTAM_KEYCODE_ALPHA2 = 50,
  GTAM_KEYCODE_ALPHA3 = 51,
  GTAM_KEYCODE_ALPHA4 = 52,
  GTAM_KEYCODE_ALPHA5 = 53,
  GTAM_KEYCODE_ALPHA6 = 54,
  GTAM_KEYCODE_ALPHA7 = 55,
  GTAM_KEYCODE_ALPHA8 = 56,
  GTAM_KEYCODE_ALPHA9 = 57,
  GTAM_KEYCODE_SEMI_COLON = 59,
  GTAM_KEYCODE_EQUAL = 61,
  GTAM_KEYCODE_A = 65,
  GTAM_KEYCODE_B = 66,
  GTAM_KEYCODE_C = 67,
  GTAM_KEYCODE_D = 68,
  GTAM_KEYCODE_E = 69,
  GTAM_KEYCODE_F = 70,
  GTAM_KEYCODE_G = 71,
  GTAM_KEYCODE_H = 72,
  GTAM_KEYCODE_I = 73,
  GTAM_KEYCODE_J = 74,
  GTAM_KEYCODE_K = 75,
  GTAM_KEYCODE_L = 76,
  GTAM_KEYCODE_M = 77,
  GTAM_KEYCODE_N = 78,
  GTAM_KEYCODE_O = 79,
  GTAM_KEYCODE_P = 80,
  GTAM_KEYCODE_Q = 81,
  GTAM_KEYCODE_R = 82,
  GTAM_KEYCODE_S = 83,
  GTAM_KEYCODE_T = 84,
  GTAM_KEYCODE_U = 85,
  GTAM_KEYCODE_V = 86,
  GTAM_KEYCODE_W = 87,
  GTAM_KEYCODE_X = 88,
  GTAM_KEYCODE_Y = 89,
  GTAM_KEYCODE_Z = 90,
  GTAM_KEYCODE_LEFT_BRACKET = 91,
  GTAM_KEYCODE_BACKSLASH = 92,
  GTAM_KEYCODE_RIGHT_BRACKET = 93,
  GTAM_KEYCODE_GRAVE_ACCENT = 96,
  GTAM_KEYCODE_WORLD1 = 161,
  GTAM_KEYCODE_WORLD2 = 162,
  GTAM_KEYCODE_ESCAPE = 256,
  GTAM_KEYCODE_ENTER = 257,
  GTAM_KEYCODE_TAB = 258,
  GTAM_KEYCODE_BACKSPACE = 259,
  GTAM_KEYCODE_INSERT = 260,
  GTAM_KEYCODE_DELETE = 261,
  GTAM_KEYCODE_RIGHT = 262,
  GTAM_KEYCODE_LEFT = 263,
  GTAM_KEYCODE_DOWN = 264,
  GTAM_KEYCODE_UP = 265,
  GTAM_KEYCODE_PAGE_UP = 266,
  GTAM_KEYCODE_PAGE_DOWN = 267,
  GTAM_KEYCODE_HOME = 268,
  GTAM_KEYCODE_END = 269,
  GTAM_KEYCODE_CAPS_LOCK = 280,
  GTAM_KEYCODE_SCROLL_LOCK = 281,
  GTAM_KEYCODE_NUM_LOCK = 282,
  GTAM_KEYCODE_PRINT_SCREEN = 283,
  GTAM_KEYCODE_PAUSE = 284,
  GTAM_KEYCODE_F1 = 290,
  GTAM_KEYCODE_F2 = 291,
  GTAM_KEYCODE_F3 = 292,
  GTAM_KEYCODE_F4 = 293,
  GTAM_KEYCODE_F5 = 294,
  GTAM_KEYCODE_F6 = 295,
  GTAM_KEYCODE_F7 = 296,
  GTAM_KEYCODE_F8 = 297,
  GTAM_KEYCODE_F9 = 298,
  GTAM_KEYCODE_F10 = 299,
  GTAM_KEYCODE_F11 = 300,
  GTAM_KEYCODE_F12 = 301,
  GTAM_KEYCODE_F13 = 302,
  GTAM_KEYCODE_F14 = 303,
  GTAM_KEYCODE_F15 = 304,
  GTAM_KEYCODE_F16 = 305,
  GTAM_KEYCODE_F17 = 306,
  GTAM_KEYCODE_F18 = 307,
  GTAM_KEYCODE_F19 = 308,
  GTAM_KEYCODE_F20 = 309,
  GTAM_KEYCODE_F21 = 310,
  GTAM_KEYCODE_F22 = 311,
  GTAM_KEYCODE_F23 = 312,
  GTAM_KEYCODE_F24 = 313,
  GTAM_KEYCODE_F25 = 314,
  GTAM_KEYCODE_NUMPAD0 = 320,
  GTAM_KEYCODE_NUMPAD1 = 321,
  GTAM_KEYCODE_NUMPAD2 = 322,
  GTAM_KEYCODE_NUMPAD3 = 323,
  GTAM_KEYCODE_NUMPAD4 = 324,
  GTAM_KEYCODE_NUMPAD5 = 325,
  GTAM_KEYCODE_NUMPAD6 = 326,
  GTAM_KEYCODE_NUMPAD7 = 327,
  GTAM_KEYCODE_NUMPAD8 = 328,
  GTAM_KEYCODE_NUMPAD9 = 329,
  GTAM_KEYCODE_NUMPAD_DECIMAL = 330,
  GTAM_KEYCODE_NUMPAD_DIVIDE = 331,
  GTAM_KEYCODE_NUMPAD_MULTIPLY = 332,
  GTAM_KEYCODE_NUMPAD_SUBTRACT = 333,
  GTAM_KEYCODE_NUMPAD_ADD = 334,
  GTAM_KEYCODE_NUMPAD_ENTER = 335,
  GTAM_KEYCODE_NUMPAD_EQUAL = 336,
  GTAM_KEYCODE_LEFT_SHIFT = 340,
  GTAM_KEYCODE_LEFT_CONTROL = 341,
  GTAM_KEYCODE_LEFT_ALT = 342,
  GTAM_KEYCODE_LEFT_SUPER = 343,
  GTAM_KEYCODE_RIGHT_SHIFT = 344,
  GTAM_KEYCODE_RIGHT_CONTROL = 345,
  GTAM_KEYCODE_RIGHT_ALT = 346,
  GTAM_KEYCODE_RIGHT_SUPER = 347,
  GTAM_KEYCODE_MENU = 348
};

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GTAMFX_CWRAP_HEADER_
