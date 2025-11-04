#ifdef __cplusplus
extern "C" {
#endif
#define MAX_TRACKS 4
extern float _globalBgmVolume;
extern float _globalSfxVolume;
struct Bgm;

typedef struct AudioBgmAsset {
	struct Bgm* BgmPtr;
	float Volume;
} AudioBgmAsset;
extern AudioBgmAsset _bgmAssets[MAX_STREAMS];

#ifdef __cplusplus
}
#endif
