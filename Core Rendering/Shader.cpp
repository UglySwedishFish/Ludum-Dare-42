#include "Shader.h"
#include <GL/glew.h>
#include <string> 
#include <fstream> 
#include <iostream> 

#define throwerror(s) std::cout << s

unsigned int LDEngine::Rendering::Core::LoadShader(unsigned int ShaderType, const char * File)
{
	std::string source; //all the shader code
	std::ifstream file(File); 

	if (!file.is_open()) {
		throwerror(static_cast<std::string>("Failed to open file: ") + File); 
		return 0; //-1 hurr durr
	}


	while (!file.eof()) {
		char line[50000]; 
		file.getline(line, 50000);
		std::string Line = line; 
		if (line[0] == '#' && line[1] == 'd') {
			char id[100];
			int i = sscanf(reinterpret_cast<char*>(line), "#dirincl %s", id);

			if (i != 0) { //probably shouldn't be copying any code from here, this is a strictly "it works" brute force way to do it 
				std::string Dir = "shaders/" + static_cast<std::string>(id);

				std::ifstream IncludeFile(Dir);

				if (!IncludeFile.is_open())
					throwerror(static_cast<std::string>("Failed to open included file: ") + id + " in shader: " + File);
				else
					while (!IncludeFile.eof()) {
						char IncludeLine[5000];

						IncludeFile.getline(IncludeLine, 5000);

						source += IncludeLine + '\n';

					}

			}
			else {
				throwerror(static_cast<std::string>("Failed to find command: ") + id + " in shader: " + File);

			}
		}
		else
			source += Line + '\n'; 
	}
	

	GLuint id = glCreateShader(ShaderType); //compile the shader 

	const char* csource = source.c_str();
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glShaderSource(id, 1, &csource, NULL);
	glCompileShader(id);
	char error[15000];
	glGetShaderInfoLog(id, 15000, NULL, error);


	throwerror(error); 
	std::string s = error;

		std::cout << "Shader: " << File << " compiled with either errors or warnings" << std::endl;

	return id;
}

LDEngine::Rendering::Core::Shader::Shader(const char * vertex, const char * fragment) :
	VertexShader(LoadShader(GL_VERTEX_SHADER, vertex)),
	FragmentShader(LoadShader(GL_FRAGMENT_SHADER, fragment)),
	ShaderID(glCreateProgram())
{
	glAttachShader(ShaderID, VertexShader);
	glAttachShader(ShaderID, FragmentShader); 
	glLinkProgram(ShaderID);
	glUseProgram(ShaderID);
}	

LDEngine::Rendering::Core::Shader::Shader() : 
	VertexShader(NULL),
	FragmentShader(NULL),
	ShaderID(NULL)
{
}

void LDEngine::Rendering::Core::Shader::Bind()
{
	glUseProgram(ShaderID); 
}

void LDEngine::Rendering::Core::Shader::UnBind()
{
	glUseProgram(NULL); 
}

LDEngine::Rendering::Core::Shader::~Shader()
{
	//TODO: free up shader
}
