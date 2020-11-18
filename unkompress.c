
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <math.h>

#include "kcsv.h"
#include "unkompress.h"
  

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
