
///////////////////////////////////////////////////////////////////////
//
// triangles.cpp
//
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "SOIL.h"


enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[3];
GLuint location;
GLuint location2;
GLuint location3;

GLuint texture[1];

const GLuint NumVertices = 24;

float translate_value = 0;
float rotate_value = 0;
float camera_distance = 2.0;

//---------------------------------------------------------------------
//
// init
//


void init(void) {
	glEnable(GL_DEPTH_TEST);
	GLfloat textureCoordinates[24][2] = {
		0.66f, 0.5f,
		1.0f, 0.5f,
		1.0f, 0.0f,
		0.66f, 0.0f,

		0.0f, 1.0f,
		0.33f, 1.0f,
		0.33f, 0.5f,
		0.0f, 0.5f,

		0.33f, 0.0f,
		0.33f, 0.5f,
		0.66f, 0.5f,
		0.66f, 0.0f,

		0.66f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.5f,
		0.66f, 0.5f,

		0.0f, 0.5f,
		0.33f, 0.5f,
		0.33f, 0.0f,
		0.0f, 0.0f,

		0.33f, 0.5f,
		0.66f, 0.5f,
		0.66f, 0.0f,
		0.33f, 0.0f
	};

	GLint width, height;
	unsigned char* image = SOIL_load_image("myDice.png", &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, image);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	GLfloat vertices[NumVertices][3] = {
		-0.5, 0.5,  0.5,
		0.5,  0.5,  0.5,
		0.5,  0.5,  -0.5,
		-0.5,  0.5,  -0.5,

		-0.5, -0.5, 0.5,
		0.5, -0.5, 0.5,
		0.5,  -0.5, -0.5,
		-0.5,  -0.5, -0.5,

		-0.5, -0.5, -0.5,
		-0.5, -0.5, 0.5,
		-0.5, 0.5, 0.5,
		-0.5, 0.5, -0.5,

		0.5, -0.5, 0.5,
		0.5, -0.5, -0.5,
		0.5, 0.5, -0.5,
		0.5, 0.5, 0.5,

		-0.5, -0.5, 0.5,
		0.5, -0.5, 0.5,
		0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5,

		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		0.5, 0.5, -0.5,
		-0.5, 0.5, -0.5
	};

	GLfloat colorData[NumVertices][3] = {
		{ 1,0,0 }, // color for vertices
		{ 0,1,0 },
		{ 0,0,1 },
		{ 1,1,1 },
		{ 1,1,0 },
		{ 0,1,1 },
		{ 1,0,1 },
		{ 0,1,1 }
	};
	
	glGenBuffers(3, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),	vertices, GL_STATIC_DRAW);
	glBindAttribLocation(program, 0, "vPosition");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glBindAttribLocation(program, 1, "vertexColor");
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glBindAttribLocation(program, 2, "vTexCoord");
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);

	location = glGetUniformLocation(program, "model_matrix");
	location2 = glGetUniformLocation(program, "camera_matrix");
	location3 = glGetUniformLocation(program, "projection_matrix");

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	
}


//---------------------------------------------------------------------
//
// display
//

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4  model_view = glm::rotate(glm::mat4(1.0), rotate_value, glm::vec3(0.0f, 0.0f, 1.0f));

	model_view = glm::translate(model_view, glm::vec3(translate_value, 0.0, 0.0));

	model_view = glm::rotate(model_view, 40*rotate_value, glm::vec3(0.0f, 0.0f, 1.0f));

	glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

	glm::mat4 camera_matrix = glm::lookAt(glm::vec3(0.0, 0.0, camera_distance), glm::vec3(0.0, 0.0, -1.0), glm::vec3(1.0, 1.0, 0.0));
	glUniformMatrix4fv(location2, 1, GL_FALSE, &camera_matrix[0][0]);

	glm::mat4 projection_matrix = glm::frustum(-1, +1, -1, +1, 1, 100);

	glUniformMatrix4fv(location3, 1, GL_FALSE, &projection_matrix[0][0]);

	glDrawArrays(GL_QUADS, 0, 4);
	glDrawArrays(GL_QUADS, 4, 4);
	glDrawArrays(GL_QUADS, 8, 4);
	glDrawArrays(GL_QUADS, 12, 4);
	glDrawArrays(GL_QUADS, 16, 4);
	glDrawArrays(GL_QUADS, 20, 4);

	glFlush();
}

void idle() {
	translate_value += 0.0001;
	rotate_value += 0.0001;
	glutPostRedisplay();
}

//---------------------------------------------------------------------
//
// main
//

void keyboardHandler(unsigned char key, int x, int y) {
	if (key == 'w') {
		translate_value -= 0.1;
	} else if (key == 's') {
		translate_value += 0.1;
	} else if (key == 'a') {
		rotate_value += 0.1;
	} else if (key == 'd') {
		rotate_value -= 0.1;
	} else if (key == '-') {
		camera_distance += 0.1;
	} else if (key == '=') {
		camera_distance -= 0.1;
	}

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("Workshop5");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init();

	glutDisplayFunc(display);

	glutKeyboardFunc(keyboardHandler);

	glutIdleFunc(idle);

	glutMainLoop();
	
	

}
