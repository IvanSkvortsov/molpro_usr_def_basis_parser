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
	bas_sh.shell() = astd::shell( buf[0][0] );
	bas_sh.atom() = buf[1];
	astd::stof<vector<T> >( bas_sh.alp(), &buf[2], buf.size()-2);
	return basisF_parse(bas_sh, buf, vs+1, vs_end);
}

//------------------------------- ECP PARSE --------------------------------//
template<class T>
int ecpFunction_parse( ecp_function<T> & ecp_f, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end )
{
	ecp_f.resize( astd::stoi( buf[0] ) );
	check_pointer<std::string>( vs+ecp_f.size()-1, vs_end );
	ecp_gauss<T> * p_g = &ecp_f[0];
	for(int i = 0; i < ecp_f.size(); ++i)
	{
		astd::strtok_s( buf, vs[i], " \t," );
		if( !astd::is_ecpFunct( buf ) ) return 1;
		p_g->n()   = astd::stoi( buf[0] );
		p_g->alp() = astd::stof( buf[1] );
		p_g->d()   = astd::stof( buf[2] );
		p_g++;
	}
	return 0;
}

template<class T>
int ecpF_parse(semilocal_ecp<T> & sl_ecp, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end)
{
	check_pointer<std::string>( vs, vs_end );
	sl_ecp.resize( sl_ecp.n_sc() + sl_ecp.n_so() + 1 );
	std::string const * ps = vs;
	for(int i = 0; i < sl_ecp.size(); ++i)
	{
		check_pointer<std::string>( ps, vs_end );
		astd::strtok_s(buf, *ps, " \t;,");
		if( !astd::is_integer( buf[0] ) || buf.size() != 1 )
		{
			error_msg( buf, *ps, "ecpF_parser");
			std::cerr << "line isn't integer" << std::endl;
			exit(1);
		}
		if( ecpFunction_parse<T>( sl_ecp[i], buf, ++ps, vs_end ) )
			exit(1);
		ps += sl_ecp[i].size();
	}
	return ps-vs;
}

template<class T>
int ecp_parse(semilocal_ecp<T> & sl_ecp, std::vector<std::string> & buf, std::string const * vs, std::string const * vs_end)
{
	sl_ecp.atom() = buf[1];
	sl_ecp.core() = astd::stoi(buf[2]);
	sl_ecp.n_sc() = astd::stoi(buf[3]);
	if( buf.size() > 4 ) sl_ecp.n_so() = astd::stoi(buf[4]);
	else sl_ecp.n_so() = 0;
	return ecpF_parse<T>( sl_ecp, buf, vs+1, vs_end );
}

int read_lines(vector<string> & vs, char const * file)
{
	ifstream inp( file );
	if( !inp.is_open() )
		return 1;
	string s;
	std::vector<std::string> buf;
	buf.reserve(4);
	vs.clear();
	while( getline(inp, s) )
	{
		if( astd::is_comment( s, '!') || astd::isStrEmpty( s ) ) continue;
		astd::strtok_s( buf, astd::cut_comment(s, '!'), ";");
		for(int i = 0; i < buf.size(); ++i)
			if( !astd::isStrEmpty( buf[i] ) ) vs.push_back( buf[i] );
	}
	if( vs.size() == 0 )
	{
		std::cerr << "Error: [read] in file \"" << file << "\" no basis found" << std::endl;
		exit(1);
	}
	return 0;
}

int read_basis(vector<string> & vs, vector<string> const & lines)
{
	std::vector<std::string> buf;
	buf.reserve( 3 );
	vs.reserve( lines.size() );
	vs.clear();
	int _start = 0, _end = 0, i = 0;
	std::string const * ps = &lines[0], *ps_end = &lines[lines.size()];
	for(i = 0; i < lines.size(); ++i)
	{
		_start = astd::is_basis_scope(ps++, ps_end);
		if( _start == 0 ) continue;
		if( _start == 1 ) break;
		if( _start == 2 ) {++i; break;}
	}
	if( _start == 0 )
	{
		std::cerr << "Error: [read_basis] no basis was found" << std::endl;
		exit(1);
	}
	ps = &lines[i];
	_start = i;
	for(; i < lines.size(); ++i)
	{
		_end = astd::is_close_scope( *ps++ );
		if( _end == 0 ) continue;
		break;
	}
	if( _end == 0 )
	{
		std::cerr << "Error: [read_basis] no basis scope not closed" << std::endl;
		exit(1);
	}
	ps = &lines[i];
	_end = i;
	int pos_op = 0, pos_cl = 0;
	if( _start == _end )
	{
		pos_op = ps->find("{");
		pos_cl = ps->find("}");
		astd::strtok_s( buf, ps->substr( pos_op+1, pos_cl-1 ), ";");
		for(int j = 0; j < buf.size(); ++j)
			if( !astd::isStrEmpty(buf[j]) ) vs.push_back(buf[j]);
	}
	ps = &lines[_start];
	pos_op = ps->find("{");
	astd::strtok_s( buf, ps->substr( pos_op+1 ), ";");
	for(int j = 0; j < buf.size(); ++j)
		if( !astd::isStrEmpty(buf[j]) ) vs.push_back(buf[j]);
	for(i = _start+1; i < _end; ++i)
		vs.push_back( lines[i] );
	ps = &lines[_end  ];
	if( pos_cl = ps->find("}") == 0 )
		return 0;
	astd::strtok_s( buf, ps->substr(0,  pos_cl-1 ), ";");
	for(int j = 0; j < buf.size(); ++j)
		if( !astd::isStrEmpty(buf[j]) ) vs.push_back(buf[j]);
	return 0;
}

template<class T>
void print_ecp(std::ostream & out, vector<semilocal_ecp<T> > const & atom_ecp)
{
	semilocal_ecp<T> const * p_ecp;
	ecp_function<T> const * p_ecpf;
	int prec = 12, w = prec + 8;
	out.precision( prec );
	out.setf( std::ios::scientific );
	for(int i = 0; i < atom_ecp.size(); ++i)
	{
		p_ecp = &atom_ecp[i];
		out <<  std::setw(3) << "" << "ECP" << std::setw(4) << p_ecp->atom() <<
			std::setw(4) << p_ecp->core() <<
			std::setw(4) << p_ecp->n_sc() <<
			std::setw(4) << p_ecp->n_so() << std::endl;
		for(int j = 0; j < p_ecp->size(); ++j)
		{
			p_ecpf = &(p_ecp->operator[](j));
			out << std::setw(4) << p_ecpf->size() << endl;
			for(int k = 0; k < p_ecpf->size(); ++k)
			{
				out <<  std::setw(4) << (*p_ecpf)[k].n() <<
					std::setw(w) << (*p_ecpf)[k].alp() <<
					std::setw(w) << (*p_ecpf)[k].d() << std::endl;
			}
		}
		out << std::endl;
	}
}

template<class T>
void print_basis(std::ostream & out, vector<basis_shell<T> > const & atom_basis)
{
	basis_shell<T> const * p_bas;
	basis_function<T> const * p_bf;
	int prec = 10, w = prec + 8;
	out.precision( prec );
	out.setf( std::ios::scientific );
	for(int i = 0; i < atom_basis.size(); ++i)
	{
		p_bas = &atom_basis[i];
		out <<  std::setw(4) << astd::spd( p_bas->shell() ) << std::setw(4) << p_bas->atom() <<
			std::setw(4) << p_bas->alp().size() <<
			std::setw(4) << p_bas->size() << std::endl;
		// alphas
		for(int j = 0; j < p_bas->alp().size(); ++j)
		{
			if( j%8==0 && j ) out << std::endl;
			out << std::setw(w) << p_bas->alp(j);
		}
		out << std::endl;
		// functions
		for(int j = 0; j < p_bas->size(); ++j)
		{
			p_bf = &(p_bas->operator[](j));
			out <<  std::setw(2) << p_bf->size() <<
				std::setw(4) << 1+p_bf->diff() << '.' << p_bf->diff() + p_bf->size()<< std::endl;
			for(int k = 0; k < p_bf->size(); ++k)
			{
				out <<  std::setw(w) << p_bf->get_alp(k) <<
					std::setw(w) << p_bf->get_d(k) << std::endl;
			}
		}
		out << std::endl;
	}
}

void heh_basis(vector<string> const & vs, char const file[] = "some.basis.out")
{
	ofstream out( file );
	out << "basis={";
	for(int i = 0; i < vs.size(); ++i)
		out << vs[i] << ";";
	out << "}";
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
	vector<string> lines, vs;
	if( read_lines( lines, file ) )
	{
		cerr << "Error: [basis_parse] can't open file '" << file << "'" << endl;
		return 1;
	}
	//for(int i = 0; i < lines.size(); ++i)
	//	std::cout << lines[i] << std::endl;
	//std::cout << "size : " << lines.size() << std::endl << std::endl;
	read_basis( vs, lines );
	//for(int i = 0; i < vs.size(); ++i)
	//	std::cout << vs[i] << std::endl;
	//std::cout << "size : " << vs.size() << std::endl << std::endl;
	//heh_basis( vs );
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
			size = ecp_parse<T>( sl_ecp, buf, &vs[i], ps_end );
			atom_ecp.push_back( sl_ecp );
			i += size;
		}
	}
	print_ecp<T>(cout, atom_ecp);
	print_basis<T>(cout, atom_basis);
	return 0;
}
int parse_d(char const * file)
{
	return basis_parse<double>( file );
}

#endif//__MOLPRO_PARSER_H__
