-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.8, 0.8, 0.8}, {0.2, 0.2, 0.2}, 25)
mat2 = gr.material({0.8, 0.2, 0.2}, {0.2, 0.2, 0.2}, 25)

-----

scene = gr.node('root')

s1 = gr.sphere('s1')
s1:translate( 0.5, 0, 0)
s1:set_material(mat2)
s2 = gr.sphere('s2')
s2:translate(-0.5, 0, 0)
s2:set_material(mat1)

ss = gr.csg('bubble', s1, '-', s2, 2)

ss:rotate('Y', 60)

--scene:add_child(s1)
--scene:add_child(s2)
scene:add_child(ss)

-----

light1 = gr.light({-50.0, 0.0, 0.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
light2 = gr.light({0.0, 0.0, 50.0},  {0.9, 0.2, 0.2}, {1, 0, 0})

gr.render(scene, 'primitives.png', 256, 256,
	  {0, 0, 5}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {light1})

          --[[
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')

simples = {}
D = 40
S = 10

s1 = gr.sphere('s1')
s1:translate( 0.5, 0, 0)
s1:set_material(mat1)
s2 = gr.sphere('s2')
s2:translate(-0.5, 0, 0)
s2:set_material(mat3)

--simples[1] = gr.csg('bubble', s1, '+', s2, 2)
--simples[2] = gr.csg('bubble', s1, '*', s2, 2)
simples[1] = gr.csg('bubble', s1, '-', s2, 2)

n = table.getn(simples)

for i = 1, n do
  elem = simples[i]

  elem:scale(S, S, S)
  
  displays = {}
  for j = 1, 4 do
    displays[j] = gr.node('d')
  end

  displays[2]:rotate('X', 25)
  displays[3]:rotate('Y', 25)
  displays[4]:rotate('Z', 25)

  for j = 1, 4 do
    displays[j]:add_child(elem)
    displays[j]:translate(D * i, D * (5 - j), 0)
    scene_root:add_child(displays[j])
  end
end

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
red_light = gr.light({-50.0, 0.0, 0.0}, {1.0, 0.1, 0.1}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'primitives.png', (n+1)*D*4, (4+1)*D*4,
	  {(n+1)*D/2, 5*D/2, (n+1)*D*1.07225}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, red_light, orange_light})
          --]]
