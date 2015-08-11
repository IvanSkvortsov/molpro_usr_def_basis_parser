#ifndef __MOLPRO_INSTRUCTION_H__
#define __MOLPRO_INSTRUCTION_H__
#include<string>// string
#include<vector>// vector

// user defined basis
struct molpro_instruction
{
	std::vector<std::string> vs;
	std::string _atom, _type;
	int _1z, _2z, _3z;
	molpro_instruction():_atom(), _type(), _1z(0), _2z(0), _3z(0){}
	molpro_instruction(std::string __atom, std::string __type, int __1z, int __2z, int __3z):
		_atom(__atom), _type(__type),_1z(__1z), _2z(__2z), _3z(__3z){}
	~molpro_instruction(){}
	void set(std::string __atom, std::string __type, int __1z, int __2z, int __3z)
	{
		_atom = __atom;
		_type = __type;
		_1z = __1z;
		_2z = __2z;
		_3z = __3z;
	}
	std::string & atom(){return _atom;}
	std::string const & atom()const{return _atom;}
	std::string const & type()const{return _type;}
	std::string & type(){return _type;}
	//
	int & z1(){return _1z;}
	int & z2(){return _2z;}
	int & z3(){return _3z;}
	int const & z1()const{return _1z;}
	int const & z2()const{return _2z;}
	int const & z3()const{return _3z;}
	//
	int & core(){return _1z;}
	int & n_sc(){return _2z;}
	int & n_so(){return _3z;}
	int const & core()const{return _1z;}
	int const & n_sc()const{return _2z;}
	int const & n_so()const{return _3z;}
	//
	int & shell() {return _1z;}
	int & n_alp() {return _2z;}
	int & n_func(){return _3z;}
	int const & shell() const{return _1z;}
	int const & n_alp() const{return _2z;}
	int const & n_func()const{return _3z;}
};

#endif//__MOLPRO_INSTRUCTION_H__
