
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <math.h>

#include "kcsv.h"
#include "unkompress.h"


bool idle( Block *p_block, uint8_t by ){ }

bool block_rcv_init( Block *p_block )
{
  uint8_t ch;
  bool ret_val = true;
  
  // allocate channel descriptors
  for( ch = 0; ch < p_block->nch; ch ++)
  {
    p_block->p_channel[ ch ] = (Channel *) malloc( sizeof( Channel ) );
    if( p_block->p_channel[ ch ] == NULL ){ ret_val = false; break; }
  }

  // prepare to receive
  p_block->rcv_byte_idx = 0;

  printf("\n");
  return ret_val;
}

// end after [F0][00][rows1][rows0][# ch][range0-3][offset0-3][range0-3][offset0-3]
//            0   1   2      3      4     5         13         21
bool block_rcv_byte( Block *p_block, uint8_t by )
{
  bool ret_val = false;
  int32_t buf_end_idx;
  uint8_t ch;
  float range, min;

  // add the byte to buf
  p_block->rcv_buf[ p_block->rcv_byte_idx ] = by;
  p_block->rcv_byte_idx++;

  // buf[ 0 ] = 0xF0, default cmd, can ignore for now
  // buf[ 1 ] = 0x00, default cmd, can ignore for now
  if( 2 == p_block->rcv_byte_idx )
  {
    printf("Block header\n");
  }

  
  if( 4 == p_block->rcv_byte_idx ) // rcvd block len
  {
    p_block->len = p_block->rcv_buf[ 2 ] * 256 + p_block->rcv_buf[ 3 ];
    printf("p_block->len: %d\n", p_block->len );
  }

  if( 5 == p_block->rcv_byte_idx ) // rcvd number of channels
  {
    p_block->nch = p_block->rcv_buf[ 4 ];
    printf("p_block->nch: %d\n", p_block->nch );
  }

  // look for final range, offset pair
  buf_end_idx = 5 + 8 * p_block->nch;
  if( p_block->rcv_byte_idx >= buf_end_idx )
  {
    for( ch = 0; ch < p_block->nch; ch++ )
    {
      bin2float( &p_block->rcv_buf[ 5 + 8 * ch     ], &range );
      bin2float( &p_block->rcv_buf[ 5 + 8 * ch + 4 ], &min   );
      printf( "\nrange, min: %f, %f. ", range, min );
    }
    ret_val = true;  // done
  }
  
  return ret_val;
}

void row_rcv_init( Block *p_block )
{
  p_block->rcv_byte_idx = 0;
  printf("\n");
}

float decode_byte( Channel *p_ch, uint8_t by )
{
  float fp_val;

  //fp_val = p_ch->min + p_ch->range * (float)by / (float)0xEF;  // EF is max val
  fp_val = p_ch->min + (float)by / (float)0xEF;  // EF is max val

  return fp_val;
}


// byte      [f1][byte0][byte1][byte2][byte3]...[byte nch-1]
// rcv_idx:  0    1     2      3       4     ... nch+1
bool row_rcv_byte( Block *p_block, uint8_t by )
{
  bool     ret_val = false;
  int32_t  buf_end_idx;
  uint8_t  ch;
  float    range, min;
  Channel *p_chan;
  float    decoded;
  
  // add the byte to buf
  p_block->rcv_buf[ p_block->rcv_byte_idx ] = by;
  p_block->rcv_byte_idx++;

  
  // look for final byte
  buf_end_idx = 1 + p_block->nch;
  if( p_block->rcv_byte_idx >= buf_end_idx )
  {
    for( ch = 0; ch < p_block->nch; ch++ )
    {
      p_chan = p_block->p_channel[ ch ];
      decoded = decode_byte( p_chan, p_block->rcv_buf[ 1 + ch ] );
      printf("%f, ", decoded );
    }
    printf("row done\n ");
  }
  
  return ret_val;
}
  

int main( int argc, char *argv[] )
{
  uint8_t kcsv_state_cur  = KCSV_IDLE;
  uint8_t kcsv_state_next = KCSV_IDLE;
  bool (*process_byte)( Block*, uint8_t ) = idle;
  Block block;
  uint8_t by;
  bool escaped = false;
  bool ret_val_local = true;
  int ret_val = 0;
  while( 1 )
  {
    by = getchar();

    printf("%02x,",by);

    if( escaped == false )
    {
      if( by == 0xFF )
      {
	escaped = true;
      } else
      {
	// look for Fx commands
	switch( by )
	{
	case BLK_HDR:  // F0
	  kcsv_state_next = KCSV_BLK;
	  process_byte = block_rcv_byte;
	  ret_val_local = block_rcv_init( &block );
	  if( ret_val_local == false ){ return( -1 ); }
	break;
	case ROW_HDR:  // F1
	  kcsv_state_next = KCSV_ROW;
	  process_byte = row_rcv_byte;
	  row_rcv_init( &block );
	break;
	}
	kcsv_state_cur = kcsv_state_next;
	process_byte( &block, by ); // run current interpreter
      }
    }
    else
    {
      process_byte( &block, by ); // run current interpreter
      escaped = false;
    }
  }

  return 0;
}
