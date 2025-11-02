#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
// Need to do glad first
#include <SDL3/SDL_opengl.h>
#else
#include <GLES3/gl3.h>
#include <SDL3/SDL_opengles2.h>
#endif
#include <Supergoon/Graphics/shader.h>
#include <Supergoon/Platform/opengl/openglGraphics.h>
#include <Supergoon/filesystem.h>
#include <Supergoon/log.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>
#include <stdio.h>
#include <string.h>

#define FILE_LENGTH 512
#define NUM_CACHED_SHADERS 30
#define DEFAULT_VERTEX_FILENAME "2dSpriteVertex"
#define DEFAULT_FRAGMENT_FILENAME "2dSpriteFragment"
#define DEFAULT_TEXT_VERTEX_FILENAME "2dTextVertex"
#define DEFAULT_TEXT_FRAGMENT_FILENAME "2dTextFragment"

typedef struct CachedShaderFile {
	char *Data;
	char *Name;
} CachedShaderFile;

typedef enum ShaderType {
	ShaderTypeVertex,
	ShaderTypeFragment,
	ShaderTypeProgram,
} ShaderType;
static Shader *_defaultShader = NULL;
static Shader *_defaultTextShader = NULL;

CachedShaderFile _cachedShaders[NUM_CACHED_SHADERS];

char *getShaderDataFromFile(const char *filename) {
	const char *suffix = "";
#ifdef __EMSCRIPTEN__
	suffix = "E";
#endif
	char *filepath;
	asprintf(&filepath, "%sassets/shaders/%s%s%s", GetBasePath(), filename,
			 suffix, ".glsl");
	char *data = GetContentOfFileString(filepath);
	free(filepath);
	return data;
}

static void cacheShader(const char *name, char *data) {
	CachedShaderFile *cache;
	for (size_t i = 0; i < NUM_CACHED_SHADERS; i++) {
		cache = &_cachedShaders[i];
		if (cache->Data) {
			continue;
		}
		cache->Data = data;
		cache->Name = strdup(name);
		break;
	}
}

static char *getCachedShader(const char *name) {
	for (size_t i = 0; i < NUM_CACHED_SHADERS; i++) {
		char *iName = _cachedShaders[i].Name;
		if (!iName) {
			continue;
		}
		if (strcmp(name, iName) == 0) {
			return _cachedShaders[i].Data;
		}
	}
	char *data = getShaderDataFromFile(name);
	if (!data) {
		return NULL;
	}
	cacheShader(name, data);
	return data;
}

static void freeCachedShader(CachedShaderFile *shader) {
	free(shader->Name);
	shader->Name = NULL;
	free(shader->Data);
	shader->Data = NULL;
}

static void checkCompileErrors(unsigned int object, ShaderType type) {
	GLint success;
#define LOG_SIZE 1024
	char infoLog[LOG_SIZE];
	switch (type) {
		case ShaderTypeProgram:
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(object, LOG_SIZE, NULL, infoLog);
				sgLogWarn("Shader Link time error for type %d: %s", type, infoLog);
			}
			break;
		default:
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(object, LOG_SIZE, NULL, infoLog);
				sgLogWarn("Shader compile time error for type %d: %s", type, infoLog);
			}
			break;
	}
}

Shader *ShaderCreateImpl(void) {
	Shader *shader = malloc(sizeof(Shader));
	shader->ID = 0;
	return shader;
}

void ShaderUseImpl(Shader *shader) { glUseProgram(shader->ID); }

void ShaderCompileImpl(Shader *shader, const char *vertexSourceFile,
					   const char *fragmentSourceFile) {
	const char *vertexData = getCachedShader(vertexSourceFile);
	const char *fragmentData = getCachedShader(fragmentSourceFile);
	if (!vertexData || !fragmentData) {
		sgLogWarn("Could not get shader data, compilation failure!");
	}
	unsigned int sVertex, sFragment;
	// vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexData, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, ShaderTypeVertex);
	// fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentData, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, ShaderTypeFragment);
	// shader program
	shader->ID = glCreateProgram();
	glAttachShader(shader->ID, sVertex);
	glAttachShader(shader->ID, sFragment);
	glLinkProgram(shader->ID);
	checkCompileErrors(shader->ID, ShaderTypeProgram);
	// delete the shaders as they're linked into our program now and no longer
	// necessary
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
}
void ShaderSetUniformFloatImpl(Shader *shader, const char *name, float value,
							   int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void ShaderSetUniformIntegerImpl(Shader *shader, const char *name, int value,
								 int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void ShaderSetUniformVector2fImpl(Shader *shader, const char *name, float x,
								  float y, int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform2f(glGetUniformLocation(shader->ID, name), x, y);
}
void ShaderSetUniformVector2fVImpl(Shader *shader, const char *name, vec2 value,
								   int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform2f(glGetUniformLocation(shader->ID, name), value[0], value[1]);
}
void ShaderSetUniformVector3fImpl(Shader *shader, const char *name, float x,
								  float y, float z, int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}
void ShaderSetUniformVector3fVImpl(Shader *shader, const char *name, vec3 value,
								   int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform3f(glGetUniformLocation(shader->ID, name), value[0], value[1],
				value[2]);
}
void ShaderSetUniformVector4fImpl(Shader *shader, const char *name, float x,
								  float y, float z, float w, int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
}
void ShaderSetUniformVector4fVImpl(Shader *shader, const char *name, vec4 value,
								   int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniform4f(glGetUniformLocation(shader->ID, name), value[0], value[1],
				value[2], value[3]);
}
void ShaderSetUniformMatrix4Impl(Shader *shader, const char *name, mat4 value,
								 int useShader) {
	if (useShader)
		ShaderUseImpl(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE,
					   value[0]);
}

void ShaderSystemShutdown(void) {
	for (size_t i = 0; i < NUM_CACHED_SHADERS; i++) {
		freeCachedShader(&_cachedShaders[i]);
	}
}

Shader *GetDefaultShaderImpl(void) {
	if (_defaultShader) {
		return _defaultShader;
	}
	_defaultShader = ShaderCreateImpl();
	ShaderCompileImpl(_defaultShader, DEFAULT_VERTEX_FILENAME, DEFAULT_FRAGMENT_FILENAME);

	return _defaultShader;
}

Shader *GetDefaultTextShaderImpl(void) {
	if (_defaultTextShader) {
		return _defaultTextShader;
	}
	_defaultTextShader = ShaderCreateImpl();
	ShaderCompileImpl(_defaultTextShader, DEFAULT_VERTEX_FILENAME, DEFAULT_TEXT_FRAGMENT_FILENAME);
	return _defaultTextShader;
}

void ShaderDestroyImpl(Shader *shader) { free(shader); }
