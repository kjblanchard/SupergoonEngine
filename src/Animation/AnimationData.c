#include <Supergoon/Animation/animationdata.h>
#include <Supergoon/json.h>
#include <Supergoon/log.h>
#include <stdlib.h>
#include <string.h>

AnimationData* CreateAnimationData() {
	AnimationData* data = malloc(sizeof(*data));
	return data;
}

void CreateAnimationDataFromAsepriteFile(AnimationData* animationData, const char* filename) {
	json_object* root = jGetObjectFromFile(filename);
	if (!root) {
		sgLogError("Could not load animation data for %s", filename);
		return;
	}
	json_object* frameObject = jobj(root, "frames");
	animationData->frameCount = jGetObjectArrayLength(frameObject);
	animationData->frames = calloc(animationData->frameCount, sizeof(Frame));
	for (size_t i = 0; i < animationData->frameCount; i++) {
		Frame* frame = &animationData->frames[i];
		json_object* currentObj = jGetObjectInObjectWithIndex(frameObject, i);
		frame->duration = jint(currentObj, "duration");
		json_object* rectObj = jobj(currentObj, "frame");
		frame->frame.x = jint(rectObj, "x");
		frame->frame.y = jint(rectObj, "y");
		frame->frame.w = jint(rectObj, "w");
		frame->frame.h = jint(rectObj, "h");
		frame->rotated = jbool(currentObj, "rotated");
		json_object* sourceSizeObject = jobj(currentObj, "sourceSize");
		frame->sourceSize.h = jint(sourceSizeObject, "h");
		frame->sourceSize.w = jint(sourceSizeObject, "w");
	}
	json_object* metaObject = jobj(root, "meta");
	animationData->meta.image = strdup(jstr(metaObject, "image"));
	json_object* frameTabObject = jobj(metaObject, "frameTags");
	animationData->meta.frameTagCount = jGetObjectArrayLength(frameTabObject);
	animationData->meta.frameTags = calloc(animationData->meta.frameTagCount, sizeof(FrameTag));
	for (size_t i = 0; i < animationData->meta.frameTagCount; i++) {
		json_object* currentObj = jGetObjectInObjectWithIndex(frameTabObject, i);
		animationData->meta.frameTags[i].name = strdup(jstr(currentObj, "name"));
		animationData->meta.frameTags[i].from = jint(currentObj, "from");
		animationData->meta.frameTags[i].to = jint(currentObj, "to");
		const char* direction = jstr(currentObj, "direction");
		if (strcmp(direction, "pingpong") == 0) {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsPingPong;
		} else if (strcmp(direction, "forward") == 0) {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsForward;
		} else {
			animationData->meta.frameTags[i].direction = AnimationDataDirectionsDefault;
		}
	}
	jReleaseObjectFromFile(root);
}
