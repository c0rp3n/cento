use geo::coord;
use typed_arena::Arena;

use core::num::NonZeroU64;
use std::ptr::NonNull;

pub type Point = geo::Point<i32>;
pub type Rect = geo::Rect<i32>;

pub type Body = Option<NonZeroU64>;

pub type Stitch = Option<NonNull<Tile>>;

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

pub struct Plane {
    arena: Arena<Tile>,
    hint: NonNull<Tile>,
}

impl Plane {
    pub fn new() -> Plane {
        let a = Arena::new();
        let h = NonNull::from(a.alloc(Tile::universe()));
        Self { arena: a, hint: h }
    }
}
