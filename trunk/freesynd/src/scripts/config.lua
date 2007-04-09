
bug = {
   -- munin = 0,
   scheherazade = 1, gaia = 1, keycall = 1, music = 1, frame = 1,
}

flake = 1;
rot=1;

Map = map(rw('data/map04.dat'))

floor   = math.floor
round   = function(x, y) return math.floor(x/y)*y end
sprsort = function(a, b) -- {{{
      --local ax, ay, az = floor(a.x/32), floor(a.y/32), floor(a.z/32)
      --local bx, by, bz = floor(b.x/32), floor(b.y/32), floor(b.z/32)
      if     a.z ~= b.z     then do return      a.z  <  b.z      end end
      if a.x+a.y ~= b.x+b.y then do return (a.x+a.y) < (b.x+b.y) end end
      if a.x-a.y ~= b.x-b.y then do return (a.x-a.y) < (b.x-b.y) end end
   end -- }}}
glom    = function(x,y,z) -- {{{
  return ( math.floor(z/32) * 65536 +
	   math.floor(y/32) *   256 +
	   math.floor(x/32) *     1  )
  end -- }}}

sprites = {
   [glom(668, 2700, 64)]={ { x=668, y=2700, z=64, a=12, f=0 }, },
   [glom(670, 2800, 64)]={ { x=670, y=2800, z=64, a=10, f=0 }, },
   add = function(o, s)
      local l = o[glom(s.x, s.y, s.z)] or {}
      table.insert(l, s)
      table.sort(l, sprsort)
      o[glom(s.x, s.y, s.z)] = l
   end,
   rem = function(o, s)
     local t = o[glom(s.x, s.y, s.z)] or {}
     for i,v in ipairs(t) do
       if v == s then table.remove(t, i) end
     end
   end,
   iter = function(o)
     return coroutine.wrap(function()
       for k,v in pairs(sprites) do
         if type(v) == 'table' then
	   for l,w in pairs(v) do coroutine.yield(w) end
         end
       end
     end)
   end
}

do
   -- note that we can't use io.write here. Gaoler took it!
   local function printf(f, ...) print(string.format(f, ...)) end

   i, j, k = 813,2899,8
   --i, j = 0, 0

   local ii=0;

   local no_i_roll = function() ii=0; end

   if mus then 
      local M = mus('music/mod.synintro')
      print('M =', M)
      local Mp = false
      pla = function() if Mp then M:stop() else M:play() end Mp = not Mp end
   else
      pla = function() print('sorry, no music') end
   end

   function rad2dir(r) return (15-floor((r * 4 - 0.25) / math.pi)) % 8 end

   cp_theta = 0
   cp_sprite = { x=668, y=2700, z=64, a=9, f=0 }
   sprites:add(cp_sprite)
   function circlepanic()
      local xc, yc, r = 668, 2700, 32
      cp_theta = (cp_theta + 0.12) % (2*math.pi)
      sprites:rem(cp_sprite)
      cp_sprite.x = xc+r*math.cos(cp_theta)
      cp_sprite.y = yc+r*math.sin(cp_theta)
      cp_sprite.a = Agent.Minigun[Walk + (k % 2)] + rad2dir(cp_theta)
      sprites:add(cp_sprite)
   end

   add_ped =
      function(b, x, y)
	 local mx, my = s2m(x, y);
	 local x, y, z = mx-mx%1, my-my%1, 64
	 sprites:add{ x=x, y=y, z=z, a=k, f=0 }
      end

   local ll,rr,uu,dd = 0,0,0,0

   local wr,wt,oi,oj, wob = 16, 0, 0,0, false
   local wobble =
      function()
	 wt = wt + math.pi / 10
	 i = oi + wr*math.cos(wt)
	 j = oj + wr*math.sin(wt)
	 if wt >= 2*math.pi then wob, wt = false, 0 end
      end
   local start_wobble = function() oi, oj, wob = i, j, true end

   local frame =
      function()
	 Map:draw(i, j)
	 if wob then wobble() end
	 i = i - uu*2*k - ll*1*k
	 i = i + dd*2*k + rr*1*k
	 j = j - uu*2*k + ll*1*k
	 j = j + dd*2*k - rr*1*k
	 k = k + ii
	 circlepanic()
      end

   local click_info =
      function(b, x, y)
	 local mx, my = s2m(x, y);
	 printf('camera at %d,%d~(%d,%d) '..
		'click s(%d,%d) '..
		   'click on m(%d,%d)~(%d,%d)',
		i, j, i/scale, j/scale, x, y, mx, my, mx/scale, my/scale);
	 local mz
	 mx, my, mz, t = point_at(x, y)
	 printf('%d %d %d ~ (%d,%d): %d', mx, my, mz, mx/scale, my/scale, t)
	 local s='tilestack:'
	 local i, t
	 for i=0,11 do
	    t = tile_at(mx, my, i * scale)
	    s = (s..' '..string.format('%d', t))
	 end
	 print(s..'\n')
      end
   local info_fun =
      function()
	 local tlx, tly = s2m(0, 0)
	 local tx,  ty  = tlx / scale, tly / scale
	 local brx, bry = s2m(320, 200)
	 local bx,  by  = brx / scale, bry / scale
	 local q = tlx - brx
	 printf("top left:     %4d,%4d  %%  %5.2f,%5.2f", tlx, tly, tx,ty)
	 printf("bottom right: %4d,%4d  %%  %5.2f,%5.2f", brx, bry, bx,by)
	 printf("i,j,k, = %f %f %f\n", i, j, k)
	 print('x','y','a','f','+','-')
         local i,j,v,w
	 for w in sprites.iter() do
	    printf("%7.2f %7.2f %7d %7d %7.2f %7.2f",
		   w.x, w.y, w.a, w.f, w.x+w.y, w.y-w.x)
	 end
      end

   common_input = {
      [    '__index' ] = common_input,
      [  sdlk.F12    ] = function() v=v+1; M:vol(v) end,
      [  sdlk.F11    ] = function() v=v-1; M:vol(v) end,
      [  sdlk.ESCAPE ] = quit,
   }
   common_input.__index = common_input

   flake_input = {
      [      1       ] = click_info,
      [      2       ] = add_ped,
      [  sdlk.SPACE  ] = info_fun,

      [   sdlk.e     ] = function() rot=rot+3; end,
      [   sdlk.r     ] = function() rot=rot+1; end,

      [   sdlk.o     ] = function() ii=   1; end,
      [   sdlk.k     ] = function() ii=  -1; end,
      [   sdlk.i     ] = function() ii=  16; end,
      [   sdlk.j     ] = function() ii= -16; end,
      [  -sdlk.o     ] = no_i_roll,
      [  -sdlk.k     ] = no_i_roll,
      [  -sdlk.i     ] = no_i_roll,
      [  -sdlk.j     ] = no_i_roll,
      [   sdlk.g     ] = function() printf("%d",collectgarbage()) end,
      [     'frame'  ] = frame,
      [   sdlk['0']  ] = function() i,j = 0,0 end,
      [  sdlk.RETURN ] = function()
			    --film = fli(rw('film/intro.dat'))
			    fli(rw('film/mselect.dat')):play();
			 end,
      [  sdlk.m      ] = pla,
      [  sdlk.a      ] = function() i,j = i-(i%scale), j-(j%scale) end,
      [  sdlk.d      ] = function() i,j = 2512,1200 end,
      [  sdlk.w      ] = start_wobble,
      [  sdlk.LEFT   ] = function() ll = 1; end,
      [ -sdlk.LEFT   ] = function() ll = 0; end,
      [  sdlk.RIGHT  ] = function() rr = 1; end,
      [ -sdlk.RIGHT  ] = function() rr = 0; end,
      [  sdlk.UP     ] = function() uu = 1; end,
      [ -sdlk.UP     ] = function() uu = 0; end,
      [  sdlk.DOWN   ] = function() dd = 1; end,
      [ -sdlk.DOWN   ] = function() dd = 0; end,
      [  sdlk.b      ] =
	 function()
	    local z = map(rw('data/map03.dat'))
	 end,
      [  sdlk.v      ] =
	 function()
	    local z = snd(rw('bequietga.wav')):play()
	 end
   }
   setmetatable(flake_input, common_input)

   ---------------------------
   --    FLI player mode    --
   ---------------------------

   local break_out = function() film:stop() end;

   fliplayer_input = {};

   for k,v in pairs(sdlk) do fliplayer_input[v] = break_out end
   fliplayer_input[ sdlk.ESCAPE ] = quit

end


print('  +  script ok')
---- END ----
