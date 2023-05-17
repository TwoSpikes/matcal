#if 0
	./run.sh ${@}
	exit $?
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <errno.h>
#include <wchar.h>

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
#define REASON_COLOR VIOLET_COLOR

/*................Section 0.2: Debug things................*/
#define WHERE_I_AM fprintf(stderr, "i am here\n");

/*................Section 0.3: Constants...................*/
#define FILE_BLOCK_SIZE 100
#define MAX_FILE_CAPACITY 100000

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
	char *actual_note_type;
	asprintf(&actual_note_type, "%s:%d:%d: %s", location->filename, location->line, location->index, note_type);
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
		char *old_note_type = calloc(sizeof(char), strlen(note_type));
		strcpy(old_note_type, note_type);
		asprintf(&note_type, "%d: %s", location, old_note_type);
	}
	common_compiler_note(note_type, message);
}
void error_primary_location_compiler_note(int location, char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
		va_end(vp);
	}
	common_primary_location_compiler_note(location, ERROR_NOTE_TYPE, message);
}
void warning_primary_location_compiler_note(int location, char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
		va_end(vp);
	}
	common_primary_location_compiler_note(location, WARNING_NOTE_TYPE, message);
}
void note_primary_location_compiler_note(int location, char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
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
	for (int index = 1; index < argc; index++) {
		handle_iteration_function(index, argc, argv);
	}
}
void default_handle_start_function(int argc, char **argv) {
	(void) argv;
	if (argc < 2) {
		error_compiler_note(GRAY_COLOR"no source files provided"RESET_COLOR);
		return;
	}
}
void default_handle_iteration_function(int index, int argc, char **argv) {
	(void) argc;
	note_primary_location_compiler_note(index, GRAY_COLOR"adding "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" to source files..."RESET_COLOR, argv[index]);
	FILE *fp = fopen(argv[index], "r, ccs=UTF-8");
	if (!fp) {
		note_primary_location_compiler_note(index, GRAY_COLOR"cannot read "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" file due to this reason: "REASON_COLOR"%s"RESET_COLOR, argv[index], strerror(errno));
		abort();
	}
	wchar_t *all_file = calloc(sizeof(wchar_t), FILE_BLOCK_SIZE);
	unsigned int i;
	for (i = 0; ; i++) {
	(void) i;
		wint_t current_symbol = fgetwc(fp);
		if (current_symbol == WEOF) {
			break;
		}
		printf("just got: %lc\n", current_symbol);
		if (!(i % FILE_BLOCK_SIZE)) {
			fprintf(stderr, "reallocated: i=%d, FILE_BLOCK_SIZE=%d\n", i, FILE_BLOCK_SIZE);
			all_file = realloc(all_file, sizeof(wchar_t)*((unsigned long)i + FILE_BLOCK_SIZE));
		}
		all_file[i] =  (wchar_t)current_symbol;
	}
	printf("all file: %ls\n", all_file);
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
