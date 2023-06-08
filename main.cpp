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
#include <functional>

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
		INCLA,  /* INvalid CLA */
		INTE,   /* INTernal E */
		MEME,   /* MEMory E */
		COME,   /* COMpile E */
		NIMPL,  /* Not IMPLemented */
		TESTOK, /* TEST OK */
		TESTUNK, /* TEST UNKnown result */
		TESTFAIL, /* TEST FAILed */
		HELPM,  /* HELP Message */
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
		if (this->status == TESTOK) {
			return "test passed";
		}
		if (this->status == TESTUNK) {
			return "test result unknown";
		}
		if (this->status == TESTFAIL) {
			return "test failed";
		}
		if (this->status == HELPM) {
			return "help message was shown";
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

template <typename T>
constexpr auto default_quotes = std::basic_string_view<T>("\"");

template <typename T>
std::basic_string<T> repr(T in) noexcept {
	if (in == T('\n')) {
		return *new std::basic_string<T>("\\n");
	}
	if (in == T('\t')) {
		return *new std::basic_string<T>("\\t");
	}
	if (in == T('\r')) {
		return *new std::basic_string<T>("\\r");
	}
	if (in == T('\\')) {
		return *new std::basic_string<T>("\\\\");
	}
	if (in == T('\0')) {
		return *new std::basic_string<T>("\\0");
	}
	auto result = *new std::basic_string<T>();
	result += in;
	return result;
}

template <typename T>
std::basic_string<T> repr(std::basic_string<T> in) noexcept {
	std::basic_string<T> result;
	std::for_each(
			in.begin(),
			in.end(),
			[&result](T current) {
				result += repr(current);
			});
	return result;
}

template <typename T>
std::basic_string<T> repr_quotes(std::basic_string<T> in, const std::basic_string_view<T> &quotes = default_quotes<T>) noexcept {
	auto result = repr(in);
	result.insert(0, std::basic_string<T>(quotes));
	result.insert(result.length(), std::basic_string<T>(quotes));
	return result;
}

template <typename T>
std::optional<T> urepr_char(std::basic_string<T> in) noexcept {
	if (in == std::basic_string<T>("\\n")) {
		return T('\n');
	}
	if (in == std::basic_string<T>("\\t")) {
		return T('\t');
	}
	if (in == std::basic_string<T>("\\r")) {
		return T('\r');
	}
	if (in == std::basic_string<T>("\\\\")) {
		return T('\\');
	}
	if (in == std::basic_string<T>("\\0")) {
		return T('\0');
	}
	if (1 > in.length() || in.length() > 1) {
		return std::nullopt;
	}
	return in[0];
}

template <typename T>
std::basic_string<T> urepr(std::basic_string<T> in) noexcept {
	auto out = *new std::basic_string<T>();
	for (size_t i = 0; i < in.length(); ) {
		if (in[i] == '\\') {
			auto result = *new std::basic_string<T>();
			result += T('\\');
			result += in[++i];
			out += *urepr_char(result);
			continue;
		}
		auto result = *new std::basic_string<T>();
		result += in[i++];
		out += *urepr_char(result);
	}
	return out;
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
std::ostream &cvector_print(
		std::ostream &out,
		std::wostream &wout,
		T object,
		size_t tabshift = 0,
		bool newline_in_first_line = false,
		bool fancy_print = true) noexcept {
	(void) newline_in_first_line;
	(void) tabshift;
	(void) fancy_print;
	wout << object;
	return out;
}
template <typename T>
std::ostream &cvector_print(
		std::ostream &out,
		std::wostream &wout,
		std::vector<T> &vector,
		size_t tabshift = 0,
		bool newline_in_first_line = false,
		bool fancy_print = true,
		std::function<std::basic_string<wchar_t> (long)> format_between_elements =
			[](size_t i) -> std::basic_string<wchar_t> {
				(void) i;
				return *new std::basic_string<wchar_t>(L",\n");
			},
		std::function<std::basic_string<wchar_t> (void)> last_element_format =
			[](void) -> std::basic_string<wchar_t> {
				return *new std::basic_string<wchar_t>(L"");
			}) noexcept {
	std::string tabshift_string;
	for (size_t i = 0; i < tabshift; ++i) {
		tabshift_string += ts::tab;
	}
	if (newline_in_first_line) {
		out << tabshift_string;
	}
	if (fancy_print) {
		out << "std::vector {" << ts::endl_fast;
	}
    if (!vector.empty()) {
		for (auto i = vector.begin(); i != --vector.end(); ++i) {
			if (fancy_print) {
				out << tabshift_string << ts::tab;
			}
			cvector_print(out, wout, *i, tabshift+1);
			wout << format_between_elements(i - vector.begin());
		}
		if (fancy_print) {
			out << tabshift_string << ts::tab;
		}
		cvector_print(out, wout, vector.at(vector.size() - 1), tabshift+1);
		wout << last_element_format();
		if (fancy_print) {
			out << ts::endl;
		}
	}
	if (fancy_print) {
		out << tabshift_string << "}" << ts::endl;
	}
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

	std::vector<std::wstring> hls_vals_max;
	for (size_t i = 0; i < hls.size(); ++i) {
		std::cout << "row: " << i << ts::endl;
		std::wstring &this_hl = hls[i];
		std::vector<std::wstring> this_val;
		std::for_each(
				vals.begin(),
				vals.end(),
				[&this_val, i](std::vector<std::wstring> &element) {
					this_val.push_back(element[i]);
				});
		auto this_val_max_it = std::max_element(
				this_val.begin(),
				this_val.end(),
				[](std::wstring &lhs, std::wstring &rhs) {
					return lhs.length() < rhs.length();
				});
		std::wstring &this_val_max = *this_val_max_it;
		std::vector<std::wstring> this_hl_val;
		{
			this_hl_val.push_back(this_hl);
			this_hl_val.push_back(this_val_max);
		}
		std::wstring this_hl_val_max = *std::max_element(
				this_hl_val.begin(),
				this_hl_val.end(),
				[](std::wstring &lhs, std::wstring &rhs) {
					return lhs.length() < rhs.length();
				});
		hls_vals_max.push_back(this_hl_val_max);
	}
	std::cout << "hls_vals_max: ";
	cvector_print(std::cout, std::wcout, hls_vals_max);

	std::cout << "-- table starts here --" << ts::endl;

/* DELETE THIS!!!!!
 * ┏━━━━━━┯━━━━━━━━━━━━━━━━━━━━━━━┓
 * ┃use   │to                     ┃
 * ┃sim s │simulate program       ┃
 * ┃help h│print this help message┃
 * ┗━━━━━━┷━━━━━━━━━━━━━━━━━━━━━━━┛ */
	std::wcout << L'┃';
	cvector_print(
			std::cout,
			std::wcout,
			hls,
			0,
			false,
			false,
			[&hls_vals_max, &hls](size_t i) -> std::basic_string<wchar_t> {
				auto spaces = *new std::basic_string<wchar_t>();
				for (size_t j = 0; j < hls_vals_max[i].length() - hls[i].length(); ++j) {
					spaces += L' ';
				}
				spaces += L'│';
				return spaces;
			},
			[&hls_vals_max, &hls](void) -> std::basic_string<wchar_t> {
				auto spaces = *new std::basic_string<wchar_t>();
				for (size_t i = 0; i < hls_vals_max[hls.size() - 1].length() - hls[hls.size() - 1].length(); ++i) {
					spaces += L' ';
				}
				return spaces;
			});
	std::wcout << L'┃';
	for (size_t i = 0; i < vals.size(); ++i) {
		std::cout << ts::endl_fast;
		std::wcout << L'┃';
		cvector_print(
				std::cout,
				std::wcout,
				vals[i],
				0,
				false,
				false,
				[&hls_vals_max, &vals, i](size_t j) -> std::basic_string<wchar_t> {
					auto spaces = *new std::basic_string<wchar_t>();
					for (size_t k = 0; k < hls_vals_max[j].length() - vals[i][j].length(); ++k) {
						spaces += L' ';
					}
					spaces += L'│';
					return spaces;
				},
				[&hls_vals_max, &vals, i](void) -> std::basic_string<wchar_t> {
					auto spaces = *new std::basic_string<wchar_t>();
					for (size_t j = 0; j < hls_vals_max[hls_vals_max.size() - 1].size() - vals[i][vals[i].size() - 1].length(); ++j) {
						spaces += L' ';
					}
					spaces += L'┃';
					return spaces;
				});
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

[[noreturn]] void _test_table(int argc, char **argv) noexcept {
	(void) argc;
	(void) argv;
	std::cout << "Hello from testing!" << ts::endl;
	std::vector<std::basic_string<wchar_t>> hls;
	hls.push_back(*new std::basic_string<wchar_t>(L"use"));
	hls.push_back(*new std::basic_string<wchar_t>(L"to"));
	std::vector<std::vector<std::basic_string<wchar_t>>> vals;
	std::vector<std::basic_string<wchar_t>> vals_1;
	vals_1.push_back(*new std::basic_string<wchar_t>(L"sim s"));
	vals_1.push_back(*new std::basic_string<wchar_t>(L"simulate program"));
	vals.push_back(vals_1);
	std::vector<std::basic_string<wchar_t>> vals_2;
	vals_2.push_back(*new std::basic_string<wchar_t>(L"help h"));
	vals_2.push_back(*new std::basic_string<wchar_t>(L"print this help message"));
	vals.push_back(vals_2);
	std::wcout << std::get<std::basic_string<wchar_t>>(table(hls, vals)) << ts::endl;
	ts::exit(*new ts::Status(ts::Status::TESTUNK));
}
const auto REPR_TEST_STRING = *new std::basic_string("\\\\Hello,\tworld!\nlol\rkek\n");
[[noreturn]] void _test_repr(int argc, char **argv) noexcept {
	(void) argc;
	(void) argv;
	std::cout << ts::repr(*new std::basic_string(REPR_TEST_STRING)) << ts::endl;
	ts::exit(*new ts::Status(ts::Status::TESTUNK));
}
[[noreturn]] void _test_repr_quotes(int argc, char **argv) noexcept {
	(void) argc;
	(void) argv;
	std::cout << ts::repr_quotes(*new std::basic_string(REPR_TEST_STRING)) << ts::endl;
	ts::exit(*new ts::Status(ts::Status::TESTUNK));
}
[[noreturn]] void _test_urepr(int argc, char **argv) noexcept {
	(void) argc;
	(void) argv;
	std::cout << ts::urepr(*new std::basic_string(REPR_TEST_STRING)) << ts::endl;
	ts::exit(*new ts::Status(ts::Status::TESTUNK));
}
[[noreturn]] void _test(int argc, char **argv) noexcept {
	_test_urepr(argc, argv);
	ts::exit(*new ts::Status(ts::Status::INTE));
}
int main(int argc, char **argv) {
#define TESTING 1
#if TESTING
	_test(argc, argv);
#else
	return _main(argc, argv);
#endif
}
