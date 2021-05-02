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
  char pname[30];    /* provider_name */ 
  int index , i;
  int newRec= 'y';   /* Add another new record */
  int rows, cols;
  int y = 2;         /* sub_form window row */
  int x = 2;         /* fsub_orm window col */
    
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
      mvwprintw(proWin,rows-12, cols-50,"row %d col %d", rows, cols); // DEBUG CODE
      post_form(providerForm); 
      wrefresh(proWin);

      //mvwprintw(proWin,y+2,x+5,"Jade Finacial Tracker");
      mvwprintw(proWin,y+2,x+5, "Active Ind:");                           
      mvwprintw(proWin,y+4,x+5, "Provider Name:");
      mvwprintw(proWin,rows-2,x+5,"Press F1 when form complete");
      wmove(proWin,y+2,x+22);     /* move cursor */
      
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

      if ((form_driver(providerForm,REQ_VALIDATION) == E_OK) && (actInd >= 1) && (!isspace(*pname)))
	{
	  strcpy(pname, trimWS(pname));  
	  echo();
	  mvwprintw(proWin,y+10,x+5,"Save: y/n: ");     
	  wmove(proWin,y+10,18);
	  while((cf = wgetch(proWin)) != 'y')
	    {
	      wmove(proWin,y+10,18);
	      if (cf == 'n')
		{
		  mvwprintw(proWin,y+12,x+5, "Data not saved");
		  break;
		}
	    }	  
	  if (cf == 'y')
	    {
	      providerInsert(actInd,pname);   /* Save data to database */
	      mvwprintw(proWin,y+12,x+5, "Data saved");
	    }
	}
      else
	{
	  mvwprintw(proWin,y+10,x+5, "Data invalid");
	}
          noecho();

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

/* Function to display Provider Type form for data entry */
void providerTypeWindow()
{
  FIELD *proTypeField[2];
  FORM *proTypeForm;
  WINDOW *proTypeWin;
  int ch;
  char newRec = 'y';
  int rows, cols;
  int y = 2;   /* UL corner form subwindow */
  int x = 2;   /* UL corner form subwindow */
  char pdesc[30];
  int cf;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr,TRUE);

  while (newRec == 'y')  /* Start loop to allow option to add subsequent records to form */
    {
      proTypeField[0] = new_field(1,30,2,18,0,0);
      proTypeField[1] = NULL;
      set_field_type(proTypeField[0],TYPE_REGEXP,"^[A-Za-z0-9 -]+$");

      proTypeForm = new_form(proTypeField);
      scale_form(proTypeForm, &rows, &cols);
      proTypeWin = newwin(rows+15,cols+5,1,1);  
      keypad(proTypeWin, TRUE);

      set_form_win(proTypeForm, proTypeWin);	
      set_form_sub(proTypeForm, derwin(proTypeWin,rows,cols,y,x));
      getmaxyx(proTypeWin,rows,cols);
      
      box(proTypeWin, 0,0);
          
      if (proTypeWin == NULL)
	{
	  addstr("Unable to create window");
	  refresh();
	  getch();	  
	}

      waddstr(proTypeWin,"Provider Type Form");
      //mvwprintw(proTypeWin,rows-12, cols-50,"row %d col %d", rows, cols);
      post_form(proTypeForm); 
      wrefresh(proTypeWin);

      mvwprintw(proTypeWin,rows-14,cols-46, "Description:");
      //mvwprintw(proTypeWin,y+2,x+5, "Description:");
      mvwprintw(proTypeWin,rows-2,cols-46,"Press F1 when form complete");
      wmove(proTypeWin,rows-14,cols-33);     /* move cursor */
      
      while((ch = wgetch(proTypeWin)) != KEY_F(1))
	{
	  switch(ch)
	    {
	    case KEY_DOWN:
	      form_driver(proTypeForm, REQ_NEXT_FIELD);
	      form_driver(proTypeForm, REQ_END_LINE);
	      break;
	    case KEY_UP:
	      form_driver(proTypeForm, REQ_PREV_FIELD);
	      form_driver(proTypeForm, REQ_END_LINE);
	      break;
	    case KEY_BACKSPACE:
	      form_driver(proTypeForm, REQ_CLR_FIELD);
	    case 10:                                           /* ASCII value for new line feed(Enter Key) */
	      form_driver(proTypeForm, REQ_VALIDATION);
	      form_driver(proTypeForm, REQ_NEXT_FIELD);
	      break;
	    default:
	      form_driver(proTypeForm, ch);
	      break;
	    }
	}      

      strcpy(pdesc,field_buffer(proTypeField[0],0));

      if ((form_driver(proTypeForm,REQ_VALIDATION) == E_OK) && (!isspace(*pdesc)))
	{
	  strcpy(pdesc, trimWS(pdesc));
	  mvwprintw(proTypeWin,rows-4,cols-46,"input val:%s",pdesc);	    
	  echo;
	  mvwprintw(proTypeWin,rows-12,cols-46, "Save y/n: ");
	  wmove(proTypeWin,rows-12,cols-27);
	  while((cf = wgetch(proTypeWin)) != 'y')
	    {
	      wmove(proTypeWin,rows-12,cols-27);
	      if (cf =- 'n')
		{
		  mvwprintw(proTypeWin,rows-10,cols-46, "Data not saved");
		  break;
		}
	    }
	  if(cf == 'y')
	    {
	      proTypeInsert(pdesc);
	      mvwprintw(proTypeWin,rows-10,cols-46, "Data saved");
	    }
	}
	  else
	    {
	      mvwprintw(proTypeWin,rows-10,cols-46, "Data invalid");
	    }
	  noecho();
	

      unpost_form(proTypeForm);
      free_form(proTypeForm);
      free_field(proTypeField[0]);
      free_field(proTypeField[1]);

      mvwprintw(proTypeWin,rows-6,cols-46,"Provide Type Entry Form");
      mvwprintw(proTypeWin,rows-5,cols-46,"Do you want to add a new record y/n: ");
      echo();
      while((newRec = wgetch(proTypeWin)) != 'y')
	{
	  wmove(proTypeWin,rows-5,cols-46);
	  if(newRec == 'n')
	    break;
	}
      noecho();

      endwin();
    }

}




  
