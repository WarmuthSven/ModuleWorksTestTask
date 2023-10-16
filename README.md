# ModuleWorks TestTask
## Mathematical Approaches
### Point in Sphere
To find an intersection between a sphere with radius $R$ and a cloud point $P$
you need to calculate the euclidean distance $d$ between
the centerpoint $P_c$ of the sphere and point $P$.
The point lies inside of the sphere if the distance $d$ is smaller than or equal to the radius $R$.

Therefore the difference vector between $P_c$ and $P$ is calculated.

$$ d_{dist} = P_c - P $$ 

Squaring all single entries of the difference vector $d_{dist}$
and taking the square root of the sum is equal to the distance $d$.

$$ \sqrt{d_{vec,x}^2 + d_{vec,y}^2 + d_{vec,z}^2} <= R $$

Using the given Point3-library, I subracted the point $P$ from the centerpoint $P_c$ to get the distance vector $d_{dist}$
and used the function length2() to get the squared distance $d^2$ of the vector.
Instead of taking the square root, I squared the radius $R$ once as well and used that to quickly compare
if a point $P$ lies inside of the sphere.

$$ d_{vec,x}^2 + d_{vec,y}^2 + d_{vec,z}^2 <= R^2 $$

This is a faster approach as it only uses elementary operations,
while using square root is a way more expensive operation.

### Point on sphere path
As the sphere path can be modelled by a cylinder between two points $P_1$ and $P_2$,
you only need to find an intersection of a cylinder and a point $P$.
For that you need to calculate the orthogonal distance $d$ between the cylinder axis $d_{cyl}$ and the point $P$.
The point $P$ lies inside an infinite long cylinder,
if the orthogonal distance $d$ is smaller than or equal to the radius $R$.
Assume a line $d_{orth}$, that is orthogonal to and intersects with the axis $d_{cyl}$ as well as the point $P$.
If the intersection point $P_{sec}$ between line $d_{orth}$ and the axis $d_{cyl}$ is between $P_1$ and $P_2$,
the cloud point $P$ lies inside the cylinder.

To get this intersection point $P_{sec}$, you need to solve an equation.
The scalar product of the distance vector $d_{dist}$ and the axis $d_{cyl}$ has to be 0,
only then they are perpendicular to each other.

$$ d_{dist}* d_{cyl} = 0 $$

The distance vector $d_{dist}$ is calculated by subtracting the intersection point $P_{sec}$ from
the cloud point $P$.

$$ d_{dist} = P - P_{sec} $$

The axis $d_{cyl}$ is calculated by subtracting the start point $P_1$ from the end point $P_2$.

$$ d_{cyl} =  P_2 - P_1 $$

The intersection point $P_{sec}$ on the axis $d_{cyl}$ is defined
by multiplying $d_{cyl}$ with an unknown scalar value $x$
and adding it to $P_1$.
$x$ has to be in the closed range $[0,1]$ for the intersection point $P_{sec}$
to lie between $P_1$ and $P_2$.

$$ P_{sec} = P_1 + d_{cyl} * x $$

Then you can insert the definition for the intersection point $P_{sec}$ into the orthogonality constraint
and solve it for the unknown scalar value $x$.

$$ x = {P * d_{cyl} - P_1 * d_{cyl} \over d_{cyl} * d_{cyl}} $$

After that you can calculate the intersection point $P_{sec}$ and with that the distance $d$ between the axis $d_{cyl}$ and the cloud point $P$.

I calculated the line Segment $x$ on which the intersection point $P_{sec}$ lies,
by solving the equation and using the scalar product of the given Point3-library.
Then I checked if $x$ is in the closed range $[0,1]$.
After that I calculated the intersection point $P_{sec}$ and the squared distance $d^2$
between $P$ and $P_{sec}$ and compared it to the squared radius $R^2$.
 
## Problems with discrete steps
On a linear path of a sphere, the discrete steps have no influence
on the precision of the calculation.
While on a non-linear path, using discrete time steps lead to a loss of precision.
A direct linear path between single time steps is assumed and loses details of the curve path 
through approximation of the curve.
For example on a full circle path with only 4 time steps, we get a square instead of a circle,
while with an infinitesimal small time delta this circle is perfectly approximated.
