
#include <vector>
#include <cstdint>
 
class Channel
{
  float scale;
  float offset;
  float buffer[];
  
 public:
  Channel( uint16_t len )
  {
    buffer = new float( len );
  }
  
};

class Block
{
 public:
  Block( uint8_t nch, uint16_t block_len )
  {
    m_nch       = nch;
    m_block_len = block_len;
    
  }

 private:
  uint16_t m_block_len;
  uint8_t m_nch;
  std::vector<Channel> channel;
};
