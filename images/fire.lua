-- Fire refraction

scene = gr.node('scene')
scene:translate(0,1.9,-10)

-------------------
-- Lens - radius 0.5

f1 = 4
f2 = 4

r1 = math.sqrt(f1 * f1 + 0.5*0.5)
r2 = math.sqrt(f2 * f2 + 0.5*0.5)

glass = gr.optics_material(1.5,0.05,{0.8,0.8,1.0})
border = gr.material({0.2,0.1,0},{0,0,0},1)

c1 = gr.sphere('c1')
c1:set_material(glass)
c1:scale(r1,r1,r1)
c1:translate(0,0,-f1)

c2 = gr.sphere('c2')
c2:set_material(glass)
c2:scale(r2,r2,r2)
c2:translate(0,0, f2)

optic = gr.csg('optical glass', c1, '*', c2, 0.5)

edge = gr.algebraic('torus', '(x^2 + y^2 + z^2 + 0.53^2 - 0.05^2)^2 - 4*0.53^2*(x^2 + y^2)', 0.53+0.05)
edge:set_material(border)

lens = gr.node('lens')
lens:add_child(optic)
lens:add_child(edge)
lens:scale(1.5,1.5,1.5)
lens:translate(0,-2.1,4)

scene:add_child(lens)

-------------------
-- Glass balls

b1 = gr.sphere('ball')
b1:set_material(glass)
b1:translate(3,-2,1)

scene:add_child(b1)

-------------------
-- Prism

prism = gr.mesh('prism', {
  {0,0,0},
  {1,0,0},
  {0.5,0,0.866025},
  {0,2,0},
  {1,2,0},
  {0.5,2,0.866025} 
}, {
  {0,1,2},
  {3,4,5},
  {0,1,4,3},
  {1,2,5,4},
  {2,0,3,5}
})
prism:set_material(glass)

pris = gr.node('prism setup')
pris:add_child(prism)
pris:rotate('Y', 43)
pris:scale(1,1.05,1)
pris:translate(-3.7,-3.1,0.5)

scene:add_child(pris)

-------------------
-- Fire

fire = gr.mesh('fire',
  { -- vertices
    {-6, 4,-2},
    { 6, 4,-2},
    { 6,-4,-2},
    {-6,-4,-2},
  }, { -- normals
  }, { -- textures
    {0,0,0},
    {6000-1,0,0},
    {6000-1,4000-1,0},
    {0,4000-1,0},
  }, { -- faces
    {
      {0,1,2,3},
      {},
      {0,1,2,3}
    }
  })
fire:set_material(gr.texture_material('fire.png', {0,0,0}, 1))
scene:add_child(fire)

-------------------
-- Marble

marble = gr.mesh('fire',
  { -- vertices
    { 6,-3,-2},
    {-6,-3,-2},
    {-6,-3, 7},
    { 6,-3, 7},
  }, { -- normals
  }, { -- textures
    {0,0,0},
    {1032-1,0,0},
    {1032-1,774-1,0},
    {0,774-1,0},
  }, { -- faces
    {
      {0,1,2,3},
      {},
      {0,1,2,3}
    }
  })
marble:set_material(gr.texture_material('blue_marble.png', {0,0,0}, 1))
scene:add_child(marble)

-------------------
-- Render

white_light = gr.light({-3,10,-4}, {1.0, 1.0, 1.0}, {1, 0, 0})
gr.render(scene, 'refracting_fire.png', 1024, 448,
	  {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})

