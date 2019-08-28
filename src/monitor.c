#include "luawii.h"

static const char LUAWII_MONITOR_MT[] = "xwiimote monitor";

static struct xwii_monitor* luawii_monitor_check(lua_State* L) {
	struct xwii_monitor** mon = luaL_checkudata(L, 1, LUAWII_MONITOR_MT);
	if (*mon) return *mon;
	else luaL_error(L, "accessing invalid monitor object");
};

static int luawii_monitor_gc(lua_State* L) {
	struct xwii_monitor** mon = luaL_checkudata(L, 1, LUAWII_MONITOR_MT);

	if (*mon) {
		xwii_monitor_unref(*mon);
		*mon = NULL;
	}
};

static int luawii_monitor_set_blocking(lua_State* L) {
	struct xwii_monitor* mon = luawii_monitor_check(L);

	int res = xwii_monitor_get_fd(mon, lua_toboolean(L, 2));
	lua_pushboolean(L, res != -1);
	lua_pushinteger(L, res);
	return 2;
};

static int luawii_monitor_poll(lua_State* L) {
	struct xwii_monitor* mon = luawii_monitor_check(L);

	char* path = xwii_monitor_poll(mon);
	if (!path) return 0;

	lua_pushstring(L, path);
	free(path);
	return 1;
};

static int luawii_monitor_iter(lua_State* L) {
	luawii_monitor_check(L);

	lua_getfield(L, 1, "poll");
	lua_pushvalue(L, 1);

	return 2;
};

const struct luaL_Reg luawii_monitor_methods[] = {
	{ "set_blocking",	luawii_monitor_set_blocking },
	{ "poll", 			luawii_monitor_poll },
	{ "iter", 			luawii_monitor_iter },
	{NULL, NULL}
};

const struct luaL_Reg luawii_monitor_metamethods[] = {
	{ "__gc",		luawii_monitor_gc },
	{ "__close",	luawii_monitor_gc },
	{NULL, NULL}
};

int luawii_monitor_new(lua_State* L) {
	bool poll = lua_toboolean(L, 1);
	bool direct = lua_toboolean(L, 2);
	struct xwii_monitor** mon = lua_newuserdata(L, sizeof(struct xwii_monitor*));
	*mon = NULL;

	if (luaL_newmetatable(L, LUAWII_MONITOR_MT)) {
		luaL_setfuncs(L, luawii_monitor_metamethods, 0);
		luaL_newlib(L, luawii_monitor_methods);
		lua_setfield(L, -2, "__index");
	};

	lua_setmetatable(L, -2);

	*mon = xwii_monitor_new(poll, direct);
	return !!*mon;
};

