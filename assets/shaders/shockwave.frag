#define PI 3.14159265359
#define SPEED 25.0
#define SHOCKWIDTH 0.5
#define SOUNDSPEED 15.0
#define FALLOFF 0.3

uniform sampler2D texture;

uniform float time;
uniform vec2 position;

void main() {
	vec2 uv = gl_TexCoord[0].xy;
    vec2 adjusted_pos = vec2(position.x, 1.0 - position.y);

    float outer_size = 1.5 * time ;
    float width = 0.1 / (1.0 + 5.0 * time);
    float strength = 0.2 / (1.0 + 10.0 * time);

    float outer_edge = 1.0 - smoothstep(outer_size - width, outer_size, length(uv - adjusted_pos));
    float inner_edge = smoothstep(outer_size - width * 2.0, outer_size, length(uv - adjusted_pos));
    float mask = outer_edge * inner_edge;

    vec2 displacement = normalize(uv - adjusted_pos) * strength * mask;
    vec4 pixel_color = texture2D(texture, uv - displacement);

    gl_FragColor = pixel_color;
}
