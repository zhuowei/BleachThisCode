#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <iostream>
typedef boost::wave::cpplexer::lex_token<> lex_token;
typedef boost::wave::cpplexer::lex_iterator<lex_token> lex_iterator_type;
using boost::wave::token_id;
int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "usage: ./bleachthiscode <input.c>" << std::endl;
		return 1;
	}
	std::ifstream instream{argv[1]};
	std::string instr{std::istreambuf_iterator<char>(instream.rdbuf()),
		std::istreambuf_iterator<char>()};
	lex_iterator_type iter(instr.begin(), instr.end(), {}, boost::wave::language_support::support_cpp);
	lex_iterator_type iter_end = lex_iterator_type();
	std::cout << "ok\n";
	for (; iter != iter_end; iter++) {
		lex_token const& token = *iter;
		std::cout << token_id(token) << " " << boost::wave::get_token_name(token_id(token)) << " " << token.get_value() << std::endl;
	}
}
