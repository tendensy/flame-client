#pragma once

#include <jni.h>
#include <typeinfo>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <thread>
#include <malloc.h>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdarg>
#include <iterator>
#include <set>
#include <cstdint>
#include <algorithm>
#include <random>
#include <iomanip>
#include <memory>
#include <functional>
#include <map>
#include <array>
#include <sys/mman.h>
#include <unistd.h>
#include <unordered_map>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CLIENT_VER	    "1.5.0"
#define UPDATE_DATE	"08.05.2021"
#define SAMP_VERSION	"0.3.7"
#define UPDATE_INFO 	"Исправление вылетов\nФикс бага - увеличение GUI элементов\nФикс чата"
#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24

#define col_all 0xCE2029
#define col_all1 0xCE
#define col_all2 0x20
#define col_all3 0x29

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969

#include "vendor/raknet/SAMP/samp_netencr.h"
#include "vendor/raknet/SAMP/SAMPRPC.h"
#include "util/str_obfuscator.hpp"
#include "util/util.h"
#include "libbass.h"

extern uintptr_t g_libGTASA;
extern char* g_pszStorage;

class CMain
{
public:
    static JavaVM* javaVM;
    static char* BaseStorage;
    static char* RootStorage;

public:
    CMain(/* args */);

    static void InitStuff();
    static void Terminate();
    static void InitSAMP();
    static void InitInMenu();
    static void InitInGame();

    static JavaVM* GetVM() { return javaVM; }
    static void SetVM( JavaVM* vm) { javaVM = vm; }

    static void SetBaseStorage(char* storage) { BaseStorage = storage; }
    static void SetRootStorage(char* storage) { RootStorage = storage; }
    static char* GetBaseStorage() { return BaseStorage; }
    static char* GetRootStorage() { return RootStorage; }
};
