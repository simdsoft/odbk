#pragma once

#include <vector>
#include <string>

namespace odbk
{
enum class proto_code : unsigned char
{
  EV_INIT = 1,
  EV_CREATE_VM,
  EV_DESTROY_VM,
  EV_LOAD_SCRIPT,
  EV_HIT_BP,
  EV_EXCEPTION,
  EV_MESSAGE,
  CMD_CONTINUE, // F5
  CMD_STEP_OVER, // F10
  CMD_STEP_INTO, // F11
  CMD_RET,           // Shift + F11
  CMD_RUN_TO_CURSOR, // Ctrl + F10
  CMD_BP, // 
  CMD_CLEAR_BP, // Clear all bp
  CMD_INTERRUPT,
  CMD_EVAL, // reponse with a same code: frontend-->backend, backend-->frontend
  CMD_LOAD_DONE,
  CMD_IGNORE_EXCEPTION,
  CMD_SET_OPTIONS,
};

std::vector<char> odbkp_create_msg(int type, const std::string &msg);
} // namespace odbk
