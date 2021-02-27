uniform sampler2D texture;

uniform vec2 direction;

void main() {
	vec4 blur_color = vec4(0.0);

	vec2 pixel = gl_TexCoord[0].xy;

	float intensity = 0.005;

	float hstep = direction.x;
	float vstep = direction.y;

	blur_color += texture2D(texture, vec2(pixel.x - 4.0 * intensity * hstep, pixel.y - 4.0 * intensity * vstep)) * 0.0162162162;
	blur_color += texture2D(texture, vec2(pixel.x - 3.0 * intensity * hstep, pixel.y - 3.0 * intensity * vstep)) * 0.0540540541;
	blur_color += texture2D(texture, vec2(pixel.x - 2.0 * intensity * hstep, pixel.y - 2.0 * intensity * vstep)) * 0.1216216216;
	blur_color += texture2D(texture, vec2(pixel.x - 1.0 * intensity * hstep, pixel.y - 1.0 * intensity * vstep)) * 0.1945945946;

	blur_color += texture2D(texture, vec2(pixel.x, pixel.y)) * 0.2270270270;

	blur_color += texture2D(texture, vec2(pixel.x + 1.0 * intensity * hstep, pixel.y + 1.0 * intensity * vstep)) * 0.1945945946;
	blur_color += texture2D(texture, vec2(pixel.x + 2.0 * intensity * hstep, pixel.y + 2.0 * intensity * vstep)) * 0.1216216216;
	blur_color += texture2D(texture, vec2(pixel.x + 3.0 * intensity * hstep, pixel.y + 3.0 * intensity * vstep)) * 0.0540540541;
	blur_color += texture2D(texture, vec2(pixel.x + 4.0 * intensity * hstep, pixel.y + 4.0 * intensity * vstep)) * 0.0162162162;

	gl_FragColor = blur_color;
}
