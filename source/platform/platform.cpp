/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cassert>
#include <cstdio>
#include <cstring>
#include <gl/glew.h>
#include <gl/glu.h>
#include <SDL.h>
#include <SDL_opengl.h>

enum ERunResult : int
{
	eRR_Success = 0,
	eRR_Error = 1
};

void PrintProgramLog(GLuint program)
{
	if (glIsProgram(program))
	{
		int infoLogLength = 0;
		int maxLength = 0;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = (char*)malloc(maxLength);
		glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			printf("[%s] %s\n", __FUNCTION__, infoLog);
		}

		free(infoLog);
	}
	else
	{
		printf("[%s] Name %d is not a program\n", __FUNCTION__, program);
	}
}

void PrintShaderLog(GLuint shader)
{
	if (glIsShader(shader))
	{
		int infoLogLength = 0;
		int maxLength = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = (char*)malloc(maxLength);
		glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
		if (infoLogLength > 0)
		{
			printf("[%s] %s\n", __FUNCTION__, infoLog);
		}

		free(infoLog);
	}
	else
	{
		printf("[%s] Name %d is not a shader\n", __FUNCTION__, shader);
	}
}

namespace platform
{

	int RunPlatform()
	{
		/////////////////////////////////////////////////////////
		// Initialization
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		// TODO: Update to 4.6
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		static const int kScreenWidth = 1024;
		static const int kScreenHeight = 768;

		SDL_Window* pWindow = SDL_CreateWindow(
			"Dirac Sea Engine",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			kScreenWidth,
			kScreenHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		if (pWindow == nullptr)
		{
			printf("SDL could not create window! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		SDL_GLContext pGLContext = SDL_GL_CreateContext(pWindow);
		if (pGLContext == nullptr)
		{
			printf("SDL could not create opengl context! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		glewExperimental = GL_TRUE;
		GLenum glewInitResult = glewInit();
		if (glewInitResult != GLEW_OK)
		{
			printf("Error initializing GLEW! %s\n", glewGetErrorString(glewInitResult));
			return eRR_Error;
		}

		// Enable VSync
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			printf("SDL could not set VSync! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		SDL_Surface* pScreenSurface = SDL_GetWindowSurface(pWindow);
		if (pScreenSurface == nullptr)
		{
			printf("SDL could not create window surface! SDL_Error: %s\n", SDL_GetError());
			return eRR_Error;
		}

		// OpenGL state
		GLuint gProgramID = 0;
		GLint gVertexPos2DLocation = -1;
		GLuint gVBO = 0;
		GLuint gIBO = 0;

		// OpenGL initialization
		{
			gProgramID = glCreateProgram();

			// Compile and attach vertex Shader
			{
				GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

				const GLchar* vertexShaderSource[] =
				{
					"#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
				};

				glShaderSource(vertexShader, 1, vertexShaderSource, nullptr);
				glCompileShader(vertexShader);
				GLint vShaderCompiled = GL_FALSE;
				glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
				if (vShaderCompiled != GL_TRUE)
				{
					printf("Unable to compile vertex shader %d!", vertexShader);
					PrintShaderLog(vertexShader);
					return eRR_Error;
				}

				glAttachShader(gProgramID, vertexShader);
			}

			// Compile and attach fragment shader
			{
				GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

				const GLchar* fragmentShaderSource[] =
				{
					"#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 0.1, 0.6, 0.7, 1.0 ); }"
				};

				glShaderSource(fragmentShader, 1, fragmentShaderSource, nullptr);
				glCompileShader(fragmentShader);

				GLint fShaderCompiled = GL_FALSE;
				glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
				if (fShaderCompiled != GL_TRUE)
				{
					printf("Unable to compile fragment shader %d!", fragmentShader);
					PrintShaderLog(fragmentShader);
					return eRR_Error;
				}

				glAttachShader(gProgramID, fragmentShader);
			}

			glLinkProgram(gProgramID);

			{
				GLint programSuccess = GL_TRUE;
				glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
				if (programSuccess != GL_TRUE)
				{
					printf("Error linking gl program %d!\n", gProgramID);
					PrintProgramLog(gProgramID);
					return eRR_Error;
				}
			}

			gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
			if (gVertexPos2DLocation == -1)
			{
				printf("LVertexPos2D is not a valid glsl program variable!\n");
				return eRR_Error;
			}

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			GLfloat vertexData[] =
			{
				-0.5f, -0.5f,
				 0.5f, -0.5f,
				 0.5f,  0.5f,
				-0.5f,  0.5f
			};

			GLuint indexData[] = { 0, 1, 2, 3 };

			// Create VBO
			{
				glGenBuffers(1, &gVBO);
				glBindBuffer(GL_ARRAY_BUFFER, gVBO);
				glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
			}

			// Create IBO
			{
				glGenBuffers(1, &gIBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
			}
		} // ~Gl Initialization



		/////////////////////////////////////////////////////////
		// Runtime

		glClear(GL_COLOR_BUFFER_BIT);
		for (size_t i = 0; i < 32; ++i)
		{
			glUseProgram(gProgramID);
			glEnableVertexAttribArray(gVertexPos2DLocation);

			// set vertex data
			glBindBuffer(GL_ARRAY_BUFFER, gVBO);
			glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

			// set index data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);

			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, nullptr);

			glDisableVertexAttribArray(gVertexPos2DLocation);
			glUseProgram(NULL);

			SDL_GL_SwapWindow(pWindow);
		}

		/////////////////////////////////////////////////////////
		// Deconstruction
		glDeleteProgram(gProgramID);
		SDL_DestroyWindow(pWindow);
		SDL_Quit();
		return eRR_Success;
	}

} // platform namespace
