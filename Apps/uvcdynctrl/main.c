/*
 * uvcdynctrl - Manage dynamic controls in uvcvideo
 *
 *
 * Copyright (c) 2006-2007 Logitech.
 *
 * This file is part of uvcdynctrl.
 * 
 * uvcdynctrl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * uvcdynctrl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with uvcdynctrl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmdline.h"
#include "controls.h"


static struct gengetopt_args_info args_info;


#define ARRAY_SIZE(a)		(sizeof(a) / sizeof((a)[0]))
#define HAS_VERBOSE()		(args_info.verbose_given)


static void
print_error (const char *error, CResult res)
{
	if((int)res < 0) {
		printf("ERROR: %s.\n", error);
	}
	else {
		char *text = c_get_error_text(res);
		if(text) {
			printf("ERROR: %s: %s (%d)\n", error, text, res);
			free(text);
		}
		else {
			printf("ERROR: %s: Unknown error (%d)\n", error, res);
		}
	}
}


static
const char *get_control_type (CControlType type)
{
	switch(type) {
		case CC_TYPE_RAW:		return "Raw";
		case CC_TYPE_BOOLEAN:	return "Boolean";
		case CC_TYPE_CHOICE:	return "Choice";
		case CC_TYPE_BYTE:		return "Byte";
		case CC_TYPE_WORD:		return "Word";
		case CC_TYPE_DWORD:		return "Dword";
		default:
			return "<Unknown>";
	}
}


// Note: The caller must free the buffer obtained from this function.
static char *
get_control_flags (CControlFlags flags)
{
	const char *delimiter = ", ";
	const char *names[] = {
		"CAN_READ",
		"CAN_WRITE",
		"CAN_NOTIFY",
		"<Unknown>",
		"<Unknown>",
		"<Unknown>",
		"<Unknown>",
		"<Unknown>",
		"IS_CUSTOM",
		"IS_RELATIVE",
		"IS_ACTION",
	};
	char buffer[14 * ARRAY_SIZE(names)];		// 14 = maximum name length + strlen(delimiter)
	memset(buffer, 0, sizeof(buffer));

	for(int i = 0; i < ARRAY_SIZE(names); i++) {
		if(flags & (1 << i)) {
			strcat(buffer, names[i]);
			if(flags >= (1 << (i + 1)))
				strcat(buffer, delimiter);
		}
	}

	return strdup(buffer);
}


// Note: The caller must free the buffer obtained from this function.
static char *
get_control_choices (CControl *control)
{
	const char *delimiter = ", ";
	char *buffer;

	unsigned int req_size = 0;
	unsigned int count = control->choices.count;
	for(int i = 0; i < count; i++) {
		req_size += 1 + strlen(control->choices.list[i].name) + 1 + 12;
		if(i < count - 1)
			req_size += strlen(delimiter);
	}

	buffer = (char *)malloc(req_size + 1);
	memset(buffer, 0, req_size + 1);

	for(int i = 0; i < control->choices.count; i++) {
		char index_buffer[13];
		sprintf(index_buffer, "[%u]", control->choices.list[i].index);

		strcat(buffer, "'");
		strcat(buffer, control->choices.list[i].name);
		strcat(buffer, "'");
		strcat(buffer, index_buffer);

		if(i < count - 1)
			strcat(buffer, delimiter);
	}

	return buffer;
}


static void
print_control (CControl *control)
{
	if(HAS_VERBOSE()) {
		char *flags = get_control_flags(control->flags);

		printf("  %s\n    ID      : 0x%08x,\n    Type    : %s,\n    Flags   : { %s },\n", control->name,
				control->id, get_control_type(control->type), flags);

		if(control->type == CC_TYPE_CHOICE) {
			char *choices = get_control_choices(control);
			printf("    Values  : { %s },\n    Default : %d\n", choices, control->def.value);
			free(choices);
		}
		else {
			printf("    Values  : [ %d .. %d, step size: %d ],\n    Default : %d\n",
					control->min.value, control->max.value, control->step.value,
					control->def.value);
		}

		free(flags);
	}
	else {
		printf("  %s\n", control->name);
	}

}


static void
print_device (CDevice *device)
{
	if(HAS_VERBOSE()) {
		printf("  %s   %s [%s, %s]\n",
				device->shortName, device->name, device->driver, device->location);
	}
	else {
		printf("  %s   %s\n", device->shortName, device->name);
	}
}


static CResult
list_controls (CHandle handle)
{
	CResult ret;
	unsigned int count = 0;
	CControl *controls = NULL;

	// Retrieve the control list
	ret = get_control_list(handle, &controls, &count);
	if(ret) goto done;

	if(count == 0) {
		printf("No controls found.\n");
		goto done;
	}

	// Print a list of all available controls
	for(int i = 0; i < count; i++) {
		CControl *control = &controls[i];
		print_control(control);
	}

done:
	if(controls) free(controls);
	if(ret)
		print_error("Unable to retrieve device list", ret);
	return ret;
}


static CResult
list_devices ()
{
	CResult ret;
	unsigned int req_size = 0, buffer_size = 0, count = 0;
	CDevice *devices = NULL;

	printf("Listing available devices:\n");

	do {
		// Allocate the required memory
		if(devices) free(devices);
		if(req_size) {		// No allocation the first time
			devices = (CDevice *)malloc(req_size);
			if(devices == NULL)
				return C_NO_MEMORY;
			buffer_size = req_size;
		}

		// Try to enumerate. If the buffer is not large enough, the required
		// size is returned.
		ret = c_enum_devices(devices, &req_size, &count);
		if(ret != C_SUCCESS && ret != C_BUFFER_TOO_SMALL)
			goto done;
	}
	while(buffer_size < req_size);

	if(count == 0) {
		printf("No devices found.\n");
		goto done;
	}

	// Print a list of all available devices
	for(int i = 0; i < count; i++) {
		CDevice *device = &devices[i];
		print_device(device);
	}

done:
	if(devices) free(devices);
	if(ret)
		print_error("Unable to retrieve device list", ret);
	return ret;
}


int
main (int argc, char **argv)
{
	CHandle handle = 0;
	CResult res = C_SUCCESS;

	// Parse the command line
	if(cmdline_parser(argc, argv, &args_info) != 0)
		exit(1);
	
	// Display help if no arguments were specified
	if(argc == 1) {
		cmdline_parser_print_help();
		exit(0);
	}

	res = c_init();
	if(res) goto done;

	// List devices
	if(args_info.list_given) {
		res = list_devices();
		goto done;
	}

	// Open the device
	handle = c_open_device(args_info.device_arg);
	if(!handle) {
		print_error("Unable to open device", -1);
		res = C_INVALID_DEVICE;
		goto done;
	}

	// List controls
	if(args_info.clist_given) {
		printf("Listing available controls for device %s:\n", args_info.device_arg);
		res = list_controls(handle);
	}
	// Import dynamic controls from XML file
	if(args_info.import_given) {
		CDynctrlInfo info = { 0 };
		info.flags = CD_REPORT_ERRORS;
		if(HAS_VERBOSE())
			info.flags |= CD_RETRIEVE_META_INFO;

		printf("Importing dynamic controls from file %s.\n", args_info.import_arg);
		res = c_add_control_mappings_from_file(args_info.import_arg, &info);

		// Print meta information if we're in verbose mode
		if(HAS_VERBOSE()) {
			printf(
				"Available meta information:\n"
				"  File format: %d.%d\n"
				"  Author:      %s\n"
				"  Contact:     %s\n"
				"  Copyright:   %s\n"
				"  Revision:    %d.%d\n",
				info.meta.version.major, info.meta.version.minor,
				info.meta.author    ? info.meta.author    : "(unknown)",
				info.meta.contact   ? info.meta.contact   : "(unknown)",
				info.meta.copyright ? info.meta.copyright : "(unknown)",
				info.meta.revision.major, info.meta.revision.minor
			);
			if(info.meta.author)
				free(info.meta.author);
			if(info.meta.contact)
				free(info.meta.contact);
			if(info.meta.author)
				free(info.meta.copyright);
		}

		// Print errors
		if(info.message_count) {
			for(int i = 0; i < info.message_count; i++) {
				CDynctrlMessage *msg = &info.messages[i];
				const char *severity = "message";
				switch(msg->severity) {
					case CD_SEVERITY_ERROR:		severity = "error";		break;
					case CD_SEVERITY_WARNING:	severity = "warning";	break;
				}
				if(msg->line && msg->col) {
					printf("%s:%d:%d: %s: %s\n", args_info.import_arg, msg->line, msg->col, severity, msg->text);
				}
				else if(msg->line) {
					printf("%s:%d: %s: %s\n", args_info.import_arg, msg->line, severity, msg->text);
				}
				else {
					printf("%s: %s: %s\n", args_info.import_arg, severity, msg->text);
				}
			}
		}

		if(info.messages)
			free(info.messages);
	}
	// Retrieve control value
	else if(args_info.get_given) {
		CControlValue value;
		
		// Resolve the control Id
		CControlId controlId = get_control_id(handle, args_info.get_arg);
		if(!controlId) {
			print_error("Unknown control specified", -1);
			goto done;
		}

		// Retrieve the control value
		res = c_get_control(handle, controlId, &value);
		if(res) {
			print_error("Unable to retrieve control value", res);
			goto done;
		}
		printf("%d\n", value.value);
	}
	else if(args_info.set_given) {
		CControlValue value;

		// Parse the control value
		if(args_info.inputs_num < 1) {
			print_error("No control value specified", -1);
			goto done;
		}
		if(parse_control_value(args_info.inputs[0], &value)) {
			print_error("Invalid control value specified", -1);
			goto done;
		}

		// Resolve the control Id
		CControlId controlId = get_control_id(handle, args_info.set_arg);
		if(!controlId) {
			print_error("Unknown control specified", -1);
			goto done;
		}

		// Set the new control value
		res = c_set_control(handle, controlId, &value);
		if(res) {
			print_error("Unable to set new control value", res);
			goto done;
		}
	}

	// Clean up
done:
	if(handle) c_close_device(handle);
	c_cleanup();
	cmdline_parser_free(&args_info);

	return res;
}
