# grav_c
a simple particle-gravity simulation  cli written in C .  
though initial assigning random velocities and position , and let the particles move with velocity in a 2d grid inside the terminal..
particles move wrt the gravitional pulls from each other . | code has an N-Body failsafe that clamps the velocity or the speed in given (x,y) directions. 





<img width="594" height="309" alt="orbiters" src="https://github.com/user-attachments/assets/d7ce6632-7f7d-4124-99b3-de4215faf3c6" />


right now , i have an inviible grid of height and width , if a particle collides to the screen bounds , it gets terminated.

also : the trail length is 8 dots | hard coded but it doesn't render every one of them , because :
   the orbiter is moving fast enough that some trail positions land on the same character cell as each other (since my grid only has whole-number/int resolution), so multiple trail history entries resolve to 
   invisible dots .







enabled TERMINAL_VIRTUAL_PROCESSING and ansi - as i was using an old mingw ; due to hardware constraints.. as i am working on a x86 32 bit or i686 arch machine.


implementations yet:
1.Struct-based simulation state 
2.Render loops + ANSI terminal control
3.N-body gravitational force calculation 
4.including softening/clamping to prevent singularities
5.Euler integration (v += a, x += v)
6.Orbital mechanics — tangential velocity, the sqrt(GM/r) relationship
7.numerical-stability bug due to units/magnitudes.
8.colors and dot trails for particles with the help of circular queue.
