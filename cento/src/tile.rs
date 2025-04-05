use crate::*;

impl Tile {
    pub fn new(r: Rect, body: Body) -> Tile {
        Self {
            bounds: r,
            body: body,
            stitches: Stitches::new(),
        }
    }

    pub(crate) fn universe() -> Tile {
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
