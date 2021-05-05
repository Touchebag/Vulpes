uniform sampler2D texture;

uniform vec2 render_size;

uniform float distance;

void main() {
    float weight = 0.25;

	vec4 blur_color = vec4(0.0);

	vec2 pixel = gl_TexCoord[0].xy;

	float hstep = 1.0 / render_size.x;
	float vstep = 1.0 / render_size.y;

    // Four corner pixels
    blur_color += texture2D(texture, vec2(pixel.x - (0.5 + distance) * hstep,
                                          pixel.y - (0.5 + distance) * vstep)) * weight;
    blur_color += texture2D(texture, vec2(pixel.x - (0.5 + distance) * hstep,
                                          pixel.y + (0.5 + distance) * vstep)) * weight;
    blur_color += texture2D(texture, vec2(pixel.x + (0.5 + distance) * hstep,
                                          pixel.y - (0.5 + distance) * vstep)) * weight;
    blur_color += texture2D(texture, vec2(pixel.x + (0.5 + distance) * hstep,
                                          pixel.y + (0.5 + distance) * vstep)) * weight;

	gl_FragColor = blur_color;
}
