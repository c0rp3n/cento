use geo::coord;
use slotmap::{new_key_type, SlotMap};

use core::num::NonZeroU64;

pub type Point = geo::Point<i32>;
pub type Rect = geo::Rect<i32>;

pub type Body = Option<NonZeroU64>;

new_key_type! { pub struct TileKey; }

pub type Stitch = Option<TileKey>;

#[derive(Debug, Copy, Clone)]
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

#[derive(Debug, Copy, Clone)]
pub struct Tile {
    pub bounds: Rect,
    pub body: Body,
    pub stitches: Stitches,
}

impl Tile {
    pub fn new(r: Rect, body: Body) -> Tile {
        Self {
            bounds: r,
            body: body,
            stitches: Stitches::new(),
        }
    }

    fn universe() -> Tile {
        Tile::new(
            Rect::new(
                coord! { x: i32::MIN, y: i32::MIN},
                coord! { x: i32::MAX, y: i32::MAX},
            ),
            None,
        )
    }
}

#[test]
fn tile_size() {
    let size = std::mem::size_of::<Tile>();
    let align = std::mem::align_of::<Tile>();
    assert!(size <= 64);
    assert_eq!(align, 8);
}

#[derive(Debug)]
pub struct Plane {
    arena: SlotMap<TileKey, Tile>,
    hint: TileKey,
}

#[macro_export]
macro_rules! tile {
    ($plane:ident, $tile_key:ident) => {
        ($plane).slots()[($tile_key)]
    };
}

#[macro_export]
macro_rules! bounds {
    ($plane:ident, $tile_key:ident) => {
        tile!($plane, $tile_key).bounds
    };
}

#[macro_export]
macro_rules! min_x {
    ($plane:ident, $tile_key:ident) => {
        bounds!($plane, $tile_key).min().x
    };
}

#[macro_export]
macro_rules! min_y {
    ($plane:ident, $tile_key:ident) => {
        bounds!($plane, $tile_key).min().y
    };
}

#[macro_export]
macro_rules! max_x {
    ($plane:ident, $tile_key:ident) => {
        bounds!($plane, $tile_key).max().x
    };
}

#[macro_export]
macro_rules! max_y {
    ($plane:ident, $tile_key:ident) => {
        bounds!($plane, $tile_key).max().y
    };
}

#[macro_export]
macro_rules! body {
    ($plane:ident, $tile_key:ident) => {
        tile!($plane, $tile_key).body
    };
}

#[macro_export]
macro_rules! stitches {
    ($plane:ident, $tile_key:ident) => {
        tile!($plane, $tile_key).stitches
    };
}

#[macro_export]
macro_rules! left {
    ($plane:ident, $tile_key:ident) => {
        stitches!($plane, $tile_key).left
    };
}

#[macro_export]
macro_rules! below {
    ($plane:ident, $tile_key:ident) => {
        stitches!($plane, $tile_key).below
    };
}

#[macro_export]
macro_rules! right {
    ($plane:ident, $tile_key:ident) => {
        stitches!($plane, $tile_key).right
    };
}

#[macro_export]
macro_rules! above {
    ($plane:ident, $tile_key:ident) => {
        stitches!($plane, $tile_key).above
    };
}

#[macro_export]
macro_rules! tile_mut {
    ($plane:ident, $tile_key:ident) => {
        ($plane).slots_mut()[($tile_key)]
    };
}

#[macro_export]
macro_rules! stitches_mut {
    ($plane:ident, $tile_key:ident) => {
        tile_mut!($plane, $tile_key).stitches
    };
}

#[macro_export]
macro_rules! left_mut {
    ($plane:ident, $tile_key:ident) => {
        stitches_mut!($plane, $tile_key).left
    };
}

#[macro_export]
macro_rules! below_mut {
    ($plane:ident, $tile_key:ident) => {
        stitches_mut!($plane, $tile_key).below
    };
}

#[macro_export]
macro_rules! right_mut {
    ($plane:ident, $tile_key:ident) => {
        stitches_mut!($plane, $tile_key).right
    };
}

#[macro_export]
macro_rules! above_mut {
    ($plane:ident, $tile_key:ident) => {
        stitches_mut!($plane, $tile_key).above
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

    pub fn find_tile_from(&self, start: TileKey, point: Point) -> TileKey {
        let mut t = start;

        // 1. First move up (or down) along the left edges of tiles until a tile
        //    is found whose vertical range contains the desired point.
        if point.y() < min_y!(self, t) {
            loop {
                t = below!(self, t).unwrap();
                if point.y() >= min_y!(self, t) { break; }
            }
        } else {
            while point.y() >= max_y!(self, t) {
                t = above!(self, t).unwrap();
            }
        }

        // 2. Then move left (or right) along the bottom edges of tiles until a
        //    tile is found whose horizontal range contains the desired point.
        if point.x() < min_x!(self, t) {
            loop {
                while point.x() < min_x!(self, t) {
                    t = left!(self, t).unwrap();
                }

                if point.y() < max_y!(self, t) {
                    break;
                }

                while point.y() < max_y!(self, t) {
                    t = above!(self, t).unwrap();
                }

                if point.x() >= min_x!(self, t) {
                    break;
                }
            }

            // 3. Since the horizontal motion may have caused a vertical
            //    misalignment, steps 1 and 2 may have to be iterated several times
            //    to locate the tile containing the point.
        } else {
            while point.x() >= max_x!(self, t) {
                loop {
                    t = right!(self, t).unwrap();
                    if point.x() < max_x!(self, t) {
                        break;
                    }
                }

                if point.y() >= min_y!(self, t) {
                    break;
                }

                loop {
                    t = below!(self, t).unwrap();
                    if point.y() >= min_y!(self, t) {
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
