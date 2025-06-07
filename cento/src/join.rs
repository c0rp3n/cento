use crate::*;

impl Plane {
    pub(crate) fn join_horz(&mut self, lkey: TileKey, rkey: TileKey) -> TileKey {
        let slots = self.slots_mut();

        let mut left: Tile = unsafe { *slots.get_unchecked(lkey) };
        let right: Tile = unsafe { *slots.get_unchecked(rkey) };

        assert_eq!(left.min_y(), right.min_y());
        assert_eq!(left.max_y(), right.max_y());
        assert_eq!(left.max_x(), right.min_x());

        let mut other_key;

        // adjust corner stitches along the top edge
        other_key = right.above();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.below != Some(rkey) {
                break;
            }

            other.stitches.below = Some(lkey);
            other_key = other.stitches.left;
        }

        // adjust corner stitches along the bottom edge
        other_key = right.below();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.above != Some(rkey) {
                break;
            }

            other.stitches.above = Some(lkey);
            other_key = other.stitches.right;
        }

        // adjust corner stitches along the right edge
        other_key = right.right();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.left != Some(rkey) {
                break;
            }

            other.stitches.left = Some(lkey);
            other_key = other.stitches.below;
        }

        left.bounds
            .set_max(Point::new(right.max_x(), left.bounds.max().y));
        left.stitches.right = right.stitches.right;
        left.stitches.above = right.stitches.above;

        unsafe { *slots.get_unchecked_mut(lkey) = left };
        slots.remove(rkey);

        lkey
    }

    pub(crate) fn join_vert(&mut self, lkey: TileKey, ukey: TileKey) -> TileKey {
        let slots = self.slots_mut();

        let mut lower: Tile = unsafe { *slots.get_unchecked(lkey) };
        let upper: Tile = unsafe { *slots.get_unchecked(ukey) };

        assert_eq!(lower.min_x(), upper.min_x());
        assert_eq!(lower.max_x(), upper.max_x());
        assert_eq!(lower.max_y(), upper.min_y());

        let mut other_key;

        // adjust corner stitches along the right edge
        other_key = upper.right();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.left != Some(ukey) {
                break;
            }

            other.stitches.left = Some(lkey);
            other_key = other.stitches.below;
        }

        // adjust corner stitches along the left edge
        other_key = upper.left();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.right != Some(ukey) {
                break;
            }

            other.stitches.right = Some(lkey);
            other_key = other.stitches.above;
        }

        // adjust corner stitches along the top edge
        other_key = upper.above();
        while let Some(okey) = other_key {
            let other = unsafe { slots.get_unchecked_mut(okey) };
            if other.stitches.below != Some(ukey) {
                break;
            }

            other.stitches.below = Some(lkey);
            other_key = other.stitches.left;
        }

        lower
            .bounds
            .set_max(Point::new(upper.max_x(), upper.max_y()));
        lower.stitches.right = upper.stitches.right;
        lower.stitches.above = upper.stitches.above;

        unsafe { *slots.get_unchecked_mut(lkey) = lower };
        slots.remove(ukey);

        lkey
    }
}
