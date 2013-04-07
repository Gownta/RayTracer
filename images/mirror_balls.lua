-- CSG spheres on spheres

scene = gr.node('scene')
scene:rotate('Y', 40)
scene:rotate('X', 25)
scene:translate(-3,0,-12)

-------------------
-- The balls

dispx = 0
dispz = 0
s     = 1
dir   = 0
n     = 30
d     = 2.2
rf    = 1

ddir  = 25 * (2 * 3.14159265 / 360)
ss    = 0.85

for idx = 0, n do
  -- place a ball
  ball = gr.sphere('ball')

  red = rf * (1 - math.pow(s, (idx + 1) / 2))
  blue = 1 - red
  red = 0.3 * red
  mat = gr.material({red, 0, blue}, {red, 0, blue}, 35, 0.6)
  ball:set_material(mat)

  ball:scale(s,s,s)
  ball:translate(dispx, s, dispz)

  scene:add_child(ball)

  dispx = dispx + d * s * math.cos(dir)
  dispz = dispz + d * s * math.sin(dir)
  s = s*ss
  dir = dir + ddir
end

-------------------
-- the floor

plane = gr.mesh('plane', {
	{-1, 0, -1},
	{ 1, 0, -1},
	{1,  0, 1},
	{-1, 0, 1},
     }, {
	{3, 2, 1, 0}
     })
scene:add_child(plane)
floor = gr.material({0.8,0.8,1.0},{0,0,0}, 1, 0.7)
plane:set_material(floor)
plane:scale(30, 30, 30)


white_light = gr.light({-10,10,10}, {1.0, 1.0, 1.0}, {1, 0, 0})
gr.render(scene, 'mirror_balls.png', 1024, 576,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

