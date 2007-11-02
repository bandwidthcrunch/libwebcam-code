#ifndef LV_CONTROLS_H
#define LV_CONTROLS_H

#include <webcam.h>


extern CControlId get_control_id (CHandle handle, const char *name);

extern CResult get_control_list (CHandle handle, CControl **controls, unsigned int *count);

extern int parse_control_value (const char *string, CControlValue *value);


#endif /* LV_CONTROLS_H */
