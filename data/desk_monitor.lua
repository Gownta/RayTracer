-- Monitor

function make_monitor(img, w, h)
  monitor_z = 0.3
  monitor_b = 1
  monitor_w = 36
  monitor_h = monitor_w * h / w

  matte = gr.material({0,0,0},{0.2,0.2,0.2},15,0.2)
  bottom = monitor_h / 2 + monitor_b

  monitor_points = {}
  for z = 0,1 do
    for h = -1,1,2 do
      for w = -1,1,2 do
        for b = 0,1 do
          dw = (monitor_w / 2 + b * monitor_b) * w
          dh = (monitor_h / 2 + b * monitor_b) * h
          dz = z
          table.insert(monitor_points, { dw, dh, dz })
        end
      end
    end
  end

  monitor_faces = {
    {0,1,3,2},
    {8,9,11,10},
    {2,3,7,6},
    {10,11,15,14},
    {6,7,5,4},
    {14,15,13,12},
    {4,5,1,0},
    {12,13,9,8},

    {0,2,10,8},
    {1,3,11,9},
    {2,6,14,10},
    {3,7,15,11},
    {6,4,12,14},
    {7,5,13,15},
    {4,0,8,12},
    {5,1,9,13}
  }

  border = gr.mesh('border', monitor_points, monitor_faces)
  border:set_material(matte)

  screen = gr.mesh('screen',
    {
      monitor_points[1],
      monitor_points[3],
      monitor_points[7],
      monitor_points[5],
    }, {}, {
      {0,0,0},
      {w-1,0,0},
      {w-1,h-1,0},
      {0,h-1,0}
    }, {{
      {0,1,2,3}, {}, {3,2,1,0}
    }})
  screen:set_material(gr.texture_material(img, {0,0,0}, 1))

  stand = gr.cube('stand stick')
  stand:translate(-0.5,-1,0)
  stand:scale(3,3*1.4,monitor_z / 2)
  stand:translate(0,-bottom,0)
  bottom = bottom + 3*1.4
  stand:set_material(matte)

  ic = gr.algebraic('infinite cylinder', 'x^2 + z^2 - 1', 2)
  bp = gr.algebraic('biplane', 'y^2 - 1', 2)
  ic:set_material(matte)
  bp:set_material(matte)
  cylinder = gr.csg('cylinder', ic, '*', bp, 1.42)
  cylinder:translate(0,-1,0)
  cylinder:scale(8,0.1,8)
  cylinder:translate(0,-bottom,0)
  bottom = bottom + 0.2
  base = cylinder

  monitor = gr.node('scene')
  monitor:add_child(border)
  monitor:add_child(screen)
  monitor:add_child(stand)
  monitor:add_child(base)
  monitor:translate(0,bottom,0)
  return monitor
end

