shader_type spatial;
render_mode unshaded, cull_disabled, depth_draw_opaque, blend_mix, ambient_light_disabled;

uniform sampler2D crawl_tex : source_color, filter_linear_mipmap;
uniform vec3 gold_color : source_color = vec3(1.0, 0.91, 0.12);
uniform float ink_threshold : hint_range(0.2, 0.9) = 0.55;

void fragment() {
	vec4 c = texture(crawl_tex, UV);
	float lum = dot(c.rgb, vec3(0.299, 0.587, 0.114));
	// render_mixed 输出浅底深字：暗像素为字
	float ink = clamp((ink_threshold - lum) / ink_threshold, 0.0, 1.0);
	ALBEDO = gold_color;
	ALPHA = ink;
}
