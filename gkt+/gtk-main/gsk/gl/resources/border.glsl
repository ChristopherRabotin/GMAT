// VERTEX_SHADER:
// border.glsl

uniform vec4 u_widths;
uniform vec4[3] u_outline_rect;

_OUT_ vec4 final_color;
_OUT_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_outside_outline;
_OUT_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_inside_outline;

void main() {
  gl_Position = u_projection * u_modelview * vec4(aPosition, 0.0, 1.0);

  final_color = gsk_scaled_premultiply(aColor, u_alpha);

  GskRoundedRect outside = gsk_create_rect(u_outline_rect);
  GskRoundedRect inside = gsk_rounded_rect_shrink (outside, u_widths);

  gsk_rounded_rect_transform(outside, u_modelview);
  gsk_rounded_rect_transform(inside, u_modelview);

  gsk_rounded_rect_encode(outside, transformed_outside_outline);
  gsk_rounded_rect_encode(inside, transformed_inside_outline);
}

// FRAGMENT_SHADER:
// border.glsl

uniform vec4[3] u_outline_rect;

_IN_ vec4 final_color;
_IN_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_outside_outline;
_IN_ _GSK_ROUNDED_RECT_UNIFORM_ transformed_inside_outline;

void main() {
  vec2 frag = gsk_get_frag_coord();

  float alpha = clamp(gsk_rounded_rect_coverage(gsk_decode_rect(transformed_outside_outline), frag) -
                      gsk_rounded_rect_coverage(gsk_decode_rect(transformed_inside_outline), frag),
                      0.0, 1.0);

  gskSetScaledOutputColor(final_color, alpha);
}
