use crate::tests::fixtures;
use crate::*;

use geo::{coord, Intersects};

use std::iter::zip;

#[test]
fn intersects() {
    {
        let r = Rect::new(coord! { x: i32::MIN, y: i32::MIN}, coord! { x: 0, y: 0});
        let p = coord! { x: 0, y: 0};
        assert!(r.intersects(&p));
    }
    {
        let r = Rect::new(coord! { x: i32::MIN, y: 0}, coord! { x: 0, y: i32::MAX});
        let p = coord! { x: 0, y: 0};
        assert!(r.intersects(&p));
    }
}

fn is_left(a: &Rect, b: &Rect) -> bool {
    (a.min().x == b.max().x) && b.intersects(&a.min())
}

fn is_right(a: &Rect, b: &Rect) -> bool {
    (a.max().x == b.min().x) && b.intersects(&a.max())
}

fn is_below(a: &Rect, b: &Rect) -> bool {
    (a.min().y == b.max().y) && b.intersects(&a.min())
}

fn is_above(a: &Rect, b: &Rect) -> bool {
    (a.max().y == b.min().y) && b.intersects(&a.max())
}

macro_rules! update_stitch {
    ($sm:ident, $from:ident, $stitch:ident, $to:ident) => {
        match $from.stitches.$stitch {
            Some(o) => {
                let old: Rect = $sm[o].bounds;
                let new: Rect = $sm[*$to].bounds;
                if new.min().x_y() < old.min().x_y() {
                    $from.stitches.$stitch = Some($to.clone())
                }
            }
            None => $from.stitches.$stitch = Some($to.clone()),
        }
    };
}

fn build_stitches(plane: &mut Plane, key: TileKey, keys: &Vec<TileKey>) {
    let sm = plane.slots_mut();
    let mut tile: Tile = sm[key];
    for n in keys.iter() {
        if *n == key {
            continue;
        }

        let nb: Rect = sm[*n].bounds;

        if is_above(&tile.bounds, &nb) {
            update_stitch!(sm, tile, above, n);
        } else if is_below(&tile.bounds, &nb) {
            update_stitch!(sm, tile, below, n);
        } else if is_left(&tile.bounds, &nb) {
            update_stitch!(sm, tile, left, n);
        } else if is_right(&tile.bounds, &nb) {
            update_stitch!(sm, tile, right, n);
        }
    }
    sm.get_mut(key).unwrap().stitches = tile.stitches;
}

pub fn create_tiles(tiles: Vec<Tile>) -> (Plane, Vec<TileKey>) {
    let (mut plane, keys) = unsafe { Plane::from_unchecked(tiles) };
    for k in keys.iter() {
        build_stitches(&mut plane, *k, &keys);
    }
    (plane, keys)
}

#[test]
fn build_axis() {
    let (plane, keys) = create_tiles(fixtures::axis());
    assert_eq!(keys.len(), 4);

    let tl = keys[0];
    let tr = keys[1];
    let bl = keys[2];
    let br = keys[3];

    let expected = vec![
        Stitches {
            left: None,
            below: Some(bl),
            right: Some(tr),
            above: None,
        },
        Stitches {
            left: Some(tl),
            below: Some(bl), // better as br
            right: None,
            above: None,
        },
        Stitches {
            left: None,
            below: None,
            right: Some(br),
            above: Some(tl),
        },
        Stitches {
            left: Some(bl),
            below: None,
            right: None,
            above: Some(tr),
        },
    ];

    for (key, e) in zip(keys.iter(), expected.iter()) {
        let a = plane.slots()[*key].stitches;
        assert_eq!(a, *e);
    }
}
