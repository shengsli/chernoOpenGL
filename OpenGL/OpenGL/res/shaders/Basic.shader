#shader vertex
#version 330 core

layout(location=0) in vec4 position; //vertices are converted to vec4 eventually

void main() {
	gl_Position = position;
};

#shader fragment
#version 330 core

layout(location=0) out vec4 color; //vertices are converted to vec4 eventually

void main() {
	color = vec4(1.0, 0.0, 0.0, 1.0);
};