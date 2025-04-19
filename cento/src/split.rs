use crate::*;

pub struct HorzSplit {
    pub upper: TileKey,
    pub lower: TileKey,
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
        while other_key.is_some() {
            let other = unsafe { slots.get_unchecked_mut(other_key?) };
            if other.stitches.below != Some(lkey) {
                break;
            }

            other.stitches.below = Some(ukey);
            other_key = other.stitches.left;
        }

        // adjust corner stitches along right edge
        other_key = lower.right();
        while other_key.is_some() {
            let other = unsafe { slots.get_unchecked_mut(other_key?) };
            if other.min_y() < y {
                break;
            }

            other.stitches.left = Some(ukey);
            other_key = other.stitches.below;
        }

        // adjust corner stitches along left edge
        other_key = lower.left();
        while other_key.is_some() {
            let other = unsafe { slots.get_unchecked_mut(other_key?) };
            if other.max_y() > y {
                break;
            }
            other_key = other.stitches.above;
        }

        upper.stitches.left = other_key;
        while other_key.is_some() {
            let other = unsafe { slots.get_unchecked_mut(other_key?) };
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
            upper: ukey,
            lower: lkey,
        })
    }
}
