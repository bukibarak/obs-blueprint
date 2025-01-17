#pragma once

/**
 * All OBS Blueprint pin types that can be used with GUI graph.
 * Pins with type \c UNKNOWN_PIN will not be displayed.
 *
 * All PinType have a corresponding type. Please see \c PinConverter for more details.
 */
enum PinType : unsigned char {
	UNKNOWN_PIN,		// void (void* ?)
	AUDIOVIDEO_PIN,		// ??? TODO
	AUDIO_PIN,			// ??? TODO
	VIDEO_PIN,			// OBSFrame()
	EXECUTION_PIN,		// std::function<void()>
	BOOLEAN_PIN,		// bool
	BYTE_PIN,			// uint8_t
	INT_PIN,			// int32_t
	FLOAT_PIN,			// float
	CHAR_PIN,			// char
	STRING_PIN,			// std::string
	COLOR_PIN,			// uint32_t
	ANY_PIN,			// Used by context menu to filter
};


