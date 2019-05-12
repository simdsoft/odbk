#include "odbk_proto.h"
#include "yasio/obstream.h"
#define ODBKP_VERSION 0x00100
namespace odbk
{
std::vector<char> odbkp_create_msg(int type, const std::string &msg)
{
  yasio::obstream obs;
  obs.push32();
  obs.write_i(ODBKP_VERSION);
  obs.write_i((uint8_t)proto_code::EV_MESSAGE);
  obs.write_va(msg);
  return std::move(obs.buffer());
}
} // namespace odbk
