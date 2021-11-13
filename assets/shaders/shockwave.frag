#define PI 3.14159265359
#define SPEED 25.0
#define SHOCKWIDTH 0.5
#define SOUNDSPEED 15.0
#define FALLOFF 0.3

uniform sampler2D texture;

uniform float time;
uniform vec2 render_size;
uniform vec2 position;

float dist(vec2 uv, float x, float y) {
    float aspect_ratio = render_size.x / render_size.y;
    float adjusted_x = abs(uv.x - x) * aspect_ratio;
	return sqrt(adjusted_x * adjusted_x + (uv.y - y) * (uv.y - y));
}

float shockwave(float i) {
    float shock = sin(i - PI) / (i - PI);
    float width = smoothstep(0.0, SHOCKWIDTH, i) - smoothstep(SHOCKWIDTH, SHOCKWIDTH * 2.0, i);
    return shock * width;
}

void main() {
	vec2 uv = gl_TexCoord[0].xy;

	float d = dist(uv, position.x, 1.0 - position.y);

	float hstep = 1.0 / render_size.x;
	float vstep = 1.0 / render_size.y;

    float offset = shockwave(time * SPEED - d * SOUNDSPEED);

    // Make it less intesne over time
    offset = max(offset - d * FALLOFF, 0.0);

    float offset_x = offset * (uv.x - hstep);
    float offset_y = offset * (uv.y - vstep);

    vec4 pixel = texture2D(texture, vec2(uv.x + offset_x, uv.y + offset_y));

    gl_FragColor = pixel;
}
