enum {
  KCSV_IDLE = 1,
  KCSV_BLK,
  KCSV_ROW,
};

bool idle( Block *p_block, uint8_t by );

bool block_rcv_init( Block *p_block );
bool block_rcv_byte( Block *p_block, uint8_t by );

void row_rcv_init( Block *p_block );
bool row_rcv_byte( Block *p_block, uint8_t by );
