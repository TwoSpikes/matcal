#if 0
	./build.sh;
	if [[ $? -ge 0 ]]; then
		matcal $@;
	fi;
	exit $?;
#endif

#include <cstdarg>
#include <wchar.h>
#include <string_view>
#include <iostream>
#include <tuple>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <optional>
#include <variant>

namespace clr {

std::string BLACK = "\033[30m";
std::string GRAY = "\033[37m";
std::string RED = "\033[91m";
std::string GREEN = "\033[92m";
std::string YELLOW = "\033[93m";
std::string BLUE = "\033[94m";
std::string VIOLET = "\033[95m";
std::string LIGHT_BLUE = "\033[96m";
std::string WHITE = "\033[97m";
std::string BLACK_BG = "\033[40m";
std::string GRAY_BG = "\033[100m";
std::string RED_BG = "\033[101m";
std::string GREEN_BG = "\033[102m";
std::string YELLOW_BG = "\033[103m";
std::string BLUE_BG = "\033[104m";
std::string VIOLET_BG = "\033[105m";
std::string LIGHT_BLUE_BG = "\033[106m";
std::string WHITE_BG = "\033[107m";
std::string BOLD = "\033[1m";
std::string NON_BOLD = "\033[22m";
std::string RESET = "\033[0m";


std::string FILENAME = VIOLET;
std::string REASON = VIOLET;

} /* namespace clr */

namespace ts {
	
/* ┏━━━━━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
 * ┃ 𝑼𝒔𝒆          │ 𝑻𝒐                           ┃
 * ┃ endl_fast    │ Do NOT flush buffer          ┃
 * ┃ endl_slow    │ Flush buffer                 ┃
 * ┗━━━━━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛ */
char endl_fast = '\n';
std::ostream &(*endl_slow)(std::ostream &) = std::endl;
auto endl = '\n';

std::string tab = "    ";

template <typename T>
class Enum {
public:
	virtual std::optional<std::basic_string_view<T>> to_string(void) noexcept = 0;
};

class Status: Enum<char> {
public:
	enum StatusType {
		OK,
		INCLA, /* INvalid CLA */
		INTE,  /* INTernal E */
		MEME,  /* MEMory E */
		COME,  /* COMpile E */
		NIMPL, /* Not IMPLemented */
	};
private:
	StatusType status;
public:
	std::optional<std::basic_string_view<char>> to_string(void) noexcept override final {
		if (this->status == OK) {
			return "ok";
		}
		if (this->status == INCLA) {
			return "invalid command line argument";
		}
		if (this->status == INTE) {
			return "internal error";
		}
		if (this->status == MEME) {
			return "memory error";
		}
		if (this->status == COME) {
			return "compile error";
		}
		if (this->status == NIMPL) {
			return "not implemented";
		}
		return std::nullopt;
	}

	StatusType operator* (void) {
		return this->status;
	}
	Status (StatusType status) noexcept: status(status) {}
};

[[noreturn]] void exit(Status &status) noexcept {
	std::cerr << clr::GRAY << "[compiler is exiting with exit code " << *status << ": " << *status.to_string() << "]" << clr::RESET << ts::endl_slow;
	std::exit(*status);
}

} /* namespace ts */

template <typename T>
std::wostream &operator<< (std::wostream &out, std::vector<T> &vector) noexcept {
	if (!vector.empty()) {
		for (auto it = vector.begin(); it != vector.end()-1; ++it) {
			out << *it << " ";
		}
		out << vector[vector.size()-1];
	}
	return out;
}

template <typename T>
std::ostream &cvector_print(std::ostream &out, std::wostream &wout, T object, size_t tabshift = 0, bool shift_in_first_line = false) noexcept {
	(void) shift_in_first_line;
	(void) tabshift;
	wout << object;
	return out;
}
template <typename T>
std::ostream &cvector_print(std::ostream &out, std::wostream &wout, std::vector<T> &vector, size_t tabshift = 0, bool shift_in_first_line = false, std::string &format_between_elements = *new std::string(",")) noexcept {
	std::string tabshift_string;
	for (size_t i = 0; i < tabshift; ++i) {
		tabshift_string += ts::tab;
	}
	if (shift_in_first_line) {
		out << tabshift_string;
	}
	out << "std::vector {" << ts::endl_fast;
    if (!vector.empty()) {
		for (auto i = vector.begin(); i != --vector.end(); ++i) {
			out << tabshift_string << ts::tab;
			cvector_print(out, wout, *i, tabshift+1);
			out << format_between_elements << ts::endl_fast;
		}
		out << tabshift_string << ts::tab;
		cvector_print(out, wout, vector.at(vector.size() - 1), tabshift+1);
		out << ts::endl_fast;
	}
	out << tabshift_string << "}" << ts::endl;
    return out;
}

struct Note_type {
	std::ostream &out;
	std::string note_type;
};
Note_type ERROR{std::cerr, clr::RED_BG+clr::BLACK+"error"+clr::RESET};
Note_type WARNING{std::cerr, clr::YELLOW+"warning"+clr::RESET};
Note_type NOTE{std::cerr, clr::GREEN+"note"+clr::RESET};
Note_type WAIT{std::cerr, clr::LIGHT_BLUE+"wait"+clr::RESET};

void note(Note_type &note_type, std::string &message) noexcept {
	note_type.out << note_type.note_type << ": " << message << ts::endl;
}
enum table_error {
	EMPHLS, /* EMPty HighLightingS */
};
std::variant<std::wstring, table_error> table(std::vector<std::wstring> hls, std::vector<std::vector<std::wstring>> vals) noexcept {
	std::cout << "table (" << ts::endl_fast << ts::tab << "hls = ";
	cvector_print(std::cout, std::wcout, hls, 1);
	std::cout << ts::tab << "vals = ";
	cvector_print(std::cout, std::wcout, vals, 1);
	std::cout << ")" << ts::endl;
	std::wstring result;
	std::cout << "hls = ";
	cvector_print(std::cout, std::wcout, hls);
	if (hls.empty()) {
		return table_error::EMPHLS;
	}
	std::vector<size_t> hls_lengths;
	std::for_each(hls.begin(),
				  hls.end(),
				  [&hls_lengths](std::wstring &element) {
				      hls_lengths.push_back(element.length());
				  });
	std::cout << "hls_lengths = ";
	cvector_print(std::cout, std::wcout, hls_lengths);
	std::vector<size_t>::iterator hls_max_it =
		std::max_element(hls_lengths.begin(),
				 hls_lengths.end());
	std::wstring &hls_max = hls[(size_t)(hls_max_it - hls_lengths.begin())];
	std::cout << "max of hls is: ";
	std::wcout << hls_max;
	std::cout << ts::endl;
	std::cout << "-- table starts here --" << ts::endl;
		for (auto it = hls.begin(); it != hls.end(); ++it) {
			std::wcout << *it;
		}
	ts::exit(*new ts::Status(ts::Status::NIMPL));
}
[[noreturn]] void foo(void) noexcept {
	std::cerr << "foo called" << ts::endl;
	ts::exit(*new ts::Status(ts::Status::INTE));
}
[[noreturn]] void short_usage(std::ostream &out) noexcept {
//	out << "Usage: matcal SUBCOMMAND [OPTION]... SOURCE... [OPTION]..." << TS_ENDL;
	(void) out;
	std::cout << "short usage called" << ts::endl;
	ts::exit(*new ts::Status(ts::Status::INTE));
}
void usage(std::ostream &out) noexcept {
	short_usage(out);
	out << "SUBCOMMAND:" << ts::endl;
//	out << table() << TS_ENDL;
}

int _main(int argc, char **argv) noexcept {
	(void) argc;
	(void) argv;
	if (argc < 2) {
		note(ERROR, *new std::string("no subcommand provided"));
		usage(std::cout);
		ts::exit(*new ts::Status(ts::Status::INCLA));
	}
	ts::exit(*new ts::Status(ts::Status::OK));
}

void _test(int argc, char **argv) noexcept {
	(void) argc;
	(void) argv;
	std::cout << "Hello from testing!" << ts::endl;
	std::vector<std::wstring> hls;
	hls.push_back(*new std::wstring(L"use"));
	hls.push_back(*new std::wstring(L"to"));
	std::vector<std::vector<std::wstring>> vals;
	std::vector<std::wstring> vals_1;
	vals_1.push_back(*new std::wstring(L"sim s"));
	vals_1.push_back(*new std::wstring(L"simulate program"));
	vals.push_back(vals_1);
	std::wcout << std::get<std::wstring>(table(hls, vals)) << ts::endl;
}
int main(int argc, char **argv) {
#define TESTING 1
#if TESTING
	_test(argc, argv);
#else
	return _main(argc, argv);
#endif
}
