map {
  _filename = "Level1_floor.png"
}

texture {
  _name = "floor1_tex"
  _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_917.png"
}

texture {
  _name = "floor2"
  _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_918.png"
}

texture {
  _name = "floor3"
  _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_96.png"
}

texture {
  _name = "floor4"
  _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_94.png"
}

texture {
  _name = "floor5"
  _filename = "tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_385.png"
}

tile {
  _name = "floor1"
  _texture = "floor1_tex"
}

tile {
  _name = "floor2"
  _texture = "floor2"
}


tile {
  _name = "floor3"
  _texture = "floor3"
}
tile {
  _name = "floor4"
  _texture = "floor4"
}
tile {
  _name = "floor5"
  _texture = "floor5"
}

entity {
  _type = "Floor"
  _tile = "floor1"
  _patterned = true
  _colorBinding = #000000
}

entity {
  _type = "Floor"
  _tile = "floor2"
  _patterned = true
  _colorBinding = #1a1a1a
}

entity {
  _type = "Floor"
  _tile = "floor3"
  _patterned = true
  _colorBinding = #303030
}

entity {
  _type = "Floor"
  _tile = "floor4"
  _patterned = true
  _colorBinding = #404040
}

entity {
  _type = "Floor"
  _tile = "floor5"
  _patterned = true
  _colorBinding = #505050
}



