local wii = require 'xwiimote'

local path = assert(wii.monitor():poll(), 'No wiimotes found!')
print('Using wiimote at '..path)
local wiiremote = wii.iface(path)

local lgi = require 'lgi'
local	Gtk,		GLib,		Gdk,		GObject,		Gio =
		lgi.Gtk,	lgi.GLib,	lgi.Gdk,	lgi.GObject,	lgi.Gio

local app = Gtk.Application { application_id = 'org.xwiimote.buttondemo' }

local gtkobj, builder

do
	local scriptpath = debug.getinfo(1).source:match("@(.*)$")
	local f = Gio.File.new_for_path(scriptpath):get_parent():get_child('buttons_demo.glade')
	local uipath = f:get_path()
	assert(f:query_exists(), 'UI file `' .. tostring(uipath) ..'` not found')
	print('Loading UI file: ' .. uipath)

	builder = Gtk.Builder()
	assert(builder:add_from_file(uipath))
	gtkobj = builder.objects
end

local source

do
	local fd = wiiremote:get_fd()
	local stream = Gio.UnixInputStream.new(fd, false)
	local source = stream:create_source()
	source:set_callback(function()
		for ev in wiiremote:iter() do
			if ev.key then
				local btn = gtkobj[ev.key]
				if btn then btn.active = (ev.state ~= 0) end
			end
		end
		return true
	end)

	source:attach(lgi.GLib.MainContext.default())
end

assert(wiiremote:open(wii.core))

app.on_activate = function()
	app:add_window(gtkobj.window)

	gtkobj.window:show_all()
end


print()
app:run({arg[0], ...})
