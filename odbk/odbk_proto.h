#pragma once

#include <vector>

namespace odbk
{
enum class message_type
{
  info  = 0,
  warn  = 1,
  error = 2,
};

enum class code_state
{
  normal      = 0, // The code is normal.
  unavailable = 1, // The code for the script was not available.
  binary      = 2, // The code was loaded as a binary/compiled file
};

enum class event_type
{
  initialize  = 11, // Sent when the backend is ready to have its initialize function called
  create_vm   = 1,  // Sent when a script VM is created.
  destroy_vm  = 2,  // Sent when a script VM is destroyed.
  load_script = 3,  // Sent when script data is loaded into the VM.
  hit_bp      = 4,  // Sent when the debugger breaks on a line.
  // event_set_bp = 5,  // Sent when a breakpoint has been added in the debugger.
  exception   = 6, // Sent when the script encounters an exception (e.g. crash).
  load_error  = 7, // Sent when there is an error loading a script (e.g. syntax error).
  message     = 9, // Event containing a string message from the debugger.
  session_end = 8, // This is used internally and shouldn't be sent.
  // event_name_vm        = 10, // Sent when the name of a VM is set.
};

enum class command
{
  execute,  // Continues execution until the next break point.
  step_over,  // Steps to the next line, not entering any functions.
  step_into,  // Steps to the next line, entering any functions.
  run_to_cursor, // Ctrl + F10
  bp,  // set/unset/,  a breakpoint on a line on and off. 
  clear_bps, // Instructs the backend to clear all breakpoints set
  interrupt,  // Instructs the debugger to break on the next line of script code.
  evaluate,  // Evaluates the value of an expression in the current context.
  detach,  // Detaches the debugger from the process.
  patch_replace_line,  // Replaces a line of code with a new line.
  patch_insert_line, // Adds a new line of code.
  patch_delete_line, // Deletes a line of code.
  load_done, // Signals to the backend that the frontend has finished processing a load.
  ignore_exception// Instructs the backend to ignore the specified exception message in the future.
};

std::vector<char> odbk_proto_create_event_init();
} // namespace odbk
