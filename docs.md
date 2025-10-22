QuantumGL

Wolfgang Thaller

3rd April 2004

Abstract

QuantumGL is an OpenGL-based tool for the visualization of volume
data. In a text ﬁle, you deﬁne the data to be visualized and the visu-
alization methods to be used. You can deﬁne an arbitrary number of
real-valued parameters which can then be modiﬁed interactively from the
graphical user interface.

Contents

I Visualization Task Files

1 Lexical Syntax

1.1 Whitespace and Statements . . . . . . . . . . . . . . . . . . . . .
1.2 Comments . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
Identiﬁers . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
1.3
. . . . . . . . . . . . . . . . . . . . . .
1.4 Floating-Point Constants

2 Variables

2.1 Predeﬁned Variables . . . . . . . . . . . . . . . . . . . . . . . . .

3 Expressions

3.1 Expression Attributes

. . . . . . . . . . . . . . . . . . . . . . . .
3.1.1 Type . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
3.1.2 Default Resolution . . . . . . . . . . . . . . . . . . . . . .
3.1.3 Range . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
3.2
Implicit type conversions . . . . . . . . . . . . . . . . . . . . . . .
3.3 Values . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
3.3.1 Literal Constants . . . . . . . . . . . . . . . . . . . . . . .
3.3.2
Imaginary Unit . . . . . . . . . . . . . . . . . . . . . . . .
3.3.3 Variable References . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . .
3.3.4 Field References
3.3.5 Field Minimum and Maximun . . . . . . . . . . . . . . . .
3.4 Operators . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
cut_to . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

3.4.1

3

3
3
3
3
4

4
4

5
5
5
5
6
6
6
6
6
6
6
7
7
7

1

scaled_to . . . . . . . . . . . . . . . . . . . . . . . . . . .
3.4.2
3.4.3 Vector Composition . . . . . . . . . . . . . . . . . . . . .
3.4.4 Unary Minus (-) . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . .
3.4.5 Arithmetic Operators
3.5 Functions . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
read . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
abs . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
Standard Unary Functions . . . . . . . . . . . . . . . . . .
Self-Made Functions . . . . . . . . . . . . . . . . . . . . .

3.5.1
3.5.2
3.5.3
3.5.4

4 Setting Options

4.1 nobox . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
4.2
colorbox . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
grid . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
4.3
4.4 background . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
fog . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
4.5
resolution . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
4.6
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
4.7 world_size

5 Field Declarations

6 Animations

7 Visualization Objects

7.1

Isosurfaces . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
7.1.1 Cyclic . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
7.1.2 Cutout . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
7.2 Slices . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
7.3 Vectors
. . . . . . . . . . . . . . . . . . . . . . . . . .
7.4 Flow Lines . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . .

7.4.1 Point Lists
7.4.2

7.3.1 Point Lists

Step Size

II Data Files

8 ASCII Data Files

9 Binary Data Files

III Extending QuantumGL

2

7
8
8
8
8
8
8
8
9

9
9
9
9
9
9
10
10

10

10

11
11
12
12
12
12
12
13
13
13

13

14

14

14

10 Predeﬁned Types

10.1 Operations on Scalar Values . . . . . . . . . . . . . . . . . . . . .
10.2 Operations on Vectors . . . . . . . . . . . . . . . . . . . . . . . .
10.3 Operations on Fields . . . . . . . . . . . . . . . . . . . . . . . . .

11 How to add a Field Function

11.1 The Header File
. . . . . . . . . . . . . . . . . . . . . . . . . . .
11.2 The Source File . . . . . . . . . . . . . . . . . . . . . . . . . . . .
11.3 The Field Function Registry . . . . . . . . . . . . . . . . . . . . .

15
16
17
17

17
17
18
18

Part I
Visualization Task Files

A Visualization Task File is a plain text ﬁle with the extension .qgl1. It consists
of a list of statements which are processed linearly when the ﬁle is opened in
the QuantumGL application.

1 Lexical Syntax

1.1 Whitespace and Statements

Whitespace (spaces, tabs and newlines) is not signiﬁcant, except where it is re-
quired as a separator. Every statement is terminated by a semicolon — multiple
statements may be put on one line, and a statement may span multiple lines.

1.2 Comments

Line comments start whenever the hash character (#) or two slash characters
(//) are encountered and last until the end of the line.

C-style block comments start with the sequence /* and last until the se-

quence */ is encountered. They can not be nested.

You can also use (* and *) to comment out a block of code. Other than the

C-style block comments above, you can nest this style of comments.

1.3

Identiﬁers

Identiﬁers consist of one or more letters, numbers or underscores (_). The ﬁrst
character of an identiﬁer must not be a number. Identiﬁers are case sensitive.

All reserved keywords used by QuantumGL are in lowercase. Future versions
of QuantumGL may have more reserved keywords, so in order to make sure that
your Visualization Task File can be used unchanged with future versions, use
at least one uppercase character in your identiﬁers.

1Pronounced “coogle”, like the German word “Kugel” (sphere)

3

1.4 Floating-Point Constants

Examples for ﬂoating-point constants:

42
3.141592654
0.5
-.5
3e17
-1.5e-6
.3e+2

Also, the constants π and 2π can be written as follows:

pi
2pi

2 Variables

Variables in QuantumGL have a real value. The QuantumGL application can
show a list of all deﬁned variables and allows to change their values. When
a variable is changed, all objects that depend on that value are automatically
re-calculated.

A variable is declared as follows:

variable identifier := constant ;

Please note that the right-hand value has to be a constant, i.e. the value cannot
depend on another variable.

When you declare a variable, you can also specify a minimum and maximum

value in square brackets:

variable identifier [constant,constant ] := constant ;

2.1 Predeﬁned Variables

QuantumGL prediﬁnes some variables for global camera and lighting paramet-
ers. You can change the initial value of these variables as follows:

identifier := constant ;

The following variables are predeﬁned in the current version of QuantumGL:

latitude The latitude angle of the camera, in degrees (−90◦ < latitude < 90◦)

longitude The longitude angle of the camera.

distance The distance of the camera from the center of the cube.

4

fovy The ﬁeld-of-view angle for the camera.

lightLatitude The latitude angle of the light source.

lightLongitude The longitude angle of the light source.

ambient The ambient lighting level.

lineWidth The width of the lines used for the bounding box and for flow

objects (in pixels).

3 Expressions

There are two kinds of expressions in QuantumGL: ﬁeld expressions and vec-
tor expressions. Scalar values are considered to be one-dimensional vectors in
QuantumGL.

A vector may have between one and ten components2. The components

of the vector may be either real or complex.

A field is a function that maps from R3 to a vector type.

3.1 Expression Attributes

Each expression and subexpression has a set of attributes which are determined
only by the expression and its subexpressions, not by its context. The ﬁrst
and most important of these attributes is the type of the expression, which has
already been mentioned above.

3.1.1 Type

Let K be either R or C and n a positive integer. A vector expression has a type
of K n and a ﬁeld expression has the type R3 7→ K n.

3.1.2 Default Resolution

This attribute applies to ﬁeld expressions only. The default resolution speciﬁes
at how many points the ﬁeld should be sampled when it is evaluated. You can
specify a diﬀerent resolution in every statement that evaluates a ﬁeld.

By default, this is initialized to a global default value (see section 4.6 on
page 10). For references to ﬁelds deﬁned using a field statement (see sec-
tion 5 on page 10), the default resolution is the resolution the ﬁeld was evalu-
ated at. For ﬁelds loaded from a ﬁle using the read function, it is the resolution
stored in the ﬁle.

For compound expressions, the default resolution is determined from the

resolutions of the subexpressions.

2This limit can be increased by modifying the maxDimensions constant in the source ﬁle

“CommonEvaluator.h” and recomiling QuantumGL.

5

3.1.3 Range

This attribute applies to ﬁeld expressions only. This attribute speciﬁes the range
of coordinates that the ﬁeld should be evaluated at. It is either an interval in
R3, or undeﬁned. Fields with undeﬁned range cannot be evaluated. You can
change this attribute using the scaled_to and cut_to operators.

For compound expressions, the ranges of all subexpressions whose range is
deﬁned have to match. This range is also the range of the compound expression.
If all subexpressions have undeﬁned range, the range of the compound expression
is undeﬁned, too.

3.2

Implicit type conversions

When you use a vector expression where a ﬁeld expression is required, the vector
is automatically converted to a constant ﬁeld. You cannot use a ﬁeld expression
where a vector expression is required. Likewise, real vectors or real vector ﬁelds
are automatically promoted to complex vectors or complex vector ﬁelds where
needed, but there is no implicit conversion in the opposite direction.

3.3 Values

3.3.1 Literal Constants

See section 1.4 on page 4 for the lexical syntax of ﬂoating point constants.
Constant expressions have the type R1.

A negative sign is not considered part of the constant. Rather, it is an

application of the unary minus operator (see 3.4.4).

3.3.2

Imaginary Unit

The constant i is deﬁned to be the imaginary unit constant. It’s type is C1.

3.3.3 Variable References

You can refer to variables (section 2 on page 4) by their name. As variables
have real values, the type of a variable reference expression is R1. The statement
containing the expression will automatically be recalculated when the value of
the variable is changed interactively.

3.3.4 Field References

You can refer to ﬁelds (section 5 on page 10) by their name. The resulting type
is, of course, that of the deﬁned ﬁeld, and it’s default resolution and range are
the resolution and range used to evaluate the ﬁeld in the field statement. If
a value between the evaluated data points of the deﬁned ﬁeld is required, it is
interpolated using trilinear interpolation. Outside of the range of the ﬁeld, its
value is considered to be 0.

6

3.3.5 Field Minimum and Maximun

field_name.min
field_name.max

You can access the minimum and maximum values of a real scalar ﬁeld by
adding .min or .max to the ﬁeld name, respectively. You can only do that for
ﬁelds that you have declared with a field statement, not to more complex ﬁeld
expressions.

3.4 Operators

3.4.1

cut_to

field_expression cut_to <[x_min,x_max ],[y_min,y_max ],
[z_min,z_max ]>

field_expression cut_to <x_diameter,y_diameter,z_diameter >
field_expression cut_to [min,max ]
field_expression cut_to diameter

The result is the same as field_expression with its range reduced to the
speciﬁed range.
If the field_expression already has a range, the speciﬁed
range must ﬁt inside it.

The range may be speciﬁed in several diﬀerent ways. You may specify the
minimum and maximum values separately for each dimension. You may also
just specify one [min ,max ] pair for all three dimensions. Instead of a pair, you
may also specify just a single value as the diameter of the interval. This is taken
to mean [-diameter/2,diameter/2].

Note that all the values on the right-hand side of the cut_to operator must

be constants, not expressions.

3.4.2

scaled_to

field_expression scaled_to <[x_min,x_max ],[y_min,y_max ],
[z_min,z_max ]>

field_expression scaled_to <x_diameter,y_diameter,z_diameter >
field_expression scaled_to [min,max ]
field_expression scaled_to diameter

The field_expression , which must already have a deﬁned range, is translated
and scaled. The old range of coordinates is mapped to the speciﬁed range. The
range of the result is equal to the speciﬁed range.

The range is speciﬁed in the same way as with cut_to (section 3.4.1). As
with cut_to, all the values on the right-hand side of the scaled_to operator
must be constants.

7

3.4.3 Vector Composition

You can compose a vector from scalar values by enclosing the components in
angle brackets, separated by commas:

<component1, component2, · · ·, componentN >

All of the components must be scalar expressions. The resulting vector is com-
plex if at least one of the components is complex, and real otherwise.

3.4.4 Unary Minus (-)

You can preﬁx any expression (both vectors and ﬁelds) with a minus sign to
negate it.

3.4.5 Arithmetic Operators

Arithmetic operators in QuantumGL may be applied to both ﬁelds and vectors.
The result is a vector if both arguments are vectors. Otherwise, the result is a
ﬁeld. If both arguments are real, the result is real, otherwise, it is complex.

The following table shows the available arithmetic operators and the allowed
combinations of vector and scalar types (let K be either R or C, and let n be a
positive integer ≥ 2):

operation
Addition
Subtraction
Multiplication
Division
Scalar Product

operator K ◦ K K ◦ K n K n ◦ K K n ◦ K n
no
no
K n
no
no

K n
K n
no
no
K

no
no
K n
K n
no

K
K
K
K
K

+
-
*
/
.

3.5 Functions

3.5.1

read

read("filename ")

The read function will read a ﬁeld from the speciﬁed ﬁle; the ﬁle format is
speciﬁed in Part II, “Data Files”. The data points from the ﬁle are spaced in the
unit cube (the range of the resulting ﬁeld is <[-1,1],[-1,1],[-1,1]>), and
the values are interpolated if necessary. The ﬁelds default resolution and type
correspond to the resolution and type speciﬁed in the data ﬁle.

3.5.2

abs

3.5.3 Standard Unary Functions

exp, sin, cos, tan, log, log10, sqrt, asin, acos, atan, sinh, cosh, tanh

8

3.5.4 Self-Made Functions

4 Setting Options

4.1 nobox

By default, QuantumGL draws a wireframe bounding box around the visualized
data. If a statement of the form

nobox;

is present in the .qgl ﬁle, the bounding box is not drawn.

4.2

colorbox

colorbox;
colorbox <r,g,b>;
colorbox <rx,gx,bx>, <ry,gy,by>, <rz,gz,bz>;

If you use a colorbox statement, the bounding box will be drawn in color. If
you specify three RGB colors, the ﬁrst parameter will be used for the edges
along the x-axis, the second for the y-axis and the third for the z-axis. If you
specify just one color, that will be used for all edges.

Using colorbox without parameters is equivalent to:

colorbox <1,0,0>, <0,1,0>, <0,0,1>;

This means that the edges along the x-axis will be drawn in red, those along
the y-axis will be drawn in green, and those parallel to the z-axis in blue.

4.3

grid

grid n ;

Draws a grid with n subdivisions on the faces of the bounding box.

4.4 background

You can set the background color using a background statement

background <red,green,blue >;

... where red, green and blue are constant values in the range 0–1.

4.5

fog

fog [near,far ];

Objects will start to fade if they are farther than near units from the camera.
At far units, they will be invisible (the same color as the background).

9

4.6

resolution

resolution res;

Fields will be evaluated at res × res × res data points by default.

4.7 world_size

world_size length ;
world_size [minimum, maximum ];
world_size <xdiameter, ydiameter, zdiameter> ;
world_size <[xmin,xmax ],[ymin,ymax ],[zmin,zmax ]>;

Deﬁnes the default size of the bounding box. If it is not speciﬁed, world_size
[-1,1] is assumed.

The world_size is automatically expanded to ﬁt all objects you deﬁne.
Therefore, you need only specify a world_size if you wish the bounding box to
be larger than the largest of your objects, or if you want to use bounding boxes
smaller than the default size of [-1,1].

The following world_size statements are equivalent:

world_size 2;
world_size [-1,1];
world_size <2,2,2>;
world_size <[-1,1],[-1,1],[-1,1]>;

5 Field Declarations

field identifier := expression ;
field identifier resolution res := expression ;

The expression must be a ﬁeld expression. The identiﬁer must not have been
declared. When this statement is encountered in the input, the ﬁeld expression
is evaluated at res × res × res datapoints, or at its default resolution (sec-
tion 3.1.2 on page 5), if no resolution is speciﬁed in the field statement.

You can later use the identiﬁer to refer to the evaluated ﬁeld (section 3.3.4 on

page 6).

6 Animations

animate variable_identifier from constant

[ and variable_identifier from constant

to constant [ inclusive]

to constant [ inclusive]] +

in n frames

[[ and variable_identifier from constant

10

in n frames] +;

to constant [ inclusive]] +

In order to create an animation with QuantumGL, you ﬁrst add an animate
statement to your .qgl ﬁle. Once you have done that, you can view the an-
imation in the QuantumGL application or export it to a series of image ﬁles
using the “Export Animation...” command. If there is more than one animate
statement, you will be able to choose which animation to export.

The simplest form of the animate statement refers to just one variable (that

must already have been declared using the variable statement):

animate foo from 0 to 1 in 10 frames;

This will set the variable foo to 0 for the ﬁrst frame of the animation, to 0.1 for
the second and so on, up to 0.9 for the tenth frame. The ﬁnal value of 1 is never
reached; use this form of the animate statement to generate an animation that
you will play back as a loop.

If you want the last frame to have exactly the ﬁnal parameter value that you

speciﬁed, add inclusive to the statement:

animate foo from 0 to 1 inclusive in 11 frames;

The variable foo will be 0 for the ﬁrst frame, 0.1 for the second, 0.9 for the
tenth and ﬁnally 1.0 for the eleventh.

You can also create an animation that consists of two parts:

animate foo from 0 to 1 in 10 frames

and foo from 1 to 2 inclusive in 20 frames;

In the above example, foo starts at 0, reaches 1 in the eleventh frame, and then
“slows down” so that it reaches 2 in the 30th frame.
You can also use two variables at the same time:

animate foo from 0 to 1 and bar from 1 to 0 in 10 frames;

7 Visualization Objects

7.1

Isosurfaces

isosurface field_expression
at value_expression

[ cyclic constant]
[ color field_expression]
[ cutout <[min_x,max_x ],[min_y,max_y ],[min_z,max_z ]>]
[ transparency value_expression]
[ shininess value_expression] ;

Renders a surface connecting all points where the ﬁeld has the value speciﬁed
after the at keyword.

11

7.1.1 Cyclic

When you want to create an isosurface from a ﬁeld whose values are periodic in
nature (angles, for example), then you have to specify the cyclic keyword and
the period (for example, 2*pi). Otherwise QuantumGL would think that the
ﬁeld has crossed the threshold value, when in fact it has just “wrapped around”
at 2π and gone back to zero without going through the threshold value.

7.1.2 Cutout

You can specify a range of coordinates to be “cut out” from the isosurface (so
you can see the interior). The values min_x, max_x, min_y, etc. have to be
constants.

7.2 Slices

slice

coordinate = value_expression

[ framed]
[ color field_expression]
[ cutout <[min_x,max_x ],[min_y,max_y ],[min_z,max_z ]>]
[ transparency value_expression]
[ shininess value_expression] ;

Renders a cross-section through the data. You can specify either x, y or z as the
coordinate . If the keyword framed is present, a white frame is drawn around
the borders of the cross-section.

7.3 Vectors

vectors field_expression

[ color field_expression]
[ secondary_color field_expression]
[ sphere_color field_expression]
[ sphere_radius field_expression]
[ at point_list]
[ shininess value_expression];

Renders one double-cone at each point speciﬁed in the point_list. One end of
the double cone is attached at the point, the length and direction of the double
cone is given by the value of the ﬁeld at that point.

7.3.1 Point Lists

Explicit List

value_expression [, value_expression]+

Each of the expressions is expected to be of type R3, a three-dimensional point.

12

Grid

grid <x,y,z>, <∆x,∆y,∆z>, <nx,ny,nz>

Rather than specifying all coordinates explicitly, you can also use the grid
keyword. The grid keyword will place nx × ny × nz points in a regular, three-
dimensional grid. One corner of the grid is speciﬁed by x, y and z; the points
are placed at ∆x, ∆y and ∆z units from each other in the x, y and z directions,
respectively.

7.4 Flow Lines

flow field_expression
[ at point_list]
[ color field_expression]
[ length value_expression]
[ stepsize expression]
[ shininess value_expression];

Renders one “ﬂow line” for each point speciﬁed in the at clause. The ﬂow lines
start in the speciﬁed points and follow the direction of the ﬁeld speciﬁed as the
ﬁrst parameter until they either reach a zero of the ﬁeld or they have reached
the maximum length. The maximum length can be speciﬁed using the length
keyword; if omitted, it defaults to 10.

7.4.1 Point Lists

You specify point lists for ﬂow lines in the same way as for vectors. See Section
7.3.1 for details.

7.4.2 Step Size

QuantumGL uses the Runge-Kutta integration method. The direction vectors
from the ﬁeld are normalized before they are used, therefore the magnitude of
the ﬁeld is irrelevant. The default step size is 0.05; you can override it using the
stepsize keyword.

You can also specify a ﬁeld as the argument of the stepsize clause in order
to achieve a variable stepsize. This might be useful if you want highly curved
ﬂowlines in one part of the image while saving computing power in others.

Part II
Data Files

Using the predeﬁned function read(“ﬁlename”), you can read in data generated
by other programs. QuantumGL currently supports two simple data formats,

13

one plain-text data format, and one binary data format. The plain-text format
is probably easier to generate in most situations, and it is more portable3. The
binary format is substantially faster and more compact.

8 ASCII Data Files

A QuantumGL ASCII data ﬁle consists of numbers separated by whitespace
(whitespace means spaces, tabs or linefeeds). It doesn’t matter if you put several
numbers in one line, or if you put all numbers in lines of their own, all that counts
is that there is at least one space, tab or linefeed in between.

• The ﬁrst number has to be either 0 or 1. A value of 0 means that it’s a

real-valued ﬁeld, a value of 1 means it’s complex.

• The second number indicates the number of components of each vector.

Use 1 for scalar ﬁelds.

• Next, there are three numbers indicating the number of data points in x,

y and z directions.

• Finally, for every data point, a vector is stored.

A complex number is stored as two consecutive real numbers. A real vector is
stored as n consecutive real numbers (real part and imaginary part). A complex
vector is stored as n consecutive complex numbers (pairs of real numbers).

9 Binary Data Files

A QuantumGL binary data ﬁle starts with a twelve-byte header structure shown
in Table 1.

After that, there’s one vector per data point. A vector consists of n consec-
utive real or complex values. A complex value consists of two consecutive real
values. A real value is stored as a big-endian 32-bit IEEE-754 ﬂoating point
value.

Part III
Extending QuantumGL

With basic knowledge of C and just a few bits of C++, you can extend Quan-
tumGL by adding your own ﬁeld-valued functions. This is both faster and more
convenient than using a separate program to generate big data ﬁles.

3Binary data ﬁles can be exchanged between Intel- and PowerPC-based computers, and
with other computers that use the IEE-754 ﬂoating point format. “Exotic” platforms might
cause trouble, though.

14

Size

4 bytes

Description

magic number

Format

ASCII codes for ’B’, ’i’, ’n’, ’F”

(for Binary Format)

1 byte

real/complex indicator

ASCII ’C’ (0x43) for complex,

’R’ (0x52) for real

1 byte

number of components per vector

unsigned one-byte integer

2 bytes

number of data points in x direction

unsigned two-byte integer, big-endian

2 bytes

number of data points in y direction

unsigned two-byte integer, big-endian

2 bytes

number of data points in z direction

unsigned two-byte integer, big-endian

12 bytes

total size

Table 1: Header for Binary Data Files

By writing a small piece of C++ code, you can deﬁne functions that can
be used in the Visualization Task File. These functions can take both ﬁelds
and real numbers as arguments. The implementation currently requires that
the ﬁelds come ﬁrst, i.e. the function can take zero or more ﬁeld arguments
followed by zero or more real arguments. The total number of arguments is
currently limited to seven, but this limit can be increased easily.

As an example, we will describe how to write a new color function that uses a
blue-red-gradient to visualize scaler values. We will write a function that takes
a real scalar ﬁeld, a (real) minimum value and a (real) maximum value, and
returns a ﬁeld of R3 vectors (which will be interpreted as RGB colors). The
function will map the minimum value (and everything below it) to blue and the
maximum value (and everything above it) to red.
The function therefore has the following type:

(cid:0)(cid:0)R3 7→ R(cid:1) × R × R(cid:1) 7→ (cid:0)R3 7→ R3(cid:1)

(1)

In C++, however, it is diﬃcult to create a function of this type, because
it is diﬃcult to dynamically create a new function (we want to return a ﬁeld,
which is a function R3 7→ R3). Instead, we will create an equivalent function
that evaluates the resulting ﬁeld at one point and returns just a plain vector.
The type of this new function is

(cid:0)R3 × (cid:0)R3 7→ R(cid:1) × R × R(cid:1) 7→ R3

(2)

If we later want to evaluate the resulting ﬁeld, we just have to call the
function repeatedly, once for each point. QuantumGL does this automatically.

10 Predeﬁned Types

QuantumGL deﬁnes a set of C++ types (available in the header ﬁle “FieldFunc-
tion.h”)

15

Real a real (ﬂoating-point) number. Use this instead of float or double4 (R).

Complex a complex number (C).

vec<Real,n> a vector with n real components5 (Rn).

vec<Complex,n > a vector with n complex components (Cn).

r1field a real-valued scalar ﬁeld (R3 7→ R).

c1field a complex-valued scaler ﬁeld (R3 7→ C).

rvecfield<n> a real vector ﬁeld with n components (R3 7→ Rn).

cvecfield<n> a complex vector ﬁeld with n components (R3 7→ Cn).

So if we write the type (2) in C++, we get the following prototype for our
function in C++:

vec<Real,3> bluered(vec<Real,3> pos,

r1field f,
Real minValue,
Real maxValue);

10.1 Operations on Scalar Values

Real and Complex in QuantumGL are just synonyms6 for float and complex<float>,
so you can use all the basic arithmetic operators and the functions deﬁned in
the C++ standard library. Consult your favourite C++ textbook.

Do not use float or double instead of Real, because in future versions,
Real might be deﬁned to double instead. Also, always use Complex rather than
complex<float> or complex<double>.

Instead of using a normal ﬂoating point literal like 0.5, you might want to

write Real(0.5) to avoid problems with C++’s type system7.

If you want a complex constant or if you want to build a value of type
Complex from two Real values, write Complex(x,y) where x is the real part
and y the complex part.

4currently, it is deﬁned to be ﬂoat. This might be changed in future versions of Quan-

tumGL.

5n has to be an integer constant — the dimension of a vector has to be known at compile

time

6deﬁned using typedef
7A plain ﬂoating point literal like 0.5 is considered to be of type double. Thus, when
working with variables of type Real or float and constants of type double, C++’s implicit
type conversions are used. This can lead to error messages that are hard to understand for
C++ beginners.

16

10.2 Operations on Vectors

You can create a real vector (type vec<Real,n>) by calling the function makeVecR()
with n parameters (of type Real). Likewise, you can use makeVecC() to create
complex vectors (type vec<Complex,n>).

You can access the nth component of a vector using the [] operator, that is

v[1] is the ﬁrst component of the vector v.

The basic arithmetic operators are deﬁned (“overloaded”) for vectors:

+ Addition

- Subtraction

* Multiplication by a scalar value. Both scalar *vector and vector *scalar

are allowed.

/ Division by a scalar value.

The function vdot(a,b) calculates the inner product of a and b, and the function
abs(x) returns the absolute value of a vector.

10.3 Operations on Fields

You can access the value of a ﬁeld at any point in space using parantheses:
myField(pos) is the value of the ﬁeld myField at the position pos. The position
must be a value of type vec<Real,3>.

You cannot explicitly create a ﬁeld yourself. You can only get them as
parameters for your function. That’s why we write our function in such a way
that we do not explicitly return a ﬁeld.

11 How to add a Field Function

When you add a new Field Function, you have to modify three diﬀerent ﬁles:
One source ﬁle, one header ﬁle, and the ﬁle FieldFunctionRegistry.cp.

11.1 The Header File

The header ﬁle contains a so-called prototype for our function; it just tells the
C++ compiler that a function with that name and that type is deﬁned else-
where. Note that you can put prototypes for more than one function in the
same header ﬁle. A header ﬁle should have a ﬁle name that ends in “.h”. For
our “bluered” example, we can create a text ﬁle named “BlueRed.h”, with the
following contents:

#include "FieldFunction.h"

namespace field

17

{

}

vec<Real,3> bluered(vec<Real,3> pos,

r1field f,
Real minValue,
Real maxValue);

11.2 The Source File

Next, we need the source ﬁle, where we will put in the actual code. The source
ﬁle should have the same name as the header ﬁle, but end in “.cp” 8 instead of
“.h”. The source ﬁle has to include the header ﬁle at the top. For our example,
it could look like this:

#include "BlueRed.h"

vec<Real,3> field::bluered(vec<Real,3> pos,

r1field f,
Real minValue,
Real maxValue)

{

}

Real t = (f(pos) - minValue) / (maxValue - minValue);
if(t < 0)

t = 0;

if(t > 1)

t = 1;

return makeVecR(t,1-t,0);

11.3 The Field Function Registry

To make our example Field Function “bluered” known to QuantumGL, we have
to add one line to the function RegisterFieldFunctions in the ﬁle FieldFunctionRegistry.cp:

RegisterFieldFunction("bluered",&field::bluered);

At the top of the same ﬁle, we have to add an #include statement so that the
compiler can see the prototype from our header ﬁle:

#include "BlueRed.h"

8The extension “.cp” for C++ source ﬁles is not very widely used — it’s an old Macintosh
tradition. You can also use “.cc”, “.cpp”, “.cxx”, or “.C”. Do not use “.c”, as this would mean
“plain” C, not C++.

18


