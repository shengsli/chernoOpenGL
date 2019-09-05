#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{
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

	while (!glfwWindowShouldClose(window)) {/* Loop until the user closes the window */
		glClear(GL_COLOR_BUFFER_BIT);/* Render here */

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);/* Swap front and back buffers */
		glfwPollEvents();/* Poll for and process events */
	}

	glfwTerminate();
	return 0;
}