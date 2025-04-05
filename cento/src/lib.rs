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

    pub fn is_solid(&self) -> bool {
        self.body.is_some()
    }

    pub fn is_space(&self) -> bool {
        self.body.is_none()
    }

    pub fn min_x(&self) -> i32 {
        self.bounds.min().x
    }

    pub fn min_y(&self) -> i32 {
        self.bounds.min().y
    }

    pub fn max_x(&self) -> i32 {
        self.bounds.max().x
    }

    pub fn max_y(&self) -> i32 {
        self.bounds.max().y
    }

    pub fn left(&self) -> Stitch {
        self.stitches.left
    }

    pub fn below(&self) -> Stitch {
        self.stitches.below
    }

    pub fn right(&self) -> Stitch {
        self.stitches.right
    }

    pub fn above(&self) -> Stitch {
        self.stitches.above
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
            None => None
        }
    }

    pub fn find_tile_from(&self, start: TileKey, point: Point) -> TileKey {
        let (mut t, mut tile) = key_tile!(self, start);

        // 1. First move up (or down) along the left edges of tiles until a tile
        //    is found whose vertical range contains the desired point.
        if point.y() < tile.min_y() {
            loop {
                (t, tile) = key_tile!(self, tile.below().unwrap());
                if point.y() >= tile.min_y() {
                    break;
                }
            }
        } else {
            while point.y() >= tile.max_y() {
                (t, tile) = key_tile!(self, tile.above().unwrap());
            }
        }

        // 2. Then move left (or right) along the bottom edges of tiles until a
        //    tile is found whose horizontal range contains the desired point.
        if point.x() < tile.min_x() {
            loop {
                while point.x() < tile.min_x() {
                    (t, tile) = key_tile!(self, tile.left().unwrap());
                }

                if point.y() < tile.max_y() {
                    break;
                }

                while point.y() < tile.max_y() {
                    (t, tile) = key_tile!(self, tile.above().unwrap());
                }

                if point.x() >= tile.min_x() {
                    break;
                }
            }

            // 3. Since the horizontal motion may have caused a vertical
            //    misalignment, steps 1 and 2 may have to be iterated several times
            //    to locate the tile containing the point.
        } else {
            while point.x() >= tile.max_x() {
                loop {
                    (t, tile) = key_tile!(self, tile.right().unwrap());
                    if point.x() < tile.max_x() {
                        break;
                    }
                }

                if point.y() >= tile.min_y() {
                    break;
                }

                loop {
                    (t, tile) = key_tile!(self, tile.below().unwrap());
                    if point.y() >= tile.min_y() {
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
