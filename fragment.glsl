#version 330 core

out vec4 color;

in VertexData {
	vec4 color;
	vec2 uv;
} vertexIn;

uniform vec3 iResolution;
uniform float iGlobalTime;

void Mandelbrot(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 position = -1.0 + 2.0 * fragCoord.xy / iResolution.xy;
	position.x *= iResolution.x / iResolution.y;

	float tz = 0.5 - 0.5 * cos(0.225 * iGlobalTime);
    float zoo = pow(0.5, 13.0 * tz);
	vec2 c = position;// / zoo;// + vec2(-0.05, 0.6805);
	vec2 z = vec2(0.0);

	float m2 = 0.0;
	int i;
	for(i = 0; i < 256; i++) {
		if(m2 > 4) break;
		z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
		m2 = dot(z, z);
	}

	float dist = sqrt(m2); 
	vec3 col = vec3(1.0 - i / (20.0 * sin(iGlobalTime) + 21));
	fragColor = vec4(col, 1.0);
}

void main() {
	Mandelbrot(color, gl_FragCoord.xy);
}
