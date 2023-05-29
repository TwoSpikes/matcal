#if 0
	./build.sh;
	if [[ $? -ge 0 ]]; then
		matcal $@;
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
#define GRAY_COLOR "\033[37m"
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
#define WAIT_NOTE_TYPE LIGHT_BLUE_COLOR"wait"RESET_COLOR

/*................Section 0.2: Debug things................*/
#define WHERE fprintf(stderr, "i am here\n");

/*................Section 0.3: Constants...................*/
#define FILE_BLOCK_SIZE 100
#define MAX_FILE_SIZE 100000
#define FILENAME_ARRAY_CAPACITY 10

/*................Section 0.4: Compiler note functions.....*/
inline void common_compiler_note(char *note_type, char *message) {
	printf("%s: %s\n", note_type, message);
}
void error_compiler_note(char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
	}
	common_compiler_note(ERROR_NOTE_TYPE, message);
}
void warning_compiler_note(char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
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
void wait_compiler_note(char *format, ...) {
	char *message;
	{
		va_list vp;
		va_start(vp, format);
		vasprintf(&message, format, vp);
	}
	common_compiler_note(WAIT_NOTE_TYPE, message);
}
struct Location {
	char *filename;
	int line;
	int index;
};
inline void common_location_compiler_note(struct Location *location, char *note_type, char *message) {
	char *actual_note_type;
	asprintf(&actual_note_type, "%s:%d:%d: %s", location->filename, location->line, location->index, note_type);
	common_compiler_note(actual_note_type, message);
}
inline void error_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, ERROR_NOTE_TYPE, message);
}
inline void warning_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, WARNING_NOTE_TYPE, message);
}
inline void note_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, NOTE_NOTE_TYPE, message);
}
inline void wait_location_compiler_note(struct Location *location, char *message) {
	common_location_compiler_note(location, WAIT_NOTE_TYPE, message);
}
/************************************************************
 *                Section 1: Command line arguments
 ***********************************************************/
/*................Section 1.0: Subcommand and sources......*/
struct Array {
	size_t element_size;
	size_t size;
	size_t capacity;
	void **ptr;
};
struct Array *make_Array(size_t element_size) {
	struct Array *object;
	object = malloc(sizeof(struct Array));
	object->element_size = element_size;
	object->size = 0;
	object->capacity = FILENAME_ARRAY_CAPACITY;
	return object;
}
void Array_append(struct Array *array, void *element) {
	if (!(array->size % array->capacity)) {
		size_t new_size = array->size + array->capacity;
		array->ptr = realloc(array->ptr, sizeof(void*)*new_size);
	}
	array->ptr[array->size++] = element;
}
void Array_print(struct Array *array, char *format) {
	printf("array->size=%zu\n", array->size);
	printf("[");
	if (array->size) {
		for (size_t i = 0; i < array->size-1; i++) {
			char *actual_format;
			asprintf(&actual_format, "%s, ", format);
			printf(actual_format, array->ptr[i]);
		}
		printf(format, array->ptr[array->size-1]);
	}
	printf("]\n");
}
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
	object->filenames = make_Array(sizeof(char*));
	Array_print(object->filenames, "%ls");
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
	Array_append(default_start_function_result->filenames, argv[index]);
}
/*
enum Retlex { N(Box<Vec<Tok>>), E, EMPTY, STOPPED, }
enum Quotes { NO, IN, POSTF };
fn lex(filename: Box<String>, file: &String) -> (Retlex, Box<String>) {
use crate::Retlex::*;
use crate::Quotes::*;
    if file.len() == 0 {
        return (EMPTY, filename);
    }
    let mut res: Vec<Tok> = Vec::new();
    let mut tmp: String = String::new();
    let mut ploc: Loc = Loc { filename: filename.clone(), lin: 1, ind: 1 };
    let mut loc:  Loc = Loc { filename: filename.clone(), lin: 1, ind: 1 };
    let mut quotes: Quotes = Quotes::NO;
    for i in file.chars() {
        loc.ind += 1;
        //" then remember it
        if i == '"' {
            tmp.push(i);
            #[allow(unreachable_patterns)]
            match quotes {
                Quotes::NO => {
                    quotes = Quotes::IN;
                },
                Quotes::IN => {
                    quotes = Quotes::POSTF;
                },
                Quotes::POSTF => {
                    res.push(Tok(ploc, tmp.to_owned()));
                    tmp = String::new();
                    ploc = loc.clone();
                    quotes = Quotes::NO;
                },
                _ => {
                    eprintln!("lex: unknown quotes: {:?}", quotes);
                    return (E, filename);
                },
            };
            continue;
        }
        #[allow(unreachable_patterns)]
        match quotes {
            NO => {},
            IN => {
                tmp.push(i);
                continue;
            },
            POSTF => {
                if i == '\n' || i == ' ' {
                    quotes = NO;
                    res.push(Tok(ploc, tmp.to_owned()));
                    tmp = String::new();
                    ploc = loc.clone();
                } else {
                    tmp.push(i);
                }
                continue;
            },
            _ => {
                eprintln!("lex: unknown quotes: {:?}", quotes);
                return (E, filename);
            },
        }
        if i == '\n' {
            loc.ind = 1;
            loc.lin += 1;
        }
        //push special symbols as special symbols
        if i == '\n' ||
            i == ':' ||
            i == '(' ||
            i == ')' ||
            i == '{' ||
            i == '}' ||
            i == '\r' {
            res.push(Tok(ploc, tmp.to_owned()));
            res.push(Tok(loc.clone(), String::from(i)));
            tmp = String::new();
            ploc = loc.clone();
            continue;
        }
        //' ' or '\t' then push tmp
        if i == ' ' || i == '\t' {
            if tmp.len() > 0 {
                res.push(Tok(ploc, tmp.to_owned()));
                tmp = String::new();
            }
            ploc = loc.clone();
            continue;
        }
        tmp.push(i);
    }
    if tmp.len() > 0 {
        res.push(Tok(ploc, tmp.to_owned()));
    }

    if unsafe { LEX_DEBUG } {
        eprintln!("{}: Lexing result: [", filename);
        for i in &res {
            eprintln!("  {}:{}: {:?}",
                      i.0.lin,
                      i.0.ind,
                      i.1);
        }
        eprintln!("]");
    }
    if unsafe { ONLY_LEX } {
        return (STOPPED, filename);
    }

    return (N(Box::new(res)), filename);
}
*/
struct LexResult {
	struct Array *result;
};
struct LexResult *make_LexResult(void) {
	struct LexResult *object;
	object = malloc(sizeof(struct LexResult));
	object->result = make_Array(sizeof(wchar_t*));
	return object;
}
struct LexResult *lex(wchar_t *content, char *filename) {
	wait_compiler_note("lexing %s...", filename);
	struct LexResult *result = make_LexResult();
	Array_append(result->result, content);
//WHERE
	Array_print(result->result, "%ls");
	return result;

}
wchar_t *get(char *filename, size_t index) {
	FILE *fp = fopen(filename, "r, ccs=UTF-8");
	if (!fp) {
		error_compiler_note("%d: "GRAY_COLOR"cannot read "NON_BOLD_COLOR FILENAME_COLOR"%s"NON_BOLD_COLOR GRAY_COLOR" file due to this reason: "REASON_COLOR"%s"RESET_COLOR, index, filename, strerror(errno));
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
	wait_compiler_note("%zu. reading %s...", index, filename);
	wchar_t *content = get(filename, index);
	note_compiler_note("file is %ls", content);
	struct LexResult *lexResult = lex(content, ((char**)filenames->ptr)[index]);
	for (size_t i = 0; i < lexResult->result->size; i++) {
		note_compiler_note("%s: lex: %d: %ls", ((char**)filenames->ptr)[index], i, ((wchar_t**)lexResult->result->ptr)[i]);
	}
}
void default_handle_end_function(int argc, char **argv, struct Default_start_function_result *default_start_function_result) {
	(void) argc;
	(void) argv;
	if (!default_start_function_result) {
		return;
	}
	note_compiler_note("filenames are:");
	printf("size is %zu\n", default_start_function_result->filenames->size);
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
