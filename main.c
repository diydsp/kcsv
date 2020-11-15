
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <math.h>

#include "kcsv.h"


void gen_sigs( uint8_t num_test_chans, float *p_out )
{
  // test signal parameters
  float Fs = 1000.0f; // sample frequency, Hz

  float freq [ MAX_CHANNELS ] = { 5.0, 6.0, 7.0, 8.0 };
  static float phase[ MAX_CHANNELS ] = { 0,0,0,0};
         float mag  [ MAX_CHANNELS ] = { 1.0,2.0,3.0,4.0 };

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
  uint32_t tr_idx, num_test_rows = 100;

  ret_val = Block_init( &block, num_test_chans, 10 );
  if( ret_val == false ){ return -1; }
  
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
      Block_header_emit( &block );
      for( idx = 0; idx < block.nch; idx++ )
      {
	Block_row_emit( &block, idx );
      }

    }
  }
  


  return 0;
}
