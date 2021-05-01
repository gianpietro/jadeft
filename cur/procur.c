#include <form.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"


/* Trim trailing whitespace from the string entered in form field */
char * trimWS(char *s)
{
  int i = 0;
  int index = -1;

  while (s[i] != '\0')
    {
      if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
	{
	  index = i;
	}
      i++;
    }
  s[index + 1] = '\0';

  return s;
}       

/* Function to display Provider form for data entry */
void providerWindow()
{
  FIELD *providerField[3];
  FORM *providerForm;
  WINDOW *proWin;
  int ch;
  int cf;            /* confirm save data */
  int actInd;        /* active_Ind */
  char pname[30];    /* provider_name DEBUG CODE to be 30*/ 
  int index , i;
  int newRec= 'y';   /* Add another new record */
  int rows, cols;
  int y = 2;         /* sub_form window row */
  int x = 2;         /* fsub_orm window col */
  //  int pStatus = 0;       /* ack value from providerInsert */
    
  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

    while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      /* Add the fields required in the form */
      /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
      providerField[0] = new_field(1, 1, 2, 22, 0, 0);      
      providerField[1] = new_field(1, 30, 4, 22, 0, 0);   //DEBUG CODE needs to be 30
      providerField[2] = NULL;

      /* Field 1 digit allowed in range from 1 to 2 */
      set_field_type(providerField[0],TYPE_INTEGER,1,1,2);
      /* Field allowed values of A-Z a-z and hyphen */
      set_field_type(providerField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
 
      providerForm = new_form(providerField);
      scale_form(providerForm, &rows, &cols);

      /* Add window which will be associated to form */
      proWin = newwin(rows+15, cols+20,1,1);  
      keypad(proWin, TRUE);
       
      /* Set main and sub windows */
      set_form_win(providerForm, proWin);
      set_form_sub(providerForm, derwin(proWin,rows,cols,y,x));
      getmaxyx(proWin,rows,cols);
      
      box(proWin, 0,0);
          
      if (proWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(proWin,"Provider Entry Form");
      post_form(providerForm); 
      wrefresh(proWin);

      // mvwprintw(proWin,1,1,"y = %d x = %d",rows,cols);

      //mvwprintw(proWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(proWin,y+2,x+5, "Active Ind:");                           //was y+6
      mvwprintw(proWin,y+4,x+5, "Provider Name:");
      mvwprintw(proWin,rows-2,x+5,"Press F1 when form complete");
      wmove(proWin,y+2,x+22);     /* move cursor */
      //refresh;
      
      while((ch = wgetch(proWin)) != KEY_F(1))
	{
	  switch(ch)
	    {
	    case KEY_DOWN:
	      form_driver(providerForm, REQ_NEXT_FIELD);
	      form_driver(providerForm, REQ_END_LINE);
	      break;
	    case KEY_UP:
	      form_driver(providerForm, REQ_PREV_FIELD);
	      form_driver(providerForm, REQ_END_LINE);
	      break;
	    case KEY_BACKSPACE:
	      form_driver(providerForm, REQ_CLR_FIELD);
	    case 10:                                           /* ASCII value for new line feed(Enter Key) */
	      form_driver(providerForm, REQ_VALIDATION);
	      form_driver(providerForm, REQ_NEXT_FIELD);
	      break;
	    default:
	      form_driver(providerForm, ch);
	      break;
	    }
	}

      /* Assign data entered in field */
      actInd = atoi(field_buffer(providerField[0],0));
      strcpy(pname, field_buffer(providerField[1],0));
      
      // int l = strlen(pname);
      //mvwprintw(proWin,20,5,"pname length %d",l);

      if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
	{
	  strcpy(pname, trimWS(pname)); //DEBUG CODE need to reapply
	  //int nl = strlen(pname);
	  //  mvwprintw(proWin,22,5,"new pname length %d",nl); 
	  //mvwprintw(proWin,12,5, "Form completed");
	  // mvwprintw(proWin,14,5, "Value Active Ind: %d",actInd);
	  // mvwprintw(proWin,15,5,"Value Provider Name: %s", pname);
	  echo();
	  mvwprintw(proWin,y+10,x+5,"Save: y/n: ");     //was y+12
	  //move(y+10, 18);
	  wmove(proWin,y+10,18);
	  while((cf = wgetch(proWin)) != 'y')
	    {
	      //move(y+10, 18);
	      wmove(proWin,y+10,18);
	      if (cf == 'n')
		{
		  mvwprintw(proWin,y+12,x+5, "Data not saved");
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      //mvwprintw(proWin,y+11,x+10, "ack %d ", pStatus);
	      //mvwprintw(proWin,y+12,x+5, "Data saved");
	      providerInsert(actInd,pname);   /* Save data to database */
	      //if (pStatus == 1)
	      //{
		mvwprintw(proWin,y+12,x+5, "Data saved");
    	        //mvwprintw(proWin,y+13,x+10, "ack %d ", pStatus);
		//pStatus = 0;		   	  
	//noecho();
          	}
	}
      else
	{
	  mvwprintw(proWin,y+10,x+5, "Data invalid");
	}
          noecho();
    
      //getch();

      unpost_form(providerForm);
      free_form(providerForm);
      free_field(providerField[0]);
      free_field(providerField[1]);
      free_field(providerField[2]);

      mvwprintw(proWin,y+14,x+5,"Provider Entry Form");
      mvwprintw(proWin,y+14,x+5,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proWin)) != 'y')
	{
	  wmove(proWin,y+14,x+42);
	  if(newRec == 'n')
	    break;
	}
      noecho();
    }    
  
  endwin();
  
}
