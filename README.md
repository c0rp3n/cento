<h1 align="center">
    Cento
</h1>
<p align="center">
    <strong>
        <a href="http://opencircuitdesign.com/">open circuit design</a> |
        <a href="http://opencircuitdesign.com/magic/archive/papers/Magic_corner_stitching_original_paper.pdf">paper</a>
    </strong>
</p>

# Cento

Cento is an implementation of John K. Outsterhout's Corner Stitching in modern
C++. It intends to be a generic library for testing different tile structures.

It is named after the latin word cento which stands for "patchwork garment",
the corner stitching data structure when visualised typically looks like a
patchwork garment or quilt so it is a fitting name.

## Abstract

Corner stitching is a technique for representing rectangular two-dimensional
objects. It appears to be especially well suited for interactive editing
systems for VLSI layouts. The data structure has two important features: first,
empty space is represented explicitly; and second, rectangular areas are
stitched together at their corner like a patchwork quilt. This organization
results in fast algorithms (linear time or better) for searching, creation,
deletion, stretching, and compaction. The algorithms are represented under a
simplified model of VLSI circuits, and the storage requirements of the structure
are discussed. Measurements indicate that corner stitching requires
approximately three times as much memory space as the simplest possible
representation.
