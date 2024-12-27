#pragma once

/**
 * All OBS Blueprint pin types that can be used with GUI graph.
 * Pins with type \c UNKNOWN_PIN will not be displayed.
 *
 * All PinType have a corresponding type. Please see \c PinConverter for more details.
 */
enum PinType {
	UNKNOWN_PIN,
	AUDIOVIDEO_PIN,		// ??? TODO
	AUDIO_PIN,			// ??? TODO
	VIDEO_PIN,			// video_frame()
	EXECUTION_PIN,		// std::function<void()>
	BOOLEAN_PIN,		// bool
	BYTE_PIN,			// uint8_t
	INT_PIN,			// int32_t
	FLOAT_PIN,			// float
	CHAR_PIN,			// char
	STRING_PIN,			// std::string
	COLOR_PIN,			// uint32_t
	ANY_PIN, // Used by context menu to filter
};

constexpr const char* PinName[] = {
	"UNKNOWN_PIN",
	"AUDIOVIDEO_PIN",
	"AUDIO_PIN",
	"VIDEO_PIN",
	"EXECUTION_PIN",
	"BOOLEAN_PIN",
	"BYTE_PIN",
	"INT_PIN",
	"FLOAT_PIN",
	"CHAR_PIN",
	"STRING_PIN",
	"COLOR_PIN",
	"ANY_PIN",
};


