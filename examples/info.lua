local wii = require 'xwiimote'

local mon = wii.monitor()
print('Connected wiimotes:')
for i=1,math.maxinteger or math.huge do
	local path = mon:poll()
	if not path then break end
	print(('WiiRemote #%d, path %s:'):format(i, path))

	local res, wiimote = pcall(wii.iface, path);
	if res then
		print(('\tDevice type: %s'):format(wiimote:get_devtype()))
		print(('\tConnected extension: %s'):format(wiimote:get_extension()))

		print(('\tBattery: %d%%'):format(wiimote:get_battery()))

		io.write('\tLed status: ')
		for i = 1, 4 do
			io.write(wiimote:get_led(i) and '#' or '.')
		end
		io.write('\n')
	else
		print("Failed to open WiiMote:", wiimote)
	end
	print()
end
