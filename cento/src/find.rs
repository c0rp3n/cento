use crate::*;

impl Plane {
    pub fn find_tile_from(&self, start: TileKey, point: Point) -> TileKey {
        let (mut t, mut tile) = key_tile!(self, start);

        // 1. First move up (or down) along the left edges of tiles until a tile
        //    is found whose vertical range contains the desired point.
        if point.y() < tile.min_y() {
            loop {
                (t, tile) = key_tile!(self, tile.below().unwrap());
                if point.y() >= tile.min_y() {
                    break;
                }
            }
        } else {
            while point.y() >= tile.max_y() {
                (t, tile) = key_tile!(self, tile.above().unwrap());
            }
        }

        // 2. Then move left (or right) along the bottom edges of tiles until a
        //    tile is found whose horizontal range contains the desired point.
        if point.x() < tile.min_x() {
            loop {
                while point.x() < tile.min_x() {
                    (t, tile) = key_tile!(self, tile.left().unwrap());
                }

                if point.y() < tile.max_y() {
                    break;
                }

                while point.y() < tile.max_y() {
                    (t, tile) = key_tile!(self, tile.above().unwrap());
                }

                if point.x() >= tile.min_x() {
                    break;
                }
            }

            // 3. Since the horizontal motion may have caused a vertical
            //    misalignment, steps 1 and 2 may have to be iterated several times
            //    to locate the tile containing the point.
        } else {
            while point.x() >= tile.max_x() {
                loop {
                    (t, tile) = key_tile!(self, tile.right().unwrap());
                    if point.x() < tile.max_x() {
                        break;
                    }
                }

                if point.y() >= tile.min_y() {
                    break;
                }

                loop {
                    (t, tile) = key_tile!(self, tile.below().unwrap());
                    if point.y() >= tile.min_y() {
                        break;
                    }
                }
            }
        }

        t
    }

    pub fn find_tile_at(&self, point: Point) -> TileKey {
        self.find_tile_from(self.hint, point)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use geo::coord;

    #[test]
    fn universe() {
        let plane = Plane::new();
        let t = plane.hint();

        assert_eq!(plane.find_tile_at(Point::new(0, 0)), t);
        assert_eq!(plane.find_tile_at(Point::new(-512, 0)), t);
        assert_eq!(plane.find_tile_at(Point::new(512, 0)), t);
        assert_eq!(plane.find_tile_at(Point::new(0, -512)), t);
        assert_eq!(plane.find_tile_at(Point::new(0, 512)), t);
    }

    #[test]
    fn axis() {
        //
        //         :
        //         :
        //    tl   :   tr
        //         :
        //         :
        // - - - - + - - - -
        //         :
        //         :
        //    bl   :   br
        //         :
        //         :
        //

        let tiles = vec![
            Tile::new(
                Rect::new(coord! { x: i32::MIN, y: 0}, coord! { x: 0, y: i32::MAX}),
                None,
            ),
            Tile::new(
                Rect::new(coord! { x: 0, y: 0}, coord! { x: i32::MAX, y: i32::MAX}),
                None,
            ),
            Tile::new(
                Rect::new(coord! { x: i32::MIN, y: i32::MIN}, coord! { x: 0, y: 0}),
                None,
            ),
            Tile::new(
                Rect::new(coord! { x: 0, y: i32::MIN}, coord! { x: i32::MAX, y: 0}),
                None,
            ),
        ];

        let (mut plane, keys) = unsafe { Plane::from_unchecked(tiles) };
        assert_eq!(keys.len(), 4);

        let tl = keys[0];
        let tr = keys[1];
        let bl = keys[2];
        let br = keys[3];

        stitches_mut!(plane, tl) = Stitches {
            left: None,
            below: Some(bl),
            right: Some(tr),
            above: None,
        };

        stitches_mut!(plane, tr) = Stitches {
            left: Some(tl),
            below: Some(br),
            right: None,
            above: None,
        };

        stitches_mut!(plane, bl) = Stitches {
            left: None,
            below: None,
            right: Some(br),
            above: Some(tl),
        };

        stitches_mut!(plane, br) = Stitches {
            left: Some(bl),
            below: None,
            right: None,
            above: Some(tr),
        };

        assert_eq!(plane.find_tile_at(Point::new(0, 0)), tr);
        assert_eq!(plane.find_tile_at(Point::new(-256, 256)), tl);
        assert_eq!(plane.find_tile_at(Point::new(256, 256)), tr);
        assert_eq!(plane.find_tile_at(Point::new(-256, -256)), bl);
        assert_eq!(plane.find_tile_at(Point::new(256, -256)), br);
    }
}
