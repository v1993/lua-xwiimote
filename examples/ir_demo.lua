-- Got by testing real WiiRemote
local IR_X_MAX = 1016
local IR_Y_MAX = 760

local dotSize = 20
local colorEmpty = {1, 1, 1}
local colorDot = {1, 0, 0}

local wii = require 'xwiimote'

local path = assert(wii.monitor():poll(), 'No wiimotes found!')
print('Using wiimote at '..path)
local wiiremote = wii.iface(path)

local lgi = require 'lgi'
local	Gtk,		GLib,		Gdk,		GObject,		Gio =
		lgi.Gtk,	lgi.GLib,	lgi.Gdk,	lgi.GObject,	lgi.Gio

local app = Gtk.Application { application_id = 'org.xwiimote.irdemo' }

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

local irNew = {}

do
	local fd = wiiremote:get_fd()
	local stream = Gio.UnixInputStream.new(fd, false)
	local source = stream:create_source()
	source:set_callback(function()
		for ev in wiiremote:iter() do
			if ev.ir then
				irNew = ev.ir
			end
		end
		return true
	end)

	source:attach(lgi.GLib.MainContext.default())
end

local function areaSize()
	return gtkobj.area:get_allocated_width(), gtkobj.area:get_allocated_height()
end

local function dotCoords(x, y)
	local w, h = areaSize()

	local xt, yt = w*x/IR_X_MAX, h*y/IR_Y_MAX
	return xt-dotSize/2, yt-dotSize/2, dotSize, dotSize
end

local function dot_draw(cr, x, y)
	cr:rectangle(dotCoords(x, y))
	cr:fill()
end

local xmax, ymax = 0/0, 0/0
local xmin, ymin = 0/0, 0/0

local functions = {
	draw = function(self, cr)
		cr:set_source_rgb(table.unpack(colorEmpty))
		cr:rectangle(0, 0, areaSize())
		cr:fill()

		cr:set_source_rgb(table.unpack(colorDot))
		for k,v in ipairs(irNew) do
			xmax, ymax = math.max(v.x, xmax), math.max(v.y, ymax)
			xmin, ymin = math.min(v.x, xmin), math.min(v.y, ymin)
			dot_draw(cr, v.x, v.y)
		end

		gtkobj.area:queue_draw_area(0, 0, areaSize())
		collectgarbage() -- X11 (not lua!) eat memory without this. Why?
	end;
}

builder:connect_signals(functions)

assert(wiiremote:open(wii.ir))

app.on_activate = function()
	app:add_window(gtkobj.window)

	gtkobj.window:show_all()
end

app:run({arg[0], ...})

-- Carefully calibrated values:
-- 1016		760			1		1

print('max x, max y, min x, min y')
print(xmax, ymax, xmin, ymin)
