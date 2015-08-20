#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out VertexData {
	vec4 color;
	vec2 uv;
} vertexOut;

void main() {
	gl_Position = vec4(vertexPosition_modelspace, 1.0);
}
