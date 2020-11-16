
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <math.h>

#include "kcsv.h"
#include "unkompress.h"


bool idle( Block *p_block, uint8_t by ){ }

void block_rcv_init( Block *p_block )
{
  p_block->rcv_byte_idx = 0;
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
  
  if( 4 == p_block->rcv_byte_idx ) // rcvd #row, block len
  {
    p_block->len = p_block->rcv_buf[ 1 ] * 256 + p_block->rcv_buf[ 2 ];
    printf("p_block->len: %d, ", p_block->len );
  }

  if( 5 == p_block->rcv_byte_idx ) // rcvd number of channels
  {
    p_block->nch = p_block->rcv_buf[ 3 ];
    printf("p_block->nch: %d, ", p_block->nch );
  }

  // look for final range, offset pair
  buf_end_idx = 5 + 8 * p_block->nch;
  if( p_block->rcv_byte_idx == buf_end_idx )
  {
    for( ch = 0; ch < p_block->nch; ch++ )
    {
      bin2float( &p_block->rcv_buf[ 5 + 8 * ch     ], &range );
      bin2float( &p_block->rcv_buf[ 5 + 8 * ch + 4 ], &min   );
      printf( "range, min: %f, %f, ", range, min );
    }
    ret_val = true;  // done
  }
  
  return ret_val;
}

void row_rcv_init( Block *p_block )
{
  
}

bool row_rcv_byte( Block *p_block, uint8_t by )
{
  bool ret_val = false;
  return ret_val;
}
  

int main( int argc, char *argv[] )
{
  uint8_t kcsv_state_cur  = KCSV_IDLE;
  uint8_t kcsv_state_next = KCSV_IDLE;
  bool (*process_byte)( Block*, uint8_t ) = idle;
  Block block;
  uint8_t ch;
  
  while( 1 )
  {
    ch = getchar();

    switch( kcsv_state_cur )
    {
    case BLK_HDR:
      kcsv_state_next = KCSV_BLK;
      process_byte = block_rcv_byte;
      block_rcv_init( &block );
      break;
    case ROW_HDR:
      kcsv_state_next = KCSV_ROW;
      process_byte = row_rcv_byte;
      row_rcv_init( &block );
      break;
    }

    
    
  }
  

  return 0;
}
