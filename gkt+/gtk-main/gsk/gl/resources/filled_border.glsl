// VERTEX_SHADER:
// filled_border.glsl

uniform vec4 u_widths;
uniform vec4[3] u_outline_rect;

_OUT_ vec4 outer_color;
_OUT_ vec4 inner_color;
_OUT_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_outside_outline;
_OUT_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_inside_outline;

void main() {
  gl_Position = u_projection * u_modelview * vec4(aPosition, 0.0, 1.0);

  outer_color = gsk_scaled_premultiply(aColor, u_alpha);
  inner_color = gsk_scaled_premultiply(aColor2, u_alpha);

  GskRoundedRect outside = gsk_create_rect(u_outline_rect);
  GskRoundedRect inside = gsk_rounded_rect_shrink (outside, u_widths);

  gsk_rounded_rect_transform(outside, u_modelview);
  gsk_rounded_rect_transform(inside, u_modelview);

  gsk_rounded_rect_encode(outside, transformed_outside_outline);
  gsk_rounded_rect_encode(inside, transformed_inside_outline);
}

// FRAGMENT_SHADER:
// filled_border.glsl

uniform vec4[3] u_outline_rect;

_IN_ vec4 outer_color;
_IN_ vec4 inner_color;
_IN_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_outside_outline;
_IN_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_inside_outline;

void main() {
  vec2 frag = gsk_get_frag_coord();
  float outer_coverage = gsk_rounded_rect_coverage(gsk_decode_rect(transformed_outside_outline), frag);
  float inner_coverage = gsk_rounded_rect_coverage(gsk_decode_rect(transformed_inside_outline), frag);

  float alpha = clamp(outer_coverage - inner_coverage, 0.0, 1.0);
  float alpha2 = clamp(inner_coverage, 0.0, 1.0);

  gskSetOutputColor((outer_color * alpha) + (inner_color * alpha2));
}
