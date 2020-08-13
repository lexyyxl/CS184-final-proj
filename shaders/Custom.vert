#version 330

// Uniform variables are constant throughout the entire shader
// execution. They are also read-only to enable parallelization.
uniform mat4 u_model;
uniform mat4 u_view_projection;

// In a vertex shader, the "in" variables are read-only per-vertex
// properties. An example of this was shown in the rasterizer project,
// where each vertex had an associated "color" or "uv" value which we
// would later interpolate using barycentric coordinates.
in vec4 in_position;
in vec4 in_normal;
in vec4 in_tangent;
in vec2 in_uv;

// In a vertex shader, the "out" variables are per-vertex properties
// that are read/write. These properties allow us to communicate
// information from the vertex shader to the fragment shader.
// That is, in the linked fragment shader, these values become the
// "in" variables.
out vec4 v_position;
out vec4 v_normal;
out vec2 v_uv;
out vec4 v_tangent;

// Every shader features a "main" function.
// This is typically where we write to the "out" variables that the
// fragment shaders get to use. It is also where "gl_Position" is set,
// which is the final screen-space location of this vertex which the
// GPU's triangle rasterizer takes in.
void main() {
  // Here, we just apply the model's transformation to the various
  // per-vertex properties. That way, when the fragment shader reads
  // them, we already have the position in world-space.
  v_position = u_model * in_position;
  v_normal = normalize(u_model * in_normal);
  v_uv = in_uv;
  v_tangent = normalize(u_model * in_tangent);

  // The final screen-space location of this vertex which the
  // GPU's triangle rasterizer takes in.
  gl_Position = u_view_projection * u_model * in_position;
}


//#version 150
/////
//// Wave effect vertex shader.
/////
//in vec4 a_position;
//in vec2 a_tex_coord;
//
//uniform vec4 u_rescale; // used to scale back to 0..1
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
//// Texture coordinates
//out vec2 v_tex_coord;
//// 4 per-wave coordinates to lookup cos values
//out vec4 v_cos_coord;
//
//void main()
//{
//    gl_Position = a_position;
//    v_tex_coord = a_tex_coord;
//    v_cos_coord.x = dot(vec4(a_tex_coord, 1, 1), u_rtex_coord[0]);
//    v_cos_coord.y = dot(vec4(a_tex_coord, 1, 1), u_rtex_coord[1]);
//    v_cos_coord.z = dot(vec4(a_tex_coord, 1, 1), u_rtex_coord[2]);
//    v_cos_coord.w = dot(vec4(a_tex_coord, 1, 1), u_rtex_coord[3]);
//}
