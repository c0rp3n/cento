use crate::*;
use geo::coord;

use std::num::NonZero;

pub fn axis() -> Vec<Tile> {
    vec![
        Tile::new(
            Rect::new(coord! { x: i32::MIN, y: 0}, coord! { x: 0, y: i32::MAX}),
            Some(NonZero::new(1).unwrap()),
        ),
        Tile::new(
            Rect::new(coord! { x: 0, y: 0}, coord! { x: i32::MAX, y: i32::MAX}),
            Some(NonZero::new(2).unwrap()),
        ),
        Tile::new(
            Rect::new(coord! { x: i32::MIN, y: i32::MIN}, coord! { x: 0, y: 0}),
            Some(NonZero::new(3).unwrap()),
        ),
        Tile::new(
            Rect::new(coord! { x: 0, y: i32::MIN}, coord! { x: i32::MAX, y: 0}),
            Some(NonZero::new(4).unwrap()),
        ),
    ]
}
