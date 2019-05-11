#pragma once

// Initialize the MinHook library. You must call this function EXACTLY ONCE
// at the beginning of your program.
void odbk_hook_init();

// Uninitialize the MinHook library. You must call this function EXACTLY
// ONCE at the end of your program.
void odbk_hook_uninit(); 

void* odbk_hook(void **ppOriginal, void *pDetour);
void odbk_unhook(void *pTarget);

void odbk_enable_hook(void* pTarget = nullptr);
void odbk_disable_hook(void *pTarget = nullptr);
