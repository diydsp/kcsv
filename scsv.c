/*
simple csv reader for test purposes, not mission-hardeened
*/

#include <stdio.h>
#include <string.h>

#define MAX_LEN ( 1000 )

#include "kcsv.h"

int main( int argc, char *argv[] )
{
  FILE *fi = fopen( argv[ 1 ], "r" );
  char buf[ MAX_LEN ];
  char *saveptr;
  char *token;
  
  if( NULL == fi ){ printf("can't open %s\n", argv[ 1 ] ); return( -1 ); }

  while( 1 )
  {
    fgets( buf, MAX_LEN-1, fi );
    if( feof( fi ) ){ break; }
    printf("%s", buf );

    token = strtok_r( buf, ",", &saveptr );
    while( NULL != token )
    {
      printf("%s\n", token);
      token = strtok_r( NULL, ",", &saveptr );
    }

    
  }
   


  return 0;
}
