#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
typedef boost::wave::cpplexer::lex_token<> lex_token;
typedef boost::wave::cpplexer::lex_iterator<lex_token> lex_iterator_type;
using boost::wave::token_id;
using namespace boost::wave;

class TokenMapper {
public:
	std::vector<std::pair<std::string, std::string>> pairs;
	std::unordered_map<std::string, std::string> tokenMap;
	int curid = 0;

	std::string mapToken(std::string const& inputToken);
	void writeHeader(std::ofstream& output);
	std::string translateToken(std::string const& inputToken);
};

std::string TokenMapper::mapToken(std::string const& inputToken) {
	if (tokenMap.count(inputToken) == 0) {
		std::string translatedToken = translateToken(inputToken);
		tokenMap[inputToken] = translatedToken;
		pairs.emplace_back(translatedToken, inputToken);
	}
	return tokenMap[inputToken];
}

std::string TokenMapper::translateToken(std::string const& inputToken) {
	//static const std::string digits[] = {"\u200b", "\u200c", "\u200d", "\ufeff"};
	static const std::string digits[] = {"a", "b", "c", "d"};
	static const std::string prefix = "bleached_";
	int rem = curid++;
	if (rem == 0) {
		return prefix + digits[0];
	}
	std::string outstr = "";
	while (rem != 0) {
		outstr = digits[rem & 0x3] + outstr;
		rem >>= 2;
	}

	return prefix + outstr;
}

void TokenMapper::writeHeader(std::ofstream& output) {
	for (auto const& pair: pairs) {
		output << "#define " << pair.first << " " << pair.second << std::endl;
	}
}

int main(int argc, char** argv) {
	if (argc < 3) {
		std::cerr << "usage: ./bleachthiscode <input.c> <output.c>" << std::endl;
		return 1;
	}
	std::ifstream instream{argv[1]};
	std::string instr{std::istreambuf_iterator<char>(instream.rdbuf()),
		std::istreambuf_iterator<char>()};
	std::ofstream fileoutput{argv[2], std::ios_base::out};
	std::stringstream output;
	lex_iterator_type iter(instr.begin(), instr.end(), {}, boost::wave::language_support::support_cpp);
	lex_iterator_type iter_end = lex_iterator_type();
	std::cout << "ok\n";
	bool lastEmittedSpace = false;
	bool lastTokenIsDefine = true;
	TokenMapper mapper;
	for (; iter != iter_end; ) {
		lex_token token = *iter;
		iter++;
		std::cout << token_id(token) << " " << boost::wave::get_token_name(token_id(token)) << " " << token.get_value() << std::endl;
		if (IS_CATEGORY(token, KeywordTokenType) || IS_CATEGORY(token, StringLiteralTokenType) ||
			IS_CATEGORY(token, OperatorTokenType) || IS_CATEGORY(token, IdentifierTokenType) || 
			IS_CATEGORY(token, IntegerLiteralTokenType)) {
			if (!lastEmittedSpace) {
				// if the last emitted is not a space, manually emit a separator
				output << " ";
			}
			// hack: leftparens belong with the previous identifier. function-like macros need this.
			if (IS_CATEGORY(token, IdentifierTokenType) && token_id(token) != T_EOF && token_id(*iter) == T_LEFTPAREN) {
				if (!lastTokenIsDefine) {
					output << mapper.mapToken((token.get_value() + "(").c_str());
				} else {
					// first arg of define is passed through unmodified.
					output << token.get_value() << "(";
				}
				lastEmittedSpace = false;
				lastTokenIsDefine = false;
				continue;
			}
			if (!lastTokenIsDefine) {
				output << mapper.mapToken(token.get_value().c_str());
			} else {
				// first arg of define is passed through unmodified.
				output << token.get_value();
			}
			lastEmittedSpace = false;
			lastTokenIsDefine = false;
		} else if (token_id(token) == T_CCOMMENT || token_id(token) == T_CPPCOMMENT) {
			// remove comments; we don't change the last emitted status
		} else if (token_id(token) == T_NEWLINE) {
			// newlines are printed directly
			output << token.get_value();
			lastEmittedSpace = true;
		} else if (IS_CATEGORY(token, WhiteSpaceTokenType)) {
			// other whitespace is turned into one space
			output << " ";
			lastEmittedSpace = true;
		} else {
			// pass through the rest
			output << token.get_value();
			lastEmittedSpace = false;
			if (token_id(token) == T_PP_DEFINE) {
				lastTokenIsDefine = true;
			} else {
				lastTokenIsDefine = false;
			}
		}
	}
	mapper.writeHeader(fileoutput);
	fileoutput << output.str();
}
