function on_open()
	local mods_panel = document["mods_add"]
	
	-- ----------------------------------------------------------------------------------
	-- How to calling badass.. core.. instance.. get mod loader.. get mods.. get info..
	-- Aaah, okay, i do it by easy way, using file. FUCK YOUR FRAMEWORK, BITCH.
	-- ----------------------------------------------------------------------------------

	print("Loaded Mods:")
	
	local mods_path = "user:mods"
	
	if file.exists(mods_path) and file.isdir(mods_path) then
		local entries = file.list(mods_path)
		local found_mods = false
		
		print("Found " .. #entries .. " entries in mods directory")
		
		for _, entry in ipairs(entries) do
			print("Checking entry: " .. entry)
			
			if file.isfile(entry) then
				local filename = file.name(entry) 
				local ext = file.ext(filename)   
				print("Filename: " .. filename .. ", extension: " .. ext)
				
				if ext == "so" or ext == "dll" then
					found_mods = true
					local mod_name = file.stem(filename)
					
					print(string.format(" - %s.%s", mod_name, ext))
					
					mods_panel:add(string.format([[
						<panel size='420,30' margin='5' color='#FFFFFF11'>
							<label pos='5,5' size='410,20'>%s.%s</label>
						</panel>
					]], 
						string.escape_xml(mod_name),
						string.escape_xml(ext)
					))
				end
			end
		end
		
		if not found_mods then
			mods_panel:add([[
				<label margin='5'>No mods found (.so or .dll files)</label>
			]])
		end
	else
		mods_panel:add([[
			<label margin='5'>Mods directory not found</label>
		]])
	end
end