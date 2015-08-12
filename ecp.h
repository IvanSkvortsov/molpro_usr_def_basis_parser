#ifndef __SEMI_LOCAL_ECP_H__
#define __SEMI_LOCAL_ECP_H__

template<class T>
struct ecp_gauss
{
	int _n;
	T _alp, _d;
	ecp_gauss():_n(0), _alp(0), _d(0){}
	int & n(){return _n;}
	T & alp(){return _alp;}
	T & d(){return _d;}
	int const & n()const{return _n;}
	T const & alp()const{return _alp;}
	T const & d()const{return _d;}
};

template<class T>
struct ecp_function
{
	std::vector<ecp_gauss<T> > _ecp_f;
	ecp_function():_ecp_f(){}
	ecp_function(std::size_t __size):_ecp_f(__size){}
	std::size_t size()const{return _ecp_f.size();}
	ecp_gauss<T> & operator[](std::size_t i){return _ecp_f[i];}
	ecp_gauss<T> const & operator[](std::size_t i)const{return _ecp_f[i];}
	void resize(std::size_t __size){ _ecp_f.resize( __size);}
};

template<class T>
struct semilocal_ecp
{
	std::vector<ecp_function<T> > _sl_ecp;
	int _n_sc, _n_so;
	//
	semilocal_ecp():_sl_ecp(), _n_sc(0), _n_so(0){}
	//
	std::size_t size()const{return _sl_ecp.size();}
	ecp_function<T> & operator[](std::size_t i){return _sl_ecp[i];}
	ecp_function<T> const & operator[](std::size_t i)const{return _sl_ec[i];}
	void resize(std::size_t __size){ _sl_ecp.resize( __size);}
	//
	int & n_sc(){return _n_sc;}
	int & n_so(){return _n_so;}
	int const & n_sc()const{return _n_sc;}
	int const & n_so()const{return _n_so;}
};
#endif//__SEMI_LOCAL_ECP_H__
