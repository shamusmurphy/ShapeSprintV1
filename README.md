# ShapeSprintV1!
My open ended project is my own variation of Geometry Dash. I created this project as a baseplate for my final project. The game in its current state has one level. The user is not able to move except for jumping as triangular obstacles approach it. Currently there is six combinations of obstacles that loop trying to hit the player. The only goal currently is to stay alive as long as possible. If you run into an obstacle, the game ends and you are prompted to return to the menu. The two main features of this project are the jumping mechanics and the collision detection.

## Jumping and Collision
When I was initially working on this project, I knew from the start the two things that needed to be done were the jumping and collision. When I was thinking about the jumping, I knew there needed to be velocity and gravity. After brainstorming the velocity for a while I decided to have it relative based on the time and the gravity I gave a number based on how my code was working. The jump height along with the gravity values came with testing my function repeatedly with different values and finally getting the height I wanted. The way the triangles move it makes it seem like the user is moving towards the triangles as well.

The collision was definitely a little tricky. At first I was trying to base it off of the coordinates of the triangle itself and it was very frustrating. It took me a couple of days but I decided that using Axis Aligned Bounding Box collision detection worked really well. I used this tutorial https://tutorialedge.net/gamedev/aabb-collision-detection-tutorial/ which was made in java to adjust the triangle overlapping function. To make the user able to jump closer to the triangles to cover more distance, I shrunk the values a little bit. It made the game run much more smoothly and worked really well.

## Future Work
I am going to continue working on this for the final project. My idea is to create a better menu for the game having a place to choose your level, change your character, and maybe even have another game. I think a game hub or something along those lines would be really cool. I can utilize my code to create other mini games. But definitely within the next few weeks the user interface will look much cleaner and more levels will be implemented.

## Code Used
A lot of the base code for the graphics was provided by my instructor for Advanced Programming Lisa Dion. She is an amazing professor and I could not imagine learning C++ with a different teacher. 
