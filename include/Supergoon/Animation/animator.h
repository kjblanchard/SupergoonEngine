/**
 * @file Animator.h
 * @author kevin blanchard (kevin@supergoon.com)
 * @brief Animators are used for heach aseprite file and handles gametime updates to all Instances of this, as it holds the animation data, and the list
 * @version 0.1
 * @date 2025-06-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int AnimatorHandle;

AnimatorHandle CreateAnimator(const char* filename);
void PlayAnimation(AnimatorHandle animator, const char* anim);
// 1.0 is regular, 0.0 is none, etc
void SetAnimatorAnimationSpeed(AnimatorHandle animator, float speed);
void DestroyAnimator(AnimatorHandle animator);

#ifdef __cplusplus
}
#endif
