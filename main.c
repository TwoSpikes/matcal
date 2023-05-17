#if 0
	./run.sh ${@}
	exit $?
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>



/************************************************************
 *                Section 0: Definings
 ***********************************************************/
/*................Section 0.1: Colors......................*/
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
#define BOLD_COLOR "\033[1m"
#define NON_BOLD_COLOR "\033[22m"

#define FILENAME_COLOR VIOLET_COLOR

/*................Section 0.2: Debug things................*/
#define WHERE_I_AM fprintf(stderr, "i am here\n");

/*................Section 0.3: Constants...................*/
#define MAX_STRING_CAPACITY 1e17

#define ERROR_NOTE_TYPE RED_COLOR"error"RESET_COLOR
#define WARNING_NOTE_TYPE YELLOW_COLOR"warning"RESET_COLOR
#define NOTE_NOTE_TYPE GREEN_COLOR"note"RESET_COLOR
/*................Section 0.4: Compiler note functions.....*/
struct Location {
	char *filename;
	int line;
	int index;
};
void common_compiler_note(char *note_type, char *message) {
	printf("%s: %s\n", note_type, message);
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
	char *actual_note_type = calloc(sizeof(char), MAX_STRING_CAPACITY);
	if (!actual_note_type) {
		fprintf(stderr, "cannot allocate memory\n");
		abort();
	}
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
void common_primary_location_compiler_note(int location, char *note_type, char *message) {
	{
		char *old_note_type = calloc(sizeof(char), MAX_STRING_CAPACITY);
		if (!old_note_type) {
			fprintf(stderr, "cannot allocate memory\n");
			abort();
		}
		strcpy(old_note_type, note_type);
		note_type = calloc(sizeof(char), MAX_STRING_CAPACITY);
		if (!note_type) {
			fprintf(stderr, "cannot allocate memory\n");
			abort();
		}
		sprintf(note_type, "%d: %s", location, old_note_type);
	}
	common_compiler_note(note_type, message);
}
void error_primary_location_compiler_note(int location, char *format, ...) {
	char *message = calloc(sizeof(char), MAX_STRING_CAPACITY);
	if (!message) {
		fprintf(stderr, "cannot allocate memory\n");
		abort();
	}
	{
		va_list vp;
		va_start(vp, format);
		vsprintf(message, format, vp);
		va_end(vp);
	}
	common_primary_location_compiler_note(location, ERROR_NOTE_TYPE, message);
}
void warning_primary_location_compiler_note(int location, char *format, ...) {
	char *message = calloc(sizeof(char), MAX_STRING_CAPACITY);
	if (!message) {
		fprintf(stderr, "cannot allocate memory\n");
		abort();
	}
	{
		va_list vp;
		va_start(vp, format);
		vsprintf(message, format, vp);
		va_end(vp);
	}
	common_primary_location_compiler_note(location, WARNING_NOTE_TYPE, message);
}
void note_primary_location_compiler_note(int location, char *format, ...) {
	char *message = calloc(sizeof(char), MAX_STRING_CAPACITY);
	if (!message) {
		fprintf(stderr, "cannot allocate memory\n");
		abort();
	}
	{
		va_list vp;
		va_start(vp, format);
		vsprintf(message, format, vp);
		va_end(vp);
	}
	common_primary_location_compiler_note(location, NOTE_NOTE_TYPE, message);
}
/************************************************************
 *                Section 1: Command line arguments
 ***********************************************************/
/*................Section 1.0: Subcommand and sources......*/
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
	(void) argv;
	if (argc < 2) {
		error_compiler_note("no source files provided");
		return;
	}
}
void default_handle_iteration_function(int index, int argc, char **argv) {
	(void) argc;
	note_primary_location_compiler_note(index, GRAY_COLOR"adding "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" to source files..."RESET_COLOR, argv[index]);
	FILE *fp = fopen(argv[index], "r");
	if (!fp) {
		note_primary_location_compiler_note(index, GRAY_COLOR"cannot read "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" file"RESET_COLOR, argv[index]);
		return;
	}
	error_primary_location_compiler_note(index, BOLD_COLOR RED_COLOR"file reading is not implemented yet"RESET_COLOR);
	fclose(fp);
}
int main(int argc, char **argv) {
	handle_command_line_arguments(
			argc,
			argv,
			default_handle_start_function,
			default_handle_iteration_function);
	return 0;
}
