uniform sampler2D texture;

uniform vec2 render_size;
uniform int time;

void main() {
    float frames_per_cycle = 240.0;
    float intensity = 20.0;
    float period = 2.0;

	float hstep = 1.0 / render_size.x;
	float vstep = 1.0 / render_size.y;

    float pi2 = 6.28;

    vec2 coord = gl_TexCoord[0].xy;

    float time_shift = mod(float(time), frames_per_cycle);

    float shifted_coord = sin(pi2 * (coord.y * period + time_shift / frames_per_cycle));

    coord += vec2(shifted_coord * hstep * intensity, 0.0);
    vec4 pixel = texture2D(texture, coord);

    gl_FragColor = pixel;
}
