#ifndef __MOLPRO_PARSER_H__
#define __MOLPRO_PARSER_H__
#include"aux.stdlib.h"
#include"molpro.instruction.h"
#include"basis.h"
#include"ecp.h"
#include<fstream>
#include<iomanip>

using namespace std;

void error_msg(const std::vector<std::string> & buf, const std::string & s, const std::string & ms = "parser")
{
	std::cerr << "Error: [" << ms << "]" << std::endl;
	std::cerr << "token.size : " << buf.size() << std::endl;
	if( s.size() ) std::cerr << s << std::endl;
}

template<class T>
int check_pointer(T const * begin, T const * end)
{
	if( begin >= end )
	{
		std::cerr << "Error: [check_pointer] overflow" << std::endl;
		std::cerr << "end   [" << end << "]" << std::endl;
		std::cerr << "begin [" << begin << "]" << std::endl;
		exit(1);
	}
	return 0;
}

int instruction_typedef(std::vector<std::string> const & buf)
{
	if( astd::is_shell( buf ) )
		return 1;
	if( astd::is_ecp( buf ) )
		return 2;
	return 0;
}

//------------------------------ BASIS PARSE -------------------------------//
template<class T>
int basisFunction_parse(basis_shell<T> & bas_sh, basis_function<T> const & bf, std::vector<std::string> & buf, std::string const * vs, int & i)
{
	int alp_sz = 0, alp_1 = 0;
	if( !astd::is_funct(buf) )
	{
		if( i == 0 )
		{
			error_msg(buf, vs[i], "basisFunction_parse");
			std::cerr << "line isn't function" << std::endl;
			exit(1);
		}
		if( astd::is_ecp( buf ) || astd::is_shell( buf ) )
			return i;
		error_msg(buf, vs[i], "basisFunction_parse");
		std::cerr << "line isn't function" << std::endl;
		exit(1);
		//return 2;
	}
	astd::c2ii( buf[1], alp_1, alp_sz );
	if( alp_sz - (alp_1 - 1) != buf.size() - 2)
	{
		error_msg(buf, vs[i], "basisFunction_parse");
		std::cerr << "size : " << alp_sz - (alp_1 - 1) << std::endl;
		exit(1);
	}
	bas_sh.push_back( bf );
	astd::stof<basis_function<T> >( bas_sh[i], &buf[2], buf.size()-2);
	bas_sh[i].set_alp( &bas_sh.alp(0), alp_1-1);
	return 0;
}

template<class T>
int basisF_parse(basis_shell<T> & bas_sh, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end)
{
	check_pointer<std::string>( vs, vs_end );
	//
	int size = vs_end - vs;
	bas_sh.reserve( size );
	bas_sh.clear();
	basis_function<T> bf;
	int iter = 0;
	for(int i = 0; i < size; ++i)
	{
		astd::strtok_s(buf, vs[i], " \t;,");
		iter = basisFunction_parse<T>( bas_sh, bf, buf, &vs[i], i);
		if( iter != 0 ) return i;
	}
	return size;
}

template<class T>
int basisSet_parse(basis_shell<T> & bas_sh, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end)
{
	bas_sh.shell() = astd::spd( buf[0][0] );
	bas_sh.atom() = buf[1];
	astd::stof<vector<T> >( bas_sh.alp(), &buf[2], buf.size()-2);
	return basisF_parse(bas_sh, buf, vs+1, vs_end);
}

//------------------------------- ECP PARSE --------------------------------//
template<class T>
int ecpFunction_parse( ecp_function<T> & ecp_f, std::vector<std::string> const & buf )
{
	ecp_f.resize( astd::stoi( buf[0] ) );
	ecp_gauss<T> * p_g = &ecp_f[0];
	std::string const * p_s = &buf[1];
	for(int i = 0; i < ecp_f.size(); ++i)
	{
		p_g->n()   = astd::stoi( *p_s );
		p_g->alp() = astd::stoi( *(p_s + 1) );
		p_g->d()   = astd::stoi( *(p_s + 2) );
		p_s += 3;
	}
	return 0;
}

template<class T>
int ecpF_parse(semilocal_ecp<T> & sl_ecp, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end)
{
	check_pointer<std::string>( vs, vs_end );
	for(int i = 0; i < sl_ecp.size(); ++i)
	{
		astd::strtok_s(buf, vs[i], " \t;,");
		if( !astd::is_ecpFunct(buf) )
		{
			error_msg( buf, vs[i], "ecp_parser");
			std::cerr << "line isn't ecp function" << std::endl;
			exit(1);
		}
		ecpFunction_parse<T>( sl_ecp[i], buf );
	}
}

template<class T>
int ecp_parse(semilocal_ecp<T> & sl_ecp, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end)
{
	sl_ecp.atom() = buf[1];
	sl_ecp.core() = astd::stoi(buf[2]);
	sl_ecp.n_sc() = astd::stoi(buf[3]);
	sl_ecp.n_so() = astd::stoi(buf[4]);
	sl_ecp.resize( sl_ecp.n_sc() + sl_ecp.n_so() + 1 );
	ecpF_parse<T>( sl_ecp, buf, vs+1, vs_end );
	return 0;
}

int read_lines(vector<string> & vs, char const * file)
{
	ifstream inp( file );
	if( !inp.is_open() ) return 1;
	string s;
	vs.clear();
	while( getline(inp, s) )
	{
		if( astd::is_comment( s, '!') || astd::isStrEmpty( s ) ) continue;
		vs.push_back( s );
	}
	return 0;
}

//-------------------------- MOLPRO BASIS PARSE ----------------------------//
template<class T>
int basis_parse(char const * file)
{
	vector<basis_shell<T> > atom_basis;
	vector<semilocal_ecp<T> > atom_ecp;
	basis_shell<T> bas_sh;
	semilocal_ecp<T> sl_ecp;
	//
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
	std::string const * ps_end = &vs[vs.size()];
	std::vector<std::string> buf;
	int ins_type = 0, size = 0;
	for(int i = 0; i < vs.size(); ++i)
	{
		astd::strtok_s(buf, vs[i], " \t;,");
		ins_type = instruction_typedef( buf );
		if( ins_type == 0 )
		{
			std::cerr << std::setw(2) << i << " " << vs[i] << std::endl;
			error_msg( buf, vs[i], "basis_parse");
			std::cerr << "line isn't any instruction" << std::endl;
			std::cerr << "line number : " << i << std::endl;
			exit(1);
		}
		if( ins_type == 1 )// shell
		{
			std::cerr << std::setw(2) << i << " " << vs[i] << std::endl;
			size = basisSet_parse<T>( bas_sh, buf, &vs[i], ps_end );
			atom_basis.push_back( bas_sh );
			i += size;
		}
		if( ins_type == 2 )// ecp
		{
			std::cerr << std::setw(2) << i << " " << vs[i] << std::endl;
			ecp_parse<T>( sl_ecp, buf, &vs[i], ps_end );
			atom_ecp.push_back( sl_ecp );
			i += sl_ecp.size();
		}
	}
	return 0;
}
int parse_d(char const * file)
{
	return basis_parse<double>( file );
}

#endif//__MOLPRO_PARSER_H__
