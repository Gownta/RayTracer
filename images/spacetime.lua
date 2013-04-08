-- Hourglass

scene = gr.node('scene')

-------------------
-- Materials

glass = gr.optics_material(1.5,0.05,{0.8,0.8,1.0})
--brown = gr.material({0.3,0.2,0},{0,0,0},1)
brown = gr.material({0.2,0.2,0.2},{0.8,0.8,0.8},15)

-------------------
-- Cylinder

ic = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
ic:set_material(brown)
bp = gr.algebraic('biplane', 'y^2 - 1', 2)
bp:set_material(brown)
cylinder = gr.csg('cylinder', ic, '*', bp, 1.42)

-------------------
-- Bulb

lemnisgate = gr.algebraic('lemniscate of Gerono', 'x^4 - x^2 + y^2 + z^2', 1)
lemnisgate:set_material(glass)

outer = gr.node('outer')
outer:add_child(lemnisgate)

joiner = gr.sphere('joiner')
joiner:set_material(glass)
joiner:scale(0.1,0.1,0.1)

bulb = gr.csg('pre bulb', outer, '+', joiner, 1)
bulb:rotate('Z', 90)

-------------------
-- Hourglass

hourglass = gr.node('hourglass')

hourglass:add_child(bulb)

topcap = gr.node('top cap')
topcap:add_child(cylinder)
topcap:scale(0.6,0.05,0.6)
topcap:translate(0,1,0)
hourglass:add_child(topcap)

topring = gr.algebraic('torus', '(x^2 + y^2 + z^2 + (1)^2 - (0.02)^2)^2 - 4*(1)^2*(x^2 + y^2)', 1+0.02)
topring:set_material(brown)
topring:rotate('X', 90)
topring:scale(1,1/0.05,1)
topcap:add_child(topring)

lowcap = gr.node('top cap')
lowcap:add_child(topcap)
lowcap:translate(0,-2,0)
hourglass:add_child(lowcap)

peg1 = gr.node('peg 1')
peg1:add_child(cylinder)
peg1:scale(0.04, 1, 0.04)
peg1:translate(0.6-0.04-0.01,0,0)
hourglass:add_child(peg1)

pegbulge = gr.sphere('pegbulge 1')
pegbulge:scale(1/0.04,1,1/0.04)
pegbulge:scale(0.06,0.06,0.06)
pegbulge:set_material(brown)
peg1:add_child(pegbulge)

pb2 = gr.node('pegbulge 2')
pb2:add_child(pegbulge)
pb2:translate(0,0.1,0)
peg1:add_child(pb2)

pb3 = gr.node('pegbulge 2')
pb3:add_child(pegbulge)
pb3:translate(0,-0.1,0)
peg1:add_child(pb3)

peg2 = gr.node('peg 2')
peg2:add_child(peg1)
peg2:rotate('Y', 120)
hourglass:add_child(peg2)

peg3 = gr.node('peg 3')
peg3:add_child(peg1)
peg3:rotate('Y', 240)
hourglass:add_child(peg3)

-------------------
-- Enclosure

enclosure = gr.mesh('enclosure',
  { -- vertices
    {-3, 3, 3},
    { 3, 3, 3},
    { 3,-3, 3},
    {-3,-3, 3},
    {-3, 3,-3},
    { 3, 3,-3},
    { 3,-3,-3},
    {-3,-3,-3},
  }, { -- normals
  }, { -- textures
    {256,256,0},
    {512,256,0},
    {512,512,0},
    {256,512,0},
    {256,0,0},
    {512,0,0},
    {768,256,0},
    {768,512,0},
    {512,768,0},
    {256,768,0},
    {0,512,0},
    {0,256,0},
  }, { -- faces
    {{4,5,6,7},{},{0,1,2,3}},
    {{4,5,1,0},{},{0,1,5,4}},
    {{5,6,2,1},{},{1,2,7,6}},
    {{6,7,3,2},{},{2,3,9,8}},
    {{7,4,0,3},{},{3,0,11,10}},
    {{0,1,2,3},{},{0,1,2,3}},
  })
enclosure:set_material(gr.texture_material('space.png', {0,0,0}, 1))
enclosure:translate(-0.1,0.1,1.5)

-------------------
-- Render

scene:add_child(hourglass)
scene:add_child(enclosure)
scene:rotate('Z',33)
scene:translate(0.01,0.01,-3)

white_light = gr.light({2,2,1}, {1.0, 1.0, 1.0}, {1, 0, 0})
gr.render(scene, 'spacetime.png', 256, 256,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

