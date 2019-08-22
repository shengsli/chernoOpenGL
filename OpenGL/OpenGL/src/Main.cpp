#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())/* Initialize the library */ return -1;
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);/* Create a windowed mode window and its OpenGL context */
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);/* Make the window's context current */

	if (glewInit() != GLEW_OK) std::cout << "Error!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	while (!glfwWindowShouldClose(window))/* Loop until the user closes the window */
	{
		glClear(GL_COLOR_BUFFER_BIT);/* Render here */

		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.0f,  0.5f);
		glVertex2f( 0.5f, -0.5f);
		glEnd();

		glfwSwapBuffers(window);/* Swap front and back buffers */
		glfwPollEvents();/* Poll for and process events */
	}

	glfwTerminate();
	return 0;
}