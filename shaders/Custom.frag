#version 330

// (Every uniform is available here.)

uniform mat4 u_view_projection;
uniform mat4 u_model;

uniform float u_normal_scaling;
uniform float u_height_scaling;

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

// Feel free to add your own textures. If you need more than 4,
// you will need to modify the skeleton.
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform sampler2D u_texture_4;

// Environment map! Take a look at GLSL documentation to see how to
// sample from this.
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
  // Your awesome shader here!
  vec3 wo = u_cam_pos - vec3(v_position);
  vec3 wi_reflect = 2 * vec3(v_normal) - wo;

    
  //refraction
    vec3 wi_refract;
    float eta = 1.0/1.33;
    float temp = 1 - eta * eta * (1 - wo.z * wo.z);
    
    if (temp < 0) { //cos_theta = wo.z
        out_color = texture(u_texture_2, vec2(wi_reflect));
        out_color.a = 0.5;
        return;
    }
    wi_refract.z = wo.z < 0? sqrt(temp): -sqrt(temp); //cos_theta_prime = wi.z
    wi_refract.x = - eta * wo.x;
    wi_refract.y = - eta * wo.y;
    
    out_color = texture(u_texture_2, vec2(wi_refract));
    out_color.a = 1;


}


//#version 150
/////
//// Wave effect fragment shader.
/////
//// Texture coordinates
//in vec2 v_tex_coord;
//// 4 per-wave coordinates to lookup cos values
//in vec4 v_cos_coord;
//
//uniform vec4 u_rescale; // used to scale back to 0..1
//uniform vec4 u_scale_bias; // ?
//uniform mat4 u_rtex_coord; // ripple texture coords
//uniform mat4 u_coef; // wave coefficients
//uniform float u_pass; // index of current pass
//
//// Consine lookup table
//uniform sampler1D u_tex0;
//// Noise map
//uniform sampler2D u_tex1;
//// Normal map
//uniform sampler2D u_tex2;
//
//out vec4 o_color;
//
//void main()
//{
//    // sample previous pass
//    vec4 dest_color = texture(u_tex2, v_tex_coord);
//
//    // wave passes [0..3]
//    if (u_pass < 4.0)
//    {
//        // Sample cosine for each wave
//        vec4 t0 = texture(u_tex0, v_cos_coord.x);
//        vec4 t1 = texture(u_tex0, v_cos_coord.y);
//        vec4 t2 = texture(u_tex0, v_cos_coord.z);
//        vec4 t3 = texture(u_tex0, v_cos_coord.w);
//        vec4 tmp =  2.0 * (t0 - 0.5) * u_coef[0] +
//                    2.0 * (t1 - 0.5) * u_coef[1] +
//                    2.0 * (t2 - 0.5) * u_coef[2] +
//                    2.0 * (t3 - 0.5) * u_coef[3];
//
//        // Bias tmp back into range [0..1]
//        vec4 c1 = tmp * u_rescale + u_rescale;
//
//        // first pass just overrides existing content, later passes use additive blending
//        o_color = c1 + dest_color * min(u_pass, 1.0);
//    }
//    // noise pass [4]
//    else
//    {
//        // sample noise texture
//        vec4 t0 = texture(u_tex1, v_cos_coord.xy);
//        vec4 t1 = texture(u_tex1, v_cos_coord.zw);
//        vec4 c1 = vec4(t0.rgb + t1.rgb - 1, t0.a + t1.a);
//        // rescale with biased scale
//        c1.rgb = c1.rgb * u_rescale.xyz + u_rescale.xyz;
//        o_color = c1 + dest_color;
//    }
//}
