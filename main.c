#if 0
	./build.sh;
	if [[ $? -ge 0 ]]; then
		./a.out $@;
	fi;
	exit $?;
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

#define ERROR_NOTE_TYPE RED_COLOR"error"RESET_COLOR
#define WARNING_NOTE_TYPE YELLOW_COLOR"warning"RESET_COLOR
#define NOTE_NOTE_TYPE GREEN_COLOR"note"RESET_COLOR

/*................Section 0.2: Debug things................*/
#define WHERE fprintf(stderr, "i am here\n");

/*................Section 0.3: Constants...................*/
#define FILE_BLOCK_SIZE 100
#define MAX_FILE_SIZE 100000
#define FILENAME_ARRAY_CAPACITY 10

/*................Section 0.4: Compiler note functions.....*/
void common_compiler_note(char *note_type, char *message) {
	printf("%s: %s\n", note_type, message);
}
void error_compiler_note(char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		asprintf(&message, format, vp);
	}
	common_compiler_note(ERROR_NOTE_TYPE, message);
}
void warning_compiler_note(char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		asprintf(&message, format, vp);
	}
	common_compiler_note(WARNING_NOTE_TYPE, message);
}
void note_compiler_note(char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
	}
	common_compiler_note(NOTE_NOTE_TYPE, message);
}
struct Location {
	char *filename;
	int line;
	int index;
};
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
/************************************************************
 *                Section 1: Command line arguments
 ***********************************************************/
/*................Section 1.0: Subcommand and sources......*/
struct Array {
	size_t size;
	size_t capacity;
	void *ptr;
};
struct Array *make_Array(void) {
	struct Array *object;
	object = malloc(sizeof(struct Array));
	object->size = 0;
	object->capacity = FILENAME_ARRAY_CAPACITY;
	return object;
}
#define ARRAY_APPEND(arr, elem, type)\
do {\
	if (!(arr)) {\
		(arr) = malloc(sizeof(struct Array));\
	}\
	if (!((arr)->size % (arr)->capacity)) {\
		(arr)->ptr = realloc((arr)->ptr, sizeof(type)*((arr)->size+(arr)->capacity));\
	}\
	((type*)(arr)->ptr)[(arr)->size++] = (elem);\
} while (0)
void handle_command_line_arguments(
		int argc,
		char **argv,
		void *(*handle_start_function)(int, char **),
		void(*handle_iteration_function)(int, int, char **, void *),
		void(*handle_end_function)(int, char **, void*)) {
	void *handle_start_function_result = handle_start_function(argc, argv);
	for (int index = 1; index < argc; index++) {
		handle_iteration_function(index, argc, argv, handle_start_function_result);
	}
	handle_end_function(argc, argv, handle_start_function_result);
}
struct Options {
	
};
struct Options *make_Options(void) {
	struct Options *object;
	object = malloc(sizeof(struct Options));
	return object;
}
struct Default_start_function_result {
	struct Options *options;
	struct Array *filenames;
};
struct Default_start_function_result *make_Default_start_function_result(void) {
	struct Default_start_function_result *object;
	object = malloc(sizeof(struct Default_start_function_result));
	object->options = make_Options();
	object->filenames = make_Array();
	return object;
}
struct Default_start_function_result *default_handle_start_function(int argc, char **argv) {
	(void) argv;
	if (argc < 2) {
		error_compiler_note(GRAY_COLOR"no source files provided"RESET_COLOR);
		return NULL;
	}
	return make_Default_start_function_result();
}
void default_handle_iteration_function(int index, int argc, char **argv, struct Default_start_function_result *default_start_function_result) {
	(void) argc;
	note_compiler_note(
			"command line argument №%d: "GRAY_COLOR"adding "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" to source files..."RESET_COLOR,
			index,
			argv[index]);
	ARRAY_APPEND(default_start_function_result->filenames, argv[index], char*);
}
wchar_t *get(char *filename, size_t index) {
	FILE *fp = fopen(filename, "r, ccs=UTF-8");
	if (!fp) {
		note_compiler_note("%d"GRAY_COLOR"cannot read "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" file due to this reason: "REASON_COLOR"%s"RESET_COLOR, index, filename, strerror(errno));
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
		if (!(i % FILE_BLOCK_SIZE)) {
			all_file = realloc(all_file, sizeof(wchar_t)*((unsigned long)i + FILE_BLOCK_SIZE));
		}
		all_file[i] =  (wchar_t)current_symbol;
	}
	fclose(fp);
	return all_file;
}
void handle_filename(size_t index, struct Array *filenames) {
	char *filename = ((char**)filenames->ptr)[index];
	note_compiler_note("%zu. reading %s...", index, filename);
	wchar_t *content = malloc(sizeof(wchar_t)*MAX_FILE_SIZE);
	wcscpy(content, get(filename, index));
	note_compiler_note("file is %ls", content);
}
void default_handle_end_function(int argc, char **argv, struct Default_start_function_result *default_start_function_result) {
	(void) argc;
	(void) argv;
	note_compiler_note("filenames are:");
	for (size_t i = 0; i < default_start_function_result->filenames->size; i++) {
		note_compiler_note("    %zu: %s", i, ((char**)default_start_function_result->filenames->ptr)[i]);
	}
	for (size_t i = 0; i < default_start_function_result->filenames->size; i++) {
		handle_filename(i, default_start_function_result->filenames);
	}
}
int main(int argc, char **argv) {
	handle_command_line_arguments(
			argc,
			argv,
			(void *(*)(int, char **))default_handle_start_function,
			(void(*)(int, int, char **, void*))default_handle_iteration_function,
			(void(*)(int, char **, void *))default_handle_end_function);
	return 0;
}
