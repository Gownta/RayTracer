-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')

simples = {}
D = 40
S = 10

require('dodeca')
require('icosa')

simples[1] = gr.sphere('sphere')
simples[2] = gr.cube('cube')
simples[3] = dodeca
simples[4] = icosa
simples[5] = gr.algebraic('torus', '(x^2 + y^2 + z^2 + 0.75^2 - 0.25^2)^2 - 4*0.75^2*(x^2 + y^2)', 0.75+0.25)
simples[6] = gr.algebraic('limacon', '(x^2 + y^2 + z^2 - x)^2 - (x^2 + y^2 + z^2)', 2)
simples[7] = gr.algebraic('lemniscate of Bernoulli', '(x^2 + y^2 + z^2)^2 - 2*(x^2 - y^2 - z^2)', 1.4143)
simples[8] = gr.algebraic('lemniscate of Gerono', 'x^4 - x^2 + y^2 + z^2', 1)
simples[9] = gr.algebraic('hippopede', '(x^2 + y^2 + z^2)^2 - 4*(x^2 + 0.2*(y^2 + z^2))', 2)
simples[10]= gr.algebraic('deltoid', '(x^2 + y^2 + z^2)^2 + 18*(x^2 + y^2 + z^2) - 27 - 8*(x^3 - 3*x*(y^2 + z^2))', 3)
                simples[10]:scale(0.3, 0.3, 0.3)
simples[11]= gr.algebraic('bean', 'x^4 + x^2*(y^2 + z^2) + (y^2 + z^2)^2 - x*(x^2 + y^2 + z^2)', 2)
simples[12]= gr.algebraic('bicuspid', '(x^2 - 1)*(x - 1)^2 + (y^2 + z^2 - 1)^2', 2)
                simples[12]:rotate('Y', -90)
n = table.getn(simples)

for i = 1, n do
  elem = simples[i]

  elem:scale(S, S, S)
  elem:set_material(mat1)
  
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
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'primitives.png', (n+1)*D*4, (4+1)*D*4,
	  {(n+1)*D/2, 5*D/2, (n+1)*D*1.07225}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
