-- CSG shape
--

-------------------
-- Materials

red   = gr.material({1,0.2,0.2},{0.8,0.8,0.8},25)
green = gr.material({0,1,0},{0.8,0.8,0.8},25)
blue  = gr.material({0,0,1},{0.8,0.8,0.8},25)

-------------------
-- Components

----- Cylinder
infcyl   = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
biplane  = gr.algebraic('biplane', 'y^2 - 1', 2)
infcyl   :set_material(green)
biplane  :set_material(green)
cyl      = gr.csg('_cylinder', infcyl, '*', biplane, 1.42)
cylinder = gr.node('cylinder')
cylinder :add_child(cyl)
cylinder :scale(0.65,1,0.65)

----- Sphere
sphere = gr.sphere('sphere')
sphere:set_material(blue)
sphere:scale(1.3,1.3,1.3)

----- Cube
cube = gr.cube('cube')
cube:set_material(red)
cube:translate(-0.5,-0.5,-0.5)
cube:scale(1.95,1.95,1.95)

-------------------
-- Constituents

----- Cylinder orientations
xcyl = gr.node('xcylinder')
ycyl = gr.node('ycylinder')
zcyl = gr.node('zcylinder')

xcyl:add_child(cylinder)
ycyl:add_child(cylinder)
zcyl:add_child(cylinder)

xcyl:rotate('Z', 90)
zcyl:rotate('X', 90)

----- Combinations
cross = gr.csg('cross', xcyl, '+', ycyl, 1.42)
spike = gr.csg('spike', zcyl, '+', cross, 1.42)
die   = gr.csg('die', cube, '*', sphere, 1.5)
oddie = gr.csg('oddie', die, '-', spike, 1.5)


-------------------
-- Wrappers

t0    = gr.node('0')
t01   = gr.node('01')
t02   = gr.node('02')
t011  = gr.node('011')
t012  = gr.node('012')
t021  = gr.node('021')
t022  = gr.node('022')
t0221 = gr.node('0221')
t0222 = gr.node('0222')

t0222:add_child(ycyl)
t0221:add_child(xcyl)
t022 :add_child(cross)
t021 :add_child(zcyl)
t02  :add_child(spike)
t012 :add_child(sphere)
t011 :add_child(cube)
t01  :add_child(die)
t0   :add_child(oddie)

rot = -30
t0   :rotate('Y', rot)
t01  :rotate('Y', rot)
t02  :rotate('Y', rot)
t011 :rotate('Y', rot)
t012 :rotate('Y', rot)
t021 :rotate('Y', rot)
t022 :rotate('Y', rot)
t0221:rotate('Y', rot)
t0222:rotate('Y', rot)

rot = 20
t0   :rotate('X', rot)
t01  :rotate('X', rot)
t02  :rotate('X', rot)
t011 :rotate('X', rot)
t012 :rotate('X', rot)
t021 :rotate('X', rot)
t022 :rotate('X', rot)
t0221:rotate('X', rot)
t0222:rotate('X', rot)

-------------------
-- Display

scene = gr.node('scene')
scene:translate(0,0,-20)

t0:scale(1.5,1.5,1.5)
t0:translate(0,3,0)
scene:add_child(t0)

t01:translate(-4,0,0)
scene:add_child(t01)

t011:translate(-6,-3,0)
scene:add_child(t011)

t012:translate(-2,-3,0)
scene:add_child(t012)

t02:translate(4,1,0)
scene:add_child(t02)

t021:translate(2,-2,0)
scene:add_child(t021)

t022:translate(6,-1,0)
scene:add_child(t022)

t0221:translate(5,-3,0)
scene:add_child(t0221)

t0222:translate(8,-3,0)
scene:add_child(t0222)


backdrop = gr.mesh('backdrop',
  { -- vertices
    {-10.58, 7.94,-2},
    { 10.58, 7.94,-2},
    { 10.58,-7.94,-2},
    {-10.58,-7.94,-2},
  }, { -- normals
  }, { -- textures
    {0,0,0},
    {1057,0,0},
    {1057,793,0},
    {0,793,0},
  }, { -- faces
    {
      {0,1,2,3},
      {},
      {0,1,2,3}
    }
  })
backdrop:set_material(gr.texture_material('Arquimedean_Scheme_mod.png', {0,0,0}, 1))
scene:add_child(backdrop)

-------------------
-- Render

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
gr.render(scene, 'csg.png', 1024, 512,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

