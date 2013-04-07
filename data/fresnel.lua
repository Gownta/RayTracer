-- Glass

scene = gr.node('scene')

-------------------
-- Materials

glass = gr.optics_material(2.0, 0.05, {0.8,0.8,1.0})

-------------------
-- Geometry

ginfcyl   = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
gbiplane  = gr.algebraic('biplane', 'y^2 - 1', 2)
ginfcyl   :set_material(glass)
gbiplane  :set_material(glass)
gcyl      = gr.csg('_cylinder', ginfcyl, '*', gbiplane, 1.42)

gsphere = gr.sphere('glass sphere')
gsphere:scale(0.1,0.1,0.1)
gsphere:translate(0.5,0.1,0.5)
gsphere:set_material(glass)
scene:add_child(gsphere)

-------------------
-- Emplacement

cup = gr.node('glass')
cup:add_child(gcyl)
cup:scale(0.2,0.2,0.2)
cup:scale(1,1.618,1)
cup:translate(0.2,0.3,0.2)

--scene:add_child(cup)

-------------------
-- Display

scene:translate(0,0,-1)

backdrop = gr.mesh('backdrop',
  { -- vertices
    {0,0+0.1,0},
    {1,0+0.1,0},
    {1,1.2276+0.1,0},
    {0,1.2276+0.1,0}
  }, { -- normals
  }, { -- textures
    {390,479,0},
    {0,479,0},
    {0,0,0},
    {390,0,0},
  }, { -- faces
    {
      {0,1,2,3},
      {},
      {0,1,2,3}
    }
  })
backdrop:set_material(gr.texture_material('Augustin_Fresnel.png', {0,0,0}, 1))

floor = gr.mesh('floor',
  { -- vertices
    {0,0.3,0},
    {1,0.3,0},
    {1,0.3,0.7222},
    {0,0.3,0.7222}
  }, { -- normals
  }, { -- textures
    {390,479,0},
    {0,479,0},
    {0,0,0},
    {390,0,0},
  }, { -- faces
    {
      {0,1,2,3},
      {},
      {0,1,2,3}
    }
  })
floor:set_material(gr.texture_material('8087-rainbow_checkerboard.png', {0,0,0}, 1))


scene:add_child(backdrop)
scene:add_child(floor)

scene:translate(-0.5,-0.4,0)

-------------------
-- Render

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
gr.render(scene, 'csg.png', 128, 256,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

