#include <Supergoon/Animation/animationdata.h>
#include <Supergoon/json.h>
#include <sgtools/log.h>
#include <stdlib.h>
#include <string.h>

AnimationData* CreateAnimationData(void) {
	AnimationData* d = malloc(sizeof(*d));
	return d;
}

static void loadAnimDataInternal(AnimationData* animationData, json_object* root) {
	json_object* frameObject = jobj(root, "frames");
	animationData->frameCount = jGetObjectArrayLength(frameObject);
	animationData->frames = calloc((unsigned long)animationData->frameCount, sizeof(Frame));
	for (int i = 0; i < animationData->frameCount; i++) {
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
	animationData->meta.frameTags = calloc((unsigned long)animationData->meta.frameTagCount, sizeof(FrameTag));
	for (int i = 0; i < animationData->meta.frameTagCount; i++) {
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
}

void CreateAnimationDataFromAsepriteBuffer(AnimationData* animationData, char* buf, size_t sz) {
	json_object* root = jGetObjectFromBuffer(buf, sz);
	if (!root) {
		sgLogError("Could not load animation data from buffer ");
		animationData->frameCount = 0;
		animationData->frames = NULL;
		animationData->meta.frameTagCount = 0;
		animationData->meta.frameTags = NULL;
		animationData->meta.image = NULL;
		return;
	}
	loadAnimDataInternal(animationData, root);
	jReleaseObjectFromFile(root);
}

void CreateAnimationDataFromAsepriteFile(AnimationData* animationData, const char* filename) {
	json_object* root = jGetObjectFromFile(filename);
	if (!root) {
		sgLogError("Could not load animation data for %s", filename);
		animationData->frameCount = 0;
		animationData->frames = NULL;
		animationData->meta.frameTagCount = 0;
		animationData->meta.frameTags = NULL;
		animationData->meta.image = NULL;
		return;
	}
	loadAnimDataInternal(animationData, root);
}

void DestroyAnimationData(AnimationData* data) {
	if (!data) return;
	free(data->frames);
	free(data->meta.image);
	if (data->meta.frameTags) {
		for (size_t i = 0; i < data->meta.frameTagCount; i++) {
			free(data->meta.frameTags[i].name);
		}
		free(data->meta.frameTags);
	}
	free(data);
}
