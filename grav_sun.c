#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define G 0.5  //grav constant | not irl value

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define NUM_PARTICLES 5
#define WIDTH 80
#define HEIGHT 24

//defining consant as i am using older mingw
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
//typedefining struct for a particle [velocity , posn , mass , symbol]

typedef struct 
{
	double x,y;
	double vx , vy;
	double mass;
	char symbol;
} Particle ;

//fxn to enable asni in old cmds 
void enable_ansi() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut , &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut , dwMode); 
}

//sun - fixed posn | orbitar v corresponding to dist btw sun and their own  
 // velocity must be perp to the line from sun to orbiter. / tangential to citcular trajectory/path
  // Vorb = √ (G * mass(sun) / dist) 
   // angle =  angular posn



//init particle fxn (*arr of particles) -> fill the array with some values
//should return void ! | set rand val of V,(x,y),m,s for each particel=index 
//within my screen bounds.

void init_particles(Particle particles[], int count) {
     //defining particle[0] = SUN [heavy , fixed at center]
	particles[0].x = WIDTH / 2.0;
	particles[0].y = HEIGHT / 2.0;
	particles[0].vx = 0;
	particles[0].vy = 0;
	particles[0].mass = 5000.0; //much heavier then orbtrs
	particles[0].symbol = '@';    //later can use unicode '⬤'
  
    
    //rest [1-5] particles are orbiters
	for (int i = 1; i < count; i++) //loop over each particle. 
	{
        
        double angle = ((double)rand() / RAND_MAX) * 2 * M_PI; //random angle around sun

        double dist = 5 + (rand() % 15); //rasndm dist from sun (5-20)

        particles[i].x = particles[0].x + cos(angle) * dist;
        particles[i].y = particles[0].y + sin(angle) * dist;

        double orbital_speed = sqrt(G * particles[0].mass / dist);
        
        //velocity perp to radius vector = tangent direction
        particles[i].vx = -sin(angle) * orbital_speed;
        particles[i].vy = cos(angle) * orbital_speed;



		particles[i].mass = 1.0;
		particles[i].symbol = 'o';
	}
}
//fxn update posn(*partlicle arr) -> updated posn rest same array
//return void! //within sceen bounds!
void update_positions(Particle particles[], int count , double ax[] , double ay[]) {
	for (int i = 1; i < count; i++) //for each partcle add v to posn | except sun = particles[0]
 	{                                         
       particles[i].vx += ax[i];  //speed incr with  corspnding acc
       particles[i].vy += ay[i];     //by euler's integration!

       //safety clamp - preetn runaway velocitys
       double speed = sqrt(particles[i].vx * particles[i].vx + particles[i].vy * particles[i].vy);
         if (speed > 5.0 ) 
         {
         	particles[i].vx = (particles[i].vx / speed) * 5.0;
         	particles[i].vy = (particles[i].vy / speed) * 5.0;
         }



       particles[i].x += particles[i].vx;
       particles[i].y += particles[i].vy;       	
	}
}
//fxn to rendergrid&particles(arry of particle) ->  print grid
  //grid of char woth hight and width
void render(Particle particles[] , int count) {
    char grid[HEIGHT][WIDTH];

    //1.fill canvas with blank spce
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
        	grid[y][x] = ' '; //grid holding space
   // plot each particle
    for (int i = 0; i < count; i++)
        	{
        	   int px = (int)particles[i].x;
        	   int py = (int)particles[i].y;

        	   if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) 
        	   {
        	   	   grid[py][px] = particles[i].symbol;
        	   }
        	}    	
        
   //jump cursro to top left
      printf("\033[H");

   //print grid row by row!
      for (int y = 0; y < HEIGHT; y++)
      {
      	for (int x = 0; x < WIDTH; x++)
      	{
      		putchar(grid[y][x]);
      	}
      	putchar('\n');
      }

}


//gravitational acc of particles - due to other particles
 //acc is not permnnt proprty of particle -no struct chnge
   

// grav F fxn (particle arr , ptr ax , ptr ay) -> void/ fill acc value in particle arr



void compute_forces(Particle particles[] , int count , double ax[] , double ay[])
{
	//init evry elm of arr with zero
	for (int i = 1; i < count; i++)
	{
		ax[i] = 0;
		ay[i] = 0;

		//calc distance , btw i  , rest of the particle =j
		for (int j = 0; j < count; j++)
		{
			double dx = particles[j].x - particles[i].x;
			double dy = particles[j].y - particles[i].y;
			double dist_sq = dx*dx + dy*dy;
			if (dist_sq < 0.25) dist_sq = 0.25; //clamping dist val to avoid near zero explosion
			double dist = sqrt(dist_sq); //distance  


			//if (dist < 0.5) dist = 0.5; //bounding dist = prevent div by near zero explosn

            //calc force
            double force = (G * particles[j].mass) / dist_sq;

            ax[i] += force * (dx / dist ); //x-compnt of pull
            ay[i] += force * (dy / dist ); //y-component of pull

		}
	}
}

//then pson updates with rt accn





//mainfxn
int main() {

	enable_ansi();  //to enable asni adn vitual terminal proceses in terminal!
	srand(time(NULL));  //to get same rnad no: every run!

	Particle particles[NUM_PARTICLES];
	init_particles(particles , NUM_PARTICLES);

    double ax[NUM_PARTICLES]; //temp accn array
    double ay[NUM_PARTICLES];


	printf("\033[2J");  //clear screeen once

	while(1) {  //indef loop
		compute_forces(particles , NUM_PARTICLES , ax , ay);
	    update_positions(particles , NUM_PARTICLES, ax , ay);
	    render(particles , NUM_PARTICLES);
	    Sleep(33);  //sleep for 33ms | 30fps around 

	}
  return 0; 
}




