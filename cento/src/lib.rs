use geo::coord;
use slotmap::{new_key_type, SlotMap};

use core::num::NonZeroU64;

pub type Point = geo::Point<i32>;
pub type Rect = geo::Rect<i32>;

pub type Body = Option<NonZeroU64>;

new_key_type! { pub struct TileKey; }

pub type Stitch = Option<TileKey>;

pub struct Stitches {
    pub below: Stitch,
    pub left: Stitch,
    pub above: Stitch,
    pub right: Stitch,
}

impl Stitches {
    pub fn new() -> Stitches {
        Self {
            below: None,
            left: None,
            above: None,
            right: None,
        }
    }
}

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

pub struct Plane {
    arena: SlotMap<TileKey, Tile>,
    hint: TileKey,
}

impl Plane {
    pub fn new() -> Plane {
        let mut a: SlotMap<TileKey, Tile> = SlotMap::with_key();
        let h = a.insert(Tile::universe());
        Self { arena: a, hint: h }
    }

    fn alloc(&mut self, r: Rect, body: Body) -> TileKey {
        self.arena.insert(Tile::new(r, body))
    }

    fn free(&mut self, tile: TileKey) {
        self.arena.remove(tile).expect("tile was already removed");
    }
}
