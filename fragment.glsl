#version 330 core

out vec4 color;

in VertexData {
	vec4 color;
	vec2 uv;
} vertexIn;

uniform sampler2D texture;
uniform vec3 iResolution;
uniform float iGlobalTime;

void Mandelbrot( out vec4 fragColor, in vec2 fragCoord )
{	
	vec2 uv = fragCoord.xy / iResolution.xy;
	uv -= 0.5;
	uv.x *= iResolution.x / iResolution.y;
	uv *= 3.0;

	vec2 s = uv;
	vec2 prevUv = uv;
	

	float lim = 5.0 * (1.0 - cos(iGlobalTime));
	
	for (int i = 0; i < 32; i++)
	{
		if (i > int(floor(lim)) || abs(uv.x) + abs(uv.x) > 16384.0)
			break;

		prevUv = uv;
		uv = vec2(uv.x * uv.x - uv.y * uv.y, 2.0 * uv.x * uv.y) + s;
	}

	uv = mix(prevUv, uv, fract(lim)) * 0.3333 + 0.5;
	 
	fragColor = texture2D(texture, uv * vec2(1, -1));
}

void main() {
	Mandelbrot(color, gl_FragCoord.xy);
}
