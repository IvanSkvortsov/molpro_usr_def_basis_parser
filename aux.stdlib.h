#ifndef __AUX_STDLIB_H__
#define __AUX_STDLIB_H__
#include<iostream>// std::cerr, std::endl
#include<string>// std::string
#include<string.h>// strlen, strtok
#include<stdlib.h>// atoi, atof
#include<stdio.h>// sprintf
#include<ctype.h>// isdigit, isalpha
#include<vector>

namespace astd
{
	bool is_number(const std::string &);
	bool is_integer(const std::string &);
	bool is_c2ii(std::string const & s);
	bool is_word(const std::string &);
	bool is_spd(const char &);
	int shell(const char & c);
	char spd(const int & _shell);
	//
	bool is_shell(std::vector<std::string> const & );
	bool is_funct(std::vector<std::string> const & );
	bool is_ecp(std::vector<std::string> const &);
	bool is_ecpFunct(std::vector<std::string> const &);
	//
	int is_basis_scope(std::string const * s, std::string const * s_end);
	int is_close_scope(std::string const & s);
	int is_open_scope(std::string const & s);

	bool is_comment(const std::string &, const char);
	std::string cut_comment(const std::string &, const char);

	// converters of std::string varibale into int and floating point variables
	int stoi(std::string const & s);// convert std::string to integer
	double stof(std::string const & s);// convert std::string to double
	template<class vector_type>
	void stof(vector_type & vf, std::string const * ps, std::size_t size);

	// converters of number [int, float] into std::string
	std::string itos( int value, size_t base );
	std::string itos(int const & i);
	std::string ftos(double const & f);
	std::string ftos(long double const & f);

	// spliters of std::string variable into words
	void strtok_s(std::vector<std::string>& v, std::string const & s );
	void strtok_s(std::vector<std::string>& v, std::string const & s, std::string tok);
	void strtok_s(std::vector<std::string>& v, std::string const & s, std::string tok, int n);

	bool isStrEmpty(std::string const & s);
	bool isStrEmpty(char const * pc, char const * end);
	//  pc = &s[0]
	// end = &s[s.size()]

	std::string toupper_s(std::string const & s);
	std::string tolower_s(std::string const & s);

	int c2ii(std::string & s, int & begin, int & end );
	void d2e(std::string & s);

	int n_char(std::string const & s, char c);// is there (in s) n times c
};

//////////////////////////////////
//				//
//	  implementation	//
//				//
//////////////////////////////////

bool is_exp(char c)
{
	return c == 'e' || c == 'E' || c == 'd' || c == 'D';
}

bool is_pt(char c )
{
	return c == '.';
}

bool is_sgn(char c)
{
	return c == '+' || c == '-';
}

bool astd::is_number(std::string const & s)
{
	if( s.size() == 0 ) return false;
	int exp_pos = -1, pt_pos = -1;
	char ch = s[0];
	int i = ( is_sgn(ch) ? 1 : 0);
	for(; i < s.size(); ++i)
	{
		ch = s[i];
		if( !isdigit( ch ) )
		{
			if( is_pt( ch ) ) pt_pos = i;
			else if( is_exp( ch ) ) exp_pos = i;
			else return false;
			break;
		}
	}
	if( i == s.size() ) return true;// -314153
	++i;
	if( exp_pos == -1 )// -3.14153
	{
		for(; i < s.size(); ++i)
		{
			ch = s[i];
			if( !isdigit( ch ) )
			{
				if( is_exp( ch ) ) exp_pos = i;
				else return false;
				break;
			}
		}
	}
	if( exp_pos == -1 ) return true;// -3.14153
	++i;
	if( exp_pos != -1 )// -3.14153e
	{
		i += ( is_sgn( s[exp_pos+1] ) ? 1 : 0 );// -3.14153e+
		if( i >= s.size() ) return false;
		for(; i < s.size(); ++i)
			if( !isdigit( s[i] ) ) return false;
		return true;// -3.14153e+05
	}
	return false;
}

bool astd::is_integer(std::string const & s)
{
	for(int i = 0; i < s.size(); ++i)
		if( !isdigit( s[i] ) ) return false;
	return true;
}

bool astd::is_word(std::string const & s)
{
	for(int i = 0; i < s.size(); ++i)
		if( !isalpha(s[i]) )
			return false;
	return true;
}

bool astd::is_spd(char const & c)
{
	char shell[] = "spdfghijklmn", * p = shell;
	int len = strlen( shell );
	for(int i = 0; i < len; ++i)
		if( *p++ == tolower(c) )
			return true;
	return false;
}

char astd::spd(const int & n)
{
	char shell[] = "spdfghijklmn";
	int len = strlen( shell );
	if( n >= len || n < 0 )
		return 'n';
	return shell[n];
}

int astd::shell(const char & c)
{
	char shell[] = "spdfghijklmn", * p = shell;
	int len = strlen( shell );
	for(int i = 0; i < len; ++i)
		if( *p++ == tolower(c) )
			return i;
	return -1;
}
bool astd::is_c2ii(std::string const & s)
{
	int p_z = 0;
	for(int i = 0; i < s.size(); ++i)
	{
		if( !isdigit(s[i]) )
		{
			if( i == s.size()-1 ) return false;
			if( s[i] == '.' ) p_z++;
			else return false;
		}
	}
	if( p_z != 1 ) return false;
	return true;
}

bool astd::is_shell(std::vector<std::string> const & vs)
{
	if( vs.size() < 3 ) return false;
	if( vs[0].size() != 1 || !astd::is_spd( vs[0][0] ) ) return false;
	if( !astd::is_word( vs[1] ) ) return false;
	for(int i = 2; i < vs.size(); ++i)
		if( !astd::is_number( vs[i] ) ) return false;
	return true;
}

bool astd::is_funct(std::vector<std::string> const & vs)
{
	if( vs.size() < 3 ) return false;
	if( vs[0].size() != 1 || !astd::is_word( vs[0] ) ) return false;
	if( !astd::is_c2ii( vs[1] ) ) return false;
	for(int i = 2; i < vs.size(); ++i)
		if( !astd::is_number( vs[i] ) ) return false;
	return true;
}

bool astd::is_ecp(std::vector<std::string> const & vs)
{
	if( vs.size() < 4 || vs.size() > 5 ) return false;
	//if( vs.size() < 5 ) return false;
	if( !astd::is_word(vs[0]) || !astd::is_word( vs[1] ) ) return false;
	if( vs[0] != "ecp" && vs[0] != "ECP" ) return false;// FIXME: check if vs[0] is "Ecp", "eCp", "ecP", "eCP", "ECp", EcP"
	for(int i = 2; i < vs.size(); ++i)
		if( !astd::is_integer( vs[i] ) ) return false;
	return true;
}
bool astd::is_ecpFunct(std::vector<std::string> const & vs)
{
	if( vs.size() != 3 ) return false;
	if( !astd::is_integer(vs[0]) ) return false;
	if( !astd::is_number(vs[1]) ) return false;
	if( !astd::is_number(vs[2]) ) return false;
	return true;
}

static void basis_scope_err_msg(std::string const & s, std::string const & msg)
{
	std::cerr << "Error: [is_basis_scope]" << std::endl;
	std::cerr << "expected \"\\s*basis\\s*=\\s*\\{\\s*\", but \"" << msg << "\" not found" << std::endl;
	std::cerr << s << std::endl;
	exit(1);
}

int astd::is_open_scope(std::string const & s)
{
	int pos = s.find_first_not_of(" \t");
	if( pos == -1 )
		return 0;
	if(s.compare(pos, 1, "{") != 0)// not found "{"
		return 0;
	return 1;
}

int astd::is_basis_scope(std::string const * s, std::string const * s_end)
{
	int pos = s->find_first_not_of(" \t", pos);
	if( pos == -1 )
		return 0;
	if(s->compare(pos, 5, "basis") != 0)// not found "basis"
		return 0;
	pos = s->find_first_not_of(" \t", pos+5);
	if( pos == -1 )
		basis_scope_err_msg(*s, "=");
	if( s->compare(pos, 1, "=") != 0 )
		basis_scope_err_msg(*s, "=");
	pos = s->find_first_not_of(" \t", pos+1);
	if( pos != -1 )
	{
		if( s->compare(pos, 1, "{") != 0 ) basis_scope_err_msg(*s, "{");
		return 1;
	}
	if( s+1 < s_end )
		basis_scope_err_msg( *s, "{");
	if( !astd::is_open_scope( *(s+1) ) )
		basis_scope_err_msg(*(s+1), "{");
	return 2;
}

int astd::is_close_scope(std::string const & s)
{
	int pos = s.find_last_not_of(" \t");
	if( pos == -1 )
		return 0;
	if(s.compare(pos, 1, "}") != 0)// not found "}"
		return 0;
	return 1;
}

template<class vector_type>
void astd::stof( vector_type & vf, std::string const * ps, std::size_t size)
{
	vf.resize( size );
	for(int i = 0; i < size; ++i)
	{
		vf[i] = astd::stof(ps[i]);
	//	std::cout << "astd::stof( vector_type &, string const *, size_t ) : " << vf[i] << "\t" << ps[i] << std::endl; 
	}
	//std::cout << "stof.size : " << vf.size() << std::endl;
}

// convert floating point type varibale to std::string
std::string astd::ftos( long double const & f )
{
	char buf[50];
	sprintf(buf,"%.20e",f);
	return buf;
}

std::string astd::ftos( double const & f )
{
	char buf[50];
	sprintf(buf,"%.16e",f);
	return buf;
}


// itos
//
// convert integer to std::string
// this is light version - see 'itos.h' to get extended variant
std::string astd::itos( int value, size_t base = 10 )
{
	if( base > 16 )
		return "convertion error";
	if( value == 0 )
		return "0";
	char ch[] = "0123456789abcdef", _sgn = 0;
	std::string s;
	s.reserve(24);
	if( value < 0 && base == 10 )
	{
		_sgn = '-';
		value = -value;
	}
	unsigned int tmp = value;
	while( true )
	{
		tmp /= base;
		s += ch[value - tmp * base];
		if( tmp == 0)
			break;
		value = tmp;
	}
	if( _sgn )
		s += _sgn;
	char *p1 = &s[0], *p2 = &s[s.size()-1], tmp_ch;
	while( p1 < p2 )
	{
		tmp_ch = *p1;
		*p1++ = *p2;
		*p2-- = tmp_ch;
	}
	return s;
}

// convert std::string to floating point type variable
double astd::stof(std::string const & s)
{
	char const * pc = &s[0];
	return atof(pc);
}

// convert std::string to integer
int astd::stoi(std::string const & s){
	char const * pc=&s[0];
	return atoi(pc); // int atoi(char const * str)
}

// cut comment :
//
// # - Bash
// % - LaTeX
// # - Maple
// ! - Molpro

std::string astd::cut_comment(std::string const & s, const char com)
{
	for(int i = 0; i < s.size(); ++i)
	{
		if(s[i] == com)
		{
			if(i == 0)
				return "";
			return s.substr(0,i);
		}
	}
	return s;
}

bool astd::is_comment(const std::string& str, const char com)
{
	int pos = (int)str.find_first_not_of(" \t");
	if(pos != -1){
		if(str[ pos ] == com)
			return true;
		else return false;
	}
	return false;// didn't find not of " \t" -> 'empty line'
}


// split std::string variable into words :
// delimiter == ' ' (white space)
// save in std::vector<std::string>
void astd::strtok_s(std::vector<std::string>& v, std::string const & s ){
	std::string str = s;
	v.reserve( 10 );
	v.clear();
	char * buf = &str[0], * pch;
	pch = strtok(buf, " ");
	while(pch != 0 ){
		v.push_back(pch);
		pch = strtok(0, " ");
	}
}

// split std::string variable into words :
// delimiter == 'delim'
// save in std::vector<std::string>
void astd::strtok_s(std::vector<std::string>& v, std::string const & s, std::string delim){
	std::string str = s;
	v.reserve( 10 );
	v.clear();
	char * buf = &str[0], * pch, * cdelim = &(delim[0]);
	pch = strtok(buf, cdelim);
	while(pch != 0 ){
		v.push_back(pch);
		pch = strtok(0, cdelim);
	}
}

// split std::string variable into words :
// number of words == n
// delimiter == 'delim'
// save in std::vector<std::string>
void astd::strtok_s(std::vector<std::string>& vs, std::string const & s, std::string delim, int n){
	std::string str = s;
	char * buf = &str[0], * pch, * cdelim = &delim[0];
	vs.reserve( n );
	vs.clear();
	pch = strtok(buf, cdelim);
	vs.push_back( pch );
	for(int i = 1; i < n; ++i)
	{
		pch = strtok(0 ,cdelim);
		if( pch == 0 ) break;
		vs.push_back( pch );
	}
}

bool astd::isStrEmpty(std::string const & s)
{
	char const * pc = &s[0], * endpc = &s[s.size()];
	while(true) 
	{
		if( pc == endpc )
			return true;
		if( *pc != ' ' && *pc != '\t' )
			return false;
		++pc;
	}
	return false;
}

bool astd::isStrEmpty(char const * pc, char const * end)
{
	while( true ) 
	{
		if( pc == end )
			return true;
		if( *pc != ' ' && *pc != '\t' )
			return false;
		++pc;
	}
	return false;
}

// void c2ii
//
// construct from std::string like "x.y" two integer values:
// begin := x
// end := y
int astd::c2ii(std::string & s, int & begin, int & end )
{
	std::string str = s;
	char * pch_begin = &str[0], *pch_cur;
	pch_cur = pch_begin;
	for( int i = 0; i < str.size(); ++i )
	{
		if( *pch_cur == '.' )
		{
			*pch_cur = 0;
			++pch_cur;
			break;
		}
		++pch_cur;
	}
	if( pch_cur == &str[str.size()] )
	{
		std::cerr << "Error: [c2ii] '" << str << "' doesn't contain any \'.\'" << std::endl;
		begin = 0; end = 0;
		return -1;
	}
	begin = atoi( pch_begin );
	end = atoi( pch_cur );
	return 1;
}

// void d2e
//
// convert first std::string "D" or "d" into "E"
// it needs in some old basis sets parsing when basis parameters (alphas and contraction coefficients)
// are given in form of "0.123456D+03" where "D" is equal to "E" - decimal exponent notation
void astd::d2e(std::string & s)
{
	char * p = &s[0], *p_end = &s[s.size()];
	while( p != p_end )
	{
		if( *p == 'D' || *p == 'd' )
		{
			*p = 'E';
			break;
		}
		++p;
	}
}

std::string astd::toupper_s(std::string const & s)
{
	std::string str = s;
	char *p = &str[0], *p_end = &str[str.size()];
	for(int i = 0; i < str.size(); ++i)
	{
		*p = toupper(*p);
		++p;
	}
	return str;
}
std::string astd::tolower_s(std::string const & s)
{
	std::string str = s;
	char *p = &str[0], *p_end = &str[str.size()];
	for(int i = 0; i < str.size(); ++i)
	{
		*p = tolower(*p);
		++p;
	}
	return str;
}

int astd::n_char(std::string const & s, char c )// is there (in s) n times c
{
	int n = 0;
	for(int i = 0; i < s.size(); ++i)
		if( c == s[i] ) n++;
	return n;
}

#endif//__AUX_STDLIB_H__
