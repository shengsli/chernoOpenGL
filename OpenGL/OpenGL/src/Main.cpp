#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char *) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader. " << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);//delete obj files
	glDeleteShader(fs);//delete obj files
	return program;
}

int main(void) {
	GLFWwindow* window;
	if (!glfwInit()) return -1;
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);/* Make the window's context current */
	if (glewInit() != GLEW_OK) std::cout << "Error!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};
	unsigned int buffer;
	glGenBuffers(1, &buffer);//generate buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);//bind buffer before draw
	glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);//put data into buffer

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void *)0);

	std::string vertexShader = 
		"#version 330 core\n"
		"\n"
		"layout(location=0) in vec4 position;\n" //vertices are converted to vec4 eventually
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;"
		"}\n";

	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location=0) out vec4 color;\n" //vertices are converted to vec4 eventually
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);"
		"}\n";
	unsigned int shader = CreateShader(vertexShader, fragmentShader); 
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window)) {/* Loop until the user closes the window */
		glClear(GL_COLOR_BUFFER_BIT);/* Render here */

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);/* Swap front and back buffers */
		glfwPollEvents();/* Poll for and process events */
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}