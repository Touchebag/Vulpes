uniform sampler2D texture;

uniform vec2 direction;
uniform vec2 render_size;

uniform int size;
uniform float sigma;

void main() {
    float PI2 = 6.28318;
    float e = 2.71828;

	vec4 blur_color = vec4(0.0);

	vec2 pixel = gl_TexCoord[0].xy;

	float hstep = 1.0 / render_size.x;
	float vstep = 1.0 / render_size.y;

    // Middle pixel
    float weight = 1.0 / sqrt(PI2 * sigma * sigma);
    blur_color += texture2D(texture, vec2(pixel.x, pixel.y)) * weight;

    // Add pixels symmetrically
    for (int i = 1; i <= size; ++i) {
        weight = pow(e, -(float(i) * float(i)) / (2 * sigma * sigma)) / sqrt (PI2 * sigma * sigma);
        blur_color += texture2D(texture, vec2(pixel.x - float(i) * hstep * direction.x,
                                              pixel.y - float(i) * vstep * direction.y)) * weight;
        blur_color += texture2D(texture, vec2(pixel.x + float(i) * hstep * direction.x,
                                              pixel.y + float(i) * vstep * direction.y)) * weight;
    }

	gl_FragColor = blur_color;
}
