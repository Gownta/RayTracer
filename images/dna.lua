-- DNA

scene = gr.node('scene')
math.randomseed(95864752)

-------------------
-- Material

red   = gr.material({1,0,0},{1.0,0.2,0.2}, 25)
green = gr.material({0.1,0.9,0.1},{0.2,1.0,0.2}, 25)
blue  = gr.material({0,0.1,1},{0.2,0.2,1.0}, 15)

pink  = gr.material({1,0.2,0.2},{1.0,0.2,0.2}, 25)
orange= gr.material({1,0.6,0},{0.7,0.4,0.2}, 25)
purple= gr.material({0.6,0.2,1},{0.2,0.2,0.2},25)
magenta=gr.material({0.8,0,0.4},{0.2,0.3,0.1},25)
yellow= gr.material({1,0.8,0.1},{0.8,0.8,0.8},25)
floor = gr.material({0.8,0.8,0.8},{0,0,0}, 1, 0.7)

lb = gr.material({0,0.4,0.8},{0.1,0.5,0.7},25)
db = gr.material({0,0.2,0.6},{0.1,0.5,0.7},25)

-------------------
-- Cylinders

ric = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
ric:set_material(red)
rbp = gr.algebraic('biplane', 'y^2 - 1', 2)
rbp:set_material(red)
rcyl = gr.csg('cylinder', ric, '*', rbp, 1.42)

gic = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
gic:set_material(green)
gbp = gr.algebraic('biplane', 'y^2 - 1', 2)
gbp:set_material(green)
gcyl = gr.csg('cylinder', gic, '*', gbp, 1.42)

bic = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
bic:set_material(blue)
bbp = gr.algebraic('biplane', 'y^2 - 1', 2)
bbp:set_material(blue)
bcyl = gr.csg('cylinder', bic, '*', bbp, 1.42)

yic = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
yic:set_material(yellow)
ybp = gr.algebraic('biplane', 'y^2 - 1', 2)
ybp:set_material(yellow)
ycyl = gr.csg('cylinder', yic, '*', ybp, 1.42)

-------------------
-- Helix

helix = gr.node('helix')

dir  = 0
ddir = 20
h    = 15
dh   = 1.8
sep  = 3
w    = 0.15

for i = -h,h,dh do
  line = gr.node('line')

  b1 = gr.sphere('b1')
  b1:set_material(db)
  b1:translate(sep,0,0)
  line:add_child(b1)

  b2 = gr.sphere('b2')
  b2:set_material(lb)
  b2:translate(-sep,0,0)
  line:add_child(b2)

  col = math.random(4)
  q1 = gr.node('q1')
  q2 = gr.node('q2')
  if col == 1 then 
    q1:add_child(gcyl)
    q2:add_child(rcyl)
  end
  if col == 2 then 
    q1:add_child(rcyl)
    q2:add_child(gcyl)
  end
  if col == 3 then
    q1:add_child(ycyl)
    q2:add_child(bcyl)
  end
  if col == 4 then
    q1:add_child(bcyl)
    q2:add_child(ycyl)
  end

  q1:translate(0,1,0)
  q1:scale(w,sep/2,w)
  q1:rotate('Z', 90)
  line:add_child(q1)

  q2:translate(0,1,0)
  q2:scale(w,sep/2,w)
  q2:rotate('Z',-90)
  line:add_child(q2)

  line:translate(0,i,0)
  line:rotate('Y', dir)
  dir = dir + ddir

  helix:add_child(line)
end

-------------------
-- Render

helix:rotate('Z',-90)
scene:add_child(helix)
scene:translate(0,0,-30)

white_light = gr.light({2,2,1}, {1.0, 1.0, 1.0}, {1, 0, 0})
gr.render(scene, 'dna.png', 512, 192,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

