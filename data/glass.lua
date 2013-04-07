-- Glass

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(0,0,-10)

-------------------
-- Glass

crystal = gr.optics_material(2.5, 0.1, {0.8,0.8,1.0})
water   = gr.optics_material(1.66, 0.03, {0.8,0.8,1.0})
air     = gr.optics_material(1, 0, {1,1,1})
white   = gr.material({1,1,1},{0,0,0},1)

gic  = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
gbp  = gr.algebraic('biplane', 'y^2 - 1', 2)
gic:set_material(crystal)
gbp:set_material(crystal)
gcyl = gr.csg('cylinder', gic, '*', gbp, 1.42)

outer = gr.node('outer')
outer:add_child(gcyl)

inner = gr.node('inner')
inner:add_child(gcyl)
inner:scale(0.95,0.95,0.95)
inner:translate(0,0.06,0)

glass = gr.csg('glass', outer, '-', inner, 1.42)
glass:translate(0,0.99,0)


wic  = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
wbp  = gr.algebraic('biplane', 'y^2 - 1', 2)
wic:set_material(water)
wbp:set_material(water)
wcyl = gr.csg('cylinder', wic, '*', wbp, 1.42)

beverage = gr.node('water')
beverage:add_child(gcyl)
beverage:scale(0.94,0.7,0.94)
beverage:translate(0,0.75,0)


scene:add_child(glass)
scene:add_child(beverage)

-------------------
-- Table

table = gr.mesh('table',
    {
      {-1,0,-1},
      { 1,0,-1},
      { 1,0, 1},
      {-1,0, 1},
    }, {}, {
      {0,0,0},
      {3000-1,0,0},
      {3000-1,2139-1,0},
      {0,2139-1,0}
    }, {{
      {0,1,2,3}, {}, {3,2,1,0}
    }})
table:set_material(gr.texture_material('wood_veneer.png', {0,0,0}, 1))
table:scale(70,70,70)
scene:add_child(table)

gr.render(scene,
	  'glass.png', 200, 120,
          {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({-30,40,-30}, {0.8, 0.8, 0.8}, {1,0,0}),
                            gr.light({0, 10, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})})

