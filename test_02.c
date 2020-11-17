/*
simple csv reader for test purposes, not mission-hardeened
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEN ( 1000 )
#define MAX_COLS ( 100 )
#include "kcsv.h"

int main( int argc, char *argv[] )
{
  // CSV file
  FILE *fi;
  char buf[ MAX_LEN ];
  char *saveptr;
  char *token;

  // temp storage
  uint8_t ncols;  // number of columns
  float float_val[ MAX_COLS ];

  // Kompressor stuff
  uint8_t ch;
  uint16_t row;
  bool ret_val;
  bool block_initted = false;
  Block block;
  bool block_done;


  // read simple CVS file
  fi = fopen( argv[ 2 ], "r" );
  if( NULL == fi ){ printf("can't open %s\n", argv[ 2 ] ); return( -1 ); }

  // Kompress it
  while( 1 )
  {
    ncols = 0;
    fgets( buf, MAX_LEN-1, fi );
    if( feof( fi ) ){ break; }
    //printf("%s", buf );

    // basic CSV reader
    token = strtok_r( buf, ",", &saveptr );
    while( NULL != token )
    {
      //printf("%s\n", token);
      float_val[ ncols ] = atof( token );
      ncols++;
      token = strtok_r( NULL, ",", &saveptr );
    }

    // init block
    if( false == block_initted )
    {
      block_initted = true;
      ret_val = Block_init( &block, ncols, 10 );
      if( ret_val == false ){ return -1; }
      if( !strcmp( argv[ 1 ], "-b" ) ){ block.mode = BLK_HDR_BINARY;}
      Block_start( &block );
    }

    // put entries into block
    for( ch = 0; ch < block.nch; ch++ )
    {
      Block_set_val( &block, ch, float_val[ ch ] );
    }
      
    // if done, emit kompressed block
    block_done = Block_next_row( &block );
    if( block_done )
    {
      Block_calc( &block );
      if( block.mode == BLK_HDR_ASCII ){ Block_pretty_print( &block ); }
      Block_header_emit( &block );
      
      for( row = 0; row < block.len; row++ )
      {
	Block_row_emit( &block, row );
      }

    }

    
  } // end while(1)
   
  // emit any straggling blocks
  for( row = 0; row < block.row; row++ )
  {
    Block_row_emit( &block, row );
  }


  return 0;
}
