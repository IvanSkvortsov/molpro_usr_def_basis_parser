#ifndef __MOLPRO_PARSER_H__
#define __MOLPRO_PARSER_H__
#include"aux.stdlib.h"
#include"molpro.instruction.h"
#include"basis.h"
#include<fstream>
#include<iomanip>

using namespace std;

void error_msg(const std::vector<std::string> & buf, const std::string & s, const std::string & ms = "parser")
{
	std::cerr << "Error: [" << ms << "]" << std::endl;
	std::cerr << "token.size : " << buf.size() << std::endl;
	std::cerr << s << std::endl;
}

template<class T>
int shell_parse(basis_shell<T> & bas_sh, string const * vs, size_t size)
{
	std::vector<std::string> buf;
	// 0
	astd::strtok_s( buf, vs[0], " \t;," );
	if( !astd::is_shell( buf ) )
	{
		error_msg(buf, vs[0], "shell_parse");
		std::cerr << "line isn't shell" << std::endl;
		exit(1);
	}
	bas_sh.shell() = astd::spd( buf[0][0] );
	bas_sh.atom() = buf[1];
	astd::stof<vector<T> >( bas_sh.alp(), &buf[2], buf.size()-2);
	// 1, ...
	bas_sh.resize( size-1 );
	int alp_sz = 0, alp_1 = 0;
	std::cout << std::setw(2) << bas_sh.shell() << ' ' << setw(4) << bas_sh.atom() << std::endl;
	for(int i = 0; i < bas_sh.size(); ++i)
	{
		astd::strtok_s(buf, vs[i+1], " \t;,");
		if( !astd::is_funct(buf) )
		{
			error_msg(buf, vs[i+1], "shell_parse");
			std::cerr << "line isn't function" << std::endl;
			exit(1);
		}
		astd::c2ii( buf[1], alp_1, alp_sz );
		if( alp_sz - (alp_1 - 1) != buf.size() - 2)
		{
			error_msg(buf, vs[i+1], "shell_parse");
			std::cerr << "size : " << alp_sz - (alp_1 - 1) << std::endl;
			exit(1);
		}
		astd::stof<basis_function<T> >( bas_sh[i], &buf[2], buf.size()-2);
		std::cout << " i : " << i << std::endl;
		bas_sh[i].set_alp( &bas_sh.alp(0), alp_1-1);
	}
}

template<class T>
int parse(char const * file)
{
	basis_shell<T> bas_sh;
	//
	std::vector<std::string> vs, buf;
	std::ifstream inp(file);
	std::string s;
	//
	while( std::getline(inp, s) )
		vs.push_back( s );
	inp.close();
	for(int i = 0; i < vs.size(); ++i)
		std::cout << vs[i] << std::endl;
	std::cout << "size : " << vs.size() << std::endl << std::endl;
	//----------------------------------------//
	shell_parse<T>( bas_sh, &vs[0], vs.size() );
	//----------------------------------------//
	std::cout << std::endl;
	std::cout << "shell : " << bas_sh.shell() << std::endl;
	std::cout << "shell.size : " << bas_sh.size() << std::endl;
	std::cout << "alphas [" << bas_sh.alp().size() << "]" << std::endl;
	for(int i = 0; i < bas_sh.alp().size(); ++i)
		std::cout << bas_sh.alp(i) << std::endl;
	std::cout << "basis_functions :" << std::endl;
	for(int i = 0; i < bas_sh.size(); ++i)
	{
		for(int j = 0; j < bas_sh[i].size(); ++j)
			std::cout << setw(24) << bas_sh[i].get_alp(j) << setw(24) << bas_sh[i].get_d(j) << std::endl;
		std::cout << std::endl;
	}
	return 0;
}

/*
int parse_d(char const * file)
{
	return parse<double>( file );
}
*/

int read_lines(vector<string> & vs, char const * file)
{
	ifstream inp( file );
	if( !inp.is_open() ) return 1;
	string s;
	vs.clear();
	while( getline(inp, s) )
		vs.push_back( s );
	return 0;
}

template<class T>
int basisSet_parse(vector<basis_shell<T> > & atom_basis, string const * vs, size_t size)
{
	vector<int> vi;
	vi.reserve(20);
	vector<string> buf;
	buf.reserve( 4 );
	for(int i = 0; i < size; ++i)
	{
		astd::strtok_s(buf, vs[i], " \t;,", 4);
		if( astd::is_shell( buf ) ) vi.push_back( i );
	}
	vi.push_back( size );
	//int * p = &vi[0];
	//for(int i = 0; i < vs.size(); ++i)
	//{
	//	if( *p == i ){ p++; std::cout << "-----------------------------------------" << std::endl;}
	//	std::cout << std::setw(2) << i << ' ' << vs[i] << std::endl;
	//}
	//std::cout << "-----------------------------------------" << std::endl;
	//for(int i = 0; i < vi.size(); ++i)
	//	std::cout << vi[i] << std::endl;
	//std::cout << "size : " << vi.size() << std::endl << std::endl;
	atom_basis.resize( vi.size()-1 );
	//int * pz = &vi[0];
	for(int i = 0; i < atom_basis.size(); ++i)
	{
		//shell_parse<T>( atom_basis[i], &vs[ *pz ], *(pz+1)-*pz );
		//p++;
		shell_parse<T>( atom_basis[i], &vs[ vi[i] ], vi[i+1]-vi[i] );
	}
	return 0;
}

template<class T>
int molpro_basis_parse(char const * file)
{
	vector<basis_shell<T> > atom_basis;
	ifstream inp( file );
	vector<string> vs;
	if( read_lines( vs, file ) )
	{
		cerr << "Error: [basis_parse] can't open file '" << file << "'" << endl;
		return 1;
	}
	for(int i = 0; i < vs.size(); ++i)
		std::cout << vs[i] << std::endl;
	std::cout << "size : " << vs.size() << std::endl << std::endl;
	//--------------------------------//
	basisSet_parse<T>( atom_basis, &vs[0], vs.size() );
	//--------------------------------//
	return 0;
}

int parse_d(char const * file)
{
	return molpro_basis_parse<double>( file );
}

#endif//__MOLPRO_PARSER_H__
