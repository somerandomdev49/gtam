#include "gtamfx.hpp"

const char *vertexShaderSource = R"glsl(
#version 330 core
struct Vertex {
  vec4 position;
  vec4 color;
  vec4 texCoord;
};

const Vertex[4] vertices = Vertex[4](
  Vertex(vec4(-0.5, -0.5, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 0.0)),
  Vertex(vec4(+0.5, -0.5, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 0.0, 0.0, 0.0)),
  Vertex(vec4(-0.5, +0.5, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 1.0, 0.0, 0.0)),
  Vertex(vec4(+0.5, +0.5, 0.0, 0.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 0.0, 0.0))
);

out vec4 sColor;
out vec2 sTexCoord;
uniform mat4 uTransform;

void main() {
  gl_Position = uTransform * vec4(vertices[gl_VertexID].position.xyz, 1.0);
  sColor = vertices[gl_VertexID].color.rgba;
  sTexCoord = vertices[gl_VertexID].texCoord.st;
}
)glsl";

const char *fragmentShaderSource = R"glsl(
#version 330 core
in vec4 sColor;
in vec2 sTexCoord;

out vec4 oColor;

uniform sampler2D uTexture;
uniform vec4 uTextureView;

void main() {
  oColor = sColor * texture2D(uTexture, sTexCoord * uTextureView.zw + uTextureView.xy);
  if(oColor.a == 0) discard;
}

)glsl";

int main() {
  try {
    gtamfx::Window window({800, 600}, "GTAMFX Test!");
    window.init();

    gtamfx::Shader *shader = window.newShader(vertexShaderSource, fragmentShaderSource, 4);
    gtamfx::Texture *texture = window.newTexture("image.png");
    gtamfx::Sprite *sprite = window.newSprite(texture, shader);
    const float speed = 140.0f;

    gtamfx::Camera *camera = window.newCamera(gtamfx::CameraType::Orthographic);

    window.setActiveCamera(camera);

    float lastTime = window.getTime();
    while(!window.shouldClose()) {
      float currentTime = window.getTime();
      float deltaTime = currentTime - lastTime;

      window.update();

      glm::vec2 movement = {0, 0};

      if(window.isKeyDown(gtamfx::KeyCode::Left)) {
        movement.x -= 1;
      }
      
      if(window.isKeyDown(gtamfx::KeyCode::Right)) {
        movement.x += 1;
      }

      if(window.isKeyDown(gtamfx::KeyCode::Up)) {
        movement.y += 1;
      }
      
      if(window.isKeyDown(gtamfx::KeyCode::Down)) {
        movement.y -= 1;
      }

      if(movement != glm::vec2{0, 0})
        sprite->position += glm::vec3(glm::normalize(movement) * speed * deltaTime, 0);

      lastTime = currentTime;
    }

    window.deinit();
  } catch(gtamfx::Exception e) {
    static const char *exceptionTypeStrings[] = {
      "Failed to initialize GLFW", // GlfwFailedInit
      "Failed to create GLFW window", // GlfwFailedCreateWindow
      "Failed to initalize GL3W", // Gl3wFailedInit
      "OpenGL major version < 2", // Gl3wBadVersion
      "Failed to load texture", // TextureLoadFail
      "Failed to load shader" // ShaderLoadFail
    };
    std::fprintf(stderr, "Error: %s: %s\n", exceptionTypeStrings[(int)e.type], e.message.c_str());
  }
}
