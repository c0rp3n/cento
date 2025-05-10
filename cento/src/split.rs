use crate::*;

pub struct HorzSplit {
    pub lower: TileKey,
    pub upper: TileKey,
}

pub struct VertSplit {
    pub left: TileKey,
    pub right: TileKey,
}

impl Plane {
    pub fn split_horz(&mut self, key: TileKey, y: i32) -> Option<HorzSplit> {
        let slots = self.slots_mut();

        let lkey = key;
        let mut lower: Tile = unsafe { *slots.get_unchecked(lkey) };

        if lower.min_y() >= y || lower.max_y() <= y {
            return None;
        }

        let ukey = slots.insert(Tile::new(
            Rect::new(
                Point::new(lower.min_x(), y),
                Point::new(lower.max_x(), lower.max_y()),
            ),
            None,
        ));
        let mut upper: Tile = unsafe { *slots.get_unchecked(ukey) };

        // update bounds
        lower.bounds.set_max(Point::new(lower.max_x(), y));

        // update stitches
        upper.stitches.below = Some(lkey);
        upper.stitches.above = lower.stitches.above;
        upper.stitches.right = lower.stitches.right;

        let mut other_key;

        // adjust corner stitches along top edge
        other_key = lower.above();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.below != Some(lkey) {
                break;
            }

            other.stitches.below = Some(ukey);
            other_key = other.stitches.left;
        }
        lower.stitches.above = Some(ukey);

        // adjust corner stitches along right edge
        other_key = lower.right();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.min_y() < y {
                break;
            }

            other.stitches.left = Some(ukey);
            other_key = other.stitches.below;
        }
        lower.stitches.right = other_key;

        // adjust corner stitches along left edge
        other_key = lower.left();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.max_y() > y {
                break;
            }
            other_key = other.stitches.above;
        }

        upper.stitches.left = other_key;
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.right() != Some(lkey) {
                break;
            }

            other.stitches.right = Some(ukey);
            other_key = other.stitches.above;
        }

        // update the data in the slot map...
        unsafe { *slots.get_unchecked_mut(ukey) = upper };
        unsafe { *slots.get_unchecked_mut(lkey) = lower };

        Some(HorzSplit {
            lower: lkey,
            upper: ukey,
        })
    }

    pub fn split_vert(&mut self, key: TileKey, x: i32) -> Option<VertSplit> {
        let slots = self.slots_mut();

        let lkey = key;
        let mut left: Tile = unsafe { *slots.get_unchecked(lkey) };

        if left.min_x() >= x || left.max_x() <= x {
            return None;
        }

        let rkey = slots.insert(Tile::new(
            Rect::new(
                Point::new(left.min_x(), left.min_y()),
                Point::new(x, left.max_y()),
            ),
            None,
        ));
        let mut right: Tile = unsafe { *slots.get_unchecked(rkey) };

        // update bounds
        left.bounds.set_max(Point::new(x, left.max_y()));

        // update stitches
        right.stitches.left = Some(lkey);
        right.stitches.right = left.stitches.right;
        right.stitches.above = left.stitches.above;

        let mut other_key;

        // adjust corner stitches along the right edge
        other_key = left.right();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.below != Some(lkey) {
                break;
            }

            other.stitches.left = Some(rkey);
            other_key = other.stitches.below;
        }
        left.stitches.right = Some(rkey);

        // adjust corner stitches along the top edge
        other_key = left.above();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.min_x() < x {
                break;
            }

            other.stitches.below = Some(rkey);
            other_key = other.stitches.left;
        }
        left.stitches.above = other_key;

        // adjust corner stitches along the bottom edge
        other_key = left.below();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.min_x() > x {
                break;
            }
            other_key = other.stitches.right;
        }

        right.stitches.below = other_key;
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.above() != Some(lkey) {
                break;
            }

            other.stitches.above = Some(rkey);
            other_key = other.stitches.right;
        }

        // update the data in the slot map...
        unsafe { *slots.get_unchecked_mut(lkey) = left };
        unsafe { *slots.get_unchecked_mut(rkey) = right };

        Some(VertSplit {
            left: lkey,
            right: rkey,
        })
    }

    pub fn split(&mut self, key: TileKey, r: Rect) -> TileKey {
        let mut ret = key;
        if let Some(split) = self.split_horz(ret, r.max().y) {
            ret = split.lower;
        }
        if let Some(split) = self.split_horz(ret, r.min().y) {
            ret = split.upper;
        }
        if let Some(split) = self.split_vert(ret, r.max().x) {
            ret = split.left;
        }
        if let Some(split) = self.split_vert(ret, r.min().x) {
            ret = split.right;
        }
        ret
    }
}
