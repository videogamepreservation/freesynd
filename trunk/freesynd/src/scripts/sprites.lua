
weapons = {names='-psumfrlgS',[0]='Unarmed','Pistol','Shotgun',
   'Uzi','Minigun','Flamer','Long','Laser','Gauss','Shield' }


Stand, StandFire, Walk, WalkFire = 1,2,3,4

Agent = { --   s-  sf  w-  wf
   Unarmed = {  1,  1,  9,  9},
   Pistol  = { 25, 97, 65,145},
   Shotgun = { 49,105, 89,153},
   Uzi     = { 25,105, 65,145},
   Minigun = { 33,113, 73,161},
   Flamer  = { 41,129, 81,177},
   Long    = { 49,137, 89,177},
   Laser   = { 49,121, 89,169},
   Gauss   = { 25,105, 65,145},
   Shield  = { 17, 17, 57, 57},
   Pick    = 193,
   Hit     = 194,--dn,rt,up,lf
   Vap     = 198,--dn,rt,up,lf
   Swamp   = 202,
   Die     = 204, --bleed+y,dead+y,bleed+x,dead+x
   Burn    = 208, --burn,burn,die,dead
   Pers    = 236,
}

Guard = { --   s-    sf    w-    wf
   Unarmed = { 1744, 1744, 1752, 1752 },
   Pistol  = { 1768, 1848, 1808, 1896 },
   Pick    = 1936,
   Hit     = 1937,--dn,rt,up,lf
   Vap     = 1941,--dn,rt,up,lf
   Swamp   = 1945,
   Die     = 1949, --bleed+y,dead+y,bleed+x,dead+x
   Pers    = 236,
}

General = { --   s-    sf    w-    wf
   Unarmed = { 1537, 1537, 1545, 1545 },
   Pistol  = { 1561, 1641, 1601, 1689 },
   Pick    = 1936,
   Hit     = 1937,--dn,rt,up,lf
   Vap     = 1941,--dn,rt,up,lf
   Swamp   = 1945,
   Die     = 1949, --bleed+y,dead+y,bleed+x,dead+x
   Pers    = 236,
}

Trees = {
   { ok = 227, burn = 228, char = 229 },
   { ok = 230, burn = 231, char = 232 },
   { ok = 233, burn = 234, char = 235 }
}

Numbers={ [-1]=212,[1]=214,[-2]=216,[2]=218,[-3]=220,[3]=222,[-4]=224,[4]=226 }

-- {{{
agent = [=[

s	-	Unarmed	1
w	-	Unarmed	9
s	-	Shield	17
s	-	Pistol	25
s	-	Minigun	33
s	-	Flamer	41
s	-	Long	49
w	-	Shield	57
w	-	Pistol	65
w	-	Minigun	73
w	-	Flamer	81
w	-	Long	89
s	f	Pistol	97
s	f	Uzi	105
s	f	Shotgun	105
s	f	Gauss	105
s	f	Minigun	113
s	f	Laser	121
s	f	Flamer	129
s	f	Long	137
w	f	Pistol	145
w	f	Uzi	145
w	f	Shotgun	145
w	f	Gauss	145
w	f	Minigun	161
w	f	Laser	153
w	f	Flamer	169
w	f	Long	177



s	-	Unarmed	1
s	f	Unarmed	1 *
w	-	Unarmed	9
w	f	Unarmed	9 *
s	-	Pistol	25
s	f	Pistol	97
w	-	Pistol	65
w	f	Pistol	145
s	-	Shotgun	49 *
s	f	Shotgun	105
w	-	Shotgun	89 *
w	f	Shotgun	145
s	-	Uzi	25 *
s	f	Uzi	105
w	-	Uzi	65 *
w	f	Uzi	145
s	-	Minigun	33
s	f	Minigun	113
w	-	Minigun	73
w	f	Minigun	153
s	-	Flamer	41
s	f	Flamer	129
w	-	Flamer	81
w	f	Flamer	169
s	-	Long	49
s	f	Long	137
w	-	Long	89
w	f	Long	177
s	-	Laser	49 *
s	f	Laser	121
w	-	Laser	89 *
w	f	Laser	161
s	-	Gauss   25 *
s	f	Gauss	105
w	-	Gauss   65 *
w	f	Gauss	145
s	-	Shield	17
s	f	Shield	17 *
w	-	Shield	57
w	f	Shield	57 *



]=]

-- }}}
