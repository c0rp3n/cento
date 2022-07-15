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
worst, espcially for operations such as compaction and streching.

A third class of data structures is based on neighbour pointers. In this
technique, each rectangle contains pointers to rectangles that are adjacent to
it in x and y. See Figure 2. Neighnour pointers are a popular data structure for
compaction programs such as Cabbage \[2\], since they provide information about
relationship between objects. For example, a simple graph traversal can be used
to determine the minimum feasible width of a cell.

Neighbour pointers have two drawbacks. First, modifications to the structure
generally require all the pointers to be recomputed. For example, if an object
is moved horizontally as in Figure 2. vertical pointers may be invalidated.
There is no simple way to correct the vertical pointers short of scanning the
entire database. The second problem with neighbour pointers is that they don't
provide much assistance in locating empty space for routing, since only the
occupied space is represented explicitly. For these two reasons, neighbor
pointers do not appear to be well-suited to interactive systems or that that
provide routing aids.
