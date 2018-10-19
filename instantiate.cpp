#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp>
#include <boost/wave/cpplexer/re2clex/cpp_re2c_lexer.hpp>
#include <iostream>
template struct boost::wave::cpplexer::new_lexer_gen<std::string::iterator>;
