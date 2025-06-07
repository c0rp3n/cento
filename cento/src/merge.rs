use crate::*;

fn can_merge_horz(left: &Rect, right: &Rect) -> bool {
    assert_eq!(left.max().x, right.min().x);
    (left.min().y == right.min().y) && (left.max().y == right.max().y)
}

fn can_merge_vert(below: &Rect, above: &Rect) -> bool {
    assert_eq!(below.max().y, above.min().y);
    (below.min().x == above.min().x) && (below.max().x == above.max().x)
}

fn can_merge_body(lhs: Body, rhs: Body) -> bool {
    // we should probably end up with a Body trait?????
    lhs == rhs
}

impl Plane {
    pub fn merge_left(&mut self, key: TileKey) -> Option<TileKey> {
        let slots = self.slots();

        let tile: Tile = unsafe { *slots.get_unchecked(key) };

        if let Some(okey) = tile.left() {
            let other: Tile = unsafe { *slots.get_unchecked(okey) };
            if can_merge_horz(&other.bounds, &tile.bounds) && can_merge_body(tile.body, other.body)
            {
                return Some(self.join_horz(okey, key));
            }
        }

        None
    }

    pub fn merge_right(&mut self, key: TileKey) -> Option<TileKey> {
        let slots = self.slots();

        let tile: Tile = unsafe { *slots.get_unchecked(key) };

        if let Some(okey) = tile.left() {
            let other: Tile = unsafe { *slots.get_unchecked(okey) };
            if can_merge_horz(&tile.bounds, &other.bounds) && can_merge_body(tile.body, other.body)
            {
                return Some(self.join_horz(key, okey));
            }
        }

        None
    }

    pub fn merge_below(&mut self, key: TileKey) -> Option<TileKey> {
        let slots = self.slots();

        let tile: Tile = unsafe { *slots.get_unchecked(key) };

        if let Some(okey) = tile.below() {
            let other: Tile = unsafe { *slots.get_unchecked(okey) };
            if can_merge_vert(&other.bounds, &tile.bounds) && can_merge_body(tile.body, other.body)
            {
                return Some(self.join_vert(okey, key));
            }
        }

        None
    }

    pub fn merge_above(&mut self, key: TileKey) -> Option<TileKey> {
        let slots = self.slots();

        let tile: Tile = unsafe { *slots.get_unchecked(key) };

        if let Some(okey) = tile.above() {
            let other: Tile = unsafe { *slots.get_unchecked(okey) };
            if can_merge_vert(&tile.bounds, &other.bounds) && can_merge_body(tile.body, other.body)
            {
                return Some(self.join_vert(key, okey));
            }
        }

        None
    }
}
