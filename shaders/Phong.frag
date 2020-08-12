#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // YOUR CODE HERE

  // (Placeholder code. You will want to replace it.)
  //out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;

  vec4 l_vec = vec4(u_light_pos, 1.0) - v_position;
  float cos_theta1 = dot(normalize(l_vec), normalize(v_normal));

  vec4 to_cam = vec4(u_cam_pos, 1.0) - v_position;
  vec4 h = (to_cam + l_vec) / 2;
  float cos_theta2 = dot(normalize(v_normal), normalize(h));
  float r = length(l_vec);


  float k_d = 1;
  float k_a = 1;
  vec3 i_a = vec3(0.68, 0.84, 0.90);
  float k_s = 0.5;
  float p = 20;

  vec3 l_d = k_d * u_light_intensity/(r*r) * max(0, cos_theta1);
  vec3 l_a = k_a * i_a;
  vec3 l_s = k_s * u_light_intensity/(r*r) * pow(max(0, cos_theta2), p);

  out_color.a = 1;
  out_color = vec4(l_s + l_a + l_d, out_color.a);
}
