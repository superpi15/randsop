#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <iostream>
#include <gmp.h>
#include <gmpxx.h>
#include <stdlib.h>

class impz_class{
public:
	mpz_t obj;
};

struct mpz_cmptor{
	bool operator()( const impz_class& num1, const impz_class& num2 ) const {
		return mpz_cmp( num1.obj, num2.obj )<0;
	}
};
struct mpz_pair_cmptor{
	bool operator()( const std::pair<impz_class,impz_class>& num1, const std::pair<impz_class,impz_class>& num2 ) const {
		return mpz_cmptor()( num1.first, num2.first );
	}
};

int count_chunk( std::string& line ){
	std::istringstream istr(line);
	std::string chunk;
	int nChunk;
	for( nChunk = 0; istr >> chunk; nChunk++ )
		;
	return nChunk;
}

int main( int argc , char * argv[] ){
	char * ifname = NULL, * ofname = NULL;
	if( argc>1 )
		ifname = argv[1];
	if( argc>2 )
		ofname = argv[2];
	std::ifstream fin( ifname, std::ios::in );
	std::string line, word;

	std::ofstream fout;
	if( ofname )
		fout.open( ofname, std::ios::out );
	std::ostream& ostr =  ofname? fout: std::cout;

	int i_width = 0, o_width = 0;
	if( !std::getline( fin, line ) ){
		std::cout<<"missing \'.i [input#]\'"<<std::endl;
		exit(1);
	}
	ostr << line <<std::endl;
	if( !std::getline( fin, line ) ){
		std::cout<<"missing \'.o [output#]\'"<<std::endl;
		exit(1);
	}
	ostr << line <<std::endl;
	if( !std::getline( fin, line ) ){
		std::cout<<"missing \'.ilb [input pin]+\'"<<std::endl;
		exit(1);
	}
	ostr << line <<std::endl;
	i_width = count_chunk( line )- 1;
	if( !std::getline( fin, line ) ){
		std::cout<<"missing \'.ob [output pin]+\'"<<std::endl;
		exit(1);
	}
	ostr << line <<std::endl;
	o_width = count_chunk( line )- 1;

	// check width
	if( i_width != o_width ){
		std::cout<<"I/O width mismatch: "<< i_width <<" vs. "<< o_width<<std::endl;
		exit(1);
	}
	int width = i_width;
	if( width <= 0 ){
		std::cout<<"Invalid widht: "<< width<<std::endl;
		exit(1);
	}
	std::cout<<"# Width: "<< width <<std::endl;

	//randomize
	int nEntry = 0;
	unsigned long seed;
	gmp_randstate_t randstate;
	gmp_randinit_mt( randstate );
	gmp_randseed_ui( randstate, seed );
	std::set<impz_class,mpz_cmptor> inset;
	for( ; std::getline( fin, line ); nEntry++ ){
		std::istringstream isstr( line );
		std::string ip, op;
		if( !(isstr >> ip) ){
			std::cout<<"missing input pattern"<<std::endl;
			exit(1);
		}
		if( !(isstr >> op) ){
			std::cout<<"missing output pattern"<<std::endl;
			exit(1);
		}

		impz_class inobj;
		mpz_init( inobj.obj );
		do {
			mpz_urandomb( inobj.obj, randstate, width );
		} while( inset.find(inobj) != inset.end() );
		inset.insert( inobj );
		ostr.fill('0');
		ostr.width( width );
		ostr << mpz_class(inobj.obj).get_str(2) 
			<<" "<< op<<std::endl;
	}
	
	gmp_randclear( randstate );

	std::cout<<"nEntry="<< nEntry<<std::endl;
	
	if( ofname )
		fout.close();
}
