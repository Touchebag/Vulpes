uniform sampler2D texture;

uniform float intensity;
uniform vec3 target_color;

void main() {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);

    vec3 diff = target_color - pixel.rgb;

    gl_FragColor = pixel + (vec4(diff, 0.0) * intensity);
}
