#pragma once

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <xwiimote.h>

int luawii_monitor_new(lua_State* L);
int luawii_iface_new(lua_State* L);
bool luawii_event_push(lua_State* L, const struct xwii_event* ev);
