-- 4th degree algebraics

scene = gr.node('scene')
scene:rotate('X', 18)
scene:translate(0,0,-20)

-------------------
-- Materials

red   = gr.material({1,0,0},{1.0,0.2,0.2}, 25)
green = gr.material({0.1,0.9,0.1},{0.2,1.0,0.2}, 25)
blue  = gr.material({0,0.1,1},{0.2,0.2,1.0}, 15)

pink  = gr.material({1,0.2,0.2},{1.0,0.2,0.2}, 25)
orange= gr.material({1,0.6,0},{0.7,0.4,0.2}, 25)
purple= gr.material({0.6,0.2,1},{0.2,0.2,0.2},25)
magenta=gr.material({0.8,0,0.4},{0.2,0.3,0.1},25)
yellow= gr.material({1,0.8,0.1},{0.8,0.8,0.8},25)
floor = gr.material({0.8,0.8,0.8},{0,0,0}, 1, 0.7)

-------------------
-- Algebraics

torus       = gr.algebraic('torus', '(x^2 + y^2 + z^2 + 0.75^2 - 0.25^2)^2 - 4*0.75^2*(x^2 + y^2)', 0.75+0.25)
limacon     =  gr.algebraic('limacon', '(x^2 + y^2 + z^2 - x)^2 - (x^2 + y^2 + z^2)', 2)
Blemnisgate = gr.algebraic('lemniscate of Bernoulli', '(x^2 + y^2 + z^2)^2 - 2*(x^2 - y^2 - z^2)', 1.4143)
Glemnisgate = gr.algebraic('lemniscate of Gerono', 'x^4 - x^2 + y^2 + z^2', 1)
hippopede   = gr.algebraic('hippopede', '(x^2 + y^2 + z^2)^2 - 4*(x^2 + 0.2*(y^2 + z^2))', 2)
deltoid     = gr.algebraic('deltoid', '(x^2 + y^2 + z^2)^2 + 18*(x^2 + y^2 + z^2) - 27 - 8*(x^3 - 3*x*(y^2 + z^2))', 3)
bean        = gr.algebraic('bean', 'x^4 + x^2*(y^2 + z^2) + (y^2 + z^2)^2 - x*(x^2 + y^2 + z^2)', 2)
bicuspid    = gr.algebraic('bicuspid', '(x^2 - 1)*(x - 1)^2 + (y^2 + z^2 - 1)^2', 2)
kummer      = gr.algebraic('kummer',
  '(x^2 + y^2 + z^2 - (1.5))^2 - (21/9)*(1-z-1.4142*x)*(1-z+1.4142*x)*(1+z-1.4142*y)*(1+z+1.4142*y)', 2)

ci = gr.algebraic('infinite cone', 'x^2 + z^2 - (y - 1)^2', 1)
ci:set_material(purple)
hp = gr.algebraic('halfcut plane', '(y - 0.5)^2 - 0.5^2', 2)
hp:set_material(purple)
cone = gr.csg('cone', ci, '*', hp, 1)

-------------------
-- Placement

bicuspid:set_material(blue)
bicuspid:rotate('Z', 90)
bicuspid:translate(-3,1,7)
scene:add_child(bicuspid)

limacon:set_material(pink)
limacon:rotate('Z', -30)
limacon:rotate('Y', 140)
limacon:translate(-5,1.5,4)
scene:add_child(limacon)

torus:set_material(orange)
torus:scale(8,8,8)
torus:translate(0,0,-3)
scene:add_child(torus)

kummer:set_material(green)
ksphere = gr.sphere('kummer bounds')
ksphere:set_material(green)
ksphere:scale(2,2,2)
dk = gr.csg('display kummer', kummer, '*', ksphere, 4)
dk:rotate('X', 90)
dk:rotate('Y', 20)
dk:translate(0,2,3)
scene:add_child(dk)

cone:rotate('Z', 180)
cone:translate(4,1,3)
cone:scale(1,3,1)
scene:add_child(cone)

Blemnisgate:set_material(magenta)
Blemnisgate:rotate('Z',-20)
Blemnisgate:rotate('Y',-25)
Blemnisgate:translate(4,3.8,3)
scene:add_child(Blemnisgate)

deltoid:set_material(yellow)
deltoid:rotate('Z',90)
deltoid:scale(0.5,0.5,0.5)
deltoid:translate(2.5,0.75,6)
scene:add_child(deltoid)

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
plane:set_material(floor)
plane:scale(30, 30, 30)

-------------------
-- Render

white_light = gr.light({-100.0, 150.0, 100.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
gr.render(scene, 'algebraics.png', 1200, 900,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

