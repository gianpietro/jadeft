#include <form.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../inc/procur.h"
#include "../inc/prolib.h"

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

void providerWindow()
{
  FIELD *providerField[3];
  FORM *providerForm;
  int ch;
  int cf;   //confirm save data
  int actInd;
  char pname[30];  
  int index , i;
    
  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  /* Add the fields required in the form */
  /* Size of field rows + cols, upper left corner row + col, offscreen rows, nbuf */
  providerField[0] = new_field(1, 1, 6, 22, 0, 0);      
  providerField[1] = new_field(1, 30, 8, 22, 0, 0);
  providerField[2] = NULL;
 
  set_field_type(providerField[0],TYPE_INTEGER,1,1,2);
  set_field_type(providerField[1],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");
 
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
        case 10:                                           /* ASCII value for new line feed(Enter Key) */
          form_driver(providerForm, REQ_VALIDATION);
	  form_driver(providerForm, REQ_NEXT_FIELD);
	  break;
        default:
	 form_driver(providerForm, ch);
	  break;
	}
    }

  actInd = atoi(field_buffer(providerField[0],0));
  strcpy(pname, field_buffer(providerField[1],0));
    
  int l = strlen(pname);
  mvprintw(20,5,"pname length %d",l);

  if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
    {
      strcpy(pname, trimWS(pname));
      int nl = strlen(pname);
      mvprintw(22,5,"new pname length %d",nl); 
      mvprintw(12,5, "Form completed\n");
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
   }
  else
    {
      mvprintw(12,5, "Data invalid\n");
    }
   
  getch();
  
  unpost_form(providerForm);
  free_form(providerForm);
  free_field(providerField[0]);
  free_field(providerField[1]);
  free_field(providerField[2]);

  endwin();  
}
