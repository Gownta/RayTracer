-------------------
-- Phone

phone = gr.mesh('phone',
  { -- vertices
    {90,142,0},
    {126,106,0},
    {483,106,0},
    {519,142,0},
    {519,865,0},
    {483,901,0},
    {126,901,0},
    {90,865,0},
    {90,142,0},
    {126,106,-1},
    {483,106,-1},
    {519,142,-1},
    {519,865,-1},
    {483,901,-1},
    {126,901,-1},
    {90,865,-1},
  }, { -- normals
    {1,0,0},
    {-1,0,0},
    {0,1,0},
    {0,-1,0},
  }, { -- textures
    {90,142,0},
    {126,106,0},
    {483,106,0},
    {519,142,0},
    {519,865,0},
    {483,901,0},
    {126,901,0},
    {90,865,0},
  }, { -- faces
    {{0,1,2,3,4,5,6,7},{},{0,1,2,3,4,5,6,7}},
    {{0,1,9,8},{2,3,3,2},{5,5,5,5}},
    {{1,2,10,9},{3,3,3,3},{5,5,5,5}},
    {{2,3,11,10},{3,1,1,3},{5,5,5,5}},
    {{3,4,12,11},{1,1,1,1},{5,5,5,5}},
    {{4,5,13,12},{1,4,4,1},{5,5,5,5}},
    {{5,6,14,13},{4,4,4,4},{5,5,5,5}},
    {{6,7,15,14},{4,2,2,4},{5,5,5,5}},
    {{7,0,8,15},{2,2,2,2},{5,5,5,5}},
  })
phone:set_material(gr.texture_material('phone.png', {0,0,0}, 1))
phone:translate(-90,-106,0)
phone:scale(1/429,1/795,1)
phone:scale(3,0.2,5.5)