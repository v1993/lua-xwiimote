-- Got by testing real WiiRemote

-- [-409; 410]
local ACCEL_MAX = 410
-- Cant say for sure but looks this way
local MPLUS_MAX = 335160

local test_mplus = ...

local MAX_VAL
if test_mplus then
	MAX_VAL = MPLUS_MAX
else
	MAX_VAL = ACCEL_MAX
end

local colorEmpty = {1, 1, 1}
local colorLine = {0, 0, 0}

local wii = require 'xwiimote'

local path = assert(wii.monitor():poll(), 'No wiimotes found!')
print('Using wiimote at '..path)
local wiiremote = wii.iface(path)

local lgi = require 'lgi'
local	Gtk,		GLib,		Gdk,		GObject,		Gio =
		lgi.Gtk,	lgi.GLib,	lgi.Gdk,	lgi.GObject,	lgi.Gio

local app = Gtk.Application { application_id = 'org.xwiimote.acceldemo' }

local gtkobj, builder

do
	local scriptpath = debug.getinfo(1).source:match("@(.*)$")
	local f = Gio.File.new_for_path(scriptpath):get_parent():get_child('area_demo.glade')
	local uipath = f:get_path()
	assert(f:query_exists(), 'UI file `' .. tostring(uipath) ..'` not found')
	print('Loading UI file: ' .. uipath)

	builder = Gtk.Builder()
	assert(builder:add_from_file(uipath))
	gtkobj = builder.objects
end

local source

local data3d = {x = 0, y = 0, z = 0}

do
	local fd = wiiremote:get_fd()
	local stream = Gio.UnixInputStream.new(fd, false)
	local source = stream:create_source()
	source:set_callback(function()
		for ev in wiiremote:iter() do
			if ev.accel then
				data3d = ev.accel
			elseif ev.mplus then
				data3d = ev.mplus
			elseif ev.watch then
				print('Wiimote disconnected, exiting')
				gtkobj.window:destroy()
			end
		end
		return true
	end)

	source:attach(lgi.GLib.MainContext.default())
end

local function areaSize()
	return gtkobj.area:get_allocated_width(), gtkobj.area:get_allocated_height()
end

local max, min = 0, 0

local functions = {
	draw = function(self, cr)
		max = math.max(max, data3d.x, data3d.y, data3d.z)
		min = math.min(min, data3d.x, data3d.y, data3d.z)
		local x, y = areaSize()
		local cx, cy = x/2, y/2
		cr:set_source_rgb(table.unpack(colorEmpty))
		cr:rectangle(0, 0, x, y)
		cr:fill()

		cr:set_source_rgb(table.unpack(colorLine))
		cr:set_line_width(5);

		-- X axis
		cr:move_to(cx,cy)
		cr:line_to(cx+data3d.x*cx/MAX_VAL,cy)
		cr:stroke()

		-- Z axis (as Y because it lays this way)
		cr:move_to(cx,cy)
		cr:line_to(cx,cy+data3d.z*cy/MAX_VAL)
		cr:stroke()

		-- Y axis (as Z because it lays this way)
		cr:move_to(cx,cy)
		cr:line_to(cx+data3d.y*cx/MAX_VAL,cy-data3d.y*cy/MAX_VAL)
		cr:stroke()

		gtkobj.area:queue_draw_area(0, 0, areaSize())
		collectgarbage() -- X11 (not lua!) eat memory without this. Why?
	end;
}

builder:connect_signals(functions)

if test_mplus then
	print('Testing MotionPlus')
	assert(wiiremote:open(wii.mplus))
	wiiremote:set_mp_normalization(0,0,0,1)
else
	print('Testing accelerometer')
	assert(wiiremote:open(wii.accel))
end

app.on_activate = function()
	app:add_window(gtkobj.window)

	gtkobj.window:show_all()
end

app:run({arg[0]})

print('max, min')
print(max, min)

if test_mplus then
	print('Calibration values:', wiiremote:get_mp_normalization())
end
