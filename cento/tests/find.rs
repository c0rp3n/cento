use cento::*;
use geo::coord;

#[test]
fn universe() {
    let plane = Plane::new();
    let t = plane.hint();

    assert_eq!(plane.find_tile_at(Point::new(0, 0)), t);
    assert_eq!(plane.find_tile_at(Point::new(-512, 0)), t);
    assert_eq!(plane.find_tile_at(Point::new(512, 0)), t);
    assert_eq!(plane.find_tile_at(Point::new(0, -512)), t);
    assert_eq!(plane.find_tile_at(Point::new(0, 512)), t);
}

#[test]
fn axis() {
    //
    //         :
    //         :
    //    tl   :   tr
    //         :
    //         :
    // - - - - + - - - -
    //         :
    //         :
    //    bl   :   br
    //         :
    //         :
    //

    let tiles = vec![
        Tile::new(
            Rect::new(coord! { x: i32::MIN, y: 0}, coord! { x: 0, y: i32::MAX}),
            None,
        ),
        Tile::new(
            Rect::new(coord! { x: 0, y: 0}, coord! { x: i32::MAX, y: i32::MAX}),
            None,
        ),
        Tile::new(
            Rect::new(coord! { x: i32::MIN, y: i32::MIN}, coord! { x: 0, y: 0}),
            None,
        ),
        Tile::new(
            Rect::new(coord! { x: 0, y: i32::MIN}, coord! { x: i32::MAX, y: 0}),
            None,
        ),
    ];

    let (mut plane, keys) = unsafe { Plane::from_unchecked(tiles) };
    assert_eq!(keys.len(), 4);

    let tl = keys[0];
    let tr = keys[1];
    let bl = keys[2];
    let br = keys[3];

    stitches_mut!(plane, tl) = Stitches {
        left: None,
        below: Some(bl),
        right: Some(tr),
        above: None,
    };

    stitches_mut!(plane, tr) = Stitches {
        left: Some(tl),
        below: Some(br),
        right: None,
        above: None,
    };

    stitches_mut!(plane, bl) = Stitches {
        left: None,
        below: None,
        right: Some(br),
        above: Some(tl),
    };

    stitches_mut!(plane, br) = Stitches {
        left: Some(bl),
        below: None,
        right: None,
        above: Some(tr),
    };

    assert_eq!(plane.find_tile_at(Point::new(0, 0)), tr);
    assert_eq!(plane.find_tile_at(Point::new(-256, 256)), tl);
    assert_eq!(plane.find_tile_at(Point::new(256, 256)), tr);
    assert_eq!(plane.find_tile_at(Point::new(-256, -256)), bl);
    assert_eq!(plane.find_tile_at(Point::new(256, -256)), br);
}
