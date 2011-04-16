function dir(e, w)
  local e = e or _G
  local w = w or 80
  local mw, n, l = 0, 0, 0
  for k, v in pairs(e) do l,n=string.len(k),n+1 if l > mw then mw = l end end
  local c = math.floor(w/(mw+1))
  local i = 0
  for k,v in pairs(e) do
    io.write(string.format('%'..mw..'s ', k))
    i = i + 1
    if math.mod(i, c) < 0.1 then io.write('\n') end
  end
  if math.mod(i, c) > 0.1 then io.write('\n') end
end
