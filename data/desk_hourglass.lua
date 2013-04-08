
function get_hourglass()
  -------------------
  -- Materials

  glass = gr.optics_material(1.5,0.05,{0.8,0.8,1.0})
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

  return hourglass
end
