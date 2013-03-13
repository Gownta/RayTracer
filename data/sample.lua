-- A game of go

scene = gr.node('scene')

-- ##############################################
-- textures
-- ##############################################

black = gr.material({0.1, 0.1, 0.1}, {0.7, 0.7, 0.7}, 30)
white = gr.material({0.9, 0.9, 0.9}, {0.7, 0.7, 0.7}, 30)
board = gr.texture_material('go_board.png', {1.0, 1.0, 1.0}, 10)

-- ##############################################
-- the pieces
-- ##############################################

hscale = 0.48
vscale = 0.9

bstone = gr.nh_sphere('b', {0,0,0}, 1)
bstone:set_material(black)
bstone:scale(vscale,hscale,vscale)

wstone = gr.nh_sphere('w', {0,0,0}, 1)
wstone:set_material(white)
wstone:scale(vscale,hscale,vscale)

-- ##############################################
-- the board surface
-- ##############################################

board_scale = 22

plane = gr.mesh('plane', {
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1,  0, 1 }, 
		   { -1, 0, 1  }
		}, {
		   {3, 2, 1, 0}
		})
board = gr.texture_material('go_board.png', {1.0, 1.0, 1.0}, 10)
plane:set_material(board)
plane:scale(board_scale, board_scale, board_scale)
scene:add_child(plane)

board_offset = 63.0 / 1000.0 * board_scale
board_diff   = (board_scale - board_offset) / 9.0

-- ##############################################
-- the pieces on the board
-- ##############################################

blocations = {
  { 2,  { 6, 12, 14 } },
  { 3,  { 2, 3, 5, 15, 16, 18 } },
  { 4,  { 2, 11, 14, 16, 17 } },
  { 5,  { 3, 5 } },
  { 6,  { 3, 4 } },
  { 7,  { 13 } },
  { 8,  { 12, 13, 14 } },
  { 9,  { 12 } },
  { 10, { 13 } },
  { 11, { 13, 14 } },
  { 12, { 11, 12, 15, 16, 17 } },
  { 13, { 17 } },
  { 14, { 13, 15, 16 } },
  { 15, { 11, 13, 17 } },
  { 16, { 12, 14 } }
}

wlocations = {
  { 1,  { 15, 17 } },
  { 2,  { 7, 10, 15, 16, 17 } },
  { 3,  { 7, 17 } },
  { 4,  { 3, 4 } },
  { 5,  { 2, 4, 16 } },
  { 6,  { 13 } },
  { 7,  { 14 } },
  { 8,  { 15, 16 } },
  { 9,  { 13, 14 } },
  { 10, { 12, 14 } },
  { 11, { 15, 16, 17 } },
  { 12, { 13 } },
  { 13, { 12, 14, 15, 16 } },
  { 14, { 12, 14, 17 } },
  { 15, { 12 } },
  { 16, { 13, 15 } },
  { 17, { 13, 14 } }
}

for _,xys in pairs(blocations) do
  x = xys[1]
  ys = xys[2]
  for _,y in pairs(ys) do
    piece = gr.node('b piece ' .. tostring(x) .. '.' .. tostring(y))
    piece:add_child(bstone)
    actualx =  (x - 9) * board_diff
    actualy = -(y - 9) * board_diff
    piece:translate(actualx, hscale, actualy)
    scene:add_child(piece)
  end
end

for _,xys in pairs(wlocations) do
  x = xys[1]
  ys = xys[2]
  for _,y in pairs(ys) do
    piece = gr.node('w piece ' .. tostring(x) .. '.' .. tostring(y))
    piece:add_child(wstone)
    actualx =  (x - 9) * board_diff
    actualy = -(y - 9) * board_diff
    piece:translate(actualx, hscale, actualy)
    scene:add_child(piece)
  end
end

-- ##############################################
-- render
-- ##############################################

scene:translate(0,0,-50)
scene:rotate('X', 30)
scene:rotate('Y', 25)
scene:translate(0.5,3,0)

gr.render(scene,
	  'personal.png', 2000, 1200,
          {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})

