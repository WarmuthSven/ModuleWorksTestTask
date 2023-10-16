# ModuleWorks TestTask
## Mathematical Approaches
### Point in Sphere
To find an intersection between a sphere with radius $R$ and a cloud point $P$
you need to calculate the euclidean distance $d$ between
the centerpoint $P_c$ of the sphere and point $P$.
The point lies inside of the sphere if the distance $d$ is smaller than or equal to the radius $R$.

Therefore the difference vector between $P_c$ and $P$ is calculated.

$$ d_{vec} = P_c - P $$ 

Squaring all single entries of the difference vector $d_{vec}$
and taking the square root of the sum is equal to the distance $d$.

$$ \sqrt{d_{vec,x}^2 + d_{vec,y}^2 + d_{vec,z}^2} <= R $$

Using the given Point3-library, I subracted the point $P$ from the centerpoint $P_c$ to get the distance vector $d_{vec}$
and used the function length2() to get the squared distance $d^2$ of the vector.
Instead of taking the square root, I squared the radius $R$ once as well and used that to quickly compare
if a point $P$ lies inside of the sphere.

$$ d_{vec,x}^2 + d_{vec,y}^2 + d_{vec,z}^2 <= R^2 $$

This is a faster approach as it only uses elementary operations,
while using square root is a way more expensive operation.

### Point on sphere path
As the sphere path can be modelled by a cylinder between two points,
you only need to find an intersection of a cylinder and a point.
For that you need to calculate the orthogonal distance between the cylinder axis and the point.
The point lies inside an infinite long sphere,
if the orthogonal distance is smaller than or equal to the radius.
Assuming a line, that is orthogonal to the axis and intersects with the axis as well as the point.
If the intersection point between this line and the axis is between the start and end point on the path,
the cloud point lies inside the cylinder with both constraints fulfilled.

To get this intersection point, you need to solve an equation.
The scalar product of the distance vector and the axis vector has to be 0,
only then they are perpendicular to each other.

$$ distanceVector * axis = 0 $$

The distance vector is calculated by subtracting the intersection point from
the cloud point.

$$ distanceVector = cloudPoint - intersectionPoint $$

The axis vector is calculated by subtracting the start point from the end point.

$$ axis =  endPoint - startPoint $$

The intersection point on the axis is defined
by multiplying the axis vector with an unknown scalar value x
and adding it to the start Point.
The scalar value has to be in the closed range [0,1] for the intersection point
to lie between start and end point.

$$ intersectionPoint = startPoint + axis * x $$

Then you can insert the definition for the intersection point into the orthogonality constraint
and solve it for the unknown scalar value.
After that you can calculate the intersection point and with that the distance between the axis and the cloud point.

I calculated the line Segment on which the intersection point lies,
by solving the equation and using the scalar product of the given Point3-library.
Then I checked if it is in the range [0,1].
After that I calculated the intersection point and the squared distance
between the cloud and intersection point and compared it to the squared radius.
 
## Problems with discrete steps
On a linear path of a sphere, the discrete steps have no influence
on the precision of the calculation.
While on a non-linear path, using discrete time steps lead to a loss of precision.
A direct linear path between single time steps is assumed and loses details of the curve path 
through approximation of the curve.
For example on a full circle path with only 4 time steps, we get a square instead of a circle,
while with an infinitesimal small time delta this circle is perfectly approximated.
