#include <Supergoon/Graphics/shader.h>
extern Shader *ShaderCreateImpl(void);
extern void ShaderDestroyImpl(Shader *shader);
extern void ShaderUseImpl(Shader *shader);
extern void ShaderCompileImpl(Shader *shader, const char *vertexSource,
                              const char *fragmentSource);
extern void ShaderSetUniformFloatImpl(Shader *shader, const char *name,
                                      float value, int useShader);
extern void ShaderSetUniformIntegerImpl(Shader *shader, const char *name,
                                        int value, int useShader);
extern void ShaderSetUniformVector2fImpl(Shader *shader, const char *name,
                                         float x, float y, int useShader);
extern void ShaderSetUniformVector2fVImpl(Shader *shader, const char *name,
                                          vec2 value, int useShader);
extern void ShaderSetUniformVector3fImpl(Shader *shader, const char *name,
                                         float x, float y, float z,
                                         int useShader);
extern void ShaderSetUniformVector3fVImpl(Shader *shader, const char *name,
                                          vec3 value, int useShader);
extern void ShaderSetUniformVector4fImpl(Shader *shader, const char *name,
                                         float x, float y, float z, float w,
                                         int useShader);
extern void ShaderSetUniformVector4fVImpl(Shader *shader, const char *name,
                                          vec4 value, int useShader);
extern void ShaderSetUniformMatrix4Impl(Shader *shader, const char *name,
                                        mat4 value, int useShader);

extern Shader *GetDefaultShaderImpl(void);

Shader *ShaderCreate(void) { return ShaderCreateImpl(); }
Shader *GetDefaultShader(void) { return GetDefaultShaderImpl(); }

void ShaderDestroy(Shader *shader) { ShaderDestroyImpl(shader); }
void ShaderUse(Shader *shader) { ShaderUseImpl(shader); }
void ShaderCompile(Shader *shader, const char *vertexSource,
                   const char *fragmentSource) {
  ShaderCompileImpl(shader, vertexSource, fragmentSource);
}

void ShaderSetUniformFloat(Shader *shader, const char *name, float value,
                           int useShader) {
  ShaderSetUniformFloatImpl(shader, name, value, useShader);
}
void ShaderSetUniformInteger(Shader *shader, const char *name, int value,
                             int useShader) {
  ShaderSetUniformIntegerImpl(shader, name, value, useShader);
}
void ShaderSetUniformVector2f(Shader *shader, const char *name, float x,
                              float y, int useShader) {
  ShaderSetUniformVector2fImpl(shader, name, x, y, useShader);
}
void ShaderSetUniformVector2fV(Shader *shader, const char *name, vec2 value,
                               int useShader) {
  ShaderSetUniformVector2fVImpl(shader, name, value, useShader);
}
void ShaderSetUniformVector3f(Shader *shader, const char *name, float x,
                              float y, float z, int useShader) {
  ShaderSetUniformVector3fImpl(shader, name, x, y, z, useShader);
}
void ShaderSetUniformVector3fV(Shader *shader, const char *name, vec3 value,
                               int useShader) {
  ShaderSetUniformVector3fVImpl(shader, name, value, useShader);
}
void ShaderSetUniformVector4f(Shader *shader, const char *name, float x,
                              float y, float z, float w, int useShader) {
  ShaderSetUniformVector4fImpl(shader, name, x, y, z, w, useShader);
}
void ShaderSetUniformVector4fV(Shader *shader, const char *name, vec4 value,
                               int useShader) {
  ShaderSetUniformVector4fVImpl(shader, name, value, useShader);
}
void ShaderSetUniformMatrix4(Shader *shader, const char *name, mat4 value,
                             int useShader) {
  ShaderSetUniformMatrix4Impl(shader, name, value, useShader);
}
