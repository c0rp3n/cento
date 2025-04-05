use crate::*;

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
}
