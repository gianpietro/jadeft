#include <form.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
  //int *prows, *pcols, *pmax;
  int index , i;
    

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  providerField[0] = new_field(1, 1, 6, 22, 0, 0);      /* height 1, width 2, y 6, x 22 */
  providerField[1] = new_field(1, 30, 8, 22, 0, 0);
  providerField[2] = new_field(1, 30, 10, 5, 0, 0);
  providerField[3] = NULL;

  //set_field_opts(providerField[1], O_STATIC);
  //field_opts_off(providerField[1], O_STATIC);
  //set_max_field(providerField[1], 30);
  //dynamic_field_info(providerField[1], prows, pcols, pmax);
  //field_opts_off(providerField[1], O_AUTOSKIP);       	/* Don't go to next field */
  field_opts_off(providerField[2], O_VISIBLE);
  field_opts_off(providerField[2], O_ACTIVE);
  set_field_buffer(providerField[2], 0, "Press F1 to complete");
 

  set_field_type(providerField[0],TYPE_INTEGER,1,1,2);
  set_field_type(providerField[1],TYPE_REGEXP,"^[A-Za-z0-9 ]+$");
  

  providerForm = new_form(providerField);
  post_form(providerForm);

  mvprintw(2,5,"Jade Finacial Tracker\n");
  mvprintw(6, 5, "Active Ind:");
  mvprintw(8, 5, "Provider Name:");  
  move(6,22);     /* move cursor */
  refresh();

  int y = 0;

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
          form_driver(providerForm, REQ_VALIDATION);
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

  actInd = atoi(field_buffer(providerField[0],0));
  strcpy(pname, field_buffer(providerField[1],0));
    
  //mvprintw(11,5,"rows %d cols d%", *prows, *pcols);
  int l = strlen(pname);
  mvprintw(20,5,"pname length %d",l);

  /* Trim the railing whitespace from the field buffer value */
  i = 0;
  while (pname[i] != '\0')
    {
      if(pname[i] != ' ' && pname[i] != '\t' && pname[i] != '\n')
	{
	  index = i;
	}
      i++;
    }

   /* Mark next character to last non-white space character as NULL */
    pname[index + 1] = '\0';
     int nl = strlen(pname);
     mvprintw(22,5,"new pname length %d",nl);
    
    
    if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
     mvprintw(12,5, "Form completed\n");
  else
     mvprintw(12,5, "Data invalid\n");
  
  // actInd = atoi(field_buffer(providerField[0],0));
  //strcpy(pname, field_buffer(providerField[1],0));
  
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

  endwin();  
}
