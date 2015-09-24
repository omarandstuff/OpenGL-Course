#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <cmath>
#include <iostream>


float quad[] =
{
	-0.5f,	 0.5f,	0.0f,	1.0f,	1.0f,	1.0f,
	 0.5f,	 0.5f,	0.0f,	0.0f,	1.0f,	1.0f,
	 0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	0.0f,
	-0.5f,	-0.5f,	0.0f,	1.0f,	0.0f,	1.0f
};

unsigned int quadIndex[] =
{
	0, 2, 3,
	0, 1, 2
};

GLuint program_ID;
GLuint quad_ID;
GLuint quad_b_ID;
GLuint quad_i_ID;

float quad_left;
float quad_top;

float current_time;
float last_time;

void init(void);
void idle(void);
void display(void);
void reshape(int width, int height);

static char* readShaderSource(const char* shaderFile);
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

int main( int argc, char **argv )
{
 glutInit( &argc, argv );
 glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
 glutInitWindowSize( 800, 480 );
 glutInitContextVersion( 3, 0 );
 glutInitContextProfile( GLUT_CORE_PROFILE );
 glutCreateWindow( "Graficos para video juegos." );

	glewExperimental = TRUE;
	glewInit();

 init();

 glutDisplayFunc( display );
 glutReshapeFunc( reshape );
 glutIdleFunc( idle );

 glutMainLoop();
 return 0;
}

void init(void)
{
	glClearColor(0.1, 0.5, 0.9, 1.0);

	program_ID = InitShader("vshader.glsl", "fshader.glsl");

	glGenVertexArrays(1, &quad_ID);
	glBindVertexArray(quad_ID);

	glGenBuffers(1, &quad_b_ID);
	glBindBuffer(GL_ARRAY_BUFFER, quad_b_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, quad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (unsigned char*)NULL + (3 * sizeof(float)));

	glGenBuffers(1, &quad_i_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_i_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, quadIndex, GL_STATIC_DRAW);
}

void idle(void)
{
	current_time = glutGet(GLUT_ELAPSED_TIME);

	float time = (current_time - last_time) / 1000.0f;

	static float angle = 0.0f;
	angle += time;

	quad_left = cos(angle);
	quad_top = sin(angle);

	last_time = current_time;
	glutPostRedisplay();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1f(glGetUniformLocation(program_ID, "left"), quad_left);
	glUniform1f(glGetUniformLocation(program_ID, "top"), quad_top);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_i_ID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}


static char* readShaderSource(const char* shaderFile)
{
	FILE* fp = fopen(shaderFile, "r");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

GLuint InitShader(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader
	{
		const char*  filename;
		GLenum    type;
		GLchar*   source;
	}shaders[2] = { { vShaderFile, GL_VERTEX_SHADER, NULL }, { fShaderFile, GL_FRAGMENT_SHADER, NULL } };

	GLuint program = glCreateProgram();

	for (int i = 0; i < 2; ++i)
	{
		Shader& s = shaders[i];
		s.source = readShaderSource(s.filename);
		if (shaders[i].source == NULL)
		{
			std::cerr << "Failed to read " << s.filename << std::endl;
			exit(EXIT_FAILURE);
		}

		GLuint shader = glCreateShader(s.type);
		glShaderSource(shader, 1, (const GLchar**)&s.source, NULL);
		glCompileShader(shader);

		GLint  compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			std::cerr << s.filename << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			std::cerr << logMsg << std::endl;
			delete[] logMsg;

			exit(EXIT_FAILURE);
		}

		delete[] s.source;

		glAttachShader(program, shader);
	}

	glBindAttribLocation(program, 0, "position");
	glBindAttribLocation(program, 1, "vcolor");

	/* link  and error check */
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	/* use program object */
	glUseProgram(program);

	return program;
}