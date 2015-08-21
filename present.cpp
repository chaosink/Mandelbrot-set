#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>

#include "optparse.hpp"
#include "shader.hpp"
#include "texture.hpp"

#define max(a, b) ((a) >= (b) ? (a) : (b))
#define min(a, b) ((a) <  (b) ? (a) : (b))
#define swap(a, b) (a) = (a) ^ (b), (b) = (a) ^ (b), (a) = (a) ^ (b)
#define PI 3.14159265358979323846

int window_width  = 1024;
int window_height = 768;
int decoration    = 1;
int fullscreen    = 0;
int resizable     = 1;

double window_ratio = window_height * 1.0 / window_width;
int run = 1;

extern struct Picture{
  int width;
  int	height;
  int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char pixel_data[1536 * 1536 * 3 + 1];
} picture;

extern char *vertex_shader;
extern char *fragment_shader;

GLuint LoadTextureFromMemory(const unsigned char *image_buffer, int image_width, int image_height, int image_channels) {
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	return texture;
}

int Digit(int n) {
	int i = 0;
	while(n) {
		n /= 10;
		i++;
	}
	return i;
}

void OptParse(char** argv) {
	struct optparse options;
	optparse_init(&options, argv);
	int option;

	while((option = optparse(&options, "dfrw: h")) != -1) {
		switch(option) {
			case 'd':
				decoration = 0;
				break;
			case 'f':
				fullscreen = 1;
				break;
			case 'r':
				resizable = 0;
				break;	
			case 'w':
				window_width = atoi(options.optarg);
				window_height = atoi(options.optarg + Digit(window_width) + 1);
				window_ratio = window_height * 1.0 / window_width;
				break;

			case 'h':
				printf(
"None\n");
				return;
				break;
		}
	}

	if(fullscreen) {
		window_width = 1920;
		window_height = 1080;
		window_ratio = window_height * 1.0 / window_width;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;
	window_ratio = window_height * 1.0 / window_width;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) run = 1 - run;
}

int InitGL() {
	if(!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, decoration && !fullscreen);
	glfwWindowHint(GLFW_RESIZABLE, resizable && !fullscreen);

	window = glfwCreateWindow(window_width, window_height, "present", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if(window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true; // Needed for core profile
	if(glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	glClearColor( 0.1f, 0.1f, 0.1f, 0.0f);

	return 0;
}

void Render() {
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

//	GLuint programID = LoadShader("vertex.glsl", "fragment.glsl");
	GLuint programID = LoadShaderFromString(vertex_shader, fragment_shader);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint textureID = glGetUniformLocation(programID, "texture");

	GLuint texture = LoadTextureFromMemory(picture.pixel_data, 1536, 1536, 3);

	do {
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,		  // The attribute we want to configure
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void*)0  // array buffer offset
		);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureID, 0);

		glUniform1f(glGetUniformLocation(programID, "iGlobalTime"), glfwGetTime());
		glUniform3f(glGetUniformLocation(programID, "iResolution"), window_width, window_height, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		double time = glfwGetTime();
		while(!run) glfwPollEvents();
		glfwSetTime(time);
	} while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));



	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
}

int main(int argc, char** argv) {
	OptParse(argv);

	if(InitGL()) return -1;

	Render();

	glfwTerminate();

	return 0;
}
