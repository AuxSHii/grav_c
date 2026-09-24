#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>   //to read key inputs

#define MAX_PARTICLES 50
#define INITIAL_PARTICLES 5

#define G 0.05  //grav constant | not irl value

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define WIDTH 80
#define HEIGHT 24

//defining consant as i am using older mingw
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
//typedefining struct for a particle [velocity , posn , mass , symbol]     
  //ds - history buufer - fixed size arr as queue = posn of previous frame get overwritten as particle moves / i.e. each frame
   //circular buffer = updating posn at frame array
    //trail_index = (trail_index + 1) % TRAIL_LENGTH ->to overwrite the oldenst entry
#define TRAIL_LENGTH 8

typedef struct 
{
	double x,y;
	double vx , vy;
	double mass;
	char symbol;

	double trail_x[TRAIL_LENGTH]; //trail posns array 
	double trail_y[TRAIL_LENGTH];
	int trail_index;     //where to write the 'next' position
} Particle ;


//Fxn to clear screen - ansi escape seq
void clearScreen() {
  printf("\e[1;1H\e[2J");  //ansi escape seq-> clear screen
}




//fxn to show menu -> int option taken from user

int show_menu(){


clearScreen();

//show the menu
    printf("========================================\n");
    printf("   ORBIT - choose a mode\n");
    printf("----------------------------------------\n");
    printf("1) Single sun, calculated orbits\n");
    printf("2) Binary suns\n");
    printf("3) Free chaos\n");
    printf("4) Sandbox mode\n");
    printf("----------------------------------------\n");

   int option = 0;

   while(option < 1 || option > 4) {  //until valid opt are given
       printf("choose an option (1-4): ");
       char key = _getch();  //whateveer typed by user take it in key

       printf("%c\n", key );  //echo key back
       option = key - '0';   //convt char digit to int
       
   }

   return option; 
}















//fxn to enable asni in old cmds 
void enable_ansi() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut , &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut , dwMode); 
}

//program title screen
void draw_atom_frame(double angle) {
    char grid[15][40];
    for (int y = 0; y < 15; y++)
        for (int x = 0; x < 40; x++)
            grid[y][x] = ' ';

    int cx = 20, cy = 7; // center of the grid

    grid[cy][cx] = '@'; // nucleus, fixed center

    // 3 electrons, each on a differently-squished, differently-offset ring
    double offsets[3] = {0, 2.1, 4.2}; // spread evenly (roughly 120 degrees apart in radians)
    double squish[3]  = {1.0, 0.4, 0.4}; // ring 1 = wide ellipse, rings 2/3 = tighter ellipses

    for (int e = 0; e < 3; e++) {
        double a = angle + offsets[e];
        int ex = cx + (int)(cos(a) * 15);
        int ey = cy + (int)(sin(a) * 5 * squish[e]);

        if (ex >= 0 && ex < 40 && ey >= 0 && ey < 15) {
            grid[ey][ex] = 'o';
        }
    }

    printf("\033[H");
    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 40; x++) {
            char c = grid[y][x];
            if (c == '@') printf("\033[33m%c\033[0m", c);
            else if (c == 'o') printf("\033[36m%c\033[0m", c);
            else putchar(c);
        }
        putchar('\n');
    }
}

//screen fxn to play atom

void show_title_screen() {
    printf("\033[2J");
    double angle = 0;

    for (int frame = 0; frame < 60; frame++) {
        draw_atom_frame(angle);
        angle += 0.15;
        Sleep(33);
    }

    printf("\033[2J\033[H");
    printf("\033[36m");

    printf("                                     ========================================\n");
    printf("                                       ____  ____   ____ ___ _____ \n");
    printf("                                      / __ \\|  _ \\ | __ )_ _|_   _|\n");
    printf("                                     | |  | | |_) ||  _ \\| |  | |  \n");
    printf("                                     | |__| |  _ < | |_) | |  | |  \n");
    printf("                                      \\____/|_| \\_\\|____/___| |_|  \n");
    printf("                                        a terminal gravity sandbox\n");
    printf("                                     ========================================\n");
    printf("\033[0m");

    printf("\n\033[90m                                               - ashi\033[0m\n");
    printf("\n                                     Press any key to continue...");
    _getch();
}




//FXN spawn particle -> void / modify the particle arr

void spawn_particle(Particle particles[], int index) {
	double angle = ((double)rand() / RAND_MAX) * 2 * M_PI;

	double dist = 5 + (rand() % 15);

	//giving it some posn
	particles[index].x = particles[0].x + cos(angle) * dist;
    particles[index].y = particles[0].y + sin(angle) * dist;

    double orbital_speed = sqrt(G * particles[0].mass / dist);
    
    particles[index].vx = -sin(angle) * orbital_speed;
    particles[index].vy = cos(angle) * orbital_speed;

    particles[index].mass = 1.0;
    particles[index].symbol = 'o';


    for (int t = 0; t < TRAIL_LENGTH; t++) {  //for evry trail indx
        particles[index].trail_x[t] = particles[index].x; //init with spawning posn
        particles[index].trail_y[t] = particles[index].y;
    }
    particles[index].trail_index = 0;


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
	particles[0].mass = 20.0; //much heavier then orbtrs
	particles[0].symbol = '@';    //later can use unicode '⬤'
  
    
    //call spawn particles 
    for (int t = 0; t < TRAIL_LENGTH; t++)
    {
    	particles[0].trail_x[t] = particles[0].x;
    	particles[0].trail_y[t] = particles[0].y;
    }
    particles[0].trail_index = 0;

    for (int i = 1; i < count; i++) {
        spawn_particle(particles, i);
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
    //RENDERING TRAIL -> render  trail on grid & live posn on top!


void render(Particle particles[] , int count) {
    char grid[HEIGHT][WIDTH];

    //1.fill canvas with blank spce
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
        	grid[y][x] = ' '; //grid holding space

   // draw trails in background
    for (int i = 0; i < count; i++) // for evry particle
    {
       for (int t = 0; t < TRAIL_LENGTH; t++)
       {
       	  int tx = (int)particles[i].trail_x[t]; //storing trail posn 
          int ty = (int)particles[i].trail_y[t];

          if (tx >= 0 && tx < WIDTH && ty >= 0 && ty < HEIGHT) 
          {
          	if (grid[ty][tx] == ' ')  // no overwriting / place trail dot only of there is space in grid.
          	{
          	 grid[ty][tx] = '.';
          	}
          }


       }
    }








   // plot each live particle
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
      	for (int x = 0; x < WIDTH; x++)  {
            char c = grid[y][x];
            if (c == '@') {
                printf("\033[33m%c\033[0m", c);   // yellow sun
            } else if (c == 'o') {
                printf("\033[36m%c\033[0m", c);   // cyan orbiter
            } else {
                putchar(c);   // blank space, no color needed
            }
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

//recording trail fxn -> void / modify each particle trail arr
void record_trail(Particle particles[] , int count) {
	for (int i = 0; i < count; i++)  //for every particle
	{
		particles[i].trail_x[particles[i].trail_index] = particles[i].x; //current x,y posn = trail arr[index]
		particles[i].trail_y[particles[i].trail_index] = particles[i].y;
        
        particles[i].trail_index = (particles[i].trail_index + 1) % TRAIL_LENGTH;
      //incremented index of trail arr by 1  
        //arr will snap back at start/0 once index reaches 7!

	}
}










//mainfxn
int main() {

	enable_ansi();  //to enable asni adn vitual terminal proceses in terminal!
  


  show_title_screen();  //start program . title screen


  int choice = show_menu();

 	srand(time(NULL));  //to get same rnad no: every run!

    
    Particle *particles = malloc(sizeof(Particle) * MAX_PARTICLES);  //alloc max mem to be needed
    //error resolving of mem alloc fail
    if (particles == NULL)
    {
    	printf("Failed to allocate memory\n");
    	return 1;
    }
    
  int active_count = INITIAL_PARTICLES;  //present number of particles req by user

  
    if (choice == 1)
    {
      init_particles(particles , active_count); // single sun
    }
    else
    {
      printf("That nodes is'nt build yet - init single sun mode\n");
      _getch();
      init_particles(particles , active_count);
    }

    printf("you picked - now enter %d\n", choice);
    _getch();



    double *ax = malloc(sizeof(double) * MAX_PARTICLES); //temp accn array mem allooc at runtime
    double *ay = malloc(sizeof(double) * MAX_PARTICLES);


	printf("\033[2J");  //clear screeen once

	while(1) {  //indef loop

      if (_kbhit())   //if true then some key is pressed
      {
      	char key = _getch();  // store that key
        
        if (key == 'n' && active_count < MAX_PARTICLES) 
        {
        	spawn_particle(particles , active_count);
        	active_count++;
        }


      }


		record_trail(particles , active_count);
		compute_forces(particles , active_count , ax , ay);

	    update_positions(particles , active_count, ax , ay);
	    render(particles , active_count);
	    Sleep(33);  //sleep for 33ms | 30fps around 

	}
                    
   free(particles);  //free mem alloctd at runtime[heap] whch is npt stack mem
   free(ax);
   free(ay);
  return 0; 
}




