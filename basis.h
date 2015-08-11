#ifndef __BASIS_H__
#define __BASIS_H__
#include<vector>

template<class T>
struct basis_function
{
protected:
	std::vector<T> d;
	T const * alp, * alp_0;
public:
	basis_function():d(), alp(0), alp_0(0){}
	// alp
	std::size_t diff()const{return alp - alp_0;}
	//T & get_alp(std::size_t i){return alp[i];}
	T const & get_alp(std::size_t i)const{return alp[i];}
	void set_alp(T const * __alp_0, std::size_t __alp)
	{
		alp = __alp_0 + __alp;
		alp_0 = __alp_0;
	}
	void set_alp(T const * __alp_0, T const * __alp)
	{
		alp = __alp;
		alp_0 = __alp_0;
	}
	// d
	std::size_t size()const{return d.size();}
	void resize(std::size_t __size){d.resize(__size);}
	void reserve(std::size_t __size){d.reserve(__size);}
	T const & operator[](std::size_t i)const{return d[i];}
	T & operator[](std::size_t i){return d[i];}
	T const & get_d(std::size_t i)const{return d[i];}
	T & get_d(std::size_t i){return d[i];}
};

#include<string>
template<class T>
struct basis_shell
{
protected:
	std::vector<T> _alp;
	std::vector<basis_function<T> > _bf;
	int _shell;
	std::string _atom;
public:
	basis_shell():_alp(), _bf(), _shell(0){}
	// bf
	std::size_t size()const{return _bf.size();}
	void resize(std::size_t __size){_bf.resize( __size);}
	void reserve(std::size_t __size){_bf.reserve( __size);}
	basis_function<T> & operator[](std::size_t i){return _bf[i];}
	basis_function<T> const & operator[](std::size_t i)const{return _bf[i];}
	// shell
	int & shell(){return _shell;}
	int const & shell()const{return _shell;}
	// alp
	std::vector<T> & alp(){return _alp;}
	std::vector<T> const & alp()const{return _alp;}
	T & alp(std::size_t i){return _alp[i];}
	T const & alp(std::size_t i)const{return _alp[i];}
	// _atom
	std::string & atom(){return _atom;}
	std::string const & atom()const{return _atom;}
};

#endif//__BASIS_H__
