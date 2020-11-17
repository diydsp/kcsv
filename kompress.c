
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <math.h>  // only used for test sig gen

#include "kcsv.h"


void gen_sigs( uint8_t num_test_chans, float *p_out )
{
  // test signal parameters
  //float Fs = 1000.0f; // sample frequency, Hz
  float Fs = 12.0f; // sample frequency, Hz

  //float freq [ MAX_CHANNELS ] = { 5.0, 5.0, 5.0, 5.0 };
  //float mag  [ MAX_CHANNELS ] = { 1.0,2.0,3.0,4.0 };
  float freq [ MAX_CHANNELS ] = { 0.3, 0.01, 0.03, 8.0 };
  float mag  [ MAX_CHANNELS ] = { 1.0,1.0,1.0,1.0 };
  static float phase[ MAX_CHANNELS ] = { 0,0,0,0};

  uint8_t idx;
  for( idx = 0; idx < num_test_chans; idx++ )
  {
    phase[ idx ]  += freq[ idx ] * 2 * M_PI / Fs;
    p_out[ idx ] = mag[ idx ] * sin( phase[ idx ] );
  }
  
}

int main( int argc, char *argv[] )
{
  Block block;
  bool ret_val;
  bool block_done;
  
  uint8_t idx;
  float out[ MAX_CHANNELS ];

  uint8_t num_test_chans = 4;
  uint32_t tr_idx, num_test_rows = 1000;

  ret_val = Block_init( &block, num_test_chans, 10 );
  if( ret_val == false ){ return -1; }

  if( 2 == argc )
  {
    if( !strcmp( argv[ 1 ], "-b" ) ){ block.mode = BLK_HDR_BINARY; }
  }
  
  Block_start( &block );

  // do a number of test blocks
  for( tr_idx = 0; tr_idx < num_test_rows; tr_idx++ )
  {
    gen_sigs( num_test_chans, &out[ 0 ] ); // synthesize one row of test signals

    // feed signals to kompressor
    for( idx = 0; idx < num_test_chans; idx++ )
    {
      Block_set_val( &block, idx, out[ idx ] );
    }
    
    // if done, emit kompressed block
    block_done = Block_next_row( &block );
    if( block_done )
    {
      Block_calc( &block );
      if( block.mode == BLK_HDR_ASCII ){ Block_pretty_print( &block ); }
      Block_header_emit( &block );
      //exit(0);
      
      for( idx = 0; idx < block.len; idx++ )
      {
	Block_row_emit( &block, idx );
      }

    }
  }
  


  return 0;
}
