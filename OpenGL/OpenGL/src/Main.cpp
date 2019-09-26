#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();//an MSVC function, which is intrinsic to the compiler. It inserts a breakpoint. 
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << ")" << function << 
			"" << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGEMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex")!= std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGEMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

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

	//vertex array objects are mandatory, they are in use even without us creating them. 
	//opengl compatability profile is a mode in opengl running by default. 
	//core profile requires us bind vertex array objects manually. 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);/* Make the window's context current */
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) std::cout << "Error!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
		-0.5f, -0.5f,//0
		 0.5f, -0.5f,//1
		 0.5f,  0.5f,//2
		-0.5f,  0.5f //3
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};

	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	// we still need this vertex buffer
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));//generate buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));//bind buffer before draw
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));//put data into buffer

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void *)0));

	// index buffer
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));//generate buffer
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//bind buffer before draw
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));//put data (i.e. 6 indices) into buffer, has to be unsigned

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource); 
	GLCall(glUseProgram(shader));//bound shader

	GLCall(int location = glGetUniformLocation(shader, "u_Color"));//retrieve location of u_Color
	ASSERT(location != -1);//-1 if the uniform is not found or is unused. 
	GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));//4 floats, int location

	//unbind everything
	GLCall(glBindVertexArray(0));
	GLCall(glUseProgram(0));//unbind shader
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));//unbind vertex buffer
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));//unbind index buffer

	float r = 0.0f;
	float increment = 0.05f;
	while (!glfwWindowShouldClose(window)) {/* Loop until the user closes the window */
		glClear(GL_COLOR_BUFFER_BIT);/* Render here */

		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));//4 floats, int location

		//vertex buffer
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		GLCall(glEnableVertexAttribArray(0));//set up the layout. do this every time, cuz it might change if we draw another object with a different layout. 
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void *)0));//do this every time
		
		//jsut bind vertex array and index buffer
		GLCall(glBindVertexArray(vao));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;
		r += increment;

		glfwSwapBuffers(window);/* Swap front and back buffers */
		glfwPollEvents();/* Poll for and process events */
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}