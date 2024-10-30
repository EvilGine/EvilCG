# EvilCG - Computational Graphics Library

**EvilCG** is a comprehensive C++ library tailored for computational graphics, providing utilities for geometric analysis, mesh manipulation, and transformations. With functionality ranging from bounding box calculations to mesh smoothing, EvilCG simplifies complex 3D computations and assists developers in rendering, collision detection, and physics simulation tasks.

---

## Features

- **Bounding Box Computation**
  - Calculate Axis-Aligned Bounding Boxes (AABB) and Oriented Bounding Boxes (OBB) for precise spatial enclosures.
- **Mesh Operations**
  - Compute mesh properties such as center, surface area, vertex summation, and covariance matrix.
- **Mesh Comparison**
  - Compare two meshes, including transformations, to determine equivalency.
- **Advanced Functions**
  - Nearest vertex search, manifold checking, and closure validation for mesh topologies.
- **Extension Points**
  - Additional advanced features, including volume calculation, mesh simplification, and smoothing, planned for future releases.

---

## Installation

To integrate EvilCG into your project:

1. Clone the repository or download the source files.
2. Include the main headers:
   ```cpp
   #include <ecg_global.h>
   #include <api_define.h>
   #include <ecg_geom_.h>
   #include <ecg_api_.h>
