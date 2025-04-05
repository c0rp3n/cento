use cento::*;
use geo::coord;

mod empty {
    use super::*;

    #[test]
    fn universe() {
        let plane = Plane::new();
        assert!(plane.empty(Rect::new(
            coord! { x: -512, y: -512},
            coord! {x: 512, y:512}
        )));
    }

}
