#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include"Ball.cpp""
#include"Camera.h"

using namespace std; 


float rotationSpeed = 1.0f;    
bool eclipseMode = false;
bool isEclipse = false;
bool lunarEclipseMode = false;

float earthAngle = 0.0f;
float moonAngle = 0.0f;







glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = 800.0f/2.0; 
float lastY = 600.0f/2.0;
float yaw = -90.0f;       
float pitch = 0.0f;        
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; 
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	const float cameraSpeed = 0.01f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;

	}
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        eclipseMode = true;
		rotationSpeed = 5.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        eclipseMode = false;
        isEclipse = false;
		lunarEclipseMode = false; 
        rotationSpeed = 1.0f;
    }
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		lunarEclipseMode = true;
		rotationSpeed = 5.0f; 
	}



}

void drawSomething(Ball& earth, Ball& moon, float& rotationSpeed, glm::mat4 view, glm::mat4 projection) {
	glm::vec3 sunPos = glm::vec3(-2, 0, 0);
	glm::vec3 earthPos = sunPos + glm::vec3(cos(earthAngle) * 10, 0, sin(earthAngle) * 10);
	glm::vec3 moonPos = earthPos + glm::vec3(cos(moonAngle) * 1.5, 0, sin(moonAngle) * 1.5);

	if (eclipseMode) {
		glm::vec3 s = glm::normalize(sunPos - earthPos);
		glm::vec3 m = glm::normalize(moonPos - earthPos);
		float a = glm::dot(s, m);
		if (a > 0.9995f) {
			isEclipse = true;
			rotationSpeed = 0.0f;
		}
		else {
			isEclipse = false;
		}
	}

	if (lunarEclipseMode) {
		glm::vec3 earthToSun = glm::normalize(sunPos - earthPos);
		glm::vec3 earthToMoon = glm::normalize(moonPos - earthPos);

		float alignment = glm::dot(earthToSun, earthToMoon);

		if (alignment < -0.9995f) {
			isEclipse = true;
			rotationSpeed = 0.0f;
		}
		else {
			isEclipse = false;
			
		}
	}

	glm::mat4 eM = glm::translate(glm::mat4(1.0f), earthPos);
	eM = glm::rotate(eM, float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
	glUseProgram(earth.shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(earth.shaderProgram, "model"), 1, GL_FALSE, &eM[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(earth.shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(earth.shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	glBindTexture(GL_TEXTURE_2D, earth.texture);
	glBindVertexArray(earth.VAO);
	glDrawElements(GL_TRIANGLES, earth.indexCount, GL_UNSIGNED_INT, 0);

	glm::mat4 mM = glm::translate(glm::mat4(1.0f), moonPos);
	glUseProgram(moon.shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(moon.shaderProgram, "model"), 1, GL_FALSE, &mM[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(moon.shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(moon.shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	glBindTexture(GL_TEXTURE_2D, moon.texture);
	glBindVertexArray(moon.VAO);
	glDrawElements(GL_TRIANGLES, moon.indexCount, GL_UNSIGNED_INT, 0);
}


void drawSun(Ball& sun, glm::mat4 view, glm::mat4 projection) {

	glm::vec3 sunPosition = glm::vec3(-2, 0, 0);
	glm::mat4 sunModel = glm::translate(glm::mat4(1.0f), sunPosition);

	glUseProgram(sun.shaderProgram);

	glUniformMatrix4fv(glGetUniformLocation(sun.shaderProgram, "model"), 1, GL_FALSE, &sunModel[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sun.shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sun.shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

	glBindTexture(GL_TEXTURE_2D, sun.texture);
	glBindVertexArray(sun.VAO);
	glDrawElements(GL_TRIANGLES, sun.indexCount, GL_UNSIGNED_INT, 0);
}








int main() {

	glfwInit(); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1600, 600, "hello there", NULL, NULL);
	glfwMakeContextCurrent(window); 
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); 

	glEnable(GL_DEPTH_TEST); 

	Ball earth(0.7, 40, 40,false); 
	earth.setTexture("textures/earth.jpg"); 
	Ball moon(0.3, 40, 40,false); 
	moon.setTexture("textures/moon.jpg"); 
	Ball sun(2.5, 30, 30,true);
	sun.setTexture("textures/sun.jpg");

	 




	 glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -15));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)1500/500, 0.1f, 100.0f);

	
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(window); 
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_DISABLED); 
		glfwSetCursorPosCallback(window, mouse_callback);    

		drawSomething(earth, moon,rotationSpeed, view, projection);
		drawSun(sun, view, projection); 
        earthAngle += rotationSpeed * 0.0004f;
        moonAngle += rotationSpeed * 0.0008f;





		glfwSwapBuffers(window);
		glfwPollEvents();
	}






	glfwTerminate(); 
	return 0; 
}