#include "odbk_lload.h"
#include "odbk_lhook.h"
#include "odbk_core.h"
#include "odbk_proto.h"
#include "yasio/ibstream.h"
// #include <Windows.h>

odbk_core::~odbk_core() {}

void odbk_core::init(void *hInst)
{
  app_inst_    = hInst;
  vfd_         = nullptr;
  app_module_  = nullptr; // GetModuleHandleW(nullptr);
  app_process_ = nullptr; // GetCurrentProcess();

  io_hostent ep("0.0.0.0", 'odbk' % 65536); // 25195
  service_.start_service(&ep, [=](event_ptr e) {
    auto k = e->kind();
    if (k == YEK_PACKET)
    {
      ibstream ibs(std::move(e->packet()));
      ibs.seek(sizeof(int), SEEK_CUR); // skip length field
      uint32_t proto_version = ibs.read_i<uint32_t>();
      // TODO: check version?
      uint8_t cmd = ibs.read_i<uint8_t>(); // cmd
      // call different handler with different cmd
    }
    else if (k == YEK_CONNECT_RESPONSE)
    {
      if (e->status() == 0)
      {
        printf("Accept frontend succeed."); // debug session estiblished with socket.
        this->vfd_ = e->transport();
      }
      else
      {
        printf("Accept frontend exception: %d", e->status());
      }
    }
    else if (k == YEK_CONNECTION_LOST)
    {
      printf("The connection with frontend is lost: %d, waiting new to income...", e->status());
    }
  });
  service_.open(0, YCM_TCP_SERVER);
}

void odbk_core::print(int type, const std::string &msg) { send(odbk::odbkp_create_msg(type, msg)); }

void odbk_core::send(std::vector<char> data) { service_.write(vfd_, data); }

extern "C" {
ODBK_API void odbk_install(void *hModule)
{
  odbk_load_lua(hModule);
  odbk_install_lua_hooks();
}
}
