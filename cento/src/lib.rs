mod explore;
mod find;
mod tile;

use geo::coord;
use slotmap::{new_key_type, SlotMap};

use core::num::NonZeroU64;

pub type Point = geo::Point<i32>;
pub type Rect = geo::Rect<i32>;

pub type Body = Option<NonZeroU64>;

new_key_type! { pub struct TileKey; }

pub type Stitch = Option<TileKey>;

#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Stitches {
    pub below: Stitch,
    pub left: Stitch,
    pub above: Stitch,
    pub right: Stitch,
}

impl Stitches {
    pub fn new() -> Stitches {
        Self {
            left: None,
            below: None,
            right: None,
            above: None,
        }
    }
}

#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Tile {
    pub bounds: Rect,
    pub body: Body,
    pub stitches: Stitches,
}

#[derive(Debug)]
pub struct Plane {
    arena: SlotMap<TileKey, Tile>,
    hint: TileKey,
}

#[macro_export]
macro_rules! tile {
    ($plane:ident, $($tile_key:ident).+) => {
        ($plane).slots()[($($tile_key).+)]
    };
}

#[macro_export]
macro_rules! key_tile {
    ($plane:expr, $tile_key:expr) => {
        ($plane).get_with_key(($tile_key)).unwrap()
    };
}

#[macro_export]
macro_rules! tile_mut {
    ($plane:ident, $($tile_key:ident).+) => {
        ($plane).slots_mut()[($($tile_key).+)]
    };
}

#[macro_export]
macro_rules! stitches_mut {
    ($plane:ident, $tile_key:ident) => {
        tile_mut!($plane, $tile_key).stitches
    };
}

impl Plane {
    pub fn new() -> Plane {
        let mut a: SlotMap<TileKey, Tile> = SlotMap::with_key();
        let h = a.insert(Tile::universe());
        Self { arena: a, hint: h }
    }

    pub unsafe fn from_unchecked(tiles: Vec<Tile>) -> (Plane, Vec<TileKey>) {
        assert!(tiles.len() >= 1, "cannot create an empty tiling");

        let mut sm: SlotMap<TileKey, Tile> = SlotMap::with_key();
        let mut keys = Vec::with_capacity(tiles.len());

        let mut it = tiles.iter();

        let h = sm.insert(it.next().unwrap().clone());
        keys.push(h);

        for t in it {
            keys.push(sm.insert(t.clone()));
        }

        (Plane { arena: sm, hint: h }, keys)
    }

    pub fn slots(&self) -> &SlotMap<TileKey, Tile> {
        &self.arena
    }

    pub fn slots_mut(&mut self) -> &mut SlotMap<TileKey, Tile> {
        &mut self.arena
    }

    pub fn hint(&self) -> TileKey {
        self.hint
    }

    pub(crate) fn alloc(&mut self, r: Rect, body: Body) -> TileKey {
        self.arena.insert(Tile::new(r, body))
    }

    pub(crate) fn free(&mut self, tile: TileKey) {
        self.arena.remove(tile).expect("tile was already removed");
    }

    pub fn get(&self, key: TileKey) -> Option<&Tile> {
        self.slots().get(key)
    }

    pub fn get_with_key(&self, key: TileKey) -> Option<(TileKey, &Tile)> {
        match self.get(key) {
            Some(t) => Some((key, t)),
            None => None,
        }
    }
}
