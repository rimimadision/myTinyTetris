#include<ctime>
#include<unistd.h>
#include<curses.h>
#include<stdlib.h>
#include<string.h>

/* local var */
int tick = 0;
int cur_x, cur_y, pre_x, pre_y, cur_form, pre_form, cur_col;
int score = 0;
int block[8][4]= {
                  {0,1,11,21},
                  {0,1,2,10},
                  {0,1,10,11},
                  {0,1,10,11},
                  {0,10,20,30},
                  {0,1,2,3},
                  {0,10,11,21},
                  {1,10,11,20},
                             };
int board[20][10];

/* get a new piece */
void newPiece()
{
	pre_form = cur_form = rand() % 8;
	cur_col = rand() % 7 + 1;
	cur_x = pre_x = 0; 
	cur_y = pre_y = rand() % 10;
	int x, y;
	for(int i = 0; i < 4; i ++)
	{
		x = cur_x + block[cur_form][i] / 10;
		y = cur_y + block[cur_form][i] % 10;
		
		if(y > 9)
		{
			newPiece();
			break;
		}
	}	
}
void setBoard(int x, int y, int col)
{
	if(x >= 0 && x <= 19 && y >= 0 && y <= 9)
		board[x][y] = col;
}
int checkHit()
{
	/* temporary assume it updates */
	int x, y, jud = 0;
	for(int i = 0; i != 4; i ++)
	{
		x = pre_x + block[pre_form][i] / 10;
		y = pre_y + block[pre_form][i] % 10;
		setBoard(x, y, 0);
	}
	for(int i = 0; i != 4; i ++) 
	{
        x = cur_x + block[cur_form][i] / 10;
		y = cur_y + block[cur_form][i] % 10;
		if(x > 19 || y < 0 || y > 9 || board[x][y])
		{
			jud = 1;
			break;
		}
    }
	for(int i = 0; i != 4; i ++)
	{
		x = pre_x + block[pre_form][i] / 10;
		y = pre_y + block[pre_form][i] % 10;
		setBoard(x, y, cur_col);
	}
	return jud;
}
void showFrame()
{
	for(int i = 0; i != 20; i ++)
	{
		/* otherwise the box won't draw */
		move(i + 1, 1);
		for(int j = 0; j != 10; j ++)
		{
			attron(COLOR_PAIR(board[i][j]));
			printw("  ");
			attroff(COLOR_PAIR(board[i][j]));
		}
	}
	move(21, 1);
	printw("Score: %d", score);
	refresh();
}
void update()
{
	int x, y;
	for(int i = 0; i < 4; i ++)
	{
		x = pre_x + block[pre_form][i] / 10;
		y = pre_y + block[pre_form][i] % 10;
		setBoard(x, y, 0);
	}
	for(int i = 0; i < 4; i ++)
	{
		x = cur_x + block[cur_form][i] / 10;
		y = cur_y + block[cur_form][i] % 10;
		setBoard(x, y, cur_col);
	}
	pre_form = cur_form;
	pre_x = cur_x;
	pre_y = cur_y;
}
void removeLine()
{
	for(int x = 0; x < 20; x ++)
	{
		int jud = 0;
		for(int y = 0; y < 10; y ++)
		{
			if(!board[x][y])
			{
				jud = 1;
				break;
			}
		}
		if(!jud)
		{
			for(int i = x; i > 0; i --)
				memcpy(&board[i + 1][0], & board[i][0], 40);
			memset(&board[0][0], 0, 40);
			score ++;
		}
	}
}
void initial()
{
	/* get random seeds */
	srand(time(0));	

	/* init the screen */
	initscr();
	start_color();
	/* get 8 pairs of colors */
	for(int i = 0; i != 8; i ++)
	{
		init_pair(i, 0, i);
	}
	for(int i = 0; i != 20; i ++)
		for(int j = 0; j != 20; j ++)
			setBoard(i, j, 0);
	/* some configuration */
	resizeterm(22, 22);
	noecho();
	timeout(false);
	curs_set(false);
	box(stdscr, 0, 0);
	refresh();
}
bool doTick()
{
	if(++tick > 30)
	{
		tick = 0;
		cur_x ++;
		if(!checkHit())
		{
			update();
		}
		else
		{
			if(-- cur_x <= 0)
				return 0;
			removeLine();
			newPiece();
		}
		showFrame();
	}

	return true;
}
void runloop()
{
	/* generate the first piece */
	newPiece();

	while(doTick())
	{
		usleep(10000);
	
		int button = getch();
		switch(button)
		{
			case 'w':
				pre_form = cur_form;
				if(cur_form % 2)
					cur_form --;
				else
					cur_form ++;
				if(checkHit())
					cur_form = pre_form;
				else
				{
					update();
					showFrame();
				}
				break;
			case 's':
				while(!checkHit())
					cur_x ++;
				cur_x --;
				update();
				showFrame();
				break;
			case 'a':
				cur_y --;
				if(checkHit())
				{
					cur_y ++;
				}else
				{
					update();
					showFrame();
				}
				break;
			case 'd':
				cur_y ++;
				if(checkHit())
				{
					cur_y --;
				}
				else
				{	
					update();
					showFrame();
				}
				break;
			case 'q':
				return;
		}

		button = 0;

		/* print one frame */
		update();
		showFrame();	
	}
}
int main()
{	
	initial();
	runloop();
	usleep(500000);
	endwin();

	return 0;
}	
