#include "nw.h"
#include "readsmappingEnclave_t.h"
using namespace std;

int nw(string seq_1, string seq_2, string& seq_1_al, string& seq_2_al, int &score, bool prm)
{
	//TRACE(7, uint64_t(&dpm_init));
	//TRACE(8, uint64_t(&nw_align));
	int i;
	int d = 2 ;                 /* gap penalty */

	int L1 = seq_1.length();
	int L2 = seq_2.length();

	// Dynamic programming matrix
	int **F = new int * [L2+1];
	for(i = 0; i <= L2; i++)  F[i] = new int[L1+1];

	// Traceback matrix
	char **traceback = new char* [L2+1];
	for(i = 0; i <= L2; i++)  traceback[i] = new char[L1+1];

	// Initialize traceback and F matrix (fill in first row and column)
	dpm_init(F, traceback, L1, L2, d);

	// Create alignment
	nw_align(F, traceback, seq_1, seq_2, seq_1_al, seq_2_al, d);
	
	//TRACE(9, uint64_t(&F[0][0]));
	//TRACE(9, uint64_t(&F[L1][L2]));
	
	score = F[L2][L1];
	//cout<<score<<endl;
	for(i = 0; i <= L2; i++) delete[] F[i];
	delete[] F;
	for(i = 0; i <= L2; i++) delete[] traceback[i];
	delete[] traceback;
	// may leak memory

	return  0 ;
}


void dpm_init(int ** F, char ** traceback, int L1, int L2, int d)
{
	F[0][0] =  0 ;
	traceback[0][0] = 'n' ;

	int i=0, j=0;

	for(j = 1; j <= L1; j++)
	{
		F[0][j] =  -j * d ;
		traceback[0][j] =  '-' ;
	}
	for(i = 1; i <= L2; i++)
	{
		F[i][0] =  -i * d ;
		traceback[i][0] =  '|' ;
	}
}


int nw_align(int **F, char **traceback, string seq_1, string seq_2, string& seq_1_al, string& seq_2_al, int d)
{
	int        k = 0, x = 0, y = 0;
	int        fU, fD, fL ;
	char       ptr, nuc ;
	int        i = 0, j = 0;

	const int  a =  2;   // Match
	const int  b = -1;   // Mismatch

	const int  s[4][4] = { { a, b, b, b },    /* substitution matrix */
	{ b, a, b, b },
	{ b, b, a, b },
	{ b, b, b, a } } ;

	int  L1 = seq_1.length();
	int  L2 = seq_2.length();

	for(i = 1; i <= L2; i++)
	{
		for(j = 1; j <= L1; j++)
		{
			nuc = seq_1[j-1] ;

			switch(nuc)
			{
			case 'A':  x = 0 ;  break ;
			case 'C':  x = 1 ;  break ;
			case 'G':  x = 2 ;  break ;
			case 'T':  x = 3 ;
			}

			nuc = seq_2[i-1] ;

			switch(nuc)
			{
			case 'A':  y = 0 ;  break ;
			case 'C':  y = 1 ;  break ;
			case 'G':  y = 2 ;  break ;
			case 'T':  y = 3 ;
			}

			fU = F[i-1][j] - d ;
			fD = F[i-1][j-1] + s[x][y] ;
			fL = F[i][j-1] - d ;

			F[i][j] = max(fU, fD, fL, &ptr) ;

			traceback[i][j] =  ptr ;
		}
	}
	i-- ; j-- ;

	while(i > 0 || j > 0)
	{
		switch(traceback[i][j])
		{
		case '|' :      seq_1_al += '-' ; 
			seq_2_al += seq_2[i-1] ; 
			i-- ;
			break ;

		case '\\':      seq_1_al += seq_1[j-1] ; 
			seq_2_al += seq_2[i-1] ; 
			i-- ;  j-- ;
			break ;

		case '-' :      seq_1_al += seq_1[j-1] ; 
			seq_2_al += '-' ; 
			j-- ;
		}
		k++ ;
	}

	reverse(seq_1_al.begin(), seq_1_al.end());
	reverse(seq_2_al.begin(), seq_2_al.end());

	return  0 ;
}


int max(int f1, int f2, int f3, char * ptr)
{
	int  max = 0 ;

	if(f1 >= f2 && f1 >= f3)  
	{
		max = f1 ;
		*ptr = '|' ;
	}
	else if(f2 > f3)              
	{
		max = f2 ;
		*ptr = '\\' ;
	}
	else
	{
		max = f3 ;
		*ptr = '-' ;
	}

	return  max ;   
}
