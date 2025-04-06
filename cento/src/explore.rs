use crate::*;

use geo::coord;

impl Plane {
    pub fn empty(&self, area: Rect) -> bool {
        // 1. Use the point finding algorithm to locate the tile containing the
        //    lower-left corner of the area of interest.
        let mut here = Point::new(area.min().x, area.max().y - 1);
        let mut key = self.find_tile_at(here);

        let mut here_y = here.y() as i64;
        while here_y >= area.min().y as i64 {
            let t = tile!(self, key);
            // 2. See if the tile is solid. If not, it must be a space tile. See if
            //    its right edge is within the area of interest. If so, either it is
            //    the edge of the layout or the edge of a solid tile.
            if t.is_solid() && t.max_x() > area.min().x {
                return false;
            }

            // 3. If a solid tile was found in step 2, then the search is complete.
            //    If no solid tile was found, then move upwards to the next tile
            //    touching the left edge of the area of interest. This can be done
            //    either by invoking the point-finding algorithm, or by traversing
            //    the lt stitch upwards and then traversing the br stitches right
            //    until the desired tile is found.
            if t.max_x() < area.max().x {
                let right = t.stitches.right.unwrap();
                let r = tile!(self, right);
                if r.is_solid() || (r.max_x() < area.max().x) {
                    return false;
                }
            }

            here_y = t.min_y() as i64 - 1;
            match i32::try_from(here_y) {
                Ok(y) => here.set_y(y),
                _ => break,
            };
            key = self.find_tile_at(here);

            // 4. Repeat steps 2 and 3 until either a solid tile is found or the top
            //    of the area of interest is reached.
        }

        return true;
    }

    fn enumerate_right(
        &self,
        enum_key: TileKey,
        enum_bottom: i32,
        area: &Rect,
        callback: &mut impl FnMut(TileKey) -> bool,
    ) -> bool {
        let at_bottom = enum_bottom <= area.min().y;

        // Begin examination of tiles along right edge.
        // A tile to the right of the one being enumerated is enumerable if:
        //  - its bottom lies at or above that of the tile being enumerated, or,
        //  - the bottom of the tile being enumerated lies at or below the
        //    bottom of the search rectangle.

        let search_bottom = if enum_bottom < area.min().y {
            area.min().y
        } else {
            enum_bottom
        };

        // We keep a copy of the tiles being enumerated incase they get freed by
        // the callback function.
        let (mut t, mut tile) = key_tile!(self, enum_key);
        let mut next_top = tile.max_y();

        while next_top > search_bottom {
            // Since the client's filter function may result in this tile
            // being deallocated or otherwise modified, we must extract
            // all the information we will need from the tile before we
            // apply the filter function.

            let (b, below) = key_tile!(self, tile.below().unwrap());
            next_top = below.max_y();

            if (tile.min_y() < area.max().y) && (at_bottom || (tile.min_y() >= enum_bottom)) {
                if callback(t) {
                    return true;
                }

                if tile.max_x() < area.max().x {
                    if self.enumerate_right(tile.right().unwrap(), tile.min_y(), area, callback) {
                        return true;
                    }
                }
            }

            (t, tile) = (b, below);
        }

        false
    }

    pub fn query(&self, area: &Rect, callback: &mut impl FnMut(TileKey) -> bool) {
        let mut here = Point::new(area.min().x, area.max().y - 1);
        let mut key = self.find_tile_at(here);

        let mut here_y = here.y() as i64;
        while here_y >= area.min().y as i64 {
            let tile: Tile = tile!(self, key);

            // Find the tile (tp) immediately below the one to be
            // enumerated (enumTile).  This must be done before we enumerate
            // the tile, as the filter function applied to enumerate
            // it can result in its deallocation or modification in
            // some other way.
            //
            // We also have to be sure we do not overflow from the infinity tile

            here_y = tile.min_y() as i64 - 1;
            match i32::try_from(here_y) {
                Ok(y) => here.set_y(y),
                _ => here.set_y(i32::MIN),
            };
            let next: TileKey = self.find_tile_at(here);

            if callback(key) {
                return;
            }

            // If the right boundary of the tile being enumerated is
            // inside of the search area, recursively enumerate
            // tiles to its right.

            if tile.max_x() < area.max().x {
                if self.enumerate_right(tile.right().unwrap(), tile.min_y(), area, callback) {
                    return;
                }
            }
            key = next;
        }
    }

    pub fn query_all(&self, callback: &mut impl FnMut(TileKey) -> bool) {
        let area = Rect::new(
            coord! { x: i32::MIN, y: i32::MIN },
            coord! { x: i32::MAX, y: i32::MAX },
        );
        self.query(&area, callback)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use geo::coord;

    mod empty {
        use super::*;

        #[test]
        fn universe() {
            let plane = Plane::new();
            assert!(plane.empty(Rect::new(
                coord! { x: -512, y: -512},
                coord! {x: 512, y:512}
            )));
        }
    }

    mod query_all {
        use super::*;

        #[test]
        fn universe() {
            let plane = Plane::new();

            let mut c = 0;
            let mut callback = |_t| { c += 1; false };

            plane.query_all(&mut callback);

            assert_eq!(c, 1);
        }
    }
}
