
/* Kompressed CSV (comma sep value) file
   Compressor/Decompressor
   Accumulates blocks of data,
   Finds mean of min and for each channel
   re-transmits 8-bit codes instead of 32-bit floats or 8-character ascii strings
   codes in range of 0x00 - 0xEF (0-239)

   Embedded control codes:
   F0 Block Header
   F1 Row Header

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "kcsv.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

bool Channel_init( Channel *p_ch, uint16_t len )
{
  bool ret_val = true;
  p_ch->p_buffer = (float *) malloc( len * sizeof( float ) );
  if( p_ch->p_buffer == NULL ){ ret_val = false; }
  p_ch->min = 0;
  p_ch->range = 0;
  return ret_val;
}


void Channel_calc( Channel *p_ch, uint16_t len )
{
  uint16_t row;
  float min = p_ch->p_buffer[ 0 ];
  float max = min;

  for( row = 0; row < len; row++ )
  {
    min = MIN( p_ch->p_buffer[ row ] , min );
    max = MAX( p_ch->p_buffer[ row ] , max );
  }
  p_ch->min = min;
  p_ch->range = max - min;
}

bool Block_init( Block *p_block, uint8_t nch, uint16_t len )
{
  p_block->nch = nch;
  p_block->len = len;
  uint8_t idx;
  bool ret_val = true;
  bool ret_val_local = true;
  
  for( idx = 0; idx < len; idx ++)
  {
    p_block->p_channel[ idx ] = (Channel *) malloc( sizeof( Channel ) );

    ret_val_local = Channel_init( p_block->p_channel[ idx ], len );
    if( ret_val_local == false ){ ret_val = false; break; }
  }

  return ret_val;
}

void Block_start( Block *p_block )
{
  p_block->row = 0;
}

void Block_set_val( Block *p_block, uint8_t ch, float val )
{
  uint16_t row = p_block->row;
  p_block->p_channel[ ch ]->p_buffer[ row ] = val;
}

bool Block_next_row( Block *p_block )
{
  bool ret_val = false;

  p_block->row++;
  if( p_block->row >= p_block->len )
  {
    ret_val = true;
    p_block->row = 0;
  }

  return ret_val;
}

void Block_calc( Block *p_block )
{
  uint8_t ch;

  // calc range and min for each channel
  for( ch = 0; ch < p_block->nch; ch++ )
  {
    Channel_calc( p_block->p_channel[ ch ], p_block->len );
  }

}

void Block_header_emit( Block *p_block )
{
  uint8_t ch;

  printf("F0 ");
  printf("00 ");
  printf("%04x ",p_block->len );
  printf("%02x ",p_block->nch );
  for( ch = 0; ch < p_block->nch; ch++ )
  {
    printf("%f, ", p_block->p_channel[ ch ]->min );
    printf("%f, ", p_block->p_channel[ ch ]->range );
  }
  printf("\n");
  
}

// e.g. min = 10, max = 20 -> range = 10
// val = 18 -> kval = (18-10)/10 = 0.8;

void Block_row_emit( Block *p_block, uint16_t row )
{
  uint8_t ch;
  float val;
  float val2;
  float kval;
  uint8_t kval_uint;
  Channel *p_ch;
  
  printf( "F1 ");
  for( ch = 0; ch < p_block->nch; ch++ )
  {
    p_ch = p_block->p_channel[ ch ]; 
    val  = p_ch->p_buffer[ row ];
    val2 = ( val - p_ch->min ) / p_ch->range;
    kval = 239.4999f * val2;
    kval_uint = kval;
    printf("%02x", kval_uint );
  }
  printf( "\n" );
  
}
