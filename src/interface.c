#include "luawii.h"

// For `strerror()`
#include <string.h>

static const char LUAWII_IFACE_MT[] = "xwiimote interface";

static struct xwii_iface* luawii_iface_check(lua_State* L) {
	struct xwii_iface** iface = luaL_checkudata(L, 1, LUAWII_IFACE_MT);
	if (*iface) return *iface;
	else luaL_error(L, "accessing invalid interface object");
};

static int luawii_iface_handle_result(lua_State* L, int result) {
	if (result == 0) {
		lua_pushboolean(L, true);
		return 1;
	} else {
		lua_pushnil(L);
		lua_pushstring(L, strerror(-result));
		return 2;
	}
};

static enum xwii_led luawii_iface_check_led(lua_State* L, lua_Integer led) {
	enum xwii_led id = XWII_LED(led);
	if (id < XWII_LED1 || id > XWII_LED4) {
		return luaL_error(L, "number %I isn't valid LED index", led);
	}
	return id;
}

static int luawii_iface_gc(lua_State* L) {
	struct xwii_iface** iface = luaL_checkudata(L, 1, LUAWII_IFACE_MT);

	if (*iface) {
		xwii_iface_unref(*iface);
		*iface = NULL;
	}
};

static int luawii_iface_get_fd(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	// No validation required... once in millenia...
	lua_pushinteger(L, xwii_iface_get_fd(iface));
	return 1;
};

static int luawii_iface_watch(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	int result = xwii_iface_watch(iface, lua_toboolean(L, 2));
	return luawii_iface_handle_result(L, result);
};

static int luawii_iface_open(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	int result = xwii_iface_open(iface, luaL_checkinteger(L, 2));
	return luawii_iface_handle_result(L, result);
};

static int luawii_iface_close(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	xwii_iface_close(iface, luaL_checkinteger(L, 2));
	// This never fails
	return 0;
};

static int luawii_iface_opened(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	lua_pushinteger(L, xwii_iface_opened(iface));
	return 1;
};

static int luawii_iface_available(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	lua_pushinteger(L, xwii_iface_available(iface));
	return 1;
};

static int luawii_iface_dispatch(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	struct xwii_event ev;

	int result = xwii_iface_dispatch(iface, &ev, sizeof(ev));
	if (result == 0) {
		// If conversion failed, return `true`
		if (!luawii_event_push(L, &ev)) lua_pushboolean(L, true);
		return 1;
	} else if (result == -EAGAIN) {
		// No events to dispatch at the moment
		return 0;
	} else return luawii_iface_handle_result(L, result);
};

static int luawii_iface_iter(lua_State* L) {
	luawii_iface_check(L);

	lua_getfield(L, 1, "poll");
	lua_pushvalue(L, 1);

	return 2;
};

static int luawii_iface_rumble(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	int result = xwii_iface_rumble(iface, lua_toboolean(L, 2));
	return luawii_iface_handle_result(L, result);
};

static int luawii_iface_get_led(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);
	enum xwii_led led = luawii_iface_check_led(L, luaL_checkinteger(L, 2));

	bool output = false;

	int result = xwii_iface_get_led(iface, led, &output);
	if (result == 0) {
		lua_pushboolean(L, output);
		return 1;
	} else return luawii_iface_handle_result(L, result);
};

static int luawii_iface_set_led(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);
	enum xwii_led led = luawii_iface_check_led(L, luaL_checkinteger(L, 2));

	int result = xwii_iface_set_led(iface, led, lua_toboolean(L, 3));
	return luawii_iface_handle_result(L, result);
};

static int luawii_iface_get_battery(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	uint8_t output = 0;

	int result = xwii_iface_get_battery(iface, &output);
	if (result == 0) {
		lua_pushinteger(L, output);
		return 1;
	} else return luawii_iface_handle_result(L, result);
};

static int luawii_iface_get_devtype(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	char* output = NULL;

	int result = xwii_iface_get_devtype(iface, &output);
	if (result == 0) {
		lua_pushstring(L, output);
		free(output);
		return 1;
	} else return luawii_iface_handle_result(L, result);
};

static int luawii_iface_get_extension(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	char* output = NULL;

	int result = xwii_iface_get_extension(iface, &output);
	if (result == 0) {
		lua_pushstring(L, output);
		free(output);
		return 1;
	} else return luawii_iface_handle_result(L, result);
};

static int luawii_iface_set_mp_normalization(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	xwii_iface_set_mp_normalization(iface,
		luaL_checkinteger(L, 2),
		luaL_checkinteger(L, 3),
		luaL_checkinteger(L, 4),
		luaL_checkinteger(L, 5)
	);

	return 0;
};

static int luawii_iface_get_mp_normalization(lua_State* L) {
	struct xwii_iface* iface = luawii_iface_check(L);

	int32_t x, y, z, factor;

	xwii_iface_get_mp_normalization(iface,
		&x,
		&y,
		&z,
		&factor
	);

	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	lua_pushinteger(L, z);
	lua_pushinteger(L, factor);

	return 4;
};

const struct luaL_Reg luawii_iface_methods[] = {
	{ "get_fd",					luawii_iface_get_fd },
	{ "watch",					luawii_iface_watch },
	{ "open",					luawii_iface_open },
	{ "close",					luawii_iface_close },
	{ "available",				luawii_iface_available },

	{ "poll",					luawii_iface_dispatch },
	{ "dispatch",				luawii_iface_dispatch },
	{ "iter",					luawii_iface_iter },

	{ "rumble",					luawii_iface_rumble },
	{ "get_led",				luawii_iface_get_led },
	{ "set_led",				luawii_iface_set_led },
	{ "get_battery",			luawii_iface_get_battery },
	{ "get_devtype",			luawii_iface_get_devtype },
	{ "get_extension",			luawii_iface_get_extension },

	{ "set_mp_normalization",	luawii_iface_set_mp_normalization },
	{ "get_mp_normalization",	luawii_iface_get_mp_normalization },
	{NULL, NULL}
};

const struct luaL_Reg luawii_iface_metamethods[] = {
	{ "__gc",		luawii_iface_gc },
	{ "__close",	luawii_iface_gc },
	{NULL, NULL}
};

int luawii_iface_new(lua_State* L) {
	const char* syspath = luaL_checkstring(L, 1);

	struct xwii_iface** iface = lua_newuserdata(L, sizeof(struct xwii_iface*));
	*iface = NULL;

	if (luaL_newmetatable(L, LUAWII_IFACE_MT)) {
		luaL_setfuncs(L, luawii_iface_metamethods, 0);
		luaL_newlib(L, luawii_iface_methods);
		lua_setfield(L, -2, "__index");
	};

	lua_setmetatable(L, -2);

	int result = xwii_iface_new(iface, syspath);
	if (result == 0) return 1;

	return luaL_error(L, "unable to create interface: %s", strerror(-result));
};

