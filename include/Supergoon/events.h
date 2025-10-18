#pragma once
#include <stdint.h>

/**
 * @brief The builtin events for the engine.  This struct will be filled with ids you
 * can use to push events of this type to the event system
 */
typedef struct BuiltinEventTypes {
	uint32_t LoadBgmEvent;
	uint32_t PlayBgmEvent;
	uint32_t StopBgmEvent;
	uint32_t PauseBgmEvent;
	uint32_t StartTweenEvent;
	uint32_t PauseTweenEvent;
	uint32_t StopTweenEvent;
} BuiltinEventTypes;
/**
 * @brief Event Ids to be used when pushing events
 */
extern BuiltinEventTypes BuiltinEventIds;
/**
 * @brief Initialize the event engine
 */
void InitializeEventEngine(void);
/**
 * @brief Push an event to be handled next update.
 * Events are handled at that start of an update loop.
 * First builtin events are handled, then custom events
 *
 * @param eventType Type of event to push
 * @param eventCode userdata
 * @param data userdata
 * @param data2 userdata
 */
void PushEvent(uint32_t eventType, int eventCode, void *data, void *data2);
/**
 * @brief Handle all the engine builtin events, called in the main loop
 *
 * @param event
 * @return int
 */
int HandleEvents(void *event);
/**
 * @brief Function to a custom event handler function, this will be called after the
 * builtin event handler
 *
 * @param EventHandlerFunction
 */
void SetCustomEventHandler(int (*eventHandlerFunction)(void *event));
/**
 * @brief Function to a custom event handler function, this will be called after the
 * builtin event handler
 */
int HandleCustomEventHandler(void *event);
