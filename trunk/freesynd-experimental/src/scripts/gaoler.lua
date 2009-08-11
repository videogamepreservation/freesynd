
--                                --
--   G A O L E R                  --
--   -----------                  --
--   CONFISCATES SHARP OBJECTS.   --
--                                --

do
   local io = io
   function dir(e, w, ind)
      local e = e or _G
      local w = w or 80
      local mw, n, l = 0, 0, 0
      for k, v in pairs(e) do
	 l,n=string.len(k),n+1
	 if l > mw then mw = l end
      end
      local c = math.floor(w/(mw+1))
      local i = 0
      io.write(ind)
      for k,v in pairs(e) do
	 io.write(string.format('%'..mw..'s ', k))
	 i = i + 1
	 if math.mod(i, c) < 0.1 then io.write('\n  +  ') end
      end
      if math.mod(i, c) > 0.1 then io.write('\n') end
   end

   local keepers = { -- {{{
      -- What,      -- You can't have it because...
      'restricted',
      'string',
      'xpcall',
      --'package',  -- You could load libraries with more caps, like, say, libc
      'tostring',
      'print',
      --'os',       -- exit, setlocale, remove, tmpname, rename, execute
      'unpack',
      --'require',  -- Again, more lib loading
      --'getfenv',  -- You could borrow the env from a special function
      'setmetatable',
      'next',
      'assert',
      'tonumber',
      --'io',       -- D'oh. (Which is not to say we can't provide you with
      'rawequal',   -- a castrated clone.
      'collectgarbage',
      'getmetatable',
      --'module',   -- Again with the module loading
      'rawset',
      'dir',
      'math',
      --'debug',    -- Turns world inside-out, à la Philip K Dick...
      'pcall',
      'table',
      --'newproxy', -- I don't know what this is. Ergo, it is not 'known-good'
      'type',
      'coroutine',
      '_G',
      'select',
      'gcinfo',
      'pairs',
      'rawget',
      'loadstring',
      'ipairs',
      '_VERSION',
      --'dofile',   -- You try to load stuff, and look at the parser errors
      'setfenv',
      'load',
      'error', -- }}}
   }

   -- swap this around from [1]='string' to ['string']=true
   for   i,v in ipairs(keepers)  do keepers[v] = true     end -- fill ['nn']=t
   while table.maxn(keepers) > 0 do table.remove(keepers) end -- drop [1]='nn'

   for k, v in pairs(_G) do
      if not keepers[k] then restricted[k] = v _G[k] = nil end
   end
   print('  +  Gaoler confiscates contraband:')
   print('  +  --< RESTRICTED >----------------------------------------------')
   dir(restricted, 70, '  +  ')
   print('  +  --< PERMITTED >-----------------------------------------------')
   restricted = nil
   dir(_G, 70, '  +  ')
   print('  +  --------------------------------------------------------------')
end
