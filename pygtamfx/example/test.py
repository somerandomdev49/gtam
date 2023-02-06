from ..pygtamfx import *
import glm

vertex_source = """
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
"""

fragment_source = """
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

"""

window = Window(glm.ivec2(800, 600), "hi!")
window.init()

texture = window.new_texture("example/image.png")
shader = window.new_shader(vertex_source, fragment_source, 4)

sprite = window.new_sprite(texture, shader)
speed = 150

camera = window.new_camera(CameraType.ORTHOGRAPHIC)
window.active_camera = camera

last_time = window.time
while not window.should_close:
  current_time = window.time
  delta_time = current_time - last_time

  window.update()

  movement = glm.vec2(0, 0)
  if window.is_key_down(KeyCode.RIGHT): movement.x += 1
  if window.is_key_down(KeyCode.LEFT): movement.x -= 1
  if window.is_key_down(KeyCode.UP): movement.y += 1
  if window.is_key_down(KeyCode.DOWN): movement.y -= 1
  sprite.position += glm.vec3(movement * speed * delta_time, 0)

  last_time = current_time

window.deinit()






