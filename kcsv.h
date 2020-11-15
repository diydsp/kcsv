
#define MAX_CHANNELS ( 30 )

#define BLK_HDR        ( 0xF0 )
#define BLK_HDR_CMD_00 ( 0x00 )
#define ROW_HDR        ( 0xF1 )

// Channel ----------------
typedef struct
{
  float range;
  float min;
  float *p_buffer;  
} Channel;

bool Channel_init( Channel *p_ch, uint16_t len );
void Channel_calc( Channel *p_ch, uint16_t len );


// Block --------------
enum {
  BLK_HDR_ASCII = 1,
  BLK_HDR_BINARY,
};

typedef struct
{
  // mode
  uint8_t mode;
  
  // params
  uint16_t len;
  uint8_t  nch;

  // active part
  Channel *p_channel[ MAX_CHANNELS ];
  uint16_t row;
  
} Block;

bool Block_init( Block *p_block, uint8_t nch, uint16_t len );
void Block_start( Block *p_block );
void Block_set_val( Block *p_block, uint8_t ch, float val );
bool Block_next_row( Block *p_block );
void Block_calc( Block *p_block );
void Block_header_emit( Block *p_block );
void Block_row_emit( Block *p_block, uint16_t row );
