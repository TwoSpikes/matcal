#if 0
	./run.sh
	exit $?
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define RESET_COLOR "\033[0m"
#define GRAY_COLOR "\033[90m"
#define RED_COLOR "\033[91m"
#define GREEN_COLOR "\033[92m"
#define YELLOW_COLOR "\033[93m"
#define BLUE_COLOR "\033[94m"
#define VIOLET_COLOR "\033[95m"
#define LIGHT_BLUE_COLOR "\033[96m"
#define WHITE_COLOR "\033[97m"
#define GRAY_BACK_COLOR "\033[100m"
#define RED_BACK_COLOR "\033[101m"
#define GREEN_BACK_COLOR "\033[102m"
#define YELLOW_BACK_COLOR "\033[103m"
#define BLUE_BACK_COLOR "\033[104m"
#define VIOLET_BACK_COLOR "\033[105m"
#define LIGHT_BLUE_BACK_COLOR "\033[106m"
#define WHITE_BACK_COLOR "\033[107m"

//for debug purposes
#define WHERE_I_AM fprintf(stderr, "i am here\n");

#define MAX_STRING_CAPACITY 1000

struct Location {
	char *filename;
	int line;
	int index;
};

char *ERROR_NOTE_TYPE;
char *WARNING_NOTE_TYPE;
char *NOTE_NOTE_TYPE;

void common_compiler_note(char *note_type, char *message) {
	printf("%s: %s", note_type, message);
}
void error_compiler_note(char *message) {
	common_compiler_note(ERROR_NOTE_TYPE, message);
}
void warning_compiler_note(char *message) {
	common_compiler_note(WARNING_NOTE_TYPE, message);
}
void note_compiler_note(char *message) {
	common_compiler_note(NOTE_NOTE_TYPE, message);
}
void common_location_compiler_note(struct Location *location, char *note_type, char *message) {
	char *actual_note_type;
	sprintf(actual_note_type, "%s:%d:%d: %s", location->filename, location->line, location->index, note_type);
	common_compiler_note(actual_note_type, message);
}
void error_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, ERROR_NOTE_TYPE, message);
}
void warning_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, WARNING_NOTE_TYPE, message);
}
void note_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, NOTE_NOTE_TYPE, message);
}
void handle_command_line_arguments(
		int argc,
		char **argv,
		void(*handle_start_function)(int, char **),
		void(*handle_iteration_function)(int, int, char **)) {
	handle_start_function(argc, argv);
	for(int index = 1; index < argc; index++) {
		handle_iteration_function(index, argc, argv);
	}
}
void default_handle_start_function(int argc, char **argv) {
	if (argc < 2) {
		error_compiler_note("no source files provided\n");
		return;
	}
}
void default_handle_iteration_function(int index, int argc, char **argv) {
	printf("%d/%d: %s\n", index, argc, argv[index]);
}
int main(int argc, char **argv) {
	ERROR_NOTE_TYPE = calloc(sizeof(char), MAX_STRING_CAPACITY);
	WARNING_NOTE_TYPE = calloc(sizeof(char), MAX_STRING_CAPACITY);
	NOTE_NOTE_TYPE = calloc(sizeof(char), MAX_STRING_CAPACITY);
	sprintf(ERROR_NOTE_TYPE, "%s%s%s", RED_COLOR, "error", RESET_COLOR);
	sprintf(WARNING_NOTE_TYPE, "%s%s%s", YELLOW_COLOR, "warning", RESET_COLOR);
	sprintf(NOTE_NOTE_TYPE, "%s%s%s", GREEN_COLOR, "note", RESET_COLOR);
	handle_command_line_arguments(
			argc,
			argv,
			default_handle_start_function,
			default_handle_iteration_function);
	return 0;
}
