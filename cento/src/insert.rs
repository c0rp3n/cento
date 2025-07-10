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
    pub fn insert(&mut self, r: Rect, b: NonZeroU64) -> Option<TileKey> {
        if !self.empty(r) { return None; }

        // 1. Find the space tile containing the top edge of the area to be occupied
        //    by the new tile (because of the strip property, a single space tile
        //    must contain the entire edge).
        let key = self.find_tile_at(r.max());
        self.split_horz(key, r.max().y);

        let key = self.find_tile_at(r.min());
        let mut ret = match self.split_horz(key, r.min().y) {
            Some(split) => split.upper,
            _ => key
        };

        
    }
}
