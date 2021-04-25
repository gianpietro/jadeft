#include <form.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"



void providerWindow()
{
  FIELD *providerField[4];
  FORM *providerForm;
  int ch;
  int cf;   //confirm save data
  int actInd;
  char pname[30];
    

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  providerField[0] = new_field(1, 1, 6, 22, 0, 0);      /* height 1, width 2, y 6, x 22 */
  providerField[1] = new_field(1, 30, 8, 22, 0, 0);
  providerField[2] = new_field(1, 30, 10, 5, 0, 0);
  providerField[3] = NULL;

  //field_opts_off(providerField[1], O_AUTOSKIP);       	/* Don't go to next field */
  field_opts_off(providerField[2], O_VISIBLE);
  field_opts_off(providerField[2], O_ACTIVE);
  set_field_buffer(providerField[2], 0, "Press F1 to complete");
    

  providerForm = new_form(providerField);
  post_form(providerForm);

  mvprintw(2,5,"Jade Finacial Tracker\n");
  mvprintw(6, 5, "Active Ind:");
  mvprintw(8, 5, "Provider Name:");  
  move(6,22);     /* move cursor */
  refresh();

  while((ch = getch()) != KEY_F(1))
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
        case 10:                                           // ASCII value for new line feed(Enter Key) 
	  form_driver(providerForm, REQ_NEXT_FIELD);
	  //form_driver(providerForm, REQ_END_LINE);
	  if (providerField[2])
	    {
	      field_opts_on(providerField[2], O_VISIBLE);
	    }	      
	  break;
        default:
	 form_driver(providerForm, ch);
	  break;
	}
    }  
  
  mvprintw(12,5, "Form completed\n");
  
  actInd = atoi(field_buffer(providerField[0],0));
  strcpy(pname, field_buffer(providerField[1],0));
  
  mvprintw(14,5, "Value Active Ind: %d",actInd);
  mvprintw(15,5,"Value Provider Name: %s", pname);
  echo();
  mvprintw(16,5,"Save: y/n :");
  move(16, 18);
    
  while((cf = getch()) != 'y')
    {
       move(16, 18);
       if (cf == 'n')
  	 {
	   mvprintw(18,5, "Not saved");
	   break;
	 }     
    }

  if (cf == 'y')
    {
      mvprintw(18,5, "Data saved");
      providerInsert(actInd,pname);
    }
     
  noecho();
    
  getch();
  
  unpost_form(providerForm);
  free_form(providerForm);
  free_field(providerField[0]);
  free_field(providerField[1]);
  free_field(providerField[2]);
  free_field(providerField[3]);
  free_field(providerField[4]);

  endwin();  
}
