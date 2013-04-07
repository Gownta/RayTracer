-- random points on the unit sphere

require('ruv')
ruv = random_unit_vectors

scene = gr.node('scene')
scene:translate(0,0,-3)

mat = gr.material({0,0,1}, {.2,.2,.8}, 15)

s = 0.01
for i = 1,1000 do
  pt = gr.sphere('pt')
  pt:scale(s,s,s)
  pt:translate(ruv[i][1], ruv[i][2], ruv[i][3])
  pt:set_material(mat)
  scene:add_child(pt)
end

gr.render(scene,
	  'randoms.png', 1024, 1024,
          {0,0,0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {gr.light({-30,40,-30}, {0.8, 0.8, 0.8}, {1,0,0}),
                            gr.light({0, 10, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})})

