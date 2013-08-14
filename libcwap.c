#include "libcwap.h"

struct libcwap_functions * registered_functions = NULL;

inline void libcwap_action(uint8_t (*read_function)(uint8_t *, uint8_t));
	char action;
	read_function(&action, 1);
	switch (action) {
		case 'T':
			uint8_t[4] data;
			if (!read_function(data, 4))
				break;
			if (registered_functions->time_set_function != NULL)
				registered_functions->time_set_function((time_t) data);
			break;
		case 'O':
			uint8_t[4] data;
			if (!read_function(data, 4))
				break;
			if (registered_functions->alarm_set_timestamp != NULL)
				registered_functions->alarm_set_timestamp((time_t) data);
			break;
		// etc.
		default:
			; // Assume the data was garbage.
	}
}

inline void libcwap_register(struct libcwap_functions * funs) {
	registered_functions = funs;
}