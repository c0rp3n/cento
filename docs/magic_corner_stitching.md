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
