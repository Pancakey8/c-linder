# C-linder

A rotating cylinder drawn directly on the terminal, written in C.

Inspired by donut.c

## Running

Make sure you link with libm. The rendering uses ANSI escape codes, and `usleep()` from `unistd.h` which might not be portable outside Linux.

```sh
gcc -lm -o main main.c
```

## Fun

A 3D camera is defined with a center point as well as three basis vectors (so, 3 axes). The axes define the orientation of the camera, and the center point defines its position.

Take the camera basis vectors are up ($\vec{u}$), right ($\vec{r}$) and front ($\vec{f}$). Then we can consider the basis matrix given by $`\bigl[ \vec{r} \mid \vec{u} \mid \vec{f} \bigr]`$. This is a matrix that defines a transformation $\mathbf{T}$ which gives us a way to transform camera coordinates into world coordinates by applying $`\mathbf{T} \cdot \bigl(\vec{p}_{cam} + \vec{O}_{cam}\bigr)`$. Here, $\vec{O}_{cam}$ is added to also translate according to camera position.

Then if we wanted to transform a point in world coordinates to camera coordinates, we consider the inverse $`\mathbf{T}^{-1} \cdot \bigl(\vec{p}_{world} - \vec{O}_{cam}\bigr)`$. Here, it turns out $\mathbf{T}^{-1}$ not only exists but it's really easy to find. In our case, the 3 vectors up, right and front are orthonormal; that is they're perpendicular directions and they're all scaled to a size of 1. This gives us a small trick where the inverse of an orthonormal matrix is its transpose (i.e. flipped on its side). This means each of $\vec{r}$, $\vec{u}$, $\vec{f}$ are the rows of the inverse matrix $\mathbf{T}$.

Using this, we can break down the matrix multiplication into 3 equations for each of x,y,z.

```math
\begin{aligned}
x_{cam} &= \vec{r} \cdot (\vec{p}_{world} - \vec{O}_{cam}) \\
y_{cam} &= \vec{u} \cdot (\vec{p}_{world} - \vec{O}_{cam}) \\
z_{cam} &= \vec{f} \cdot (\vec{p}_{world} - \vec{O}_{cam})
\end{aligned}
```

Usually, another step is needed to finish the projection. We need only 2 coordinates, x and y, as the result of a projection. This means $z_{cam}$ has another purpose, which is to act as a scaling factor.

In our case, we do not apply an "proper" projection. Instead we scale x and y to match a good scale based on `WIDTH` and `HEIGHT` as given in code. We also eyeball a good translation amount after everything is done to ensure the cylinder fits nicely on the screen. As for the z-value, we use it for shading over scaling. We are working on a lower resolution which means using characters with more space for farther points, and characters with less space for closer points is easier. We have a gradient defined for this. We track the minimum and maximum distances of our scene and we distribute our gradient accordingly.

The cylinder itself has a radius of 1 and height of 2, and it's centered at the origin. The camera is fixed at $\bigl(3,0,0\bigr)$ and pointed at $\bigl(0,0,0\bigr)$ giving us the vectors $\vec{f} = \bigl(-1,0,0\bigr)$, $\vec{u} = \bigl(0,0,1\bigr)$ and $\vec{r} = \bigl(0,-1,0\bigr)$. We can iterate the points of the cylinder with (surprise!) cylindrical coordinates.
