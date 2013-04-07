-- A keyboard

-- Texture positions
keyposmap = {
  ["esc"]   = {  20, 20, 50, 29 },
  ["f1"]    = {  87, 20, 50, 29 },
  ["f2"]    = { 154, 20, 50, 29 },
  ["f3"]    = { 222, 20, 50, 29 },
  ["f4"]    = { 288, 20, 50, 29 },
  ["f5"]    = { 356, 20, 50, 29 },
  ["f6"]    = { 422, 20, 50, 29 },
  ["f7"]    = { 490, 20, 50, 29 },
  ["f8"]    = { 556, 20, 50, 29 },
  ["f9"]    = { 624, 20, 50, 29 },
  ["f10"]   = { 690, 20, 50, 29 },
  ["f11"]   = { 760, 20, 50, 29 },
  ["f12"]   = { 825, 20, 50, 29 },
  ["help"]  = { 892, 20, 50, 29 },

  ["~"]     = {  20, 66, 48, 48 },
  ["1"]     = {  85, 66, 48, 48 },
  ["2"]     = { 150, 66, 48, 48 },
  ["3"]     = { 215, 66, 48, 48 },
  ["4"]     = { 281, 66, 48, 48 },
  ["5"]     = { 346, 66, 48, 48 },
  ["6"]     = { 411, 66, 48, 48 },
  ["7"]     = { 476, 66, 48, 48 },
  ["8"]     = { 541, 66, 48, 48 },
  ["9"]     = { 606, 66, 48, 48 },
  ["0"]     = { 671, 66, 48, 48 },
  ["-"]     = { 736, 66, 48, 48 },
  ["="]     = { 801, 66, 48, 48 },
  ["back"]  = { 866, 66, 75, 48 },

  ["tab"]   = {  20, 131, 73, 48 },
  ["q"]     = { 110, 131, 48, 48 },
  ["w"]     = { 176, 131, 48, 48 },
  ["e"]     = { 241, 131, 48, 48 },
  ["r"]     = { 306, 131, 48, 48 },
  ["t"]     = { 371, 131, 48, 48 },
  ["y"]     = { 436, 131, 48, 48 },
  ["u"]     = { 501, 131, 48, 48 },
  ["i"]     = { 566, 131, 48, 48 },
  ["o"]     = { 631, 131, 48, 48 },
  ["p"]     = { 696, 131, 48, 48 },
  ["["]     = { 761, 131, 48, 48 },
  ["]"]     = { 827, 131, 48, 48 },
  ["|"]     = { 892, 131, 48, 48 },

  ["caps"]  = {  20, 196, 91, 48 },
  ["a"]     = { 129, 196, 48, 48 },
  ["s"]     = { 194, 196, 48, 48 },
  ["d"]     = { 259, 196, 48, 48 },
  ["f"]     = { 324, 196, 48, 48 },
  ["g"]     = { 389, 196, 48, 48 },
  ["h"]     = { 454, 196, 48, 48 },
  ["j"]     = { 519, 196, 48, 48 },
  ["k"]     = { 584, 196, 48, 48 },
  ["l"]     = { 649, 196, 48, 48 },
  [";"]     = { 714, 196, 48, 48 },
  ["'"]     = { 780, 196, 48, 48 },
  ["|n"]    = { 844, 196, 98, 48 },

  ["Lshft"] = {  20, 261, 124, 48 },
  ["z"]     = { 162, 261, 48, 48 },
  ["x"]     = { 227, 261, 48, 48 },
  ["c"]     = { 292, 261, 48, 48 },
  ["v"]     = { 357, 261, 48, 48 },
  ["b"]     = { 422, 261, 48, 48 },
  ["n"]     = { 487, 261, 48, 48 },
  ["m"]     = { 552, 261, 48, 48 },
  [","]     = { 617, 261, 48, 48 },
  ["."]     = { 682, 261, 48, 48 },
  ["/"]     = { 747, 261, 48, 48 },
  ["Rshft"] = { 812, 261, 124, 48 },

  ["Lctrl"] = {  20, 325, 73, 48 },
  ["Lopt"]  = { 110, 325, 73, 48 },
  ["Lalt"]  = { 201, 325, 73, 48 },
  [" "]     = { 292, 325, 376, 48 },
  ["Ralt"]  = { 682, 325, 73, 48 },
  ["Ropt"]  = { 774, 325, 73, 48 },
  ["Rctrl"] = { 867, 325, 73, 48 },
}

-- Base build-up
keyboard_data = { 'keyboard', {}, {}, { {2,2,0} }, {} }
keyboard_meta = { }
keyboard_scale = 20

function addsection(km, w, h)
  table.insert(km, { w, h })
end

function addspace(km, e, w)
  km[e][1] = km[e][1] + w
end

-- printing helper
function tprint (tbl, indent)
  if not indent then indent = 0 end
  for k, v in pairs(tbl) do
    formatting = string.rep("  ", indent) .. k .. ": "
    if type(v) == "table" then
      print(formatting)
      tprint(v, indent+1)
    else
      print(formatting .. v)
    end
  end
end

-- add a key to the keyboard
function addkey(kb, km, sym, row)
  ks = keyboard_scale
  -- the key is at position km[row], with textures from keyposmap
  topleft = km[row]
  key = keyposmap[sym]

  v_idx = table.getn(kb[2])
  table.insert(kb[2], {topleft[1],          0, -topleft[2]         })
  table.insert(kb[2], {topleft[1] + key[3], 0, -topleft[2]         })
  table.insert(kb[2], {topleft[1] + key[3], 0, -topleft[2] - key[4]})
  table.insert(kb[2], {topleft[1],          0, -topleft[2] - key[4]})

  b_idx = table.getn(kb[2])
  table.insert(kb[2], {topleft[1]          - ks/2, -ks/4, -topleft[2]          + ks/2})
  table.insert(kb[2], {topleft[1] + key[3] + ks/2, -ks/4, -topleft[2]          + ks/2})
  table.insert(kb[2], {topleft[1] + key[3] + ks/2, -ks/4, -topleft[2] - key[4] - ks/2})
  table.insert(kb[2], {topleft[1]          - ks/2, -ks/4, -topleft[2] - key[4] - ks/2})

  t_idx = table.getn(kb[4])
  table.insert(kb[4], {key[1],          key[2],          0})
  table.insert(kb[4], {key[1] + key[3], key[2],          0})
  table.insert(kb[4], {key[1] + key[3], key[2] + key[4], 0})
  table.insert(kb[4], {key[1],          key[2] + key[4], 0})

  face = { { v_idx, v_idx + 1, v_idx + 2, v_idx + 3 },
           {},
           { t_idx, t_idx + 1, t_idx + 2, t_idx + 3 } }
  table.insert(kb[5], face)

  for i = 0,3 do
    j = (i + 1) % 4
    side = { { b_idx + i, v_idx + i, v_idx + j, b_idx + j },
             {},
             { 0,0,0,0 } }
    table.insert(kb[5], side)
  end

  km[row][1] = km[row][1] + key[3] + ks
end

---------------------------------------
-- Keyboard Specification
---------------------------------------

addsection(keyboard_meta, 0, 0)
addkey(keyboard_data, keyboard_meta, "esc", 1)
addkey(keyboard_data, keyboard_meta, "f1", 1)
addkey(keyboard_data, keyboard_meta, "f2", 1)
addkey(keyboard_data, keyboard_meta, "f3", 1)
addkey(keyboard_data, keyboard_meta, "f4", 1)
addkey(keyboard_data, keyboard_meta, "f5", 1)
addkey(keyboard_data, keyboard_meta, "f6", 1)
addkey(keyboard_data, keyboard_meta, "f7", 1)
addkey(keyboard_data, keyboard_meta, "f8", 1)
addkey(keyboard_data, keyboard_meta, "f9", 1)
addkey(keyboard_data, keyboard_meta, "f10", 1)
addkey(keyboard_data, keyboard_meta, "f11", 1)
addkey(keyboard_data, keyboard_meta, "f12", 1)
addkey(keyboard_data, keyboard_meta, "help", 1)

addsection(keyboard_meta, 0, -51)
addkey(keyboard_data, keyboard_meta, "~", 2)
addkey(keyboard_data, keyboard_meta, "1", 2)
addkey(keyboard_data, keyboard_meta, "2", 2)
addkey(keyboard_data, keyboard_meta, "3", 2)
addkey(keyboard_data, keyboard_meta, "4", 2)
addkey(keyboard_data, keyboard_meta, "5", 2)
addkey(keyboard_data, keyboard_meta, "6", 2)
addkey(keyboard_data, keyboard_meta, "7", 2)
addkey(keyboard_data, keyboard_meta, "8", 2)
addkey(keyboard_data, keyboard_meta, "9", 2)
addkey(keyboard_data, keyboard_meta, "0", 2)
addkey(keyboard_data, keyboard_meta, "-", 2)
addkey(keyboard_data, keyboard_meta, "=", 2)
addkey(keyboard_data, keyboard_meta, "back", 2)

addsection(keyboard_meta, 0, -121)
addkey(keyboard_data, keyboard_meta, "tab", 3)
addkey(keyboard_data, keyboard_meta, "q", 3)
addkey(keyboard_data, keyboard_meta, "w", 3)
addkey(keyboard_data, keyboard_meta, "e", 3)
addkey(keyboard_data, keyboard_meta, "r", 3)
addkey(keyboard_data, keyboard_meta, "t", 3)
addkey(keyboard_data, keyboard_meta, "y", 3)
addkey(keyboard_data, keyboard_meta, "u", 3)
addkey(keyboard_data, keyboard_meta, "i", 3)
addkey(keyboard_data, keyboard_meta, "o", 3)
addkey(keyboard_data, keyboard_meta, "p", 3)
addkey(keyboard_data, keyboard_meta, "[", 3)
addkey(keyboard_data, keyboard_meta, "]", 3)
addkey(keyboard_data, keyboard_meta, "|", 3)

addsection(keyboard_meta, 0, -191)
addkey(keyboard_data, keyboard_meta, "caps", 4)
addkey(keyboard_data, keyboard_meta, "a", 4)
addkey(keyboard_data, keyboard_meta, "s", 4)
addkey(keyboard_data, keyboard_meta, "d", 4)
addkey(keyboard_data, keyboard_meta, "f", 4)
addkey(keyboard_data, keyboard_meta, "g", 4)
addkey(keyboard_data, keyboard_meta, "h", 4)
addkey(keyboard_data, keyboard_meta, "j", 4)
addkey(keyboard_data, keyboard_meta, "k", 4)
addkey(keyboard_data, keyboard_meta, "l", 4)
addkey(keyboard_data, keyboard_meta, ";", 4)
addkey(keyboard_data, keyboard_meta, "'", 4)
addkey(keyboard_data, keyboard_meta, "|n", 4)

addsection(keyboard_meta, 0, -261)
addkey(keyboard_data, keyboard_meta, "Lshft", 5)
addspace(keyboard_meta, 5, 6)
addkey(keyboard_data, keyboard_meta, "z", 5)
addkey(keyboard_data, keyboard_meta, "x", 5)
addkey(keyboard_data, keyboard_meta, "c", 5)
addkey(keyboard_data, keyboard_meta, "v", 5)
addkey(keyboard_data, keyboard_meta, "b", 5)
addkey(keyboard_data, keyboard_meta, "n", 5)
addkey(keyboard_data, keyboard_meta, "m", 5)
addkey(keyboard_data, keyboard_meta, ",", 5)
addkey(keyboard_data, keyboard_meta, ".", 5)
addkey(keyboard_data, keyboard_meta, "/", 5)
addspace(keyboard_meta, 5, 6)
addkey(keyboard_data, keyboard_meta, "Rshft", 5)

addsection(keyboard_meta, 0, -331)
addkey(keyboard_data, keyboard_meta, "Lctrl", 6)
addkey(keyboard_data, keyboard_meta, "Lopt", 6)
addkey(keyboard_data, keyboard_meta, "Lalt", 6)
addspace(keyboard_meta, 6, 12)
addkey(keyboard_data, keyboard_meta, " ", 6)
addspace(keyboard_meta, 6, 12)
addkey(keyboard_data, keyboard_meta, "Ralt", 6)
addkey(keyboard_data, keyboard_meta, "Ropt", 6)
addkey(keyboard_data, keyboard_meta, "Rctrl", 6)
--]]
keyboard_length = 980
keyboard_height = 401
keyboard_buffer = 10

-- do some post processing
ks = keyboard_scale
b_idx = table.getn(keyboard_data[2])
table.insert(keyboard_data[2], {-keyboard_buffer                   - ks, -ks/3, -ks -keyboard_buffer                  })
table.insert(keyboard_data[2], { keyboard_buffer + keyboard_length - ks, -ks/3, -ks -keyboard_buffer                  })
table.insert(keyboard_data[2], { keyboard_buffer + keyboard_length - ks, -ks/3,     -keyboard_buffer + keyboard_height})
table.insert(keyboard_data[2], {-keyboard_buffer                   - ks, -ks/3,     -keyboard_buffer + keyboard_height})

table.insert(keyboard_data[2], {-keyboard_buffer                  , -ks/1.5, keyboard_buffer})
table.insert(keyboard_data[2], { keyboard_buffer + keyboard_length, -ks/1.5, keyboard_buffer})

table.insert(keyboard_data[5], {{ b_idx + 0, b_idx + 1, b_idx + 2, b_idx + 3 }, {}, {0,0,0,0}})
--table.insert(keyboard_data[5], {{ b_idx + 0, b_idx + 1, b_idx + 5, b_idx + 4 }, {}, {0,0,0,0}})
--table.insert(keyboard_data[5], {{ b_idx + 4, b_idx + 5, b_idx + 2, b_idx + 3 }, {}, {0,0,0,0}})
--table.insert(keyboard_data[5], {{ b_idx + 0, b_idx + 4, b_idx + 3 }, {}, {0,0,0}})
--table.insert(keyboard_data[5], {{ b_idx + 1, b_idx + 5, b_idx + 2 }, {}, {0,0,0}})

-- Return the keyboard mesh
keyboard = gr.mesh(keyboard_data[1], keyboard_data[2], keyboard_data[3], keyboard_data[4], keyboard_data[5])
kbt = gr.texture_material('keyboard.png', {0,0,0,}, 1)
keyboard:set_material(kbt)
keyboard:scale(1/keyboard_scale, 1/keyboard_scale, 1/keyboard_scale)

-- ##############################################
-- render
-- ##############################################



scene = gr.node('scene')
scene:add_child(keyboard)


scene:rotate('X', 90)
scene:translate(-30,10,-70)

gr.render(scene,
	  'keyboard.png', 200, 120,
          {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({-30,40,-30}, {0.8, 0.8, 0.8}, {1,0,0}), gr.light({0, 10, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})})

