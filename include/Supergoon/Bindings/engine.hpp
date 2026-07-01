#pragma once
#include <Supergoon/Primitives/Color.h>
#include <Supergoon/Primitives/rectangle.h>

#include <string>
struct Sprite;
struct Text;
struct sgGameObject;
typedef struct Texture Texture;

namespace Etf {

enum class CurrentSceneLoadingState {
	NotLoading,
	NextSceneQueued,
	WaitingForFadeOut,
	LoadingStart,
	LoadingMap,
	LoadingGameObjects,
	LoadingUI,
	LoadingDialog,
	LoadingFinish,
	// Use this to not add a big jump to the loading after a load
	JustLoaded,
	FadingIn,
	FadingInAllowUpdate,
};

enum class ScreenFadeTypes {
	NotFading,
	FadeIn,
	FadeOut,
};

struct Engine {
   public:
	static void InitializeEngine();
	static void ShutdownEngine();
	// static const std::string& CurrentScene();
	static void LoadScene(const std::string& name = "", float fadeOutTime = 1.0f, float fadeInTime = 1.0f, bool playTransitionSound = true);
	// TODO Do we even need this anymore?  Probably not
	static Sprite* CreateSpriteFull(const std::string& name, float* followX, float* followY, RectangleF sourceRect, RectangleF offsetSizeRect);
	// Used for UI mainly, cause we need to handle drawing it outselves on top of everything
	static Sprite* CreateManualSpriteFull(const std::string& name, float* followX, float* followY, RectangleF sourceRect, RectangleF offsetSizeRect);
	static void SetSpriteVisible(Sprite* sprite, bool visible);
	static void DrawRectPrimitive(RectangleF& rect, Color color = {255, 0, 0, 255}, bool filled = false, bool cameraOffset = true);
	// Returns if we are currently loading a scene
	static bool HandleMapLoad();
	// Fades out the full screen FBO if we aren't already fading
	static void StartFullScreenFade(float time, ScreenFadeTypes fadeType);
	// if screen is fading, updates the time on it and tweens the fade.
	static void UpdateScreenFade();
	static void PreloadAssets();
	struct Textures {
		static void LoadTextureFromBuffer(Texture* tex, const std::string& name);

	} Textures;

	struct Audio {
		static void PlayBGM(const std::string& name, float volume = 1.0f);
		static void PlayBGMBackground(const std::string& name, float volume = 1.0f);
		static void PlaySFX(const std::string& name, float volume);
		static void PlaySFXBuffer(const std::string& name, float volume);
		static void StopBGMBackground();

	} Audio;

	struct Tweening {
		enum class TweenEaseTypes {
			Linear,
			QuintOut
		};
		static float GetTweenedValue(float start, float end, float timeSeconds, float totalSeconds, TweenEaseTypes ease = TweenEaseTypes::Linear);
	} Tweening;

	struct TextBoi {
		static Text* CreateText(const std::string& fontName, unsigned int fontSize, RectangleF location, const std::string& text, unsigned int numChars, bool centeredX, bool centeredY);
		static void DrawText(Text* text, float xOffset, float yOffset, Color& color);
	} Textboi;

	struct Json {
		static void GetJsonBufferFromDirectory(const char* name, char** buf, size_t* sz);
		static RectangleF GetRectFromObject(void* object, const std::string& key);
		template <typename Lambda>
		static void jforeach_lambda(void* obj, Lambda&& lambda) {
			struct Wrapper {
				Lambda* fn;
			};
			Wrapper wrapper{&lambda};

			jforeach_obj(
				obj,
				[](const char* key, void* value, void* userData) {
					auto* w = static_cast<Wrapper*>(userData);
					(*w->fn)(key, value);
				},
				&wrapper);
		}
	};

	// function engine.Collision.CheckForCollision(a, b)
	// return a.x < b.x + b.w and
	// a.x + a.w > b.x and
	// a.y < b.y + b.h and
	// a.y + a.h > b.y
	// end

	static inline bool CheckForRectCollision(RectangleF& lhs, RectangleF& rhs) {
		return lhs.x < rhs.x + rhs.w && lhs.x + lhs.w > rhs.x && lhs.y < rhs.y + rhs.h && lhs.y + lhs.h > rhs.y;
	}

   private:
	static void loadSceneInternal();
};

}  // namespace Etf
