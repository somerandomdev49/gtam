#include <gtamfx.hpp>
#include <cgtamfx.h>

struct GtamWindow_T { gtamfx::Window v; };
// struct GtamTexture_T { gtamfx::Texture v; };
// struct GtamSprite_T { gtamfx::Sprite v; };
// struct GtamShader_T { gtamfx::Shader v; };
// struct GtamCamera_T { gtamfx::Camera v; };

#define E(CODE) try { CODE; } catch(gtamfx::Exception e) { error_.code = (int)e.type; error_.message = e.message; }

static struct {
  int code;
  std::string message;
} error_;

template<typename T, typename U> static inline void write2(T *v, U w) { v->x = w.x; v->y = w.y; }
template<typename T, typename U> static inline void write3(T *v, U w) { v->x = w.x; v->y = w.y; v->z = w.z; }

extern "C" {

EXPORT int gtamGetError(void) { return error_.code; }
EXPORT const char *gtamGetErrorMessage(void) { return error_.code ? error_.message.c_str() : NULL; }
EXPORT GtamWindow *gtamCreateWindow(GtamVec2i size, const char *title) { E(return new GtamWindow { gtamfx::Window({size.x, size.y}, title) }); return NULL; }
EXPORT void gtamDestroyWindow(GtamWindow *window) { delete window; }
EXPORT void gtamInitWindow(GtamWindow *window) { E(window->v.init()); }
EXPORT int gtamWindowShouldClose(const GtamWindow *window) { return window->v.shouldClose(); }
EXPORT void gtamCloseWindow(GtamWindow *window) { window->v.close(); }
EXPORT void gtamUncloseWindow(GtamWindow *window) { window->v.unclose(); }
EXPORT void gtamUpdateWindow(GtamWindow *window, int depth) { E(window->v.update(depth)); }
EXPORT void gtamDeinitWindow(GtamWindow *window) { E(window->v.deinit()); }
EXPORT float gtamWindowGetTime(GtamWindow *window) { return window->v.getTime(); }
EXPORT int gtamWindowIsKeyDown(GtamWindow *window, int keycode) { return window->v.isKeyDown((gtamfx::KeyCode)keycode); }
EXPORT int gtamWindowIsMouseDown(GtamWindow *window, int button) { return window->v.isMouseDown(button); }
EXPORT void gtamWindowGetMousePosition(GtamWindow *window, GtamVec2 *position) { write2(position, window->v.getMousePosition()); }
EXPORT GtamTexture *gtamWindowNewTexture(GtamWindow *window, const char *path) { E(return (GtamTexture*)window->v.newTexture(path)); return NULL; }
EXPORT void gtamWindowDelTexture(GtamWindow *window, GtamTexture *texture) { window->v.delTexture((gtamfx::Texture*)texture); }
EXPORT GtamShader *gtamWindowNewShader(GtamWindow *window, const char *vertex, const char *fragment, size_t vertexCount)
  { E(return (GtamShader*)window->v.newShader(vertex, fragment, vertexCount)); return NULL; }
EXPORT void gtamWindowDelShader(GtamWindow *window, GtamShader *shader) { E(window->v.delShader((gtamfx::Shader*)shader)); }
EXPORT GtamSprite *gtamWindowNewSprite(GtamWindow *window, GtamTexture *texture, GtamShader *shader)
  { E(return (GtamSprite*)window->v.newSprite((gtamfx::Texture*)texture, (gtamfx::Shader*)shader)); return NULL; }
EXPORT void gtamWindowDelSprite(GtamWindow *window, GtamSprite *sprite) { E(window->v.delSprite((gtamfx::Sprite*)sprite)); }
EXPORT GtamCamera *gtamWindowNewCamera(GtamWindow *window, int type) { E(return (GtamCamera*)window->v.newCamera((gtamfx::CameraType)type)); return NULL; }
EXPORT void gtamWindowDelCamera(GtamWindow *window, GtamCamera *camera) { E(window->v.delCamera((gtamfx::Camera*)camera)); }
EXPORT void gtamWindowSetActiveCamera(GtamWindow *window, GtamCamera *camera) { window->v.setActiveCamera((gtamfx::Camera*)camera); }
EXPORT GtamCamera *gtamWindowGetActiveCamera(const GtamWindow *window) { return (GtamCamera*)window->v.getActiveCamera(); }
EXPORT void gtamWindowGetFramebufferSize(const GtamWindow *window, GtamVec2 *framebufferSize) { write2(framebufferSize, window->v.getFramebufferSize()); }
EXPORT float gtamWindowGetAspectRatio(const GtamWindow *window) { return window->v.getAspectRatio(); }

}
