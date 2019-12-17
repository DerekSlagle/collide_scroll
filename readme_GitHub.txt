https://github.com/pfloyd123/Physics-Engine

readme:

Collisions between various types, blast effects and more in this 7 level tester.
This readme gives a partial description which will be added to.

Five class heirarchies are developed.

1. Extended body objects which move and collide with each other. Abstract base class is mvHit (they move and hit each other). Instantiable types are ball (round), regPolygon (regular polygon shapes) and some types derived from them. See readme in collider folder for details.
 Momentum is conserved in collisions. Kinetic energy is conserved if the coefficient of restitution = 1.0. Object overlap is prevented by corrective position assignment in collision response, but the center of mass position of the colliding pair is preserved. This allows energy conservation if force fields (eg. gravity) are being applied. Acceleration may be applied to each mvHit by passing as argument in an overloaded update function. These types can also have a fixed position (via bool is_free data member) to serve as a rigid obstacle. This is used in 2 levels where type gun (a regPolygon type) has a fixed position. If is_free is made true (by use of gun control) then recoil applies and gun will move when fired. The gun can behave as a rocket in full auto fire mode. expandPolygon is a special type which can be trigerred to grow or shrink between limits at an assigned rate. The velocity of the expanding boundary is added when colliding with other objects. Each of these are tested in several levels.

2. Line or curve shaped colliders interact with mvHits but not each other. Abstract base type is segHit (line or curve segment which can be hit). Instantiable types are lineSeg (straight line) which has a fixed position and derived types which can move. Examples of moving Types derived from lineSeg are:
lineSegRotate rotates at a constant rate about one end.
lineSegSwing rotation speed changes when hit. Angular momentum is conserved in collisions with mvHits.
lineSegElevator moves at a constant velocity between assigned position limits.
lineSegConveyor (tangential velocity is transformed) acts as a conveyor belt for mvHits.
lineSegFlip acts like a flipper in a pinball game.
lineBumper adds velocity to mvHit normal to the bumper. Acts like an active bumper in a pinball game. It has a brief visual movement when hit as well.
Most of these types are used on level 2.
waveSeg. This curved type has left and right traveling sine waves. The amplitude and wavelength can be varied for both components. waveSeg and composite types using it (waterTanks) are tested on level 1. Other composite types are the pipes connecting the tanks. Fluid speed in the pipes can be varied (see floatSpot UI type below). A velocity cone applies at pipe ends so mvHits can be sucked into, or blown away from the pipes.

All segHit types can be either "hard" (rigid collisions) or "soft" (buoyancy is applied). In soft mode lineSegConveyor can act as a river to mvHits, which will bob on the surface. waveSeg will toss mvHits about on the surface. Fluid density can be varied. buoyancy is applied above the surface as well so mvHits can float in the lower density air above the wave. All types can be penetrated in soft mode. This mode supports some basic water physics.

3. Leg types. These allow programatic motion of mvHits. Leg = a path segment (or "Leg" of a path). The adapter type pathMover coordinates usage. A pathmover has 2 move modes: Rigid mode assigns the position of a mvHit (or target or ship) to the path position. Collision response with these mvHits is poor.
Damped oscillator mode. The mvHit is treated as a damped ocsillator about the assigned path position.
The Leg class is abstract. The parametric equations of motion are virtual so the segment shape is defined in each instantiable type.
Legs have pointers to the previous and next Legs in the path. Movement from one Leg to the next is managed and can be in either direction.
Several Leg type constructors support construction of connected Legs and closed paths. A control is provided on levels 5 and 6 for varying the path speed (and direction) as well as the restoring and damping forces applied to each mvHit on the path. Many closed paths are used in level 6 - Shots and Targets.

4. Level types. These are used to define play levels. Abstract base type is Level. It includes a variety of built in resources so that instantiable Level types can be defined more simply. Each of the 7 levels is derived from type Level.
Built in features include:
Keyboard actions. The Escape key will immediately close the app (though shutdown is orderly), F1 = play/pause toggle. F2 will step one frame while paused. Use the Esc key (or red X) to close the app when finished.
Mouse action. If a static mvHit pointer is assigned the mvHit can be flung. Left mouse button down (to place), drag (to assign velocity) and release mouse button to fling the collider. Some levels allow a mvHit to be selected for flinging. Hover over the mvHit (use F1 to stop it if needed) and press F4. The selected mvHit is highlighted with a cyan dot at the center.
A state recorder. This appears on most levels. It can be used to record and replay a simulation. Control element (UI) settings are also recorded. Recorder usage is a bit tricky. It records an assigned number of frames (1000 typically, allowing about 15-20 seconds to be recorded). The simulation is paused when the frame limit is reached. Use F1 to resume simulation. States will be overwritten so the last 1,000 frames are always recorded. This allows the recorder to be left running. Any observed behavior can be reviewed in replay if recording is stopped in time (15 seconds or so). The recorder has forward and reverse single frame jog functions. Play can be rewound fully, or by 40 frame increments to support finding recorded events, which can then be jogged across frame by frame.
A map view is also supported. Level 1 has an extended area. Slide bars allow scrolling of the view. View scrolling can be recorded and replayed. The map view also works with the control elements. Controls can be fixed or positioned on and move with the elements controlled.
For example, the wave controls on level 1 stay on the tanks they are associated with as the view is scrolled.

5. UI objects. Abstract base type is button (it all started as just a clickable button). This class includes static methods which support the integrated use of all UI elements in a level. Once a button object is initialized (including callback definition) and "registered" its usage requires no further coding in the level. Static methods also track mouse state, whether mouse is over any button, etc.
 Derived types include:
A. buttonRect. A rectangular button. The button state (selected or not) can be toggled, or it can act as a momentary contact (NOHC). This property can be assigned for each instance.
B. buttonList. This drop list type can have any derived button types as members. buttonList can have just buttonRect members making it a standard drop list. A constructor allows simple construction for this case. 
Examples of lists with other member types include:
The wave control for each waveSeg on level 1, which is a drop list of slide bars (or clickable strips) for varying the amplitude and wavelength of each wave component.
The gun control on levels 1 and 6 are a buttonList with 2 buttons: joyButton (allows aiming and firing) and another buttonList. The sub list has a radioButton type for varying fire rate and a clickable strip for varying bullet speed.
Type buttonList manages its members and serves as a proxy for them to the static management methods.
C. joyButton. This is a joystick. The "stick" is "magnetic" and doesn't need to be held. Move mouse over the joystick button at the center to "pickup" the control. The stick is dropped (and returns to center) when the mouse is moved outside the control boundary. This operation allows both mouse buttons to be used for other than holding the control. When used as a gun control (levels 1,6) the left mouse button fires the gun and the right button toggles the is_free member of the gun. When is_free = true, recoil applies and the gun will move in response to firing.

*** I apologize for the hastily chosen type names. I may change them since I've now posted this work publicly.



Level descriptions:


1. Tanks and Pipes. 
Water tanks and connecting pipes are constructed from line segment (lineSeg) type. 
Ball colliders with neutral buoyancy flow through the system. 
Flow direction and speed in the pipes are set using the control type floatSpot. Hover over Cyan spot to view the fluid speed. The fluid speed can be changed by left click on the spot, then clicking on the value strip which appears below the spot.

Waves in the tank are the sum of left and right traveling waves. Each tank has a Wave control button. Click to open 4 value setting controls.
The left and right wave component each have a control to assign amplitude and wavelength.

Use the Gun control to aim and fire the balls into the tanks. 
The Gun, balls and 2 other colliders can be selected for flinging. Hover over the object then press F4 to select it.


Each collider can be flung: Left mouse button down (to place), drag (to assign velocity) and release mouse button to fling the collider.
The Hexagonal collider is type expandPolygon. Use Key Z to shrink or expand. Use the mouse scroll wheel to vary the expanded size.

To be contd...



2. State tester
Several mvHit types and most lineSeg types are used together.



3. Blasting
Effects of an expanding blast wave on a grid of balls is explored here. Left click to detonate at mouse position. Right click to reset ball positions. Controls are provided for varying blast parameters and the restoring and damping forces applied in the default mode where positions are restored. Waves will propagate through the grid.
The balls can also be freed to move and collide with each other. See instructions displayed in the upper left window corner.



4. Intersections


Fling the selected mvHit to cause it to intersect several others. A drag force is applied so it will come to rest.
Intersection begin and end events are detected. Output is provided to show results.

5. Platformer
Elements of a platformer type game are being developed here. Mario style wall jumping is possible. The player starts in a well, though the player can be positioned elsewhere via left click. Key H toggles display of keyboard usage, such as F5 for mounting/dismounting ladders between platform levels. Controls are provided to vary many parameters.
A hybrid collision detection method is developed. mvHit collisions (as in bullet vs baddie) are position based (not ray based) to allow precision in the collision response but this is a problem when small fast moving mvHits (bullets) are in use. Bullets move many times their size per frame so they could cross over targets. Bullet motion and collision testing is done in partial steps. The distance to the nearest target is found. If a target is close enough to be hit the bullet is moved to a position very near the target then "baby stepped" to the collision point. Full steps and baby steps are counted and displayed (in the console window) to help assess the efficacy of this method.
It seems to work well.



6. Shots and Targets
Numerous closed (and other type) paths for the programatic motion of targets are tested. A droplist is provided for selection of path to be tested. Several new shot types are tested here as well.
A. Regular bullets in single or full auto fire.
B. Exploding rounds. Fuse is timer based. Vary shot speed to control distance travelled when shell detonates into numerous fragments.
C. Homing shot. Use F6 to cycle through targets. The targets health bar will activate so you can tell which target the homing shot will go after. Single shot mode only is supported. Target assignment can be changed after the shot is fired (via F6).
D. Laser. This applies damage on a per frame basis. Use the radio button control labeled "shot type" to vary. The shot types aren't individually labeled (apologies for this shortcoming. I may fix it with a hover effect).
The order is: Homing shot, Exploding shot, Normal bullet (full auto works for this one) and the laser.
The kill count in the bottom right corner is an array of type flipDigit. Who remembers the 1st digital clocks with mechanical digits which hinge in the middle and flip downward to reveal the next number? When I stumbled across an image of these digits online one day I knew what to do. sf::Quads are used so I can vary the length of the top edge to give some illusion of the upper half falling forward. It can be tested directy. Key 1 will increase the count by 1. Key 5 increases the amount by 5. The display counts by ones so increment values > 1 must be cached so the display can catch up.



7. Conways Game of Life
This is a bit out of place in this tester app.
A drop list allows numerous seed populations to be assigned. A click strip allows the generation period to be varied so simulation can be fast/slow. I'm working on efficient cell update methods. I want to restrict the number of cells updated each generation to a minimum.
A bounding box is maintained (and shown as a red box bounding the live cell area. Only a single bounding area is used (so far) so in some simulations (eg Gosper gun) the bounding area grows indefinitely. Work in progress.
Ideas for improvement are quite welcome.