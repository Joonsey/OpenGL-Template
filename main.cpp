#include <iostream>
#include <optional>
#include <fstream>
#include <string>
#include <sstream>


#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>


struct shaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

void update()
{

}

static shaderProgramSource parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class shaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];
	shaderType type = shaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
			{
				// identify a shader in the shader script

			if (line.find("vertex") != std::string::npos)
				{
					type = shaderType::VERTEX;
				}
			else if (line.find("fragment") != std::string::npos)
				{
					type = shaderType::FRAGMENT;
				}

		}
		else
		{
			// push content shader content into stringstream
			//
			ss[(int)type] << line << "\n";
		}
	}
	return {ss[0].str(), ss[1].str()};
}

static int compileShader(unsigned int type, const std::string& source)
{

	unsigned int shader = glCreateShader(type);
	const char* src = &source[0];
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length*sizeof(char));

		glGetShaderInfoLog(shader,length, &length, message);
		std::cout << "Error compiling shader of type " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(shader);
	}
	return shader;
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, 1);
		}
}
void input(GLFWwindow* window)
{
	glfwPollEvents();
	glfwSetKeyCallback(window, key_callback);
}

void draw(GLFWwindow* window)
{

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// draw before this
	glfwSwapBuffers(window);

}


int main(){
	int SCREEN_WIDTH = 1080;
	int SCREEN_HEIGHT = 720;
	GLFWwindow* window;
	if (!glfwInit())
	{
		// checking openGL didn't cuck me again
		std::cout << "Failed initializing openGL \n";
	}

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL,NULL);

	if (!window)
	{
		// check for window initialization
		glfwTerminate();
		std::cout << "Failed to make window \n";
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		// check for glew compatatiblity
		std::cout << "Error initializing glew!\n";
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};
	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6*2*sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
			0,				 // start
			2,				 // size or length of attribute
			GL_FLOAT,		 // GL_TYPE in the buffer
			GL_FALSE,		 // normalized | dont need to do this for float
			2*sizeof(float), // size of data
			0                // offset in buffer
			);


	unsigned int indexBufferObject;
	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indices, GL_STATIC_DRAW);

	shaderProgramSource shader_sources = parseShader("res/shaders/basic.shader");
	//std::cout << shader_sources.VertexSource << std::endl;
	//std::cout << shader_sources.FragmentSource << std::endl;
	glUseProgram(createShader(shader_sources.VertexSource, shader_sources.FragmentSource));

	while(!glfwWindowShouldClose(window))
	{
		// main loop
		update();
		input(window);
		draw(window);
	};
	glfwTerminate();
	return 0;
}

