#include <Supergoon/json.h>
#include <Supergoon/log.h>
#include <json_object.h>
#include <json_types.h>
#include <json_util.h>
#include <json.h>

json_object* jobj(json_object* o, const char* key) {
	json_object* out = NULL;
	json_object_object_get_ex(o, key, &out);
	return out;
}

int jint(json_object* o, const char* key) {
	json_object* v = jobj(o, key);
	return v ? json_object_get_int(v) : 0;
}

int jintIndex(json_object* o, int index) {
	json_object* v = json_object_array_get_idx(o, index);
	return v ? json_object_get_int(v) : 0;
}

float jfloat(json_object* o, const char* key) {
	json_object* v = jobj(o, key);
	return v ? (float)json_object_get_double(v) : 0.0f;
}

const char* jstr(json_object* o, const char* key) {
	json_object* v = jobj(o, key);
	return v ? json_object_get_string(v) : NULL;
}

const char* jstrIndex(json_object* o, int index){
	json_object* v = json_object_array_get_idx(o, index);
	return v ? json_object_get_string(v) : "";
}

int jbool(json_object* o, const char* key) {
	json_object* v = jobj(o, key);
	return json_object_get_boolean(v);
}


JsonObjectTypes jGetObjectType(json_object* o) {
	switch (json_object_get_type(o)) {
		case json_type_int:
			return JsonObjectTypes_Int;
		case json_type_double:
			return JsonObjectTypes_Float;
		case json_type_string:
			return JsonObjectTypes_String;
		default:
			sgLogError("Could not get object type from json, it's not implemented currently");
			return JsonObjectTypes_Default;
	}
	return JsonObjectTypes_Default;
}
json_object* jGetObjectInObjectWithIndex(json_object* o, int index) {
	return json_object_array_get_idx(o, index);
}
int jGetObjectArrayLength(json_object* o) { return json_object_array_length(o); }
json_object* jGetObjectFromFile(const char* file) { return json_object_from_file(file); }
void jReleaseObjectFromFile(json_object* o) { json_object_put(o); }

void jforeach_obj(void* obj, JsonIterFn fn, void* userData) {
	json_object* o = (json_object*)obj;

	json_object_object_foreach(o, key, val) {
		fn(key, val, userData);
	}
}
