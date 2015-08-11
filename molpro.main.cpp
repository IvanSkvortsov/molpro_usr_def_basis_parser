#include"molpro.parser.h"

int main(int argc, char *argv[])
{
	if( argc != 2 )
	{
		cerr << "Error: [main] usage './main.exe file.inp'" << endl;
		return 1;
	}
	parse_d(argv[1]);
	return 0;
}
