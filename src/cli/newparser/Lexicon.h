#pragma once

#include "Common.h"

namespace smtrat {
namespace parser {

struct RationalPolicies : qi::real_policies<Rational> {
    template <typename It, typename Attr>
    static bool parse_nan(It&, It const&, Attr&) { return false; }
    template <typename It, typename Attr>
    static bool parse_inf(It&, It const&, Attr&) { return false; }
};

/**
 * Parses numerals: `(0 | [1-9][0-9]*)`
 */
struct NumeralParser: qi::uint_parser<Integer,10,1,-1> {};

/**
 * Parses decimals: `numeral.0*numeral`
 */
struct DecimalParser: qi::real_parser<Rational, RationalPolicies> {};

/**
 * Parses hexadecimals: `#x[0-9a-fA-F]+`
 */
struct HexadecimalParser: public qi::grammar<Iterator, Rational(), Skipper> {
    HexadecimalParser(): HexadecimalParser::base_type(main, "hexadecimal") {
		main = "#x" > number;
	}
    qi::uint_parser<Integer,16,1,-1> number;
    qi::rule<Iterator, Rational(), Skipper> main;
};

/**
 * Parses binaries: `#b[01]+`
 */
struct BinaryParser: public qi::grammar<Iterator, Integer(), Skipper> {
    BinaryParser(): BinaryParser::base_type(main, "binary") {
		main = "#b" > number;
	}
    qi::uint_parser<Integer,2,1,-1> number;
    qi::rule<Iterator, Integer(), Skipper> main;
};

/**
 * Parses strings: `".+"` with escape sequences `\"` and `\\`
 */
struct StringParser: public qi::grammar<Iterator, std::string(), Skipper> {
    StringParser(): StringParser::base_type(main, "string") {
        main = qi::no_skip[qi::char_('"') > +(escapes | ~qi::char_('"')) > qi::char_('"')];
        main.name("string");
        escapes.add("\\\\", '\\');
        escapes.add("\\\"", '"');
        escapes.name("escape sequences");
    }
    qi::symbols<char, char> escapes;
    qi::rule<Iterator, std::string(), Skipper> main;
};

/*
 * Reserved words: Not used yet.
 */

/**
 * Parses symbols: `simple_symbol | quoted_symbol` where
 * - `simple_symbol` is any string of `[0-9a-zA-Z~!@$%^&*_-+=<>.?/]` that does not start with a digit and is not a reserved word.
 * - `quoted_symbol` is any string of printable characters (including space, tab, line-breaks) except `\` and `|` enclosed in `|` characters.
 */
struct SimpleSymbolParser: public qi::grammar<Iterator, std::string(), Skipper> {
    SimpleSymbolParser(): SimpleSymbolParser::base_type(main, "simple symbol") {
        // Attention: "-" must be the first or last character!
        main = qi::lexeme[ (qi::alpha | qi::char_("~!@$%^&*_+=<>.?/-")) > *(qi::alnum | qi::char_("~!@$%^&*_+=<>.?/-"))];
    }
    qi::rule<Iterator, std::string(), Skipper> main;
};
struct SymbolParser: public qi::grammar<Iterator, std::string(), Skipper> {
    SymbolParser(): SymbolParser::base_type(main, "symbol") {
        main = quoted | simple;
        main.name("symbol");
        quoted = qi::lit('|') > qi::no_skip[+(~qi::char_("|")) > qi::lit('|')];
        quoted.name("quoted symbol");
    }
    qi::rule<Iterator, std::string(), Skipper> main;
    qi::rule<Iterator, std::string(), Skipper> quoted;
    SimpleSymbolParser simple;
};

/**
 * Parses keywords: `:simple_symbol`
 */
struct KeywordParser: public qi::grammar<Iterator, std::string(), Skipper> {
    KeywordParser(): KeywordParser::base_type(main, "keyword") {
        main = qi::lit(":") >> simple;
        main.name("keyword");
    }
    qi::rule<Iterator, std::string(), Skipper> main;
    SimpleSymbolParser simple;
};

}
}

namespace boost { namespace spirit { namespace traits {
    template<> inline void scale(int exp, smtrat::Rational& r) {
        if (exp >= 0)
            r *= carl::pow(smtrat::Rational(10), (unsigned)exp);
        else
            r /= carl::pow(smtrat::Rational(10), (unsigned)(-exp));
    }
    template<> inline bool is_equal_to_one(const smtrat::Rational& value) {
        return value == 1;
    }
	/**
	 * Specialization of standard implementation to fix compilation errors.
	 * Standard implementation looks like this:
	 * <code>return neg ? -n : n;</code>
	 * However, if using gmpxx <code>-n</code> and <code>n</code> have different types.
	 * This issue is fixed in this implementation.
	 */
	template<> inline smtrat::Rational negate(bool neg, const smtrat::Rational& n) {
		return neg ? smtrat::Rational(-n) : n;
	}
}}}