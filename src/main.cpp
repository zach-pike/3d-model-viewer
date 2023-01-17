// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <vector>

#include "shader/shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "objparser/objparser.hpp"

#include <istream>
#include <fstream>

template <typename T>
void bufferData(GLuint bufferId, std::vector<T>& data) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
}

glm::vec3 getLookVector(float pitch, float yaw) {
	glm::vec3 lookVector;

	lookVector.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    lookVector.y = sin(glm::radians(pitch));
    lookVector.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    return glm::normalize(lookVector);
}

void setViewMat(glm::mat4& view, float pitch, float yaw, glm::vec3 pos) {
	glm::vec3 lookVector = getLookVector(pitch, yaw);

	view = glm::lookAt(
		pos,
		pos + lookVector,
		glm::vec3(0,1,0)
	);
}

glm::vec3 getRightVector(glm::vec3 lookVector) {
	return glm::normalize(glm::cross(lookVector, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void cameraManager(glm::mat4& view, GLFWwindow* win) {
	static float pitch = 0.f;
	static float yaw = 0.f;

	static glm::vec3 pos = glm::vec3(4, 3, -3);

	static const float sensitvity = 0.25f;
	static const float moveSpeed = .5f;

	static bool lockMouse = true;
	static bool eKeyReleased = true;
	static bool firstRun = true;

	static int winX;
	static int winY;

	if (firstRun) {
		glfwGetWindowSize(win, &winX, &winY);

		firstRun = false;
	}

	if (glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS) {
		if (eKeyReleased == true) {
			// If the mouse is being unlocked, set the cursor to the middle of the screen before the update
			if (!lockMouse) glfwSetCursorPos(win, (float)winX / 2.f, (float)winY / 2.f);

			lockMouse = !lockMouse;
			eKeyReleased = false;
		}
	} else eKeyReleased = true;

	if (lockMouse) {
		double mouseX;
		double mouseY;
		
		// Get the cursor
		glfwGetCursorPos(win, &mouseX, &mouseY);

		// reset cursor to middle of screen
		glfwSetCursorPos(win, (float)winX / 2.f, (float)winY / 2.f);

		double movedX = mouseX - ((float)winX / 2.f);
		double movedY = mouseY - ((float)winY / 2.f);

		pitch -= movedY * sensitvity;
		yaw += movedX * sensitvity;

		if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
			glm::vec3 look = getLookVector(pitch, yaw);
			look *= moveSpeed;

			pos += look;
		} else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
			glm::vec3 look = getLookVector(pitch, yaw);
			look *= moveSpeed;

			pos += -look;
		} else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
			glm::vec3 look = getLookVector(pitch, yaw);
			look = getRightVector(look);
			look *= moveSpeed;

			pos += look;
		} else if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
			glm::vec3 look = getLookVector(pitch, yaw);
			look = getRightVector(look);
			look *= moveSpeed;
			
			pos += -look;
		}

		setViewMat(view, pitch, yaw, pos);
	}
}

int main( void ) {
	// Initialise GLFW
	if(!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "3D model viewer", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders(
        "../src/vertex.glsl",
        "../src/fragment.glsl"
    );

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(70.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View;
	glm::mat4 Model = glm::mat4(1.0f);

	glUseProgram(programID);

	std::ifstream f("../Teapot.obj");
	ObjParser parser;
	parser.parse(f);

	// if (!parser.modelHasFeature(ModelFileParser::Features::VERTEX) 
	// || !parser.modelHasFeature(ModelFileParser::Features::COLOR)) {
	// 	return 1;
	// }
	
    std::vector<Vertex> vertexBufferData = parser.assembleToVertexes();
	std::vector<Color> colorBuffer;

	colorBuffer.resize(vertexBufferData.size());

	for (int i=0; i < vertexBufferData.size(); i++) {
		colorBuffer[i] = Color{ 1.f, 1.f, 1.f };
	}
	
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);

	// Buffer the quad
	bufferData(vertexbuffer, vertexBufferData);
	bufferData(colorbuffer, colorBuffer);

	// Some variables
    using time_point = std::chrono::system_clock::time_point;
    long frameDelta = std::floor(1.f / 60.f) * 1000.f;

	// glEnable(GL_CULL_FACE);

	do{
        time_point frameStart = std::chrono::system_clock::now();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cameraManager(View, window);
		glm::mat4 MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, vertexBufferData.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

        time_point frameEnd = std::chrono::system_clock::now();

        long difference = std::chrono::time_point_cast<std::chrono::milliseconds>(frameEnd).time_since_epoch().count();
        difference -= std::chrono::time_point_cast<std::chrono::milliseconds>(frameStart).time_since_epoch().count();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelta - difference));

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
