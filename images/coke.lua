-- Coca Cola

scene = gr.node('scene')

-------------------
-- Materials

silver= gr.material({0.6,0.6,0.6},{0.9,0.9,0.9},25)

-------------------
-- Can

v = {}
n = {}
t = {}
f = {}

tw = 1400
th = 740

h    = 2*1.618
diph = 0.2
dipa = 25 * (2 * 3.1415926 / 360)
dipw = 1 - math.tan(dipa)*diph

divs = 36
for i = 0,divs do
  angle = i * (2 * 3.14159265) / divs
  ca = math.cos(angle)
  sa = math.sin(angle)

  table.insert(v, {ca,0,sa})
  table.insert(v, {ca,h-diph-0.05,sa})
  table.insert(v, {ca,h-diph,sa})
  table.insert(v, {ca*dipw,h,sa*dipw})

  table.insert(n, {ca,0,sa})
  table.insert(n, {ca,0,sa})
  table.insert(n, {ca*ca,sa,sa*ca})
  table.insert(n, {ca*ca,sa,sa*ca})

  table.insert(t, {i/divs*tw,0,0})
  table.insert(t, {i/divs*tw,(1 - (diph + 0.05) / h)*th,0})
  table.insert(t, {i/divs*tw,(1 - diph / h)*th,0})
  table.insert(t, {i/divs*tw,th,0})
end

for j = 0,divs-1 do
  i = 4*j
  table.insert(f, {{i+0,i+1,i+5,i+4},{i+0,i+1,i+5,i+4},{i+0,i+1,i+5,i+4}})
  table.insert(f, {{i+1,i+2,i+6,i+5},{i+1,i+2,i+6,i+5},{i+1,i+2,i+6,i+5}})
  table.insert(f, {{i+2,i+3,i+7,i+6},{i+2,i+3,i+7,i+6},{i+2,i+3,i+7,i+6}})
end

coke = gr.mesh('can of coke', v, n, t, f)
coke:set_material(gr.texture_material('coke.png', {0,0,0}, 1))

blip = gr.algebraic('torus', '(x^2 + y^2 + z^2 + (1-0.1)^2 - (0.1)^2)^2 - 4*(1-0.1)^2*(x^2 + y^2)', 1)
blip:set_material(silver)
blip:scale(0.99,.99,.99)
blip:rotate('X',90)

tlip = gr.algebraic('torus', '(x^2 + y^2 + z^2 + (1-0.05)^2 - (0.05)^2)^2 - 4*(1-0.05)^2*(x^2 + y^2)', 1)
tlip:set_material(silver)
tlip:rotate('X',90)
tlip:scale(dipw,dipw,dipw)
tlip:translate(0,h,0)

ctv = {}
ctt = {}
col = {}
for i = 1,divs do
  angle = i * (2 * 3.14159265) / divs
  ca = math.cos(angle)
  sa = math.sin(angle)
  table.insert(ctv, {ca,0,sa})
  table.insert(ctt, {200+ca*175,200+sa*175,0})
  table.insert(col, i-1)
end
coketop = gr.mesh('coketop', ctv, {}, ctt, {{col,{},col}})
coketop:set_material(gr.texture_material('coketop.png', {0.8,0.8,0.8},25))
coketop:scale(dipw,dipw,dipw)
coketop:translate(0,h-0.05,0)

can = gr.node('can of coke')
can:translate(0,0.1,0)
can:add_child(coke)
can:add_child(blip)
can:add_child(tlip)
can:add_child(coketop)

-------------------
-- the floor

ice = gr.mesh('ice',
  { -- vertices
    {-5, 0, -3.75},
    { 5, 0, -3.75},
    { 5, 0,  3.75},
    {-5, 0,  3.75},
  }, { -- normals
  }, { -- textures
    {0,0,0},
    {500-1,0,0},
    {500-1,375-1,0},
    {0,375-1,0},
  }, { -- faces
    {
      {0,1,2,3},
      {},
      {0,1,2,3}
    }
  })
ice:set_material(gr.texture_material('ice.png', {0,0,0}, 1))
ice:translate(0,0,-1)
scene:add_child(ice)

-------------------
-- Render

can1 = gr.node('can1')
can1:add_child(can)
can1:translate(-2,0,0)
scene:add_child(can1)

can2 = gr.node('can2')
can2:add_child(can)
can2:rotate('Y',20)
can2:rotate('X',-90)
can2:rotate('Y',60)
can2:translate(2.5,1,1)
scene:add_child(can2)

scene:rotate('X',30)
scene:translate(0,-1.5,-8)

gr.render(scene,
	  'coke.png', 1024, 576,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({200, 202, 430}, {0.8, 0.8, 0.8}, {1, 0, 0})})
