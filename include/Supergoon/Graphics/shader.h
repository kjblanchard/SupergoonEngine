#pragma once
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct Shader Shader;

// Creates a shader, not usable until it is compiled with some kind of a shader
// with Shader compile.
Shader *ShaderCreate(void);
void ShaderDestroy(Shader *shader);
void ShaderUse(Shader *shader);
Shader *GetDefaultShader(void);
Shader *GetDefaultTextShader(void);
Shader *GetDefaultRectShader(void);
// Compiles shader info
void ShaderCompile(Shader *shader, const char *vertexFile,
				   const char *fragmentFile);
void ShaderSetUniformFloat(Shader *shader, const char *name, float value,
						   int useShader);
void ShaderSetUniformInteger(Shader *shader, const char *name, int value,
							 int useShader);
void ShaderSetUniformVector2f(Shader *shader, const char *name, float, float,
							  int useShader);
void ShaderSetUniformVector2fV(Shader *shader, const char *name, vec2 value,
							   int useShader);
void ShaderSetUniformVector3f(Shader *shader, const char *name, float, float,
							  float, int useShader);
void ShaderSetUniformVector3fV(Shader *shader, const char *name, vec3 value,
							   int useShader);
void ShaderSetUniformVector4f(Shader *shader, const char *name, float, float,
							  float, float, int useShader);
void ShaderSetUniformVector4fV(Shader *shader, const char *name, vec4 value,
							   int useShader);
void ShaderSetUniformMatrix4(Shader *shader, const char *name, mat4 value,
							 int useShader);
#ifdef __cplusplus
}
#endif
