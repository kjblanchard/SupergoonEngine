#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef struct json_object json_object;

typedef enum JsonObjectTypes {

	JsonObjectTypes_Default,
	JsonObjectTypes_Int,
	JsonObjectTypes_Float,
	JsonObjectTypes_String,

} JsonObjectTypes;

json_object* jobj(json_object* o, const char* key);

int jint(json_object* o, const char* key);
int jintIndex(json_object* o, int index);

float jfloat(json_object* o, const char* key);
int jbool(json_object* o, const char* key);

const char* jstr(json_object* o, const char* key);

JsonObjectTypes jGetObjectType(json_object* o);
json_object* jGetObjectInObjectWithIndex(json_object* o, int index);
int jGetObjectArrayLength(json_object* o);
json_object* jGetObjectFromFile(const char* file);
void jReleaseObjectFromFile(json_object* o);

#ifdef __cplusplus
}
#endif
