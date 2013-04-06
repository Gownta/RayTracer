-- A game of go

scene = gr.node('scene')

-- ##############################################
-- textures
-- ##############################################

black = gr.material({0.1, 0.1, 0.1}, {0.7, 0.7, 0.7}, 30)
white = gr.material({0.9, 0.9, 0.9}, {0.7, 0.7, 0.7}, 30)
board = gr.texture_material('go_board.png', {1.0, 1.0, 1.0}, 10)

-- ##############################################
-- the board surface
-- ##############################################

board_scale = 22

plane = gr.mesh('plane', 
                { -- vertices
		   { -1, 0, -1 }, 
		   {  1, 0, -1 }, 
		   {  1, 0,  1 }, 
		   { -1, 0,  1 }
		},

                { -- normals
                },

                { -- textures
                   { 0,0,0 },
                   { 0,1999,0 },
                   { 1999,0,0 },
                   { 1999,1999,0 }
                },

                { -- faces
                   { {3, 2, 1, 0},
                     {},
                     {3,2,0,1}
                   }
                })
board = gr.texture_material('go_board.png', {1.0, 1.0, 1.0}, 10)
plane:set_material(board)
plane:scale(board_scale, board_scale, board_scale)
scene:add_child(plane)

board_offset = 63.0 / 1000.0 * board_scale
board_diff   = (board_scale - board_offset) / 9.0

-- ##############################################
-- render
-- ##############################################

scene:translate(0,0,-50)
scene:rotate('X', 30)
scene:rotate('Y', 25)
scene:translate(0.5,3,0)

gr.render(scene,
	  'personal.png', 200, 120,
          {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})

