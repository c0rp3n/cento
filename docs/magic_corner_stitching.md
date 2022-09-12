<h1 align="center">
    Corner Stiching:<br>
    A Data Structuring Technique for<br>
    VLSI Layout Tools
</h1>
<p align="center">
    John K. Outsterhout<br>
    Electrical Engineering and Computer Science<br>
    University of California<br>
    Berkeley, CA 94720<br>
    415-642-0865<br>
    ARPAnet address: ousterhout@berkeley
</p>

<br>

<p align="center">
    <strong>Abstract</strong><br>
</p>

Corner stiching is a technique for representing rectangular two-dimensional
objects. It appears to be especially well suited for interactive editing
systems for VLSI layouts. The data structure has two important features: first,
empty space is represented explicitly; and second, rectangular areas are stiched
together at their corner like a patchwork quilt. This organization results in
fast algorithms (linear time or better) for searching, creation, deletion,
stretching, and compaction. The algorithms are represented under a simplified
model of VLSI circuits, and the storage requirements of the structure are
discussed. Measurements indicate that corner stiching requires approximately
three times as much memory space as the simplest possible representation.

<br>

_The work described here was supported in part by the Defense Advanced Research_
_Projects Agency (DoD), ARPA Order No. 3803, monitored by the Naval Electronic_
_System Command under Contract No. N00039-81-K-0251_

<br>

<p style="text-align:left;">
    Corner Stiching
    <span style="float:right;">
        December 13, 1982
    </span>
</p>

<br>

### 1. Introduction

Interactive layout tools for integrated circuits place special burdens on their
internal data structures. The data structures must be able to deal with large
amount of information (one-half million or more geometrical elements in current
layouts \[6\]) while providing instantaneous response to the designer. As the
complexity of designs increases, tools must give more and more powerful
assistance to the designer in such areas as routing and validation. To support
these intelligent tools, the underlying data structures must provide fast
geometrical operations, such as locating neighbours for stretching and
compaction, and locating empty space for routing. The data structures must also
permit fast incremental modifications so that they can be used in interactive
systems.

Corner stiching is a data structure that meets these needs. It is limited to
designs with Manhattan features (horizontal and vertical edges only) but within
that framework it provides a variety of powerful operations, such as
neighbour-finding, stretching, compaction and channel-finding. The algorithms
for the operations depend only on local information (the objects in the
immediate vicinity of the operation). Their running times may be linear in the
overall design size. Corner stiching is espcially effective when the objects are
relatively uniform in size, as is the case for low-level mask features. It also
works well when there is variation in feature size, e.g. hierarchical layouts
containing large subcells and small wires to connect them.

Corner stiching permits modifications to the database to be made quickly, since
only local information is used in making the updates. Most existing systems that
provide powerful operations such as routing and compaction do not provide
inexpensive updates: small changes to the database can result in large amounts
of recomputation. Corner stiching's combination of powerful operations and easy
updates means that powerful tools previously only in "batch" mode can now be
embedded in interactive systems.

### 2. A Simplified Model of VLSI Layouts

A VLSI layout normally is specified as a hierarchical collection of cells, where
each cell contains geometrical shapes on several mask layers and pointer to
subcells. As a convenience in presenting the data structure and algorithms, a
simplified model will be used in this paper. Only a single mask layer will be
considered, and hierarchy will not be considered. For this paper, I define a
"circuit" to be a collection of rectangles. There is a single design rule in the
model: rectangles may not overlap. The simplified model makes it easier to
present the datastructure and algorithms. Section 8 discusses how the simplified
model might be generalized to handle real VLSI layouts.

### 3. Existing Mechanisms

The simplest possible technique for representing rectangles is just to keep all
of them in one large list. This technique is used in the Caeser system \[5\]:
each cell is represented by a list of rectangles for each of the mask layers.
Even though operations such as neighbour-finding require entire lists to be
searched, the structure works well in Caeser for two reasons. First, large
layouts are broken down hierarchically into many small cells; only the top-most
cells in the hierarchy ever contain more than a few hundred rectangles or a few
children \[8\]. Second, Caeser provides only very simple operations like
painting and erasing. More complex functions such as design rule checking and
compaction could not be implemented efficiently using rectangle lists.

The most popular structures for VLSI are based on _bins_ \[1\]. In bin-based
systems, an imaginary square grid divides the area of the circuit into bins, as
in Figure 1. All of the rectangles intersecting a particular bin are linked
together, and a two-dimensional array is used to locate the lists for different
bins. The rectangles in a given area can be located quickly by indexing into the
array and searching the (short) lists of relevant bins. The bin size is chosen
as a tradeoff between time and space; as bins get smaller, rectangles begin to
overlap several bins and hence occupy space on several lists.

<p align="center">
    Place Holder<br>
    <strong>
        Figure 1. In bin-based data structures, the circuit is divided by an
        imaginary grid, and all the rectangles intersecting a subarea are linked
        together.
    </strong>
</p>

Bin Structures are most effective when rectangles have nearly uniform size and
spatial distributions; they suffer from space and/or time inefficiencies when
these conditions are not met. A pathological case is a cell with a few large
child cells and many small rectangles to interconnect them: of bins are small,
then there will be many empty bins in the large areas of the subcells, resulting
in slow searches. Hierarchical bin structures \[3\] have been proposed as a
solution of non-uniformity. Although bins provide for quick location of all
objects in an area, they do not directly embody the notion of _nearness_. To
find the nearest object to a given one, it is necessary to search adjacent bins,
working out from the object in a spiral fashion. Furthermore, bin structures do
not indicate which areas of the chip are empty; empty areas must be
reconstructed by scanning the bins. The need to constantly scan the bins to
recreate information makes bin structures clumsy at best, and inefficient at
worst, especially for operations such as compaction and stretching.

A third class of data structures is based on neighbour pointers. In this
technique, each rectangle contains pointers to rectangles that are adjacent to
it in x and y. See Figure 2. Neighnour pointers are a popular data structure for
compaction programs such as Cabbage \[2\], since they provide information about
relationship between objects. For example, a simple graph traversal can be used
to determine the minimum feasible width of a cell.

<p align="center">
    Place Holder<br>
    <strong>
        Figure 2. Neighbour pointers can be used to indicate horizontal and
        vertical adjacency. However, if D is moved right, it is hard to update
        the vertical pointers without scanning the entire database.
    </strong>
</p>

Neighbour pointers have two drawbacks. First, modifications to the structure
generally require all the pointers to be recomputed. For example, if an object
is moved horizontally as in Figure 2. vertical pointers may be invalidated.
There is no simple way to correct the vertical pointers short of scanning the
entire database. The second problem with neighbour pointers is that they don't
provide much assistance in locating empty space for routing, since only the
occupied space is represented explicitly. For these two reasons, neighbor
pointers do not appear to be well-suited to interactive systems or that that
provide routing aids.

<p align="center">
    Place Holder<br>
    <strong>
        Figure 3. An example of tiles in a corner stitched data structure. Solid
        tile are represented with dark lines, space tiles with dotted lines. The
        entire area of the circuit is covered with tiles. Space tiles are made
        as wide as possible.
    </strong>
</p>

### 4. Corner Stitching

Corner stitching arose from a consideration of the weakness of the above
mechanisms, and has two features that distinguish it from them. The first
important feature is that all space, both empty and occupied, is represented
explicitly in the database. The second feature is a novel way of linking
together the objects at their corners. These _corner stitches_ permit easy
modification of the database, and lead to efficient implementations for a
variety of operations.

Figure 3 shows four objects represented in the corner stitching scheme. The
picture resembles the mosaic with rectangular tiles of two types, space and
solid. Tiles contain there lower and left edges, but not their upper or right
edges, so every point in the plane is present in exactly one tile. The tiles
must be rectangles with sides parallel to the axes.

The space tiles are organised as a _maximal horizontal strips_. This means that
no space tile has other space tiles immediately to its right or left. When
modifying the database, adjacent space tiles that are horizontally adjacent must
be split into shorter tiles and then joined into maximal strips, as shows in
Figure 4. After making sure that space tiles are as wide as possible,
vertically adjacent tiles can be merged together if they have the same
horizontal span. The representation of space in of no consequence to the VLSI
layout or to the designer, and will not even be visible in real systems.
However, the maximal horizontal strip representation is crucial to the space and
time efficiency of the tools, as we shall see in Sections 5 and 6. Among its
other properties, the horizontal strip representation is unique: there is one
and only one decomposition of space for each arrangement of solid tiles.

<p align="center">
    Place Holder<br>
    <strong>
        Figure 4. No space tile may have another space tile to its immediate
        right or left. In this example, tiles A and B in (a) must be split into
        the shorter tiles of (b), then merged together into wide strips in (c),
        and finally meged vertically in (d).
    </strong>
</p>

Tiles are linked by a set of pointers at their corners, called _corner_
_stitches_. Each tile contains eight, two at each corner, as illustrated in
Figure 5. By linking together all adjacent tiles, corner stitches provide
something equivalent to neighbour pointers.

<p align="center">
    Place Holder<br>
    <strong>
        Figure 5. Each tile is connected to its neighbour by eight pointers
        called corner stitches. The name of each stitch indicates the location
        of the stitch (lb means left bottom corner), and the second letter
        indicates the direction in which the stitch points. Thus lb refers to
        the pointer at the left edge of the tile pointing out its bottom.
    </strong>
</p>

The tile/stitch representation has several attractive features, which will be
illustrated in the sections that follow. First, the mechanism combines both
horizontal and vertical information in a single structure. The space tiles
provide a form of registration between the horizontal and vertical information
and make it easy to keep all the pointers up to date as the circuit is modified.
Because the space tiles may vary in size (as oposed to fixed-sizee bins), the
structure adapts naturally to variations in the size of the solid tiles. The
maximal horizontal strip representation of space results in clean upper bounds
on the number of space tiles and also on the complexity of the algorithms. All
tiles have the same number of pointers to other tiles, which simplifies the
database management.

### 5. Algorithms

This section presents algorithms for manipulating the tiles and corner stitches.
The algorithms are presented in simplified form here; a few of them are
described in more detail in the appendices. The most important attribute of all
the algorithms is their locality: each algorithm depends only on information in
the immediate vicinity of the operation. None of the algorithms has an average
running time any worse than linear in the number of tiles in the affected area.
One can devise pathological cases where the algorithms require time linear in
the overall layout size, but in practice (particularly for VLSI layouts, which
tend to be densely packed) their running times will be small and nearly
independent if the size of the layout.

In discussing the performance of the algorithms, the corner stitches provide a
good unit of measure. The complexity of the algorithms will be discussed in
terms of the number of stitches that must be traversed (or, altered natively,
the number of tiles that must be visited) and/or the number of stitches that
must be modified.

#### 5.1. Point Finding

Several different kinds of searching are facilitated by corner stitching. One
of the most common operations is to find the tile given (x,y) location. Figure 6
illustrates how this can be done with corner stitching. The algorithm iterates
in x and y, starting from any given tile in the database.

<p align="center">
    Place Holder<br>
    <strong>
        Figure 6. To locate the tile containing a given point, alternate between
        up/down and left/right motions.
    </strong>
</p>

1. First move up or down along the left edges of tiles (following lt and lb
stitches) until a tile is found whose vertical range contains the desired point.

2. Then move left or right along the bottom edges of tiles (following br and bl
stitches) until a tile is found whose horizontal range contains the desired
point.

3. Since the horizontal motion may have caused a vertical misalignment, steps 1
and 2 may have to be iterated several times to locate the tile containing the
point.

In the worst case, this algorithm may require every tile in the entire structure
to be searched (this happens, for example, if all the tiles in the structure are
in a single column or row). Fortunately, the average case behaviour is much
better than this. If there are a total of N space or solid tiles and they are of
relatively uniform size, then on the order of √N tiles will be passed through in
the average case. For a layout containing a million tiles (which is typical of
the fully expanded mask sets of current VLSI circuits) this means a few thousand
tiles will have to be touched.

In interactive systems, there is a simple way to reduce the time spent in
searches of this sort: keep around a pointer to any tile in the approximate area
where the designer is working. When a large design is being edited, the
designer's attention is generally focussed on a small piece of the design (e.g.
a piece that can be viewed comfortably on a graphic device). If a tile in this
area is remembered for reference, then search time depends only on how much is
on the screen, not how large the design is.
