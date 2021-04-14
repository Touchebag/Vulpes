uniform sampler2D texture;

uniform vec2 direction;
uniform vec2 render_size;

void main() {
	vec4 blur_color = vec4(0.0);

	vec2 pixel = gl_TexCoord[0].xy;

	float hstep = 1.0 / render_size.x;
	float vstep = 1.0 / render_size.y;

	blur_color += texture2D(texture, vec2(pixel.x - 4.0 * hstep * direction.x, pixel.y - 4.0 * vstep * direction.y)) * 0.0162162162;
	blur_color += texture2D(texture, vec2(pixel.x - 3.0 * hstep * direction.x, pixel.y - 3.0 * vstep * direction.y)) * 0.0540540541;
	blur_color += texture2D(texture, vec2(pixel.x - 2.0 * hstep * direction.x, pixel.y - 2.0 * vstep * direction.y)) * 0.1216216216;
	blur_color += texture2D(texture, vec2(pixel.x - 1.0 * hstep * direction.x, pixel.y - 1.0 * vstep * direction.y)) * 0.1945945946;

	blur_color += texture2D(texture, vec2(pixel.x, pixel.y)) * 0.2270270270;

	blur_color += texture2D(texture, vec2(pixel.x + 1.0 * hstep * direction.x, pixel.y + 1.0 * vstep * direction.y)) * 0.1945945946;
	blur_color += texture2D(texture, vec2(pixel.x + 2.0 * hstep * direction.x, pixel.y + 2.0 * vstep * direction.y)) * 0.1216216216;
	blur_color += texture2D(texture, vec2(pixel.x + 3.0 * hstep * direction.x, pixel.y + 3.0 * vstep * direction.y)) * 0.0540540541;
	blur_color += texture2D(texture, vec2(pixel.x + 4.0 * hstep * direction.x, pixel.y + 4.0 * vstep * direction.y)) * 0.0162162162;

	gl_FragColor = blur_color;
}
