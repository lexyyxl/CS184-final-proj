#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;

uniform float u_normal_scaling;
uniform float u_height_scaling;


uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform sampler2D u_texture_4;

uniform samplerCube u_texture_cubemap;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;


out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  return texture(u_texture_2, uv)[0];
}

void main() {
  // YOUR CODE HERE

  // (Placeholder code. You will want to replace it.)
  //out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;

  vec3 wo = u_cam_pos - vec3(v_position);
  vec3 wi_reflect = 2 * vec3(v_normal) - wo;

   //refraction
  vec3 wi_refract;
  float eta = 1.0/1.33;
  float temp = 1 - eta * eta * (1 - wo.z * wo.z);
    
  if (temp < 0) { //cos_theta = wo.z
        out_color = 0.5 * texture(u_texture_cubemap, wi_reflect) + texture(u_texture_1, vec2(wi_reflect));
        out_color.a = 0.5;
        return;
  }
  wi_refract.z = wo.z < 0? sqrt(temp): -sqrt(temp); //cos_theta_prime = wi.z
  wi_refract.x = - eta * wo.x;
  wi_refract.y = - eta * wo.y;
   //bottom side
  vec4 out_color2 = texture(u_texture_4, vec2(wi_refract));

  out_color = 0.2*out_color2 + 0.5* texture(u_texture_1, v_uv);
  out_color.a = 0.5;
  

}
